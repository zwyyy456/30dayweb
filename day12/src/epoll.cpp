#include "epoll.h"
#include "util.h"
#include <cstdint>
#include "channel.h"
#include <sys/epoll.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>

#define MAX_EVENTS 1000

Epoll::Epoll() :
    epfd_(-1), events_(nullptr) {
    epfd_ = epoll_create1(0);
    events_ = new epoll_event[MAX_EVENTS];
    memset(events_, 0, sizeof(epoll_event) * MAX_EVENTS);
}

Epoll::~Epoll() {
    if (epfd_ != -1) {
        close(epfd_);
        epfd_ = -1;
    }
    delete[] events_;
}

// 使用 Channel 类之后似乎没有再用过了
void Epoll::addFd(int fd, uint32_t op) {
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.data.fd = fd;
    ev.events = op;
    errif(epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll add event error!\n");
}

auto Epoll::Poll(int timeout) -> std::vector<Channel *> {
    std::vector<Channel *> active_channels;
    int nfds = epoll_wait(epfd_, events_, MAX_EVENTS, timeout);
    errif(nfds == -1, "epoll wait error\n");
    active_channels.reserve(nfds);
    for (int i = 0; i < nfds; ++i) {
        Channel *ch = static_cast<Channel *>(events_[i].data.ptr);
        ch->set_active_events(events_[i].events);
        active_channels.push_back(ch);
    }
    return active_channels;
}

void Epoll::UpdateChannel(Channel *ch) {
    int fd = ch->getfd();
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.data.ptr = ch;
    ev.events = ch->get_events();
    if (!ch->get_in_epoll()) {
        // 添加到 epoll 中
        errif(epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll add error!\n");
        ch->set_in_epoll();
    } else {
        errif(epoll_ctl(epfd_, EPOLL_CTL_MOD, fd, &ev) == -1, "epoll mod error!\n");
    }
}

void Epoll::DeleteChannel(Channel *ch) {
    int fd = ch->getfd();
    errif(epoll_ctl(fd, EPOLL_CTL_DEL, fd, nullptr) == -1, "epoll delete error");
    ch->set_in_epoll(false);
}