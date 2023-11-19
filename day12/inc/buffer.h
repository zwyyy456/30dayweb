#pragma once
#include <string>
#include <sys/types.h>

class Buffer {
  private:
    std::string buf_;

  public:
    Buffer();
    ~Buffer();
    void Append(const char *str, int size);
    ssize_t size();
    const char *c_str();
    void Clear();
    void Getline();

    void set_buf(const char *);
};