#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <vector>
#include "server.h"
#include "util.h"
#include "epoll.h"
#include "inet_address.h"
#include "socket.h"
#include "channel.h"
#include "eventloop.h"

#define MAX_EVENTS 1024
#define READ_BUFFER 1024

void setnonblocking(int fd) {
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

void handleReadEvent(int);

int main() {
    auto *loop = new EventLoop();
    auto *server = new Server(loop);
    loop->Loop();
    return 0;
}
