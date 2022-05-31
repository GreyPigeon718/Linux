#include <stdio.h>
#include <unistd.h>
#include <pthread.h>


// ģ��һ����Ʊ�߼�
int tickets = 200; //�ٽ���Դ
pthread_mutex_t lock;  //lock��Ҫ�����е��߳���Ҫ����!!!,������Ҳ��һ���ٽ���Դ��ֻ��Ҫ��֤lock��unlock��ԭ�ӵļ��ɣ�

//��ν�ļ����Ǳ���ÿ���̶߳������صĹ���
// tickets--:������һ��Ʊ
void* Route(void* args)
{
    printf("before:\n");
    pthread_mutex_lock(&lock);//�������Ĵ��룬ֻ����һ����������
    while (1) {
        pthread_mutex_lock(&lock);//�������Ĵ��룬ֻ����һ����������
        printf("after:\n");
        //�ٽ���
        if (tickets > 0) {
            usleep(30000); //usleep��ģ����Ʊ��ʱ��
            //�ٽ���
            printf("0x%x: get a ticket : %d\n", (int)pthread_self(), tickets);
            tickets--;
            pthread_mutex_unlock(&lock); //���𣿣�
        }
        else {
            pthread_mutex_unlock(&lock); //���𣿣�
            break;
        }
    }
    printf("0x%x: quit, tickets: %d\n", (int)pthread_self(), tickets);

    return NULL;
}

int main()
{
    pthread_mutex_init(&lock, NULL); //��ʼ����
#define NUM 5
    pthread_t nums[NUM];
    for (int i = 0; i < NUM; i++) {
        pthread_create(nums + i, NULL, Route, NULL);
    }

    for (int i = 0; i < NUM; i++) {
        pthread_join(nums[i], NULL);
    }

    pthread_mutex_destroy(&lock); //�ͷ�����Դ
    return 0;
}


