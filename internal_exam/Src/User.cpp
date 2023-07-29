//
// Created by keqiu on 2023/7/21.
//

#include "../Inc/User.h"
#include <cstring>
#include <vector>

std::vector<User>userList;
std::vector<Manager>managerList;

User::User()
{
    memset(userName,0, sizeof(userName));
    memset(passWord,0, sizeof(passWord));
    memset(realName,0, sizeof(realName));
    usertype = 0;
    seatNumber = 0;
    seatStatus = 0;
    garbageTime = 0;
    activityNumber = 0;
}

User::User(char *userName, char *passWord, char *realName, uint8_t usertype, uint64_t seatNumber,uint8_t seatStatus,uint64_t garbageTime,uint8_t activityNumber)
{
    strcpy(this->userName,userName);
    strcpy(this->passWord,passWord);
    strcpy(this->realName,realName);
    this->usertype = usertype;
    this->seatNumber = seatNumber;
    this->seatStatus = seatStatus;
    this->garbageTime = garbageTime;
    this->activityNumber = activityNumber;
}

User::User(char *userName, char *passWord, uint8_t usertype, char *realName)
{
    strcpy(this->userName,userName);
    strcpy(this->passWord,passWord);
    strcpy(this->realName,realName);
    this->usertype = usertype;
    this->seatNumber = 0;
    this->seatStatus = 0;
    this->garbageTime = 0;
    this->activityNumber = 0;
}

