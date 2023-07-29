//
// Created by keqiu on 2023/7/27.
//

#include "../Inc/doorLock.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <ctime>
doorLock::doorLock()
{
   header.magic = 514;
   header.size = 0;
   header.type = 0;
   memset(header.token,0,sizeof(header.token));
}

void doorLock::headerSend(uint16_t size, uint8_t type,int& socket_fd)
{
    if(write(socket_fd,&header.magic,sizeof(header.magic))==-1)
    {
        perror("write");
        exit(1);
    }
    if(write(socket_fd,&size,sizeof(size))==-1)
    {
        perror("write");
        exit(1);
    }
    if(write(socket_fd,&type,sizeof(type))==-1)
    {
        perror("write");
        exit(1);
    }

    uint8_t token[16];
    uint32_t time = (uint32_t)std::time(nullptr);
    for (int i = 0; i < 16; i++) {
        token[i] = (uint8_t)(time >> (i * 2));
    }

    strncpy((char*)header.token,(char*)token,sizeof(token));

    if(write(socket_fd,token,sizeof(token))==-1)
    {
        perror("write");
        exit(1);
    }

}

void doorLock::getPacketHeader(int &socket_fd)
{
    if(read(socket_fd,&header.magic,sizeof(header.magic))==-1)
    {
        perror("read");
        exit(1);
    }

    if(read(socket_fd,&header.size,sizeof(header.size))==-1)
    {
        perror("read");
        exit(1);
    }

    if(read(socket_fd,&header.type,sizeof(header.type))==-1)
    {
        perror("read");
        exit(1);
    }

    if(read(socket_fd,header.token,sizeof(header.token))==-1)
    {
        perror("read");
        exit(1);
    }

}

void doorLock::openDoorRequest(int& socket_fd)
{
    uint8_t userName[16];
    uint16_t userNameSize;
    std::cout << "请输入用户名:";
    std::cin >> userName;
    userNameSize = strlen((char*)userName);

    headerSend(userNameSize+2,1,socket_fd);

    if(write(socket_fd,&userNameSize,sizeof(userNameSize))==-1)
    {
        perror("write");
        exit(1);
    }

    if(write(socket_fd,userName,userNameSize)==-1)
    {
        perror("write");
        exit(1);
    }
}

void doorLock::closeDoorRequest(int &socket_fd)
{
    uint8_t userName[16];
    uint16_t userNameSize;
    std::cout << "请输入用户名:";
    std::cin >> userName;
    userNameSize = strlen((char*)userName);

    headerSend(userNameSize+2,2,socket_fd);

    if(write(socket_fd,&userNameSize,sizeof(userNameSize))==-1)
    {
        perror("write");
        exit(1);
    }

    if(write(socket_fd,userName,userNameSize)==-1)
    {
        perror("write");
        exit(1);
    }
}

void doorLock::doorStateRequest(int &socket_fd)
{
    uint8_t userName[16];
    uint16_t userNameSize;
    std::cout << "请输入用户名:";
    std::cin >> userName;
    userNameSize = strlen((char*)userName);

    headerSend(userNameSize+2,3,socket_fd);

    if(write(socket_fd,&userNameSize,sizeof(userNameSize))==-1)
    {
        perror("write");
        exit(1);
    }

    if(write(socket_fd,userName,userNameSize)==-1)
    {
        perror("write");
        exit(1);
    }
}


void doorLock::getOpenDoorResponse(int &socket_fd)
{
    getPacketHeader(socket_fd);

    bool isSuccess;
    if(read(socket_fd, &isSuccess, sizeof(isSuccess)) == -1)
    {
        perror("read");
        exit(1);
    }

    if(isSuccess)
    {
        std::cout<< "开门成功" << std::endl;
    }
    else
    {
        std::cout<< "开门失败" << std::endl;
    }

}

void doorLock::getCloseDoorResponse(int &socket_fd)
{
    getPacketHeader(socket_fd);

    bool isSuccess;
    if(read(socket_fd, &isSuccess, sizeof(isSuccess)) == -1)
    {
        perror("read");
        exit(1);
    }

    if(isSuccess)
    {
        std::cout<< "锁门成功" << std::endl;
    }
    else
    {
        std::cout<< "锁门失败" << std::endl;
    }
}

void doorLock::getDoorStateResponse(int &socket_fd)
{
    getPacketHeader(socket_fd);

    bool isSuccess,status;

    if(read(socket_fd, &isSuccess, sizeof(isSuccess)) == -1)
    {
        perror("read");
        exit(1);
    }

    if(read(socket_fd, &status, sizeof(status)) == -1)
    {
        perror("read");
        exit(1);
    }

    if(isSuccess)
    {
        if(status)
        {
            std::cout<< "门是开着的" << std::endl;
        }
        else
        {
            std::cout<< "门是关着的" << std::endl;
        }
    }
    else
    {
        std::cout<< "获取门状态失败" << std::endl;
    }

}