#ifndef BUFFERQUEUE_H
#define BUFFERQUEUE_H

#ifdef DEBUG_OUTPUT
#include <iostream>
#endif

#include <vector>
#include <atomic>
#include <condition_variable>
#include <mutex>

class Semaphore
{
public:
    explicit Semaphore(int i = 0) {
        m_semaphore.store(i < 0 ? 0 : i);
    }

    Semaphore(const Semaphore &) = delete;
    Semaphore& operator=(const Semaphore &) = delete;

    void acquire(int i = 1) {
        if (i <= 0) return;

        std::unique_lock<std::mutex> lock(m_mutex);
        if (m_semaphore.load() < i) {
            m_conditionVar.wait(lock);
        }
        m_semaphore.fetch_sub(i);
    }

    bool tryAcquire(int i = 1) {
        if (i <= 0) return false;

        if (m_semaphore.load() >= i) {
            m_semaphore.fetch_sub(i);
            return true;
        } else return false;
    }

    void release(int i = 1) {
        if (i <= 0) return;

        m_semaphore.fetch_add(i);
        m_conditionVar.notify_one();
    }

    int available() const {
        return m_semaphore.load();
    }

private:
    std::condition_variable m_conditionVar;
    std::atomic_int m_semaphore;
    std::mutex m_mutex;
};

template <class T> class BufferQueue {
public:
    BufferQueue(int bufferSize = 100) { setBufferSize(bufferSize); }

    ~BufferQueue() {
        init();
        std::vector<T>().swap(m_bufferQueue);
    }

    void setBufferSize(int bufferSize) {
        m_bufferSize = bufferSize;
        m_bufferQueue = std::vector<T>(bufferSize);
        m_useableSpace.acquire(m_useableSpace.available());
        m_freeSpace.release(m_bufferSize - m_freeSpace.available());
        m_front = m_rear = 0;
    }

    void enqueue(const T &element) {
#ifdef DEBUG_OUTPUT
        std::cout << "[freespace " << m_freeSpace.available()
                  << "] --- [useablespace " << m_useableSpace.available() << "]"
                  << std::endl;
#endif
        m_freeSpace.acquire();
        m_bufferQueue[m_front++ % m_bufferSize] = element;
        m_useableSpace.release();
    }

    bool tryEnqueue(const T &element) {
        if (!m_freeSpace.tryAcquire())
            return false;
        m_bufferQueue[m_front++ % m_bufferSize] = element;
        m_useableSpace.release();
        return true;
    }

    T dequeue() {
#ifdef DEBUG_OUTPUT
        std::cout << "[freespace " << m_freeSpace.available()
                  << "] --- [useablespace " << m_useableSpace.available() << "]"
                  << std::endl;
#endif
        m_useableSpace.acquire();
        T element = m_bufferQueue[m_rear++ % m_bufferSize];
        m_freeSpace.release();

        return element;
    }

    /**
   * @brief tryDequeue
   * @note 尝试获取一个元素，并且在失败时不会阻塞调用线程
   * @return 成功返回对应T元素，失败返回默认构造的T元素
   */
    T tryDequeue() {
        T element;
        bool success = m_useableSpace.tryAcquire();
        if (success) {
            element = m_bufferQueue[m_rear++ % m_bufferSize];
            m_freeSpace.release();
        }

        return element;
    }

    void init() {
        m_useableSpace.acquire(m_useableSpace.available());
        m_freeSpace.release(m_bufferSize - m_freeSpace.available());
        m_front.store(0);
        m_rear.store(0);
    }

private:
    Semaphore m_freeSpace;
    Semaphore m_useableSpace;
    std::atomic_int m_rear;
    std::atomic_int m_front;
    std::vector<T> m_bufferQueue;
    int m_bufferSize;
};

#endif
