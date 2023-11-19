#pragma once

#include <functional>
#include <stdexcept>
#include <type_traits>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>

class ThreadPool {
  private:
    std::vector<std::thread> threads_;
    std::queue<std::function<void()>> tasks_;
    std::mutex tasks_mtx_;
    std::condition_variable cv_;
    bool stop_;

  public:
    explicit ThreadPool(int size = 8);
    ~ThreadPool();

    template <class F, class... Args>
    auto add_task(F &&f, Args &&...args) -> std::future<typename std::result_of<F(Args...)>::type>;
};

template <class F, class... Args>
auto ThreadPool::add_task(F &&f, Args &&...args) -> std::future<typename std::result_of<F(Args...)>::type> {
    using return_type = typename std::result_of<F(Args...)>::type;
    auto ptask = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    /* auto ptask = std::make_shared<std::packaged_task<return_type()>>([f = std::forward<F>(f), args = std::make_tuple(std::forward<Args>(args)...)]() mutable { */
    /*     return std::apply(std::move(f), std::move(args)); */
    /* }); */
    std::future<return_type> res = ptask->get_future();
    {
        std::unique_lock<std::mutex> lock(tasks_mtx_);
        // don't allow enqueueing after stopping the pool
        if (stop_) {
            throw std::runtime_error("enqueue on stopped ThreadPool");
        }
        tasks_.emplace([ptask]() { (*ptask)(); });
    }
    cv_.notify_one();
    return res;
}