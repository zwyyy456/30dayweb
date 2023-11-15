#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <vector>
#include "util.h"
#include "epoll.h"
#include "inet_address.h"
#include "socket.h"
#include "channel.h"

#define MAX_EVENTS 1024
#define READ_BUFFER 1024

void setnonblocking(int fd) {
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

void handleReadEvent(int);

int main() {
    Socket *serv_sock = new Socket();
    InetAddress *serv_addr = new InetAddress("127.0.0.1", 8888);
    serv_sock->Bind(serv_addr);
    serv_sock->Listen();
    Epoll *ep = new Epoll();
    serv_sock->Setnonblocking();
    Channel *serv_channel = new Channel(ep, serv_sock->getfd());
    serv_channel->enableReading();
    while (true) {
        std::vector<Channel *> active_channels = ep->Poll();
        int nfds = active_channels.size();
        for (int i = 0; i < nfds; ++i) {
            int chfd = active_channels[i]->getfd();
            if (chfd == serv_sock->getfd()) {
                InetAddress *clnt_addr = new InetAddress();
                Socket *clnt_sock = new Socket(serv_sock->Accpet(clnt_addr));
                printf("new client fd %d! IP: %s Port: %d\n", clnt_sock->getfd(), inet_ntoa(clnt_addr->addr.sin_addr), ntohs(clnt_addr->addr.sin_port));
                clnt_sock->Setnonblocking();
                Channel *clnt_ch = new Channel(ep, clnt_sock->getfd());
                clnt_ch->enableReading();
            } else if (active_channels[i]->get_active_events() & EPOLLIN) {
                handleReadEvent(active_channels[i]->getfd());
            } else {
                // 其他事件，之后版本实现
                printf("something else happened\n");
            }
        }
    }
    delete serv_sock;
    delete serv_addr;
    return 0;
}

void handleReadEvent(int sockfd) {
    char buf[READ_BUFFER];
    while (true) { //由于使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
        bzero(&buf, sizeof(buf));
        ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
        if (bytes_read > 0) {
            printf("message from client fd %d: %s\n", sockfd, buf);
            write(sockfd, buf, sizeof(buf));
        } else if (bytes_read == -1 && errno == EINTR) { //客户端正常中断、继续读取
            printf("continue reading");
            continue;
        } else if (bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) { //非阻塞IO，这个条件表示数据全部读取完毕
            printf("finish reading once, errno: %d\n", errno);
            break;
        } else if (bytes_read == 0) { // EOF，客户端断开连接
            printf("EOF, client fd %d disconnected\n", sockfd);
            close(sockfd); //关闭socket会自动将文件描述符从epoll树上移除
            break;
        }
    }
}