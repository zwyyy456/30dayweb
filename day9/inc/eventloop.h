#pragma once

#include "channel.h"
#include "epoll.h"
#include "server.h"
class Epoll;
class Channel;
class EventLoop {
  private:
    Epoll *ep_;
    bool quit_;

  public:
    EventLoop();
    ~EventLoop();
    void Loop();
    void UpdateChannel(Channel *ch);
};