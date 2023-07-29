//
// Created by keqiu on 2023/7/20.
//

#include "../Inc/Socket.h"
#include <arpa/inet.h>
#include <iostream>
#include "../Inc/doorLockMenu.h"

void Socket::socket_start(int sin_port)
{
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);//创建socket
    if(socket_fd==-1)
    {
        perror("socket");
        exit(1);
    }

    server_addr.sin_family = AF_INET;//设置地址族
    server_addr.sin_port = htons(sin_port);//设置端口
    server_addr.sin_addr.s_addr = INADDR_ANY;//设置IP地址
    //设置端口复用

    int opt = 1;
    if(setsockopt(socket_fd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt)))
    {
        perror("setSockOpt");
        exit(1);
    }

    if(bind(socket_fd,(struct sockaddr*)&server_addr, sizeof(server_addr))==-1)//绑定
    {
        perror("bind");
        exit(1);
    }

    if(listen(socket_fd,128)==-1)//监听
    {
        perror("listen");
        exit(1);
    }

}

void Socket::socket_start_client(uint8_t _type)
{

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);//创建socket
    if(socket_fd==-1)
    {
        perror("socket");
        exit(1);
    }

    char ip[16];
    int sin_port;
    std::cout<<"请输入服务器的ip地址：";
    std::cin>>ip;
    std::cout<<"请输入服务器的端口号：";
    std::cin>>sin_port;

    client_addr.sin_family = AF_INET;//设置地址族
    client_addr.sin_port = htons(sin_port);//设置连接端口
    inet_pton(AF_INET, ip, &client_addr.sin_addr.s_addr);//设置通信的ip地址

    if(connect(socket_fd,(struct sockaddr*)&client_addr, sizeof(client_addr))==-1)//连接
    {
        perror("connect");
        exit(1);
    }

    doorLock* door_lock = new doorLock();
    while(true)
    {
        uint8_t type = _type;

        if(type == 1)
        {
            requestMenu(type, door_lock, socket_fd);//直接发送开门请求,用于生物验证设备
            break;
        }
        else
        {
            showMenu();
            type = getType();
            if (type == 4)
            {
                break;
            }
            requestMenu(type, door_lock, socket_fd);//正常的客户端发送请求
        }
    }
    delete door_lock;
}

