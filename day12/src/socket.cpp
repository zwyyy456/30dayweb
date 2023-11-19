#include "inet_address.h"
#include "util.h"
#include "socket.h"
#include <cstring>
#include <netinet/in.h>
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
    errif(bind(fd, (sockaddr *)&(addr->addr_), addr->addr_len_) == -1, "socket bind error\n");
}

void Socket::Listen() {
    errif(listen(fd, SOMAXCONN) == -1, "socket listen error\n");
}

void Socket::Setnonblocking() {
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

int Socket::Accpet(InetAddress *addr) {
    struct sockaddr_in clnt_addr;
    memset(&clnt_addr, 0, sizeof(clnt_addr));
    socklen_t clnt_addr_len = sizeof(clnt_addr);
    int clnt_sockfd = accept(fd, (sockaddr *)(&clnt_addr), &clnt_addr_len);

    errif(clnt_sockfd == -1, "socket accept error\n");
    addr->set_inetaddr(clnt_addr, clnt_addr_len);
    return clnt_sockfd;
}

void Socket::Connect(InetAddress *addr) {
    struct sockaddr_in clnt_addr = addr->get_addr();
    errif(connect(fd, (sockaddr *)&addr, sizeof(clnt_addr)) == -1, "socket connect error");
}

int Socket::getfd() {
    return fd;
}