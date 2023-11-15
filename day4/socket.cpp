
#include "inet_address.h"
#include "util.h"
#include "socket.h"
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

Socket::Socket() :
    fd(-1) {
    fd = socket(AF_INET, SOCK_STREAM, 0);
    errif(fd == -1, "socket create error");
}

Socket::Socket(int _fd) :
    fd(_fd) {
    errif(fd == -1, "socket create error");
}

Socket::~Socket() {
    if (fd != -1) {
        close(fd);
        fd = -1;
    }
}

void Socket::Bind(InetAddress *addr) {
    errif(bind(fd, (sockaddr *)&(addr->addr), addr->addr_len) == -1, "socket bind error\n");
}

void Socket::Listen() {
    errif(listen(fd, SOMAXCONN) == -1, "socket listen error\n");
}

void Socket::Setnonblocking() {
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

int Socket::Accpet(InetAddress *addr) {
    int clnt_sockfd = accept(fd, (sockaddr *)&(addr->addr), &(addr->addr_len));
    errif(clnt_sockfd == -1, "socket accept error\n");
    return clnt_sockfd;
}

int Socket::getfd() {
    return fd;
}