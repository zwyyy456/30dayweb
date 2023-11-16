
#pragma once

#include <arpa/inet.h>
#include <cstdint>
#include <netinet/in.h>
#include <sys/socket.h>

class InetAddress {
  public:
    struct sockaddr_in addr_;
    socklen_t addr_len_;
    InetAddress();
    InetAddress(const char *ip, uint16_t port);
    ~InetAddress();

    void set_inetaddr(sockaddr_in addr, socklen_t addr_len);
    sockaddr_in get_addr();
    socklen_t get_addr_len();
};