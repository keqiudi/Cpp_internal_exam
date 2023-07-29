//
// Created by keqiu on 2023/7/26.
//

#ifndef INTERNAL_EXAM_BIOPACKET_H
#define INTERNAL_EXAM_BIOPACKET_H

#include "Socket.h"
#include <cstdint>

class bioPacket {

private:
    struct
    {
        uint16_t magic;
        uint16_t size;
        uint8_t  type;
        uint8_t token[16];
    }header{};//包头

public:
    uint8_t photo[64]{};//失败者用户照片
    //包头获取函数
    bool getPacketHeader(Client_info* client_info);
    //生物身份验证菜单
    void bioTypeMenu(Client_info* client_info);
    //身份验证成功响应
    bool getSuccessResponse(Client_info* client_info);
    void bioSuccessResponse(Client_info* client_info);
    //身份验证失败响应
    bool getFailResponse(Client_info* client_info);
    void bioFailResponse(Client_info* client_info);
    //包头发送函数
    void headerSend(Client_info* client_info,uint16_t magic,uint16_t size,uint8_t _type);
};


#endif //INTERNAL_EXAM_BIOPACKET_H
