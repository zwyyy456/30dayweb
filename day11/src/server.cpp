#include "server.h"
#include "socket.h"
#include "inet_address.h"
#include "channel.h"
#include "eventloop.h"
#include "connection.h"

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

void Server::HandleReadEvent(int sockfd) { // 已经由 Connection 类负责了
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

void Server::NewConn(Socket *sock) {
    auto *conn = new Connection(loop_, sock); // 这里应该是 clnt_sock
    std::function<void(Socket *)> callback = [this](auto &&PH1) { DeleteConn(std::forward<decltype(PH1)>(PH1)); };
    conn->set_delete_conn_callback(callback);
    connections_[sock->getfd()] = conn;
}

void Server::DeleteConn(Socket *sock) {
    Connection *conn = connections_[sock->getfd()];
    connections_.erase(sock->getfd());
    // 存在 Segmant fault
    delete conn;
    // close(sockfd); // 正常
}