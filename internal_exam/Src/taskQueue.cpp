//
// Created by keqiu on 2023/7/21.
//

#include "../Inc/taskQueue.h"


TaskQueue::TaskQueue()
{
    pthread_mutex_init(&taskQueueMutex, nullptr);
}

TaskQueue::~TaskQueue()
{
    pthread_mutex_destroy(&taskQueueMutex);
}

void TaskQueue::addTask(const Task& task)
{
    pthread_mutex_lock(&taskQueueMutex);
    taskQueue.push(task);
    pthread_mutex_unlock(&taskQueueMutex);
}

void TaskQueue::addTask(function working, void *arg)
{
    pthread_mutex_lock(&taskQueueMutex);
    taskQueue.push(Task(working,arg));
    pthread_mutex_unlock(&taskQueueMutex);
}

Task TaskQueue::takeTask()
{
   Task task;
   if(!taskQueue.empty())
   {
       pthread_mutex_lock(&taskQueueMutex);
       task = taskQueue.front();//取出队列头部的任务
       taskQueue.pop();//删除队列头部的任务
       pthread_mutex_unlock(&taskQueueMutex);
   }
    return task;
}