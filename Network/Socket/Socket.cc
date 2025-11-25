#include <iostream>
#include<unistd.h>
#include <string.h>

#include <arpa/inet.h>  //字节序转换(ip，port)
#include <sys/socket.h> //网络套接字接口
#include <sys/types.h>  //一些基本类型sockaddr_t,socket.h也有定义

int main()
{
    // 1.socket获取套接字文件描述符
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    // 2. 设置套接字选项（可选，见下文）
    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 3.创建结构体，bind绑定到文件描述符
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    bind(listen_fd, (const struct sockaddr *)&server_addr, sizeof(server_addr));

    // 4.listen监听套接字
    listen(listen_fd, 5);

    // 5.accept接受链接
    while (1)
    {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_len);

        // 6. 处理客户端请求（多线程/多进程/IO多路复用）
        // recv() / send() 与客户端通信

        // 7. 关闭客户端连接
        close(client_fd);
    }
    close(listen_fd);

    return 0;
}