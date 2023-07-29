//
// Created by keqiu on 2023/7/21.
//

#ifndef INTERNAL_EXAM_TASKQUEUE_H
#define INTERNAL_EXAM_TASKQUEUE_H

#include<queue>
#include<pthread.h>


using function = void(*)(void* arg);
struct Task{

    function working;
    void* arg;

    Task()
    {
        working = nullptr;
        arg = nullptr;
    }

    Task(function working,void* arg)
    {
        this->working = working;
        this->arg = arg;
    }
};

class TaskQueue {

private:
        std::queue<Task>taskQueue;
        pthread_mutex_t taskQueueMutex;
public:
        TaskQueue();
        ~TaskQueue();

        void addTask(const Task& task);

        void addTask(function working,void* arg);

        Task takeTask();

        inline size_t getTaskSize()
        {
            return taskQueue.size();
        }

};


#endif //INTERNAL_EXAM_TASKQUEUE_H
