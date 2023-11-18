#pragma once
#include <string>
#include <sys/types.h>

class Buffer {
  private:
    std::string buf_;

  public:
    Buffer();
    ~Buffer();
    void append(const char *str, int size);
    ssize_t size();
    const char *c_str();
    void clear();
    void getline();

    void set_buf(const char *);
};