#pragma once
#pragma once

#include "sock.hpp"
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>

namespace ns_select
{
    using namespace ns_sock;

#define NUM (sizeof(fd_set) * 8)

    const int g_default = 8080;

    // class EndPoint{
    //     int fd;
    //     std::string buffer;
    // };

    class SelectServer
    {
    private:
        u_int16_t port_;
        int listen_sock_;

        int fd_arrar_[NUM];

        // EndPoint fd_array_[NUM];

    public:
        SelectServer(int port = g_default) : port_(port), listen_sock_(-1)
        {
            for (int i = 0; i < NUM; i++)
            {
                fd_arrar_[i] = -1;
            }
        }
        void InitSelectServer()
        {
            listen_sock_ = Sock::Socket();
            Sock::Bind(listen_sock_, port_);
            Sock::Listen(listen_sock_);
            fd_arrar_[0] = listen_sock_;
        }
        std::ostream& PrintFd()
        {
            for (int i = 0; i < NUM; i++)
            {
                if (fd_arrar_[i] != -1) std::cout << fd_arrar_[i] << ' ';
            }

            return std::cout;
        }
        // 1111 1111
        // 0000 1111
        void HandlerEvent(const fd_set& rfds)
        {
            //判断我的有效sock，是否在rfds中
            for (int i = 0; i < NUM; i++)
            {
                if (-1 == fd_arrar_[i]) {
                    continue;
                }
                //如何区分: 新链接到来，真正的数据到来？
                if (FD_ISSET(fd_arrar_[i], &rfds))
                {
                    if (fd_arrar_[i] == listen_sock_)
                    {
                        //新链接到来
                        struct sockaddr_in peer;
                        socklen_t len = sizeof(peer);
                        int sock = accept(listen_sock_, (struct sockaddr*)&peer, &len);
                        if (sock < 0)
                        {
                            std::cout << "accept error" << std::endl;
                        }
                        else
                        {
                            //不能直接读取新的sock，为什么要见它添加到数组就完了？？
                            //将新的sock添加到文件描述符数组中！
                            // int index = -1;
                            int j = 0;
                            for (; j < NUM; j++)
                            {
                                if (fd_arrar_[j] == -1)
                                {
                                    // index = j;
                                    break;
                                }
                            }
                            // if(index == -1) 
                            if (j == NUM)
                            {
                                std::cout << "fd_array 已经满了!" << std::endl;
                                close(sock);
                            }
                            else
                            {
                                fd_arrar_[j] = sock;
                                // fd_arrar_[index] = sock;
                                std::cout << "获取新的链接成功, sock: " << sock << " 已经添加到数组中了, 当前: " << std::endl;
                                PrintFd() << " [当前]" << std::endl;
                            }
                        }
                    }
                    else
                    {
                        // 数据到来
                        // 这样写是有BUG的！这里不解决，epoll
                        // 你能保证你用1024就能读取完毕吗？？有没有可能有粘包问题？？
                        // 网络通信，定制协议，业务场景有关
                        // 是不是每一个sock，都必须有自己独立的buffer
                        char buffer[1024];
                        ssize_t s = recv(fd_arrar_[i], buffer, sizeof(buffer), 0);
                        if (s > 0)
                        {
                            buffer[s] = '\0';
                            std::cout << "clint say# " << buffer << std::endl;
                        }
                        else if (s == 0)
                        {
                            std::cout << "client quit ---- sock: " << fd_arrar_[i] << std::endl;

                            // 对端链接关闭
                            close(fd_arrar_[i]);

                            // 从rfds中，去掉该sock
                            fd_arrar_[i] = -1;

                            PrintFd() << " [当前]" << std::endl;

                        }
                        else
                        {
                            //读取异常，TODO
                            std::cerr << "recv error" << std::endl;
                        }
                    }
                }
            }
        }
        void Loop()
        {
            //这样写有问题吗？？

            //在服务器最开始的时候，我们只有一个sock，listen_sock
            //有读事件就绪，读文件描述符看待的！
            fd_set rfds; // 3, 4,5,6
            // fd_set wfds;
            // fd_set efds;
            // FD_SET(listen_sock_, &rfds);
            while (true)
            {
                // struct timeval timeout = {0, 0};
                // 对位图结构进行清空
                FD_ZERO(&rfds);
                int max_fd = -1;

                for (int i = 0; i < NUM; i++)
                {
                    if (-1 == fd_arrar_[i])
                        continue;
                    FD_SET(fd_arrar_[i], &rfds);
                    if (max_fd < fd_arrar_[i])
                        max_fd = fd_arrar_[i];
                }

                // select是可以等待多个fd的，listen_sock_只是其中之一
                // 如果有新的链接到来，一定对应的是有新的sock，你如何保证新的sock也被添加到select 中？
                // rfds: 1111 1111 (输入)
                //       1000 0000 (输出)
                // select 要被使用，需要借助于一个第三方数组，管理所有的有效sock

                // fd_set: 不要把它当做具有sock保存的功能，它只有互相通知(内核<->用户)的能力
                // select模型: 要保存历史所有的sock(为什么？)，需要借助于第三方的数组.
                // select : 通知sock就绪之后，上层读取，可能还需要继续让select帮我们进行检测，对rfds进行重复设置
                int n = select(max_fd + 1, &rfds, nullptr, nullptr, nullptr);
                switch (n)
                {
                case 0:
                    std::cout << "timeout ..." << std::endl;
                    break;
                case -1:
                    std::cout << "select error" << std::endl;
                    break;
                default:
                    // select成功, 至少有一个fd是就绪的
                    HandlerEvent(rfds);
                    // std::cout << "有事件发生了..." << std::endl;
                    break;
                }
            }
        }
        ~SelectServer()
        {
            //没什么意义
            if (listen_sock_ >= 0)
                close(listen_sock_);
        }
    };
} // namespace ns_select
