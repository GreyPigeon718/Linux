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
    pthread_cond_t have_space;  //������
    pthread_cond_t have_data;   //������
    //���¾����ٽ���Դ
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
    void Put(const T& in) //const type&: �����Ͳ���
    {
        //�����Ĺ���
        //p_lock;
        //lock(&p_lock);

        pthread_mutex_lock(&lock);
        while (IsFull()) {
            //�����߲�Ӧ������������Ӧ�ý���wait
            //һ���ȴ����ͻᱻ�����������������������ٽ����ڲ�������,�Ǵ���"��"����ģ���ò��û�л����Ƿ�������
            pthread_cond_wait(&have_space, &lock); //Ϊʲôwait�����һ��lock�أ�������ʱ���Զ��ͷ��������㻽���ǣ��Զ���ȡ��!
        }

        //bug?
        //Ŀǰ�����ߵ����һ���пռ�
        bq.push(in);

        //if(!IsFull()){
        //    bq.push(in); //����������˵Ļ������ǲ�Ӧ���ڽ���������
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
    void Get(T* out) //Type *: ����Ͳ�����type &: ��������Ͳ���
    {
        //���ѵĹ���,c_lock;
        //lock(&c_lock)
        pthread_mutex_lock(&lock);
        while (IsEmpty()) {
            pthread_cond_wait(&have_data, &lock);
        }
        //bug?
        *out = bq.front();
        bq.pop();
        // if(!IsEmpty()){
        //     *out = bq.front(); //���������ݲ���front��ȡ�أ����������Ϊ�յ�����£��Ͳ�Ӧ�ý���������
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
















