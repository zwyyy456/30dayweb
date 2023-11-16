
#include "eventloop.h"
EventLoop::EventLoop() :
    ep_(nullptr), quit_(false) {
    ep_ = new Epoll();
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