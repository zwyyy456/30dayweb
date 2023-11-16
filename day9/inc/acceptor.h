#pragma once

#include "channel.h"
#include "inet_address.h"
#include "server.h"
#include "socket.h"
#include <functional>
class Socket;
class Channel;
class EventLoop;
class Acceptor {
  private:
    EventLoop *loop_;
    Channel *accept_ch_;
    Socket *sock_;
    InetAddress *addr_;
    std::function<void(Socket *)> new_conn_callback_;

  public:
    Acceptor(EventLoop *loop);
    ~Acceptor();
    void AcceptConn();

    void set_new_conn_callback(std::function<void(Socket *)> &callback);
};