#pragma once

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
    std::vector<epoll_event> Poll(int timeout = -1);
};