//
// Created by keqiu on 2023/7/20.
//

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <vector>
#include "../Inc/packet.h"
#include <mutex>

using std::cout;

std::mutex vectorMutex;
extern std::vector<User>userList;
extern std::vector<Manager>managerList;

packet::packet()
{
    header.magic = 0;
    header.size = 0;
    header.type = 0;
    memset(header.token,0, sizeof(header.token));
}

bool packet::getPacketHeader(Client_info* client_info)
{
    /*接收处理包头*/
    while(true) {
        size_t len1,len2,len3,len4;
        len1= read(client_info->accept_fd, &header.magic, sizeof(header.magic));
        len2= read(client_info->accept_fd, &header.size, sizeof(header.size));
        len3= read(client_info->accept_fd, &header.type, sizeof(header.type));
        len4= read(client_info->accept_fd, header.token, sizeof(header.token));
        if(len1==-1||len2==-1||len3==-1||len4==-1)
        {
            perror("read");
            exit(1);
        }
        else if(len1==0||len2==0||len3==0||len4==0)
        {
            cout<<"客户端断开连接"<<std::endl;
            close(client_info->accept_fd);
            return false;
        }

        if(header.magic==114)
        typeMenu(client_info);
    }
}

void packet::typeMenu(Client_info *client_info)
{
        loadData();//加载数据
        switch (header.type)
        {
            case 1:
                getRegisterRequest(client_info);
                registerResponse(client_info);
                break;
            case 2:
                loginResponse(client_info);
                break;
            case 3:
                openResponse(client_info);
                break;
            case 4:
                seatResponse(client_info);
                break;
            case 5:
                garbageResponse(client_info);
                break;
            case 6:
                activityResponse(client_info);
                break;
            default:
                break;
        }
        /*管理员用户特有操作*/
        if(tempUser.getUserType()==1)
        {
            switch (header.type)
            {
                case 7:
                    addUserResponse(client_info);
                    break;
                case 8:
                    deleteUserResponse(client_info);
                    break;
                case 9:
                    resetPasswordResponse(client_info);
                    break;
                case 10:
                    bindSeatResponse(client_info);
                    break;
                case 11:
                    unbindSeatResponse(client_info);
                    break;
                case 12:
                    assignSeatResponse(client_info);
                    break;
            }
        }
}

void packet::headerSend(Client_info *client_info,uint16_t magic,uint16_t size,uint8_t _type)
{
    if(write(client_info->accept_fd,&magic,sizeof(magic))==-1)
    {
        perror("write");
    }

    if(write(client_info->accept_fd,&size,sizeof(size))==-1)
    {
        perror("write");
    }

    if(write(client_info->accept_fd,&_type,sizeof(_type))==-1)
    {
        perror("write");
    }

    if(write(client_info->accept_fd,header.token,sizeof(header.token))==-1)
    {
        perror("write");
    }

}

void packet::getRegisterRequest(Client_info *client_info)
{
    uint16_t userNameSize,passwordSize,realNameSize;
    uint8_t userType;
    char userName[64];
    char password[64];
    char realName[64];


    if(read(client_info->accept_fd,&userNameSize,sizeof(userNameSize))==-1)
    {
        perror("read");
        exit(1);
    }
    //为userName设置userNameSize大小的空间
    memset(userName,0,userNameSize);
    if(read(client_info->accept_fd,userName,userNameSize)==-1)
    {
        perror("read");
        exit(1);
    }
    if(read(client_info->accept_fd,&passwordSize,sizeof(passwordSize))==-1)
    {
        perror("read");
        exit(1);
    }
    memset(password,0,passwordSize);
    if(read(client_info->accept_fd,password,passwordSize)==-1)
    {
        perror("read");
        exit(1);
    }
    if(read(client_info->accept_fd,&userType,sizeof(userType))==-1)
    {
        perror("read");
        exit(1);
    }
    if(read(client_info->accept_fd,&realNameSize,sizeof(realNameSize))==-1)
    {
        perror("read");
        exit(1);
    }
    memset(realName,0,realNameSize);
    if(read(client_info->accept_fd,realName,realNameSize)==-1)
    {
        perror("read");
        exit(1);
    }

    //将用户信息写入数据库
    if(userType==1)
    {
        vectorMutex.lock();
        managerList.emplace_back(userName,password,userType,realName);
        vectorMutex.unlock();
    }
    else
    {
        vectorMutex.lock();
        userList.emplace_back(userName,password,userType,realName);
        vectorMutex.unlock();
    }

}

