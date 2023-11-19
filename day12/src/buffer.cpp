#include "buffer.h"
#include <cstring>
#include <iostream>

Buffer::Buffer() {
}

Buffer::~Buffer() {
}

void Buffer::Append(const char *_str, int _size) {
    for (int i = 0; i < _size; ++i) {
        if (_str[i] == '\0') {
            break;
        }
        buf_.push_back(_str[i]);
    }
}

auto Buffer::size() -> ssize_t {
    return buf_.size();
}

auto Buffer::c_str() -> const char * {
    return buf_.c_str();
}

void Buffer::Clear() {
    buf_.clear();
}

void Buffer::Getline() {
    buf_.clear();
    std::getline(std::cin, buf_);
}

void Buffer::set_buf(const char *mstr) {
    buf_.clear();
    buf_.append(mstr);
}