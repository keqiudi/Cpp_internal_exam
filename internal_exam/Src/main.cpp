#include <iostream>

#include <arpa/inet.h>
#include "../Inc/Socket.h"
#include "../Inc/threadPool.h"
#include "../Inc/packet.h"
#include "../Inc/bioPacket.h"
#include <pthread.h>
using std::cout;

void accept_client(void * arg);
void working(void* arg);
void bioAccept_client(void* arg);
void bioWorking(void* arg);

threadPool pool(10,100);

int main() {

        Socket* socket=new Socket();//
        Socket* bioSocket = new Socket();//生物身份验证设备的socket
//        Socket* doorLockSocket= new Socket();//门锁的socket
//
//        doorLockSocket->socket_start_client(0); //客户端

        socket->socket_start(9999);//服务端
        bioSocket->socket_start(8888);//生物身份验证设备服务端,不同的端口号客户端就可以连接到不同的服务端

        pool.addTask(Task(accept_client,socket));//用于检测连接的线程
        pool.addTask(Task(bioAccept_client,bioSocket));//用于检测生物身份验证设备连接的线程
        /*添加任务到线程池*/

        while(true)
        {

        }
        /*防止父线程结束*/

//        pthread_exit(nullptr);
//        return 0;
}



void accept_client(void * arg)
{
    auto socket = static_cast<Socket*>(arg);
    socklen_t len = sizeof(struct sockaddr_in);
    while (true)
    {
        Client_info* client_info = new Client_info;
        client_info->accept_fd = accept(socket->getSocketFd(),(struct sockaddr*)&client_info->client_addr,&len);
        if(client_info->accept_fd==-1)
        {
            perror("accept");
            exit(1);
        }
        cout<<"客户端连接成功,IP：" << inet_ntoa(client_info->client_addr.sin_addr) << "端口：" << ntohs(client_info->client_addr.sin_port) << std::endl;
        pool.addTask(Task(working, client_info));//添加任务
    }

    delete socket;
}//用于检测连接的线程


void working(void* arg)
{
    auto client_info = static_cast<Client_info*>(arg);
    packet packet;

    while(packet.getPacketHeader(client_info))
    {

    }

}//其他用于处理客户端请求的线程

void bioAccept_client(void* arg)
{

    auto socket = static_cast<Socket*>(arg);
    socklen_t len = sizeof(struct sockaddr_in);
    while (true)
    {
        Client_info* client_info = new Client_info;
        client_info->accept_fd = accept(socket->getSocketFd(),(struct sockaddr*)&client_info->client_addr,&len);
        if(client_info->accept_fd==-1)
        {
            perror("accept");
            exit(1);
        }
        cout<<"生物身份验证设备:客户端连接成功,IP：" << inet_ntoa(client_info->client_addr.sin_addr) << "端口：" << ntohs(client_info->client_addr.sin_port) << std::endl;
        pool.addTask(Task(bioWorking, client_info));//添加任务
    }

    delete socket;
}//检测生物身份验证设备的连接的线程

void bioWorking(void* arg)
{
    auto client_info = static_cast<Client_info*>(arg);
     bioPacket bioPacket;

    while(bioPacket.getPacketHeader(client_info))
    {

    }

}//其他用于处理生物身份验证设备请求的线程