void packet::registerResponse(Client_info *client_info)
{
    bool isRegister = saveData();
    /*是否注册成功*/
    uint16_t magic = 114;
    uint16_t size = sizeof(bool);
    uint8_t _type = 13;

    headerSend(client_info,magic,size,_type);
    /*发送包头*/
    if(write(client_info->accept_fd,&isRegister,sizeof(isRegister))==-1)
    {
        perror("write");
    }
    /*发送包体*/
}

bool packet::getLoginRequest(Client_info *client_info)
{

    uint16_t userNameSize,passwordSize;

    if(read(client_info->accept_fd,&userNameSize,sizeof(userNameSize))==-1)
    {
        perror("read");
        exit(1);
    }
    memset(tempUser.getUserName(),0,userNameSize);
    if(read(client_info->accept_fd,tempUser.getUserName(),userNameSize)==-1)
    {
        perror("read");
        exit(1);
    }

    if(read(client_info->accept_fd,&passwordSize,sizeof(passwordSize))==-1)
    {
        perror("read");
        exit(1);
    }
    memset(tempUser.getUserPassword(),0,passwordSize);
    if(read(client_info->accept_fd,tempUser.getUserPassword(),passwordSize)==-1)
    {
        perror("read");
        exit(1);
    }

    /*接收包体*/
    return checkPassword();
}

bool packet::checkPassword()
{
    vectorMutex.lock();
    for(auto & i : userList)
    {
        if(strcmp(tempUser.getUserName(),i.getUserName())==0)
        {
            if(strcmp(tempUser.getUserPassword(),i.getUserPassword())==0)
            {
                tempUser.setUserType(i.getUserType());
                vectorMutex.unlock();
                return true;
            }
            else
            {
                vectorMutex.unlock();
                return false;
            }
        }
    }

    for(auto & i : managerList)
    {
        if(strcmp(tempUser.getUserName(),i.getUserName())==0)
        {
            if(strcmp(tempUser.getUserPassword(),i.getUserPassword())==0)
            {
                tempUser.setUserType(i.getUserType());
                vectorMutex.unlock();
                return true;
            }
            else
            {
                vectorMutex.unlock();
                return false;
            }
        }
    }

    vectorMutex.unlock();
    return false;
}

void packet::loginResponse(Client_info *client_info)
{
       bool isLogin = getLoginRequest(client_info);
        /*是否登录成功*/
        uint16_t magic = 114;
        uint16_t size = sizeof(bool)+sizeof(header.token);
        uint8_t _type = 14;

        headerSend(client_info,magic,size,_type);
        /*发送包头*/
        if(write(client_info->accept_fd,&isLogin,sizeof(isLogin))==-1)
        {
            perror("write");
        }
        if(write(client_info->accept_fd,&header.token,sizeof(header.token))==-1)
        {
            perror("write");
        }
        /*发送包体*/
}

bool packet::getOpenRequest(Client_info *client_info)
{
    bool isOpen;
    if(read(client_info->accept_fd,&isOpen,sizeof(isOpen))==-1)
    {
        perror("read");
        exit(1);
    }

    return isOpen;
}

void packet::openResponse(Client_info *client_info)
{
    bool isOpen = getOpenRequest(client_info);
    /*是否开门成功*/

    uint16_t magic = 114;
    uint16_t size = sizeof(bool);
    uint8_t _type = 15;

    headerSend(client_info,magic,size,_type);
    /*发送包头*/
    if(write(client_info->accept_fd,&isOpen,sizeof(isOpen))==-1)
    {
        perror("write");
    }
}

