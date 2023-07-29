//
// Created by keqiu on 2023/7/28.
//


#include "../Inc/doorLockMenu.h"
#include <iostream>
using std::cout;
using std::endl;
using std::cin;

void showMenu()
{
    cout<<"1.开门请求"<<endl;
    cout<<"2.锁门请求"<<endl;
    cout<<"3.查询门锁状态请求"<<endl;
    cout<<"4.退出"<<endl;
}

uint8_t getType()
{
    uint8_t type;
    cout<<"请输入请求类型：";

    while(true)
    {
        cin >> type;
        if(type<=4&&type>=1)
            break;
        else
            cout<<"输入错误，请重新输入：";
    }

    return type;
}

void requestMenu(uint8_t type,doorLock* door_lock,int& socket_fd)
{

    switch(type)
    {
        case 1:
            door_lock->openDoorRequest(socket_fd);
            door_lock->getOpenDoorResponse(socket_fd);
            break;
        case 2:
            door_lock->closeDoorRequest(socket_fd);
            door_lock->getCloseDoorResponse(socket_fd);
            break;
        case 3:
            door_lock->doorStateRequest(socket_fd);
            door_lock->getDoorStateResponse(socket_fd);
            break;
        default:
            break;
    }

}

