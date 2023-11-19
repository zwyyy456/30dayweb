#pragma once
#include "acceptor.h"
#include "thread_pool.h"

#include <map>
#include <vector>

class Acceptor;
class EventLoop;
class Socket;
class Connection;
class Server {
  private:
    EventLoop *main_reactor_;
    Acceptor *acceptor_;
    std::map<int, Connection *> connections_;
    std::vector<EventLoop *> sub_reactors_;
    ThreadPool *thpool_;

  public:
    Server(EventLoop *evl);
    ~Server();

    void HandleReadEvent(int fd);
    void NewConn(Socket *serv_sock);
    void DeleteConn(int sockfd);
};