uint64_t packet::getSeatRequest(Client_info *client_info)
{
    char userName[64];
    uint16_t userNameSize;

    if(read(client_info->accept_fd,&userNameSize,sizeof(userNameSize))==-1)
    {
        perror("read");
        exit(1);
    }
    memset(userName,0,userNameSize);
    if(read(client_info->accept_fd,userName,userNameSize)==-1)
    {
        perror("read");
        exit(1);
    }
    /*接收包体*/

    vectorMutex.lock();
    for(auto &i:userList)
    {
        if(strcmp(userName,i.getUserName())==0)
        {
            vectorMutex.unlock();
            return i.getUserSeatNumber();
        }
    }

    for(auto &i:managerList)
    {
        if(strcmp(userName,i.getUserName())==0)
        {
            vectorMutex.unlock();
            return i.getUserSeatNumber();
        }
    }
    /*查询座位号,查询失败返回0*/
    vectorMutex.unlock();
    return 0;
}

void packet::seatResponse(Client_info *client_info)
{
    bool isQuery;
    /*获取座位号*/
    uint64_t seatNumber = getSeatRequest(client_info);
    uint8_t seatStatus;
    uint16_t magic = 114;
    uint16_t size = sizeof(uint64_t)+sizeof(uint8_t)+sizeof(bool);
    uint8_t _type = 16;

    if(seatNumber)
    {
        isQuery = true;
        headerSend(client_info,magic,size,_type);
        seatStatus = 1;
        if(write(client_info->accept_fd,&isQuery,sizeof(isQuery))==-1)
        {
            perror("write");
        }
        if(write(client_info->accept_fd,&seatStatus,sizeof(seatStatus))==-1)
        {
            perror("write");
        }
        if(write(client_info->accept_fd,&seatNumber,sizeof(seatNumber))==-1)
        {
            perror("write");
        }
    }
    else
    {
        isQuery = false;
        headerSend(client_info,magic,size,_type);
        seatStatus = 0;
        if(write(client_info->accept_fd,&isQuery,sizeof(isQuery))==-1)
        {
            perror("write");
        }
        if(write(client_info->accept_fd,&seatStatus,sizeof(seatStatus))==-1)
        {
            perror("write");
        }
        if(write(client_info->accept_fd,&seatNumber,sizeof(seatNumber))==-1)
        {
            perror("write");
        }
    }
}

uint64_t packet::getGarbageRequest(Client_info *client_info)
{
        char userName[64];
        uint16_t userNameSize;

        if(read(client_info->accept_fd,&userNameSize,sizeof(userNameSize))==-1)
        {
            perror("read");
            exit(1);
        }
        memset(userName,0,userNameSize);
        if(read(client_info->accept_fd,userName,userNameSize)==-1)
        {
            perror("read");
            exit(1);
        }
        /*接收包体*/

        vectorMutex.lock();
        for(auto &i:userList)
        {
            if(strcmp(userName,i.getUserName())==0)
            {
                vectorMutex.unlock();
                return i.getUserGarbageTime();
            }
        }

        for(auto &i:managerList)
        {
            if(strcmp(userName,i.getUserName())==0)
            {
                vectorMutex.unlock();
                return i.getUserGarbageTime();
            }
        }
        /*查询倒垃圾时间,查询失败返回0*/
        vectorMutex.unlock();
        return 0;
}

