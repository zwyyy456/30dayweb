#pragma once
#include "acceptor.h"

class Acceptor;
class EventLoop;
class Socket;
class Server {
  private:
    EventLoop *loop_;
    Acceptor *acceptor_;

  public:
    Server(EventLoop *evl);
    ~Server();

    void HandleReadEvent(int fd);
    void NewConn(Socket *serv_sock);
};