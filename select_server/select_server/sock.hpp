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
            //> domain:��ʾ�׽���ʹ�õ�Э���AF_INET��TCP/IP-IPv4��
            //> type:�׽������� SOCK_STREAM��TCP����
            //> protocol:һ������Ϊ��0��������˵���Ѿ�ȷ���׽���ʹ�õ�Э��غ�����ʱ�����������ֵ��Ϊ0��������ʱ�򴴽���ʼ�׽���ʱ����domain����δ֪����£��������Э��غ�����ʱ��protocol������������ȷ��Э������ࡣ
            //> ����ֵ�����׽��ִ����ɹ�ʱ�������׽��֣�ʧ�ܷ��ء�-1�������������д�롰errno���С�
            int sock = socket(AF_INET, SOCK_STREAM, 0);
            if (sock < 0)
            {
                std::cerr << "socket error!" << std::endl;
                exit(SOCKET_ERR);
            }
            int opt = 1;
            //> int setsockopt( int socket, int level, int option_name,const void *option_value, size_t ��ption_len);
            //> int sockfd : �׽���������
            //> int level : ѡ���Ĳ�Σ�Ŀǰ��֧��SOL_SOCKET��IPPROTO_TCP���
            //> int optname : �����õ�ѡ�� SO_REUSEADDR:�򿪻�رյ�ַ���ù���
            //> const void* optval : ָ�룬ָ����ѡ��ֵ�Ļ�����
            //> socklen_t optlen : optval�������ĳ���
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