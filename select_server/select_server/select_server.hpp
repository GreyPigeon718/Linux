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
            //�ж��ҵ���Чsock���Ƿ���rfds��
            for (int i = 0; i < NUM; i++)
            {
                if (-1 == fd_arrar_[i]) {
                    continue;
                }
                //�������: �����ӵ��������������ݵ�����
                if (FD_ISSET(fd_arrar_[i], &rfds))
                {
                    if (fd_arrar_[i] == listen_sock_)
                    {
                        //�����ӵ���
                        struct sockaddr_in peer;
                        socklen_t len = sizeof(peer);
                        int sock = accept(listen_sock_, (struct sockaddr*)&peer, &len);
                        if (sock < 0)
                        {
                            std::cout << "accept error" << std::endl;
                        }
                        else
                        {
                            //����ֱ�Ӷ�ȡ�µ�sock��ΪʲôҪ������ӵ���������ˣ���
                            //���µ�sock��ӵ��ļ������������У�
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
                                std::cout << "fd_array �Ѿ�����!" << std::endl;
                                close(sock);
                            }
                            else
                            {
                                fd_arrar_[j] = sock;
                                // fd_arrar_[index] = sock;
                                std::cout << "��ȡ�µ����ӳɹ�, sock: " << sock << " �Ѿ���ӵ���������, ��ǰ: " << std::endl;
                                PrintFd() << " [��ǰ]" << std::endl;
                            }
                        }
                    }
                    else
                    {
                        // ���ݵ���
                        // ����д����BUG�ģ����ﲻ�����epoll
                        // ���ܱ�֤����1024���ܶ�ȡ����𣿣���û�п�����ճ�����⣿��
                        // ����ͨ�ţ�����Э�飬ҵ�񳡾��й�
                        // �ǲ���ÿһ��sock�����������Լ�������buffer
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

                            // �Զ����ӹر�
                            close(fd_arrar_[i]);

                            // ��rfds�У�ȥ����sock
                            fd_arrar_[i] = -1;

                            PrintFd() << " [��ǰ]" << std::endl;

                        }
                        else
                        {
                            //��ȡ�쳣��TODO
                            std::cerr << "recv error" << std::endl;
                        }
                    }
                }
            }
        }
        void Loop()
        {
            //����д�������𣿣�

            //�ڷ������ʼ��ʱ������ֻ��һ��sock��listen_sock
            //�ж��¼����������ļ������������ģ�
            fd_set rfds; // 3, 4,5,6
            // fd_set wfds;
            // fd_set efds;
            // FD_SET(listen_sock_, &rfds);
            while (true)
            {
                // struct timeval timeout = {0, 0};
                // ��λͼ�ṹ�������
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

                // select�ǿ��Եȴ����fd�ģ�listen_sock_ֻ������֮һ
                // ������µ����ӵ�����һ����Ӧ�������µ�sock������α�֤�µ�sockҲ����ӵ�select �У�
                // rfds: 1111 1111 (����)
                //       1000 0000 (���)
                // select Ҫ��ʹ�ã���Ҫ������һ�����������飬�������е���Чsock

                // fd_set: ��Ҫ������������sock����Ĺ��ܣ���ֻ�л���֪ͨ(�ں�<->�û�)������
                // selectģ��: Ҫ������ʷ���е�sock(Ϊʲô��)����Ҫ�����ڵ�����������.
                // select : ֪ͨsock����֮���ϲ��ȡ�����ܻ���Ҫ������select�����ǽ��м�⣬��rfds�����ظ�����
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
                    // select�ɹ�, ������һ��fd�Ǿ�����
                    HandlerEvent(rfds);
                    // std::cout << "���¼�������..." << std::endl;
                    break;
                }
            }
        }
        ~SelectServer()
        {
            //ûʲô����
            if (listen_sock_ >= 0)
                close(listen_sock_);
        }
    };
} // namespace ns_select
