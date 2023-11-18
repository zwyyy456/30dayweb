#include "channel.h"
#include "eventloop.h"
#include "epoll.h"
#include "server.h"
#include <cstdint>
#include <functional>

Channel::Channel(EventLoop *loop, int _fd) :
    loop_(loop), fd(_fd), events(0), active_events(0), in_epoll(false) {
}

Channel::~Channel() {
}

void Channel::EnableReading() {
    events = EPOLLIN | EPOLLET;
    loop_->UpdateChannel(this);
}

int Channel::getfd() {
    return fd;
}

uint32_t Channel::get_events() {
    return events;
}

uint32_t Channel::get_active_events() {
    return active_events;
}

bool Channel::get_in_epoll() {
    return in_epoll;
}

void Channel::set_in_epoll() {
    in_epoll = true;
}

void Channel::set_active_events(uint32_t _ev) {
    active_events = _ev;
}

void Channel::set_callback(std::function<void()> &callback) {
    callback_ = callback;
}

void Channel::HandleEvent() {
    loop_->add_thread(callback_);
    /* callback_(); */
}