#include "thread_pool.h"
#include <functional>
#include <cstdio>
#include <mutex>

ThreadPool::ThreadPool(int size) :
    stop_(false) {
    for (int i = 0; i < size; ++i) {
        // 线程创建成功
        printf("create thread!\n");
        auto func = [this]() {
            while (true) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(tasks_mtx_);
                    cv_.wait(lock, [this]() {
                        return stop_ || !tasks_.empty();
                    });
                    if (stop_ && tasks_.empty()) {
                        return;
                    }
                    task = tasks_.front();
                    printf("get task\n");
                    tasks_.pop();
                }
                printf("run task!\n");
                task();
            }
        };
        threads_.emplace_back(func);
    }
    printf("thread count: %d\n", threads_.size());
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(tasks_mtx_);
        stop_ = true;
    }
    cv_.notify_all();
    for (auto &thread : threads_) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

/* void ThreadPool::add_task(std::function<void()> func) { */
/*     { */
/*         std::unique_lock<std::mutex> lock(tasks_mtx_); */
/*         printf("add task\n"); */
/*         if (stop_) { */
/*             throw std::runtime_error("ThreadPool already stop, can't add task any more"); */
/*         } */
/*         tasks_.emplace(func); */
/*         cv_.notify_one(); */
/*     } */
/* } */