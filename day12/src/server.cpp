#include "server.h"
#include "acceptor.h"
#include "socket.h"
#include "inet_address.h"
#include "channel.h"
#include "eventloop.h"
#include "connection.h"
#include "thread_pool.h"

#include <cstdio>
#include <functional>
#include <cstring>
#include <unistd.h>
#include <cerrno>

#define READ_BUFFER 1024

Server::Server(EventLoop *loop) :
    main_reactor_(loop), acceptor_(nullptr) {
    acceptor_ = new Acceptor(main_reactor_);
    std::function<void(Socket *)> callback = [this](auto &&PH1) { NewConn(std::forward<decltype(PH1)>(PH1)); };
    acceptor_->set_new_conn_callback(callback);
    auto size = std::thread::hardware_concurrency(); // 获取 CPU 核心数?
    thpool_ = new ThreadPool(size);
    for (int i = 0; i < size; ++i) {
        sub_reactors_.push_back(new EventLoop());
    }

    for (int i = 0; i < size; ++i) {
        auto sub_loop = [capture0 = sub_reactors_[i]] { capture0->Loop(); };
        thpool_->add_task(sub_loop);
    }
}

Server::~Server() {
    delete acceptor_;
    delete thpool_;
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
    if (sock->getfd() != -1) {
        auto idx = sock->getfd() % sub_reactors_.size(); // 将连接随机分配到 sub_reactor
        auto *conn = new Connection(sub_reactors_[idx], sock);
        auto callback = [this](auto &&ph1) { DeleteConn(std::forward<decltype(ph1)>(ph1)); };

        conn->set_delete_conn_callback(callback);
        connections_[sock->getfd()] = conn;
    }
}

void Server::DeleteConn(int sockfd) {
    if (sockfd != -1) {
        auto iter = connections_.find(sockfd);
        if (iter != connections_.end()) {
            auto *conn = connections_[sockfd];
            connections_.erase(sockfd);
            delete conn; // 有可能出现 segment fault;
            // close(sockfd); // 正常
        }
    }
}