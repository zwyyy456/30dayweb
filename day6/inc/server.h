#pragma once

class EventLoop;
class Socket;
class Server {
  private:
    EventLoop *loop_;

  public:
    Server(EventLoop *evl);
    ~Server();

    void HandleReadEvent(int fd);
    void NewConn(Socket *serv_sock);
};