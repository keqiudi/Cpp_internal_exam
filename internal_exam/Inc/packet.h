//
// Created by keqiu on 2023/7/20.
//

#ifndef INTERNAL_EXAM_PACKET_H
#define INTERNAL_EXAM_PACKET_H

#include<arpa/inet.h>
#include "Socket.h"
#include "User.h"

class packet {

private:
    struct
    {
      uint16_t magic;
      uint16_t size;
      uint8_t  type;
      uint8_t token[64];
    }header{};//包头


public:
    User tempUser;//储存当前登录用户的用户名和密码以及用户类型,用于判断是普通用户还是管理员

    packet();
    //包头获取函数
    bool getPacketHeader(Client_info* client_info);
    //包头发送函数
    void headerSend(Client_info* client_info,uint16_t magic,uint16_t size,uint8_t _type);
    //响应菜单
    void typeMenu(Client_info* client_info);
    //注册请求函数
    void getRegisterRequest(Client_info* client_info);
    void registerResponse(Client_info* client_info);
    //登录请求函数
    bool getLoginRequest(Client_info* client_info);
    void loginResponse(Client_info* client_info);
    bool checkPassword();
    //开门请求函数
    bool getOpenRequest(Client_info* client_info);
    void openResponse(Client_info* client_info);
    //查询座位请求
    uint64_t getSeatRequest(Client_info* client_info);
    void seatResponse(Client_info* client_info);
    //查询倒垃圾时间请求
    uint64_t getGarbageRequest(Client_info* client_info);
    void garbageResponse(Client_info* client_info);
    //查询活动证明请求
    uint8_t getActivityRequest(Client_info* client_info);
    void activityResponse(Client_info* client_info);
    //管理员添加用户请求
    bool getAddUserRequest(Client_info* client_info);
    void addUserResponse(Client_info* client_info);
    //管理员删除用户请求
    bool getDeleteUserRequest(Client_info* client_info);
    void deleteUserResponse(Client_info* client_info);
    //管理员重置用户密码请求
    bool getResetPasswordRequest(Client_info* client_info);
    void resetPasswordResponse(Client_info* client_info);
    //管理员绑定固定座位请求
    bool getBindSeatRequest(Client_info* client_info);
    void bindSeatResponse(Client_info* client_info);
    //管理员解绑固定座位请求
    bool getUnbindSeatRequest(Client_info* client_info);
    void unbindSeatResponse(Client_info* client_info);
    //管理员分配流动座位请求
    bool getAssignSeatRequest(Client_info* client_info);
    void assignSeatResponse(Client_info* client_info);
    uint64_t getFreeSeat();

    bool saveData();
    bool loadData();
};


#endif //INTERNAL_EXAM_PACKET_H
