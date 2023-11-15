#pragma once

#include <cstdint>
#include <sys/epoll.h>

class Epoll;
class Channel {
  private:
    Epoll *ep;
    int fd;
    uint32_t events;        // 要关注的事件
    uint32_t active_events; // 当前正在发生的事件
    bool in_epoll;

  public:
    Channel(Epoll *_ep, int _fd);
    ~Channel();

    void enableReading();

    int getfd();
    uint32_t get_events();
    uint32_t get_active_events();
    bool get_in_epoll();
    void set_in_epoll();

    void set_active_events(uint32_t _ev);
};
