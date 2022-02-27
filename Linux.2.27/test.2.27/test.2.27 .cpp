#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

char g_command[1024];

int GetCommand()
{
    printf("[minishell@localhost minishell] ");
    fflush(stdout);
    memset(g_command, '\0', sizeof(g_command));
    if (fgets(g_command, sizeof(g_command) - 1, stdin) == NULL)
    {
        perror("fgets error");
        return -1;
    }
    printf("%s\n", g_command);
    return 0;
}

int DealCommand(char* command)
{
    // ls -l    ==>   ls ��ִ�г���   -l ���ݸ�ls�������в���
    // 1. �зֶ��������ַ����� ����һ��ʲô�ǿ�ִ�г��� ʲô�������в���
    //      1.1 �зֳ����ĵ�һ���ַ������ǿ�ִ�г���
    //      1.2 ����Ķ��������в���
    //      1.3 �ÿո���Ϊ�ָ����������и�
    // ls -l ..
    int argc = 0;
    char* argv[1024] = { 0 };
    while (*command)
    {
        //�жϵ�ǰ���ַ��Ƿ��ǿո����\0
        if (!isspace(*command) && *command != '\0')
        {
            argv[argc] = command;
            argc++;

            while (!isspace(*command) && *command != '\0')
            {
                command++;
            }
            *command = '\0';
        }
        command++;
    }

    argv[argc] = NULL;

    for (int i = 0; i < argc; i++)
    {
        printf("%d:%s\n", i, argv[i]);
    }

    //�����ӽ���
    //�ӽ��̳����滻
    //execvp
}

int main()
{
    while (1)
    {
        //1.��ȡ�û����������
        if (GetCommand() == -1)
        {
            continue;
        }
        //2.�����ӽ���
        //3.���ӽ��̽��г����滻
        //4.���̵ȴ�
        DealCommand(g_command);
    }
    return 0;
}
