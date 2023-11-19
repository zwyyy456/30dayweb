#include "acceptor.h"
#include "socket.h"
#include "inet_address.h"
#include "channel.h"
#include "server.h"
#include <functional>
#include <cstdio>

Acceptor::Acceptor(EventLoop *loop) :
    loop_(loop), sock_(nullptr), accept_ch_(nullptr) {
    sock_ = new Socket();
    auto *addr = new InetAddress("127.0.0.1", 6789); // 6789679
    sock_->Bind(addr);
    sock_->Listen();
    sock_->Setnonblocking(); // Acceptor 建议使用阻塞式
    accept_ch_ = new Channel(loop_, sock_->getfd());
    std::function<void()> callback = [this] {
        AcceptConn();
    };
    // accept_ch_->set_use_threadpool(false); // 主 Acceptor 不使用线程池
    accept_ch_->set_read_callback(callback);
    accept_ch_->EnableReading();
    delete addr;
}

Acceptor::~Acceptor() {
    delete sock_;
    delete accept_ch_;
}
void Acceptor::AcceptConn() {
    auto *clnt_addr = new InetAddress();
    auto *clnt_sock = new Socket(sock_->Accpet(clnt_addr));
    printf("new client fd %d! IP: %s Port: %d\n", clnt_sock->getfd(), inet_ntoa(clnt_addr->get_addr().sin_addr), ntohs(clnt_addr->get_addr().sin_port));
    clnt_sock->Setnonblocking();
    new_conn_callback_(clnt_sock);
    delete clnt_addr;
}

void Acceptor::set_new_conn_callback(std::function<void(Socket *)> &callback) {
    new_conn_callback_ = callback;
}
