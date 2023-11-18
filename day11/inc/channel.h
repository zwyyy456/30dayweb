#pragma once

#include "server.h"
#include <cstdint>
#include <sys/epoll.h>
#include <functional>

class Epoll;
class EventLoop;
class Channel {
  private:
    EventLoop *loop_;
    int fd;
    uint32_t events;         // 要关注的事件
    uint32_t active_events_; // 当前正在发生的事件
    bool in_epoll_;
    bool use_threadpool_;
    std::function<void()> read_callback_;
    std::function<void()> write_callback_;

  public:
    Channel(EventLoop *loop, int _fd);
    ~Channel();

    void HandleEvent();
    void EnableReading();

    int getfd();
    uint32_t get_events();
    uint32_t get_active_events();
    bool get_in_epoll();
    void set_in_epoll(bool in_epoll = true);

    void UseEt();
    void set_active_events(uint32_t _ev);
    void set_read_callback(std::function<void()> &callback);

    void set_use_threadpool(bool use = true);
};