void packet::garbageResponse(Client_info *client_info)
{
    /*获取倒垃圾时间*/
    bool isQuery;
    uint64_t garbageTime = getGarbageRequest(client_info);
    uint16_t magic = 114;
    uint16_t size = sizeof(uint64_t)+sizeof(bool);
    uint8_t _type = 17;

    if(garbageTime)
    {
        isQuery = true;
        headerSend(client_info,magic,size,_type);
        if(write(client_info->accept_fd,&isQuery,sizeof(isQuery))==-1)
        {
            perror("write");
        }
        if(write(client_info->accept_fd,&garbageTime,sizeof(garbageTime))==-1)
        {
            perror("write");
        }
    }
    else
    {
        isQuery = false;
        headerSend(client_info,magic,size,_type);
        if(write(client_info->accept_fd,&isQuery,sizeof(isQuery))==-1)
        {
            perror("write");
        }
        if(write(client_info->accept_fd,&garbageTime,sizeof(garbageTime))==-1)
        {
            perror("write");
        }
    }
}

uint8_t packet::getActivityRequest(Client_info *client_info)
{
   char userName[64];
   uint16_t userNameSize;

   if(read(client_info->accept_fd,&userNameSize,sizeof(userNameSize))==-1)
   {
       perror("read");
       exit(1);
   }
   if(read(client_info->accept_fd,userName,userNameSize)==-1)
   {
       perror("read");
       exit(1);
   }
   /*接受包体*/
   vectorMutex.lock();
    for(auto &i:userList)
    {
         if(strcmp(userName,i.getUserName())==0)
         {
             vectorMutex.unlock();
              return i.getUserActivityNumber();
         }
    }

    for(auto &i:managerList)
    {
        if(strcmp(userName,i.getUserName())==0)
        {
            vectorMutex.unlock();
            return i.getUserActivityNumber();
        }
    }
    /*查询活动次数,查询失败返回0*/
    vectorMutex.unlock();
    return 0;
}

void packet::activityResponse(Client_info *client_info)
{
    bool isQuery;
    uint8_t activityNumber = getActivityRequest(client_info);
    uint16_t activityAddressSize[activityNumber];
    uint16_t magic = 114;
    uint16_t size = sizeof(activityNumber)+sizeof(bool)+sizeof(activityNumber)*2;
    uint8_t _type = 18;

    if(activityNumber)
    {
        isQuery = true;
        headerSend(client_info,magic,size,_type);
        if(write(client_info->accept_fd,&isQuery,sizeof(isQuery))==-1)
        {
            perror("write");
        }
        if(write(client_info->accept_fd,&activityNumber,sizeof(activityNumber))==-1)
        {
            perror("write");
        }
        if(write(client_info->accept_fd,activityAddressSize,sizeof(activityAddressSize))==-1)
        {
            perror("write");
        }
    }
    else
    {
        isQuery=false;
        headerSend(client_info,magic,size,_type);
        if(write(client_info->accept_fd,&isQuery,sizeof(isQuery))==-1)
        {
            perror("write");
        }
        if(write(client_info->accept_fd,&activityNumber,sizeof(activityNumber))==-1)
        {
            perror("write");
        }
        if (write(client_info->accept_fd,activityAddressSize,sizeof(activityAddressSize))==-1)
        {
            perror("write");
        }
    }

}

bool packet::getAddUserRequest(Client_info *client_info)
{
    uint16_t userNameSize,passwordSize,realNameSize;
    uint8_t userType;
    char userName[64];
    char password[64];
    char realName[64];

    if(read(client_info->accept_fd,&userNameSize,sizeof(userNameSize))==-1)
    {
        perror("read");
        exit(1);
    }
    //为userName设置userNameSize大小的空间
    memset(userName,0,userNameSize);
    if(read(client_info->accept_fd,userName,userNameSize)==-1)
    {
        perror("read");
        exit(1);
    }
    if(read(client_info->accept_fd,&passwordSize,sizeof(passwordSize))==-1)
    {
        perror("read");
        exit(1);
    }
    memset(password,0,passwordSize);
    if(read(client_info->accept_fd,password,passwordSize)==-1)
    {
        perror("read");
        exit(1);
    }
    if(read(client_info->accept_fd,&userType,sizeof(userType))==-1)
    {
        perror("read");
        exit(1);
    }
    if(read(client_info->accept_fd,&realNameSize,sizeof(realNameSize))==-1)
    {
        perror("read");
        exit(1);
    }
    memset(realName,0,realNameSize);
    if(read(client_info->accept_fd,realName,realNameSize)==-1)
    {
        perror("read");
        exit(1);
    }

    //将用户信息写入数据库
    if(userType==1)
    {
        vectorMutex.lock();
        managerList.emplace_back(userName,password,userType,realName);
        vectorMutex.unlock();
    }
    else
    {
        vectorMutex.lock();
        userList.emplace_back(userName,password,userType,realName);
        vectorMutex.unlock();
    }

    return saveData();
}

