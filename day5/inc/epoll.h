
#pragma once

#include "channel.h"
#include <cstdint>
#include <sys/epoll.h>
#include <vector>

class Epoll {
  private:
    int epfd;
    struct epoll_event *events;

  public:
    Epoll();
    ~Epoll();
    void addFd(int fd, uint32_t op);
    auto Poll(int timeout = -1) -> std::vector<Channel *>;
    void UpdateChannel(Channel *ch);
};