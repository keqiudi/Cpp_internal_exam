//
// Created by keqiu on 2023/7/21.
//

#ifndef INTERNAL_EXAM_THREADPOOL_H
#define INTERNAL_EXAM_THREADPOOL_H

#include "taskQueue.h"



class threadPool{

private:
    TaskQueue* taskQueue;
    pthread_t managerID;
    pthread_t* threadIDs;
    int threadMinNum;
    int threadMaxNum;
    int busyThreadNum;
    int liveThreadNum;
    int exitThreadNum;

    pthread_mutex_t threadPoolMutex;
    pthread_mutex_t busyNumMutex;
    pthread_cond_t notEmpty;

    bool shutdown;

    static void* worker(void* arg);

    static void* manager(void* arg);

    void threadExit();
public:
    threadPool(int minNum,int maxNum);

    threadPool() = default;

    ~threadPool();

    void addTask(Task task);

    inline int getBusyThreadNum()
    {
        pthread_mutex_lock(&busyNumMutex);
        int busyNum = busyThreadNum;
        pthread_mutex_unlock(&busyNumMutex);
        return busyNum;
    }

    inline int getAliveThreadNum()
    {
        pthread_mutex_lock(&threadPoolMutex);
        int aliveNum = liveThreadNum;
        pthread_mutex_unlock(&threadPoolMutex);
        return aliveNum;
    }

};


#endif //INTERNAL_EXAM_THREADPOOL_H