void packet::addUserResponse(Client_info *client_info)
{
    bool isAddUser = getAddUserRequest(client_info);
    uint16_t magic = 114;
    uint16_t size = sizeof(isAddUser);
    uint8_t _type = 19;

    headerSend(client_info,magic,size,_type);
    if(write(client_info->accept_fd, &isAddUser, sizeof(isAddUser)) == -1)
    {
        perror("write");
    }
}

bool packet::getDeleteUserRequest(Client_info *client_info)
{
   char userName[64];
   uint16_t userNameSize;

   if(read(client_info->accept_fd,&userNameSize,sizeof(userNameSize))==-1)
   {
       perror("read");
       exit(1);
   }
   memset(userName,0,userNameSize);
   if(read(client_info->accept_fd,userName,userNameSize)==-1)
    {
        perror("read");
        exit(1);
    }

    vectorMutex.lock();
    for(int i=0;i<userList.size();i++)
    {
        if(strcmp(userName,userList[i].getUserName())==0)
        {
            userList.erase(userList.begin()+i-1);//删除要求的用户
        }
    }

    for(int i=0;i<managerList.size();i++)
    {
        if(strcmp(userName,managerList[i].getUserName())==0)
        {
            managerList.erase(managerList.begin()+i-1);
        }
    }
    vectorMutex.unlock();

    return saveData();

}

void packet::deleteUserResponse(Client_info *client_info)
{
    bool isDeleteUser = getDeleteUserRequest(client_info);
    uint16_t magic = 114;
    uint16_t size = sizeof(isDeleteUser);
    uint8_t _type = 20;

    headerSend(client_info,magic,size,_type);
    if(write(client_info->accept_fd,&isDeleteUser,sizeof(isDeleteUser))==-1)
    {
        perror("write");
    }

}

bool packet::getResetPasswordRequest(Client_info *client_info)
{
   char userName[64];
   uint16_t userNameSize;
   char password[64] = {'1','2','3','4','5','6','\0'};
   if(read(client_info->accept_fd,&userNameSize,sizeof(userNameSize))==-1)
   {
       perror("read");
       exit(1);
   }
   memset(userName,0,userNameSize);
   if(read(client_info->accept_fd,userName,userNameSize))
   {
       perror("read");
       exit(1);
   }

    vectorMutex.lock();
    for(auto &i : userList)
    {
        if(strcmp(userName,i.getUserName())==0)
        {
            //把密码重置为123456
            i.setUserPassword(password);
        }
    }
    for(auto &i : managerList)
    {
        if(strcmp(userName,i.getUserName())==0)
        {
            i.setUserPassword(password);
        }
    }
    vectorMutex.unlock();

    return saveData();
}

void packet::resetPasswordResponse(Client_info *client_info)
{
    bool isResetPassword = getResetPasswordRequest(client_info);
    uint16_t magic = 114;
    uint16_t size = sizeof(isResetPassword);
    uint8_t _type = 21;

    headerSend(client_info,magic,size,_type);
    if(write(client_info->accept_fd,&isResetPassword,sizeof(isResetPassword))==-1)
    {
        perror("write");
    }
}

