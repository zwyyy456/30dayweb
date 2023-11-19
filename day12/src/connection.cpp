#include "connection.h"
#include "channel.h"
#include "buffer.h"
#include "eventloop.h"
#include "socket.h"
#include "util.h"
#include <asm-generic/errno-base.h>
#include <cstring>
#include <functional>
#include <cstdio>
#include <cerrno>
#include <utility>
#include <unistd.h>

#define READ_BUFFER 1024

Connection::Connection(EventLoop *loop, Socket *sock) :
    loop_(loop), sock_(sock), channel_(nullptr), in_buffer_(new std::string()), read_buffer_(nullptr) {
    channel_ = new Channel(loop, sock->getfd());
    channel_->EnableReading();
    channel_->UseEt(); // 调用了两次 epoll_ctl，感觉可以优化
    int fd = sock_->getfd();
    std::function<void()> callback = [this, fd]() {
        echo(fd);
    };
    // channel_->set_use_threadpool(true); // Connection 中的 Channel 要使用线程池
    channel_->set_read_callback(callback);
    read_buffer_ = new Buffer();
}

Connection::~Connection() {
    delete channel_;
    delete sock_;
    delete read_buffer_;
}

void Connection::echo(int sockfd) {
    char buf[READ_BUFFER];
    while (true) {
        memset(buf, 0, sizeof(buf));
        ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
        if (bytes_read > 0) {
            read_buffer_->Append(buf, bytes_read);       // 将数据从 buf 读取到 buffer 类中
        } else if (bytes_read == -1 && errno == EINTR) { //客户端正常中断、继续读取
            printf("continue reading");
            continue;
        } else if (bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) { //非阻塞IO，这个条件表示数据全部读取完毕
            printf("finish reading once, errno: %d\n", errno);
            printf("message from client fd %d: %s\n", sockfd, read_buffer_->c_str());
            errif(write(sockfd, read_buffer_->c_str(), read_buffer_->size()) == -1, "socket write error");
            break;
        } else if (bytes_read == 0) { // EOF，客户端断开连接
            printf("EOF, client fd %d disconnected\n", sockfd);

            // 多线程中会有 bug，这是因为 delete_conn_callback_(sockfd) 可能同时被多个线程执行
            delete_conn_callback_(sockfd); // 调用这个关闭连接，而不是手动 close(fd);
            break;
        } else {
            printf("Connection reset by peer\n");
            delete_conn_callback_(sockfd);
        }
    }
}

void Connection::set_delete_conn_callback(std::function<void(int)> callback) {
    delete_conn_callback_ = std::move(callback);
}

void Connection::send(int sockfd) {
    char buf[read_buffer_->size()];
    strcpy(buf, read_buffer_->c_str());
    auto data_size = read_buffer_->size();
    auto data_left = data_size;
    while (data_left > 0) {
        auto bytes_write = write(sockfd, buf + data_size - data_left, data_left);
        if (bytes_write == -1 && errno == EAGAIN) {
            break; // 写完了
        }
        data_left -= bytes_write;
    }
}