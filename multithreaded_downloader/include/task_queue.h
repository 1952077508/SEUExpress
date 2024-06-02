#ifndef TASK_QUEUE_H
#define TASK_QUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>

/**
 * @brief 任务队列类，用于线程安全地添加和获取任务。
 * 
 * 该类提供了一个线程安全的任务队列实现，允许多个线程安全地向队列中添加任务，
 * 以及一个线程安全地从队列中获取并执行任务。使用互斥锁和条件变量来保证线程安全。
 * 
 * @tparam T 任务的类型。
 */
template <typename T>
class TaskQueue {
public:
    /**
     * 默认构造函数。
     */
    TaskQueue() = default;

    /**
     * 默认析构函数。
     */
    ~TaskQueue() = default;

    /**
     * 向队列中添加一个任务。
     * 
     * @param task 要添加的任务。
     */
    void push(const T& task) {
        std::unique_lock<std::mutex> lock(mutex_);
        queue_.push(task);
        lock.unlock();
        condition_.notify_one();
    }

    /**
     * 从队列中获取并返回一个任务。
     * 
     * 此方法是阻塞的，如果队列为空，它将等待直到有任务可用。
     * 
     * @return T 获取的任务。
     */
    T pop() {
        std::unique_lock<std::mutex> lock(mutex_);
        condition_.wait(lock, [this] { return !queue_.empty(); });
        T task = queue_.front();
        queue_.pop();
        return task;
    }

private:
    std::queue<T> queue_; /**< 存储任务的队列。 */
    std::mutex mutex_; /**< 用于同步访问队列的互斥锁。 */
    std::condition_variable condition_; /**< 用于在队列为空时等待任务的条件变量。 */
};

#endif // TASK_QUEUE_H