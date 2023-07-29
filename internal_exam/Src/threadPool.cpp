//
// Created by keqiu on 2023/7/21.
//

#include "../Inc/threadPool.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include "../Inc/Socket.h"
using namespace std;

#define ONE_TIME 3
#define ADD_THREAD 3
#define EXIT_THREAD 3

template <typename T>
inline void safe_delete_void_ptr(void* &target);

threadPool::threadPool(int minNum, int maxNum)
{
    do{
        taskQueue = new (std::nothrow)TaskQueue();
        if(taskQueue == nullptr)
        {
            cout<<"new taskQueue failed"<<endl;
            break;
        }

        threadIDs = new (std::nothrow)pthread_t[maxNum];
        if(threadIDs == nullptr)
        {
            cout<<"new threadIDs failed"<<endl;
            break;
        }
        memset(threadIDs,0,sizeof(pthread_t)*maxNum);

        threadMinNum = minNum;
        threadMaxNum = maxNum;
        busyThreadNum = 0;
        liveThreadNum = minNum;
        exitThreadNum = 0;
        shutdown = false;

        if(pthread_mutex_init(&threadPoolMutex, nullptr)!=0||
        pthread_mutex_init(&busyNumMutex, nullptr)!=0||
           pthread_cond_init(&notEmpty, nullptr) != 0)
        {
            cout<<"init threadPoolMutex failed"<<endl;
            break;
        }

        pthread_create(&managerID, nullptr,manager,this);

        for(int i=0;i<3;i++)
        {
            pthread_create(&threadIDs[i], nullptr,worker,this);
            pthread_detach(threadIDs[i]);
        }

        return ;
    }while(false);

        if(taskQueue)
        {
            delete taskQueue;
            taskQueue = nullptr;
        }

        if(threadIDs)
        {
            delete[] threadIDs;
            threadIDs = nullptr;
        }
}

threadPool::~threadPool()
{
    this->shutdown = true;

    pthread_join(managerID, nullptr);

    for(int i=0;i<liveThreadNum;i++)
    {
        pthread_cond_signal(&notEmpty);
    }

    pthread_mutex_destroy(&threadPoolMutex);
    pthread_mutex_destroy(&busyNumMutex);
    pthread_cond_destroy(&notEmpty);

    if(taskQueue)
    {
        delete taskQueue;
        taskQueue = nullptr;
    }
    if(threadIDs)
    {
        delete[] threadIDs;
        threadIDs = nullptr;
    }

}

void threadPool::threadExit()//保证线程退出后thread_id为0
{
  pthread_t id = pthread_self();

  for(int i=0;i<threadMaxNum;i++)
  {
    if(threadIDs[i]==id)
    {
      threadIDs[i]=0;
      break;
    }
  }

  pthread_exit(nullptr);//线程退出
}

void threadPool::addTask(Task task)
{
    if(shutdown)
    {
        return;
    }

    taskQueue->addTask(task);//添加任务
    pthread_cond_broadcast(&notEmpty);//唤醒所有线程
}

void* threadPool::worker(void* arg)
{
    auto pool = static_cast<threadPool*>(arg);

    while(true)
    {
        pthread_mutex_lock(&pool->threadPoolMutex);

        while(pool->taskQueue->getTaskSize()==0&&!pool->shutdown)
        {
            pthread_cond_wait(&pool->notEmpty, &pool->threadPoolMutex);

            if (pool->exitThreadNum > 0)//判断是否销毁线程
            {
                pool->exitThreadNum--;

                if (pool->liveThreadNum > pool->threadMinNum)//销毁线程线程退出
                {
                    pool->liveThreadNum--;
                    pthread_mutex_unlock(&pool->threadPoolMutex);
                    pool->threadExit();
                }
            }

            if(pool->shutdown)//如果线程池关闭
            {
                pthread_mutex_unlock(&pool->threadPoolMutex);
                pool->threadExit();
            }
        }

        Task task = pool->taskQueue->takeTask();

        pthread_mutex_unlock(&pool->threadPoolMutex);
        pool->busyThreadNum++;
        pthread_mutex_unlock(&pool->busyNumMutex);
        cout << "thread:"<< pthread_self() << " start working" << endl;

        task.working(task.arg);//执行任务
        safe_delete_void_ptr<Client_info>(task.arg);//安全删除任务函数参数

        cout << "thread:"<< pthread_self() << " end working" << endl;

        pthread_mutex_lock(&pool->busyNumMutex);
        pool->busyThreadNum--;
        pthread_mutex_unlock(&pool->busyNumMutex);


    }
}

void* threadPool::manager(void* arg)
{
    auto* pool = static_cast<threadPool*>(arg);

    while(!pool->shutdown)
    {
        sleep(ONE_TIME);//管理者线程每隔3s检测一次

        pthread_mutex_lock(&pool->threadPoolMutex);
        size_t queueSize = pool->taskQueue->getTaskSize();
        int liveThreadNum = pool->liveThreadNum;
        int busyThreadNum = pool->busyThreadNum;
        pthread_mutex_unlock(&pool->threadPoolMutex);

        //创建线程规则
        /*1.任务队列中的任务数量大于存活线程数
         *2. 存活的线程数小于最大线程数
         */
        if(queueSize>liveThreadNum&&liveThreadNum<pool->threadMaxNum)
        {

            pthread_mutex_lock(&pool->threadPoolMutex);
            /*遍历寻找thread_id为空的线程*/
            int counter=0;
            for(int i=0;counter<ADD_THREAD&&i<pool->threadMaxNum&&pool->liveThreadNum<pool->threadMaxNum;i++)
            {
                if(pool->threadIDs[i]==0)
                {
                    pthread_create(&pool->threadIDs[i],nullptr,worker,pool);
                    pthread_detach(pool->threadIDs[i]);//分离线程
                    counter++;
                    pool->liveThreadNum++;
                }
            }
            pthread_mutex_unlock(&pool->threadPoolMutex);
        }

        //销毁线程规则
        /*1.任务队列中的任务数量*2小于忙的线程数
         *2. 存活的线程数大于最小线程数
         */
        if(queueSize*2<busyThreadNum&&liveThreadNum>pool->threadMinNum)
        {
            pthread_mutex_lock(&pool->threadPoolMutex);
            pool->exitThreadNum = EXIT_THREAD;//线程同步
            pthread_mutex_unlock(&pool->threadPoolMutex);//

            for(int i=0;i<EXIT_THREAD;i++)
            {
                pthread_cond_signal(&pool->notEmpty);//唤醒线程,让其自行销毁
            }
        }
    }
    return nullptr;
}

template <typename T>
void safe_delete_void_ptr(void* &target)
{
    if(target)
    {
        T* tmp = static_cast<T*>(target);
        delete tmp;
        tmp = nullptr;
        target = nullptr;
    }
}