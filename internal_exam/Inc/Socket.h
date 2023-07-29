//
// Created by keqiu on 2023/7/20.
//

#ifndef INTERNAL_EXAM_SOCKET_H
#define INTERNAL_EXAM_SOCKET_H


#include <arpa/inet.h>


typedef struct Client_info
{
    int accept_fd;
    struct sockaddr_in client_addr;
}Client_info;


class Socket {
    private:
         struct sockaddr_in server_addr;//储存本机的ip和端口
         struct sockaddr_in client_addr;//储存客户端的ip和端口
         int socket_fd;
    public:
        Socket ()=default;

        void socket_start(int sin_port);

        void socket_start_client(uint8_t _type);

        [[nodiscard]] inline int getSocketFd() const
        {
            return socket_fd;
        }

};


#endif //INTERNAL_EXAM_SOCKET_H
