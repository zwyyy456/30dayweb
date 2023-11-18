
#include "eventloop.h"
#include "thread_pool.h"
EventLoop::EventLoop() :
    ep_(nullptr), thread_pool_(nullptr), quit_(false) {
    ep_ = new Epoll();
    thread_pool_ = new ThreadPool();
}

EventLoop::~EventLoop() {
    delete ep_;
}

void EventLoop::Loop() {
    while (!quit_) {
        auto chs = ep_->Poll(); // Poll 返回的是活跃的 Channel 的集合
        for (auto *ch : chs) {
            ch->HandleEvent();
        }
    }
}

void EventLoop::UpdateChannel(Channel *ch) {
    ep_->UpdateChannel(ch);
}

void EventLoop::add_thread(std::function<void()> func) {
    thread_pool_->add_task(func);
}