#include "acceptor.h"
#include "socket.h"
#include "inet_address.h"
#include "channel.h"
#include "server.h"
#include <functional>

Acceptor::Acceptor(EventLoop *loop) :
    loop_(loop), sock_(nullptr), accept_ch_(nullptr), addr_(nullptr) {
    sock_ = new Socket();
    addr_ = new InetAddress("127.0.0.1", 8888);
    sock_->Bind(addr_);
    sock_->Listen();
    sock_->Setnonblocking();
    accept_ch_ = new Channel(loop_, sock_->getfd());
    std::function<void()> callback = [this] {
        AcceptConn();
    };
    accept_ch_->set_callback(callback);
    accept_ch_->EnableReading();
}

Acceptor::~Acceptor() {
    delete sock_;
    delete addr_;
    delete accept_ch_;
}
void Acceptor::AcceptConn() {
    new_conn_callback_(sock_);
}

void Acceptor::set_new_conn_callback(std::function<void(Socket *)> &callback) {
    new_conn_callback_ = callback;
}
