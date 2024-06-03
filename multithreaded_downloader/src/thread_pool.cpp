#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include <functional> // std::bind
#include <future>     // std::packaged_task
#include <queue>

class ThreadPool
{
public:
    /*
        这是ThreadPool类的构造函数的声明。
        它接受一个size_t类型的参数，用于指定线程池中线程的数量。

        关键字explicit用于防止C++的隐式类型转换。
        在这个例子中，如果没有explicit关键字，那么我们可以直接使用一个size_t类型的值来创建一个ThreadPool对象，
        如ThreadPool pool = 5;
        但是这样的代码可能会让人困惑，不清楚这个5是什么意思。
        因此，我们使用explicit关键字，强制要求在创建ThreadPool对象时明确地调用构造函数，
        如ThreadPool pool(5);

        在实现中，这个构造函数会创建指定数量的工作线程，并将它们添加到workers向量中。
        每个工作线程都在一个无限循环中运行，等待任务队列中出现任务。
        当任务队列不为空时，线程会取出一个任务并执行它。
        如果线程池被设置为停止，并且任务队列为空，那么线程会退出循环，结束执行。
    */
    explicit ThreadPool(size_t);
    template <class F, class... Args>
    decltype(auto) enqueue(F &&f, Args &&...args);
    ~ThreadPool();

private:
    // need to keep track of threads so we can join them
    std::vector<std::thread> workers; // 存储线程的容器
    // the task queue
    std::queue<std::packaged_task<void()>> tasks; // 存储任务的队列

    // synchronization
    std::mutex queue_mutex;                      // 任务队列的互斥锁
    std::condition_variable condition;           // 用于线程间的条件变量
    std::condition_variable condition_producers; // 用于线程池析构时的条件变量，在析构函数中等待所有任务执行完
    bool stop;                                   // 标志线程池是否停止
};

// the constructor just launches some amount of workers
inline ThreadPool::ThreadPool(size_t threads)//inline关键字用于告诉编译器，将函数体内联到每个调用它的地方
    : stop(false) // stop初始化为false
{
    for (size_t i = 0; i < threads; ++i) // 创建指定数量的工作线程
        workers.emplace_back(
            // 使用lambda表达式创建线程
            /*
            lambda表达式的定义形式为：
            [capture list] (params) -> return_type { function body }
            这里的capture list是一个方括号括起来的部分，用于指定lambda表达式的捕获列表。
            capture list中的变量会复制到lambda表达式中，并作为lambda表达式的局部变量使用。
            在这个例子中，我们使用[this]作为捕获列表，表示lambda表达式中可以访问ThreadPool对象的成员变量。
            */
            [this]
            {
                for (;;)
                {
                    std::packaged_task<void()> task;
                    // packaged_task是一个模板类，它可以包装任何可调用对象（函数、函数对象、lambda表达式等），

                    {
                        std::unique_lock<std::mutex> lock(this->queue_mutex); // 使用互斥锁保护任务队列
                        // 等待任务队列不为空或线程池停止
                        this->condition.wait(lock,
                                             [this]
                                             { return this->stop || !this->tasks.empty(); });
                        // 如果线程池停止并且任务队列为空，线程退出循环
                        if (this->stop && this->tasks.empty())
                            return;
                        // 取出任务
                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                        if (tasks.empty())
                        {
                            condition_producers.notify_one(); // 通知析构函数任务队列已空
                        }
                    }

                    task();
                }
            });
}

// add new work item to the pool
// 使用template模板实现enqueue函数
// enqueue(en):
template <class F, class... Args>// F是可调用对象的类型，Args是可调用对象的参数类型
decltype(auto) ThreadPool::enqueue(F &&f, Args &&...args) // enqueue函数接受一个可调用对象和它的参数
{
    using return_type = std::invoke_result_t<F, Args...>;// 使用std::invoke_result_t获取可调用对象的返回类型

    std::packaged_task<return_type()> task(// 创建一个packaged_task对象，用于包装可调用对象
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));// 使用std::bind绑定可调用对象和它的参数

    // future 是一个模板类，它提供了一个异步操作的结果，可以通过get()方法获取异步操作的结果。
    // future的名字来源于“将来”，表示异步操作的结果是在将来的某个时间点获取的。
    std::future<return_type> res = task.get_future();// 获取packaged_task的future对象，作为enqueue函数的返回值
    // 将packaged_task对象添加到任务队列中
    {
        std::unique_lock<std::mutex> lock(queue_mutex);// 使用互斥锁保护任务队列

        // don't allow enqueueing after stopping the pool
        // 如果线程池已经停止，抛出异常
        if (stop)
            throw std::runtime_error("enqueue on stopped ThreadPool");

        // 将packaged_task对象添加到任务队列中
        // std::move(task)将task对象的所有权转移给tasks队列
        // std::move是一个右值引用，它将左值转换为右值引用，表示对象的所有权可以转移
        tasks.emplace(std::move(task));
    }
    // 通知一个线程取出任务执行
    condition.notify_one();
    return res;
}

// the destructor joins all threads
inline ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);// 使用互斥锁保护任务队列

        // 线程池停止
        condition_producers.wait(lock, [this]
                                 { return tasks.empty(); });
        stop = true;
    }

    // 通知所有线程取出任务执行
    condition.notify_all();

    // 等待所有线程执行完毕
    for (std::thread &worker : workers)
    {
        worker.join();// 等待线程结束
    }
}

#endif