线程池是一种用于管理和分配线程的机制，它可以有效地利用系统资源，提高程序的性能和响应速度。线程池的工作原理是预先创建一定数量的线程，当有任务需要执行时，从线程池中取出一个空闲的线程来执行任务，任务执行完毕后，线程不会被销毁，而是返回到线程池中等待下一个任务。

线程池通常与任务队列一起使用。任务队列是一个存储任务的数据结构，它可以存储等待执行的任务。当有新的任务需要执行时，它会被添加到任务队列中。线程池中的线程会不断地从任务队列中取出任务来执行。

使用线程池的优点包括：

1. 线程的创建和销毁需要消耗系统资源，如果每次有任务时都创建新线程，那么系统资源的消耗会非常大。而线程池可以预先创建一定数量的线程，减少了线程创建和销毁的开销。

2. 线程池可以有效地利用系统资源，提高系统的整体性能。

3. 线程池可以控制并发线程的数量，防止系统资源被过度占用。

4. 线程池可以提供任务执行和任务调度的功能，使得任务的执行更加灵活和高效。

---

//实现参考 https://www.cnblogs.com/carsonzhu/p/16799198.html

`thread_pool.cpp`定义了一个名为`ThreadPool`的类，它用于管理和调度一组工作线程。这个类有三个公开的成员函数：一个构造函数，一个名为`enqueue`的函数，以及一个析构函数。

构造函数`ThreadPool(size_t threads)`接受一个参数，表示线程池中线程的数量。在构造函数中，它创建了指定数量的工作线程，并将它们添加到`workers`向量中。每个工作线程都在一个无限循环中运行，等待任务队列中出现任务。当任务队列不为空时，线程会取出一个任务并执行它。如果线程池被设置为停止，并且任务队列为空，那么线程会退出循环，结束执行。

`enqueue`函数是一个模板函数，它接受一个可调用对象和一组参数，创建一个任务，并将这个任务添加到任务队列中。这个函数返回一个`std::future`对象，这个对象可以用于获取任务的结果。如果线程池已经被设置为停止，那么这个函数会抛出一个运行时错误。

析构函数会等待所有的任务都被执行完毕，然后设置`stop`为`true`，唤醒所有等待的工作线程，并等待它们结束执行。这样可以确保在`ThreadPool`对象被销毁时，所有的工作线程都已经安全地结束了。

此外，`ThreadPool`类还有一些私有成员：一个`std::vector`用于存储工作线程，一个`std::queue`用于存储任务，一个`std::mutex`用于保护任务队列，两个`std::condition_variable`用于线程同步，以及一个`bool`变量`stop`用于指示线程池是否应该停止。

---

usage for thread_pool.cpp
```cpp
// create thread pool with 4 worker threads
ThreadPool pool(4);
 
// enqueue and store future
auto result = pool.enqueue([](int answer) { return answer; }, 42);
 
// get result from future
std::cout << result.get() << std::endl;
```


```cpp
#include "ThreadPool.hpp"
#include <vector>
#include <string>

// 网络模块
std::vector<char> download_data(const std::string& url) {
    // 从URL下载数据，并返回数据
    // 这里只是一个示例，你需要实现这个函数
}

// 文件模块
void write_to_file(const std::string& file_path, const std::vector<char>& data) {
    // 将数据写入文件
    // 这里只是一个示例，你需要实现这个函数
}

int main() {
    // 创建一个包含10个线程的线程池
    ThreadPool pool(10);

    // 下载任务列表
    std::vector<std::string> download_list = {
        "http://example.com/file1",
        "http://example.com/file2",
        // ...
    };

    for (const auto& url : download_list) {
        // 将下载任务添加到线程池
        pool.enqueue([url] {
            auto data = download_data(url);
            write_to_file("/path/to/download/directory/" + url, data);
        });
    }

    return 0;
}
```