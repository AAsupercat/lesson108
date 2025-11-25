#include<iostream>
#include<string>
#include "UdpServer.hpp"



int main(int argc,char* argv[])
{

    if(argc!=2)
    {
        std::cout<<"You should : proc + port"<<std::endl;
    }
    uint16_t port=std::stoi(argv[1]);
    UdpServer server(port);
    server.Init();
    server.Run();

    return 0;
}