bool packet::getBindSeatRequest(Client_info *client_info)
{
    char userName[64];
    uint16_t userNameSize;
    uint64_t seatNumber;

    if(read(client_info->accept_fd,&userNameSize,sizeof(userNameSize))==-1)
    {
        perror("read");
        exit(1);
    }
    memset(userName,0,userNameSize);
    if(read(client_info->accept_fd,userName,userNameSize)==-1)
    {
        perror("read");
        exit(1);
    }
    if(read(client_info->accept_fd,&seatNumber,sizeof(seatNumber))==-1)
    {
        perror("read");
        exit(1);
    }

    vectorMutex.lock();
    for(auto &i : userList)
    {
        if(strcmp(userName,i.getUserName())==0)
        {
            i.setUserSeatNumber(seatNumber);
            i.setUserSeatStatus(1);
        }
    }

    for(auto &i : managerList)
    {
        if(strcmp(userName,i.getUserName())==0)
        {
            i.setUserSeatNumber(seatNumber);
            i.setUserSeatStatus(1);
        }
    }
    vectorMutex.unlock();

    return saveData();
}

void packet::bindSeatResponse(Client_info *client_info)
{
    bool isBindSeat = getBindSeatRequest(client_info);
    uint16_t magic = 114;
    uint16_t size = sizeof(isBindSeat);
    uint8_t _type = 22;

    headerSend(client_info,magic,size,_type);
    if(write(client_info->accept_fd,&isBindSeat,sizeof(isBindSeat))==-1)
    {
        perror("write");
    }
}

bool packet::getUnbindSeatRequest(Client_info *client_info)
{
    char userName[64];
    uint16_t userNameSize;

    if(read(client_info->accept_fd,&userNameSize,sizeof(userNameSize))==-1)
    {
        perror("read");
        exit(1);
    }
    memset(userName,0,userNameSize);
    if(read(client_info->accept_fd,userName,userNameSize)==-1)
    {
        perror("read");
        exit(1);
    }

    vectorMutex.lock();
    for(auto &i:userList)
    {
        if(strcmp(userName,i.getUserName())==0)
        {
            i.setUserSeatNumber(0);
            i.setUserSeatStatus(0);
        }
    }
    for(auto &i:managerList)
    {
        if(strcmp(userName,i.getUserName())==0)
        {
            i.setUserSeatNumber(0);
            i.setUserSeatStatus(0);
        }
    }
    vectorMutex.unlock();

    return saveData();
}

void packet::unbindSeatResponse(Client_info *client_info)
{
    bool isUnbindSeat = getUnbindSeatRequest(client_info);
    uint16_t magic = 114;
    uint16_t size = sizeof(isUnbindSeat);
    uint8_t _type = 23;

    headerSend(client_info,magic,size,_type);
    if(write(client_info->accept_fd,&isUnbindSeat,sizeof(isUnbindSeat))==-1)
    {
        perror("write");
    }

}

uint64_t packet::getFreeSeat()
{
    uint64_t seatNumber = 0;
    std::vector<uint64_t> seatList;
    /*获得已经占用的流动座位*/
    for(auto &i : userList)
    {
        if(i.getUserSeatStatus()==2)
        {
           seatList.push_back(i.getUserSeatNumber());
        }
    }
    for(auto &i : managerList)
    {
        if(i.getUserSeatStatus()==2)
        {
            seatList.push_back(i.getUserSeatNumber());
        }
    }
   /*寻找未占用的流动座位*/
    for(uint64_t i=1;i<=10;i++)
    {
        bool isFree = true;//第i个座位状态
        for(auto &j : seatList)
        {
            if(i==j)
            {
                isFree = false;
                break;
            }
        }
        for(auto &j : seatList)
        {
            if(i==j)
            {
                isFree = false;
                break;
            }
        }
        if(isFree)
        {
            seatNumber = i;
            break;
        }
    }
    return seatNumber;//得到分配的流动座位

}

