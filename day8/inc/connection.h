#pragma once

#include "channel.h"
#include "eventloop.h"
#include "server.h"
#include "socket.h"
#include <functional>

class EventLoop;
class Socket;
class Channel;

class Connection {
  private:
    EventLoop *loop_;
    Socket *sock_;
    Channel *channel_;
    std::function<void(Socket *)> delete_conn_callback_;

  public:
    Connection(EventLoop *loop, Socket *sock);
    ~Connection();

    void echo(int sockfd);
    void set_delete_conn_callback(std::function<void(Socket *)> callback);
};