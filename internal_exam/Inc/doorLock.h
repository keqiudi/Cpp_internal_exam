//
// Created by keqiu on 2023/7/27.
//

#ifndef INTERNAL_EXAM_DOORLOCK_H
#define INTERNAL_EXAM_DOORLOCK_H

#include <cstdint>
#include "Socket.h"

class doorLock{

private:
    struct
    {
        uint16_t magic;
        uint16_t size;
        uint8_t  type;
        uint8_t token[16];
    }header{};//包头

public:
    doorLock();
    //获取包头
    void getPacketHeader(int& socket_fd);
    //发送包头
    void headerSend(uint16_t size,uint8_t type,int& socket_fd);
    //开门请求
    void openDoorRequest(int& socket_fd);
    //锁门请求
    void closeDoorRequest(int& socket_fd);
    //查询门锁状态请求
    void doorStateRequest(int& socket_fd);
    //获取开门响应
    void getOpenDoorResponse(int& socket_fd);
    //获取锁门响应
    void getCloseDoorResponse(int& socket_fd);
    //获取查询门锁状态响应
    void getDoorStateResponse(int& socket_fd);

};


#endif //INTERNAL_EXAM_DOORLOCK_H
