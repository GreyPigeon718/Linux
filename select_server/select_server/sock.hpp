#pragma once

#include <iostream>
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace ns_sock
{
    enum
    {
        SOCKET_ERR = 2,
        BIND_ERR,
        LISTEN_ERR
    };

    const int g_backlog = 5;

    class Sock
    {
    public:
        static int Socket()
        {
            //> int socket(int domain, int type, int protocol)
            //> domain:表示套接字使用的协议簇AF_INET（TCP/IP-IPv4）
            //> type:套接字类型 SOCK_STREAM（TCP流）
            //> protocol:一般设置为“0”，就是说在已经确定套接字使用的协议簇和类型时，这个参数的值就为0。但是有时候创建初始套接字时，在domain参数未知情况下，即不清楚协议簇和类型时，protocol参数可以用来确定协议的种类。
            //> 返回值：当套接字创建成功时，返回套接字，失败返回“-1”，错误代码则写入“errno”中。
            int sock = socket(AF_INET, SOCK_STREAM, 0);
            if (sock < 0)
            {
                std::cerr << "socket error!" << std::endl;
                exit(SOCKET_ERR);
            }
            int opt = 1;
            //> int setsockopt( int socket, int level, int option_name,const void *option_value, size_t ，ption_len);
            //> int sockfd : 套接字描述符
            //> int level : 选项定义的层次；目前仅支持SOL_SOCKET和IPPROTO_TCP层次
            //> int optname : 需设置的选项 SO_REUSEADDR:打开或关闭地址复用功能
            //> const void* optval : 指针，指向存放选项值的缓冲区
            //> socklen_t optlen : optval缓冲区的长度
            setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
            return sock;
        }

        static void Bind(const int& sock, const u_int16_t& port)
        {
            struct sockaddr_in local;
            bzero(&local, sizeof(local));
            local.sin_family = AF_INET;
            local.sin_port = htons(port);
            local.sin_addr.s_addr = INADDR_ANY;

            if (bind(sock, (struct sockaddr*)&local, sizeof(local)) < 0)
            {
                std::cerr << "bind error!" << std::endl;
                exit(BIND_ERR);
            }
        }

        static void Listen(const int& sock)
        {
            if (listen(sock, g_backlog) < 0)
            {
                std::cerr << "bind error!" << std::endl;
                exit(LISTEN_ERR);
            }
        }
    };
}