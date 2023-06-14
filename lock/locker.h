#ifndef LOCKER_H
#define LOCKER_H

#include <exception>
#include <pthread.h>
#include <semaphore.h>

class sem
{
public:
    sem()
    {
        // 初始化信号量，初始计数为0
        if (sem_init(&m_sem, 0, 0) != 0)
        {
            throw std::exception();
        }
    }
    sem(int num)
    {
        // 初始化信号量，初始计数为num
        if (sem_init(&m_sem, 0, num) != 0)
        {
            throw std::exception();
        }
    }
    ~sem()
    {
        // 销毁信号量
        sem_destroy(&m_sem);
    }
    bool wait()
    {
        // 等待信号量，如果计数为0，线程阻塞
        return sem_wait(&m_sem) == 0;
    }
    bool post()
    {
        // 增加信号量的计数
        return sem_post(&m_sem) == 0;
    }

private:
    sem_t m_sem;  // 信号量对象
};

class locker
{
public:
    locker()
    {
        // 初始化互斥锁
        if (pthread_mutex_init(&m_mutex, NULL) != 0)
        {
            throw std::exception();
        }
    }
    ~locker()
    {
        // 销毁互斥锁
        pthread_mutex_destroy(&m_mutex);
    }
    bool lock()
    {
        // 加锁，如果锁已被其他线程占用，则当前线程阻塞
        return pthread_mutex_lock(&m_mutex) == 0;
    }
    bool unlock()
    {
        // 解锁
        return pthread_mutex_unlock(&m_mutex) == 0;
    }
    pthread_mutex_t *get()
    {
        // 获取互斥锁对象的指针
        return &m_mutex;
    }

private:
    pthread_mutex_t m_mutex;  // 互斥锁对象
};

class cond
{
public:
    cond()
    {
        // 初始化条件变量
        if (pthread_cond_init(&m_cond, NULL) != 0)
        {
            throw std::exception();
        }
    }
    ~cond()
    {
        // 销毁条件变量
        pthread_cond_destroy(&m_cond);
    }
    bool wait(pthread_mutex_t *m_mutex)
    {
        int ret = 0;
        // 等待条件变量，传入互斥锁对象，函数内部会自动解锁互斥锁并等待条件变量被激活后重新加锁
        ret = pthread_cond_wait(&m_cond, m_mutex);
        return ret == 0;
    }
    bool timewait(pthread_mutex_t *m_mutex, struct timespec t)
    {
        int ret = 0;
        // 类似于wait函数，但可以设置最长等待时间，超过指定时间后会返回
        ret = pthread_cond_timedwait(&m_cond, m_mutex, &t);
        return ret == 0;
    }
    bool signal()
    {
        // 激活一个等待该条件变量的线程
        return pthread_cond_signal(&m_cond) == 0;
    }
    bool broadcast()
    {
        // 激活所有等待该条件变量的线程
        return pthread_cond_broadcast(&m_cond) == 0;
    }

private:
    pthread_cond_t m_cond;  // 条件变量对象
};

#endif
