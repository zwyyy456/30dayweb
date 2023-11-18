#include "channel.h"
#include "eventloop.h"
#include "epoll.h"
#include "server.h"
#include <cstdint>
#include <functional>
#include <sys/epoll.h>

Channel::Channel(EventLoop *loop, int _fd) :
    loop_(loop), fd(_fd), events(0), active_events_(0), in_epoll_(false), use_threadpool_(true) {
    // 即默认是使用线程池的
}

Channel::~Channel() {
}

void Channel::EnableReading() {
    events = EPOLLIN | EPOLLPRI; // LT 模式，查看是否有读事件
    loop_->UpdateChannel(this);
}

void Channel::UseEt() {
    events |= EPOLLET; // Connection 中的 Channel 才会启用
    loop_->UpdateChannel(this);
}

int Channel::getfd() {
    return fd;
}

uint32_t Channel::get_events() {
    return events;
}

uint32_t Channel::get_active_events() {
    return active_events_;
}

bool Channel::get_in_epoll() {
    return in_epoll_;
}

void Channel::set_in_epoll(bool in_epoll) {
    in_epoll_ = in_epoll;
}

void Channel::set_active_events(uint32_t _ev) {
    active_events_ = _ev;
}

void Channel::set_read_callback(std::function<void()> &callback) {
    read_callback_ = callback;
}

void Channel::HandleEvent() {
    if ((active_events_ & (EPOLLIN | EPOLLPRI)) != 0U) { // 感觉不如将读和写的 Channel 分开
        if (use_threadpool_) {
            loop_->add_thread(read_callback_); // 添加到线程池
        } else {
            read_callback_(); // 表示使用主线程处理
        }
    }
    if ((active_events_ & (EPOLLOUT)) != 0) {
        if (use_threadpool_) {
            loop_->add_thread(write_callback_);
        } else {
            write_callback_();
        }
    }
}
void Channel::set_use_threadpool(bool use) {
    use_threadpool_ = use;
}