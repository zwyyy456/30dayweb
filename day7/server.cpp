#include "server.h"
#include "socket.h"
#include "inet_address.h"
#include "channel.h"
#include "eventloop.h"

#include <cstdio>
#include <functional>
#include <cstring>
#include <unistd.h>
#include <cerrno>

#define READ_BUFFER 1024

Server::Server(EventLoop *loop) :
    loop_(loop), acceptor_(nullptr) {
    acceptor_ = new Acceptor(loop);
    std::function<void(Socket *)> callback = [this](auto &&PH1) { NewConn(std::forward<decltype(PH1)>(PH1)); };
    acceptor_->set_new_conn_callback(callback);
}

Server::~Server() {
    delete acceptor_;
}

void Server::HandleReadEvent(int sockfd) {
    char buf[READ_BUFFER];
    while (true) {
        memset(buf, 0, sizeof(buf));
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

void Server::NewConn(Socket *serv_sock) {
    InetAddress *clnt_addr = new InetAddress();
    Socket *clnt_sock = new Socket(serv_sock->Accpet(clnt_addr));
    printf("new client fd %d! IP: %s Port: %d\n", clnt_sock->getfd(), inet_ntoa(clnt_addr->addr.sin_addr), ntohs(clnt_addr->addr.sin_port));
    clnt_sock->Setnonblocking();
    int sockfd = clnt_sock->getfd();
    Channel *clnt_ch = new Channel(loop_, sockfd);
    std::function<void()> callback = [this, sockfd] { HandleReadEvent(sockfd); };
    clnt_ch->set_callback(callback);
    clnt_ch->EnableReading();
}