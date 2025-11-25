#pragma once
#include<iostream>
#include<string.h>
#include<unistd.h>

#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/types.h>

//服务器设计，ip默认绑定"0.0.0.0"
std::string DefaultIP="0.0.0.0";


class UdpServer 
{
public:
    UdpServer(uint16_t port):sockfd_(0),ip_(DefaultIP),port_(port),isrunning_(false)
    {
    }

    void Init()
    {
        // 1. 获取套接字信息，设置ip类型，以及数据传输形式
        int fd = socket(AF_INET,SOCK_DGRAM,0);

        // 2. 创建本地结构体，绑定网络套接字端口号信息
        struct sockaddr_in local;
        bzero(&local,sizeof(local));
        local.sin_family=AF_INET;
        local.sin_port=htons(port_);
        //local.sin_addr.s_addr=INADDR_ANY; 
        inet_pton(AF_INET,ip_.c_str(),&local.sin_addr);

    }

    void Run()
    {
        isrunning_=true;
        char inbuffer[255];
        while(isrunning_)
        {
            struct sockaddr_in client;
            socklen_t len=sizeof(client);
            // sizeof(inbuffer)-1 的原因：
            // 1. inbuffer 数组大小为 255 字节
            // 2. 预留最后 1 个字节给字符串结束符 '\0'
            // 3. 这样最多接收 254 个字节，确保接收后可以安全地当作字符串使用
            ssize_t n= recvfrom(sockfd_,inbuffer,sizeof(inbuffer)-1,0,(sockaddr*)&client,&len);
            inbuffer[n]='\0';
            std::cout<<"UdpServer is running~"<<std::endl;
            sendto(sockfd_,inbuffer,n,0,(sockaddr*)&client,len);
        }


    }

    ~UdpServer(){}
private:
    int sockfd_;
    std::string ip_;
    uint16_t port_;
    bool isrunning_;
};