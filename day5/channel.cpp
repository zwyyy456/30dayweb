#include "channel.h"
#include "epoll.h"
#include <cstdint>

Channel::Channel(Epoll *_ep, int _fd) :
    ep(_ep), fd(_fd), events(0), active_events(0), in_epoll(false) {
}

Channel::~Channel() {
}

void Channel::enableReading() {
    events = EPOLLIN | EPOLLET;
    ep->UpdateChannel(this);
}

int Channel::getfd() {
    return fd;
}

uint32_t Channel::get_events() {
    return events;
}

uint32_t Channel::get_active_events() {
    return active_events;
}

bool Channel::get_in_epoll() {
    return in_epoll;
}

void Channel::set_in_epoll() {
    in_epoll = true;
}

void Channel::set_active_events(uint32_t _ev) {
    active_events = _ev;
}