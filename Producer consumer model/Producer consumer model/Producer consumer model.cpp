#pragma once

#include <iostream>
#include <queue>
#include <pthread.h>

template <class T>
class BlockQueue {
private:
    int cap;
    pthread_mutex_t lock;
    //  pthread_mutex_t p_lock;
    //  pthread_mutex_t c_lock;
    pthread_cond_t have_space;  //生产者
    pthread_cond_t have_data;   //消费者
    //本事就是临界资源
    std::queue<T> bq;
private:
    bool IsFull()
    {
        return bq.size() == cap ? true : false;
    }
    bool IsEmpty()
    {
        return bq.size() == 0 ? true : false;
    }
public:
    BlockQueue(int _cap) :cap(_cap)
    {
        pthread_mutex_init(&lock, nullptr);
        pthread_cond_init(&have_space, nullptr);
        pthread_cond_init(&have_data, nullptr);
    }
    void Put(const T& in) //const type&: 输入型参数
    {
        //生产的过程
        //p_lock;
        //lock(&p_lock);

        pthread_mutex_lock(&lock);
        while (IsFull()) {
            //生产者不应该在生产，而应该进行wait
            //一旦等待，就会被挂起阻塞，可是我们是在临界区内部！！！,是带着"锁"挂起的，我貌似没有机会是否锁啊！
            pthread_cond_wait(&have_space, &lock); //为什么wait最后会带一个lock呢？？挂起时，自动释放锁，当你唤醒是，自动获取锁!
        }

        //bug?
        //目前我们走到这里，一定有空间
        bq.push(in);

        //if(!IsFull()){
        //    bq.push(in); //如果队列满了的话，我们不应该在进行生产了
        //}
        //else{
        //    std::cout << "queue is full! put() should block" << std::endl;
        //}
        if (bq.size() >= cap / 2) {
            pthread_cond_signal(&have_data);
        }
        pthread_mutex_unlock(&lock);
        //unlock(&p_lock);
    }
    void Get(T* out) //Type *: 输出型参数，type &: 输入输出型参数
    {
        //消费的过程,c_lock;
        //lock(&c_lock)
        pthread_mutex_lock(&lock);
        while (IsEmpty()) {
            pthread_cond_wait(&have_data, &lock);
        }
        //bug?
        *out = bq.front();
        bq.pop();
        // if(!IsEmpty()){
        //     *out = bq.front(); //必须有数据才能front提取呢？？如果队列为空的情况下，就不应该进行消费了
        //     bq.pop();
        // }
        // else{
        //     std::cout << "queue is empty! get() should block" << std::endl;
        // }
        pthread_cond_signal(&have_space);
        pthread_mutex_unlock(&lock);
        //unlock(&c_lock);
    }
    ~BlockQueue()
    {
        pthread_mutex_destroy(&lock);
        pthread_cond_destroy(&have_space);
        pthread_cond_destroy(&have_data);
    }

};
















