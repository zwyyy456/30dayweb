#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <cstdio>
#include <sys/types.h>
#include <cerrno>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>

#include "util.h"

#define MAX_EVENTS 1024
#define READ_BUFFER 1024

void setnonblocking(int fd) {
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    errif(sockfd == -1, "socket create error\n");
    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(8888);

    errif(bind(sockfd, (sockaddr *)&serv_addr, sizeof(serv_addr)) == -1, "socket bind error!\n");
    errif(listen(sockfd, SOMAXCONN) == -1, "socket listen error\n");

    int epfd = epoll_create1(0);
    struct epoll_event events[MAX_EVENTS], ev;
    ev.events = EPOLLIN;
    ev.data.fd = sockfd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);
    while (true) {
        int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
        for (int i = 0; i < nfds; ++i) {
            if (events[i].data.fd == sockfd) {
                // 说明有新的客户端连接
                struct sockaddr_in clnt_addr;
                socklen_t clnt_addr_len = sizeof(clnt_addr);
                bzero(&clnt_addr, clnt_addr_len);

                int clnt_sockfd = accept(sockfd, (sockaddr *)&clnt_addr, &clnt_addr_len);
                ev.data.fd = clnt_sockfd;
                ev.events = EPOLLIN | EPOLLET;
                setnonblocking(clnt_sockfd);
                epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sockfd, &ev);
            } else if (events[i].events & EPOLLIN) {
                // 发生事件的是客户端的 fd，并且是可读事件
                char buf[READ_BUFFER]; // 读取缓冲区
                while (true) {
                    bzero(buf, sizeof(buf));
                    ssize_t bytes_read = read(events[i].data.fd, buf, sizeof(buf));
                    if (bytes_read > 0) {
                        // 读到了数据，但是还没读完
                        printf("message from client fd %d: %s\n", events[i].data.fd, buf);
                        continue;
                    } else if (bytes_read == -1 && errno == EINTR) {
                        // 客户端正常中断，继续读取
                        continue;
                    } else if (bytes_read == -1 && (errno == EWOULDBLOCK || errno == EAGAIN)) {
                        printf("finish reading once, errno: %d\n", errno);
                        break;
                    } else if (bytes_read == 0) {
                        printf("EOF, client fd %d disconnected\n", events[i].data.fd);
                        close(events[i].data.fd);
                        break;
                    }
                }
            } else {
                printf("something else!\n");
            }
        }
    }
    close(sockfd);
    return 0;
}