bool packet::getAssignSeatRequest(Client_info *client_info)
{
    char userName[64];
    uint16_t userNameSize;

    if(read(client_info->accept_fd,&userNameSize,sizeof(userNameSize))==-1)
    {
        perror("read");
        exit(1);
    }
    memset(userName,0,userNameSize);
    if(read(client_info->accept_fd,userName,userNameSize)==-1)
    {
        perror("read");
        exit(1);
    }

    vectorMutex.lock();
    uint64_t seat = getFreeSeat();
    for(auto &i:userList)
    {
        if(strcmp(userName,i.getUserName())==0)
        {
            i.setUserSeatNumber(seat);
            i.setUserSeatStatus(2);
        }
    }
    for(auto &i:managerList)
    {
        if(strcmp(userName,i.getUserName())==0)
        {
            i.setUserSeatNumber(seat);
            i.setUserSeatStatus(2);
        }
    }
    vectorMutex.unlock();

    return saveData();
}

void packet::assignSeatResponse(Client_info *client_info)
{
   bool isSuccess = getAssignSeatRequest(client_info);
    uint16_t magic = 114;
    uint16_t size = sizeof(isSuccess);
    uint8_t _type = 24;

    headerSend(client_info,magic,size,_type);
    if(write(client_info->accept_fd,&isSuccess,sizeof(isSuccess))==-1)
    {
        perror("write");
    }
}

bool packet::saveData()
{

    std::ofstream userOutFile,managerOutFile;
    userOutFile.open("user_data.txt", std::ios::out);//以写入的方式打开文件
    managerOutFile.open("manager_data.txt", std::ios::out);//以写入的方式打开文件

    if(!userOutFile.is_open()||!managerOutFile.is_open())
    {
        cout << "文件打开失败" << std::endl;
        return false;
    }

    vectorMutex.lock();
    for(auto i:userList)
    {
        userOutFile << i.getUserName() << " " << i.getUserPassword() << " " << i.getUserType() << " " << i.getUserRealName() << "" << i.getUserSeatNumber() << " " << i.getUserSeatStatus() << " " << i.getUserGarbageTime() << " " << i.getUserActivityNumber() << std::endl;
    }//将用户信息写入文件

    for(auto i:managerList)
    {
        managerOutFile << i.getUserName() << " " << i.getUserPassword() << " " << i.getUserType() << " " << i.getUserRealName() << "" << i.getUserSeatNumber() << " " << i.getUserSeatStatus() << " " << i.getUserGarbageTime() << " " << i.getUserActivityNumber() << std::endl;
    }//将管理员信息写入文件
    vectorMutex.unlock();

    userOutFile.close();
    managerOutFile.close();

    return true;
}

bool packet::loadData()
{
    std::ifstream userInFile,managerInFile;
    userInFile.open("user_data.txt", std::ios::in);
    managerInFile.open("manager_data.txt", std::ios::in);

    if(!userInFile.is_open()||!managerInFile.is_open())
    {
        cout << "文件打开失败" << std::endl;
        return false;
    }

    vectorMutex.lock();
    while(!userInFile.eof())
    {
        uint8_t userType;
        uint64_t seatNumber;
        uint8_t seatStatus;
        uint64_t garbageTime;
        uint8_t activityNumber;
        char userName[64];
        char password[64];
        char realName[64];
        userInFile >> userName >> password >> userType >> realName >> seatNumber >> seatStatus >> garbageTime >> activityNumber;
        userList.emplace_back(userName,password,realName,userType,seatNumber,seatStatus,garbageTime,activityNumber);
    }

    while(!managerInFile.eof())
    {
        uint8_t userType;
        uint64_t seatNumber;
        uint8_t seatStatus;
        uint64_t garbageTime;
        uint8_t activityNumber;
        char userName[64];
        char password[64];
        char realName[64];
        managerInFile >> userName >> password >> userType >> realName >> seatNumber >> seatStatus >> garbageTime >> activityNumber;
        managerList.emplace_back(userName,password,realName,userType,seatNumber,seatStatus,garbageTime,activityNumber);
    }
    vectorMutex.unlock();

    userInFile.close();
    managerInFile.close();
    return true;
}