#pragma once
#include "acceptor.h"

#include <map>

class Acceptor;
class EventLoop;
class Socket;
class Connection;
class Server {
  private:
    EventLoop *loop_;
    Acceptor *acceptor_;
    std::map<int, Connection *> connections_;

  public:
    Server(EventLoop *evl);
    ~Server();

    void HandleReadEvent(int fd);
    void NewConn(Socket *serv_sock);
    void DeleteConn(Socket *);
};