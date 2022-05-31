#include <stdio.h>
#include <unistd.h>
#include <pthread.h>


// 模拟一下抢票逻辑
int tickets = 200; //临界资源
pthread_mutex_t lock;  //lock需要被所有的线程先要看到!!!,本质他也是一种临界资源？只需要保证lock，unlock是原子的即可！

//所谓的加锁是必须每个线程都必遵守的规则
// tickets--:抢到了一张票
void* Route(void* args)
{
    printf("before:\n");
    pthread_mutex_lock(&lock);//接下来的代码，只有我一个人在运行
    while (1) {
        pthread_mutex_lock(&lock);//接下来的代码，只有我一个人在运行
        printf("after:\n");
        //临界区
        if (tickets > 0) {
            usleep(30000); //usleep来模拟抢票的时间
            //临界区
            printf("0x%x: get a ticket : %d\n", (int)pthread_self(), tickets);
            tickets--;
            pthread_mutex_unlock(&lock); //对吗？？
        }
        else {
            pthread_mutex_unlock(&lock); //对吗？？
            break;
        }
    }
    printf("0x%x: quit, tickets: %d\n", (int)pthread_self(), tickets);

    return NULL;
}

int main()
{
    pthread_mutex_init(&lock, NULL); //初始化锁
#define NUM 5
    pthread_t nums[NUM];
    for (int i = 0; i < NUM; i++) {
        pthread_create(nums + i, NULL, Route, NULL);
    }

    for (int i = 0; i < NUM; i++) {
        pthread_join(nums[i], NULL);
    }

    pthread_mutex_destroy(&lock); //释放锁资源
    return 0;
}


