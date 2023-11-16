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
    epfd(-1), events(nullptr) {
    epfd = epoll_create1(0);
    events = new epoll_event[MAX_EVENTS];
    memset(events, 0, sizeof(epoll_event) * MAX_EVENTS);
}

Epoll::~Epoll() {
    if (epfd != -1) {
        close(epfd);
        epfd = -1;
    }
    delete[] events;
}

// 使用 Channel 类之后似乎没有再用过了
void Epoll::addFd(int fd, uint32_t op) {
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.data.fd = fd;
    ev.events = op;
    errif(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll add event error!\n");
}

auto Epoll::Poll(int timeout) -> std::vector<Channel *> {
    std::vector<Channel *> active_channels;
    int nfds = epoll_wait(epfd, events, MAX_EVENTS, timeout);
    errif(nfds == -1, "epoll wait error\n");
    active_channels.reserve(nfds);
    for (int i = 0; i < nfds; ++i) {
        Channel *ch = (Channel *)events[i].data.ptr;
        ch->set_active_events(events[i].events);
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
        errif(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll add error!\n");
        ch->set_in_epoll();
    } else {
        errif(epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev) == -1, "epoll mod error!\n");
    }
}