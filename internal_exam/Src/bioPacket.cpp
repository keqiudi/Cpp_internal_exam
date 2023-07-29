//
// Created by keqiu on 2023/7/26.
//

#include "../Inc/bioPacket.h"
#include "../Inc/User.h"
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <ctime>
#include <mutex>
#include <vector>
#include "../Inc/doorLock.h"
extern std::mutex vectorMutex;
extern std::vector<User>userList;
extern std::vector<Manager>managerList;

bool bioPacket::getPacketHeader(Client_info* client_info)
{

    while(true)
    {
        size_t len1,len2,len3,len4;
        len1 = read(client_info->accept_fd,&header.magic, sizeof(header.magic));
        len2 = read(client_info->accept_fd,&header.size, sizeof(header.size));
        len3 = read(client_info->accept_fd,&header.type, sizeof(header.type));
        len4 = read(client_info->accept_fd,header.token, sizeof(header.token));

        if(len1==-1||len2==-1||len3==-1||len4==-1)
        {
            perror("read");
            exit(1);
        }
        /*令牌解码*/
        uint32_t time = (uint32_t)std::time(nullptr);
        uint32_t tokenTime = 0;
        for (int i = 0; i < 16; i++) {
            tokenTime |= (uint32_t)header.token[i] << (i * 2);
        }
        if (time - tokenTime > 60) {
            continue;
            // 令牌已过期
        }

        else if(len1==0||len2==0||len3==0||len4==0)
        {
            std::cout<<"客户端已断开连接"<<std::endl;
            close(client_info->accept_fd);
            return false;
        }

        if(header.magic==101)
            bioTypeMenu(client_info);
    }
}

void bioPacket::bioTypeMenu(Client_info *client_info)
{

    switch (header.type)
    {
        case 1:
            bioSuccessResponse(client_info);
            break;
        case 2:
            bioFailResponse(client_info);
            break;
    }


}

void bioPacket::headerSend(Client_info *client_info, uint16_t magic, uint16_t size, uint8_t _type)
{
    if(write(client_info->accept_fd,&magic,sizeof(magic))==-1)
    {
        perror("write");
        exit(1);
    }

    if(write(client_info->accept_fd,&size,sizeof(size))==-1)
    {
        perror("write");
        exit(1);
    }

    if(write(client_info->accept_fd,&_type,sizeof(_type))==-1)
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

    if(write(client_info->accept_fd,token,sizeof(token))==-1)
    {
        perror("write");
        exit(1);
    }
}

bool bioPacket::getSuccessResponse(Client_info *client_info)
{
    char userName[64];
    uint16_t usernameSize;

    if(read(client_info->accept_fd,&usernameSize,sizeof(usernameSize))==-1)
    {
        perror("read");
        exit(1);
    }
    memset(userName,0,usernameSize);
    if(read(client_info->accept_fd,userName,usernameSize)==-1)
    {
        perror("read");
        exit(1);
    }

    vectorMutex.lock();
    for (auto & i : userList)
    {
       if(strcmp(userName,i.getUserName())==0)
       {
           return true;
       }
    }
    for(auto & i : managerList)
    {
        if(strcmp(userName,i.getUserName())==0)
        {
            return true;
        }
    }
    vectorMutex.unlock();

    return false;
}

void bioPacket::bioSuccessResponse(Client_info *client_info)
{
    bool isSuccess = getSuccessResponse(client_info);
    /*生物身份验证设备成功后直接开门*/
    if(isSuccess)
    {
        Socket* doorLockSocket= new Socket();
        doorLockSocket->socket_start_client(1);
    }

    uint16_t magic = 101;
    uint16_t size = sizeof(isSuccess);
    uint8_t type = 2;

    headerSend(client_info,magic,size,type);
    if(write(client_info->accept_fd,&isSuccess,sizeof(isSuccess))==-1)
    {
        perror("write");
        exit(1);
    }

}

bool bioPacket::getFailResponse(Client_info *client_info)
{
    uint16_t photoSize;
    if(read(client_info->accept_fd,&photoSize,sizeof(photoSize))==-1)
    {
         perror("read");
         return false;
    }
    memset(photo,0,photoSize);
    if(read(client_info->accept_fd,photo,photoSize)==-1)
    {
        perror("read");
        return false;
    }

    return true;
}

void bioPacket::bioFailResponse(Client_info *client_info)
{
    bool isSuccess = getFailResponse(client_info);
    uint16_t magic = 101;
    uint16_t size = sizeof(isSuccess);
    uint8_t type = 4;

    headerSend(client_info,magic,size,type);
    if(write(client_info->accept_fd,&isSuccess,sizeof(isSuccess))==-1)
    {
        perror("write");
        exit(1);
    }
}