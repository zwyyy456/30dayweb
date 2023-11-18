#pragma once

#include "inet_address.h"
class InetAddress;

class Socket {
  private:
    int fd;

  public:
    Socket();
    Socket(int);
    ~Socket();
    void Bind(InetAddress *);
    void Listen();
    void Setnonblocking();

    int Accpet(InetAddress *);

    void Connect(InetAddress *);

    int getfd();
};