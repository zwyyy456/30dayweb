#include <iostream>
#include <string>
#include "thread_pool.h"

void print(int a, double b, const char *c, std::string d) {
    std::cout << a << b << c << d << std::endl;
}

void test() {
    std::cout << "hellp" << std::endl;
}

int main(int argc, char const *argv[]) {
    ThreadPool *pool = new ThreadPool();
    std::function<void()> func = [] { return print(1, 3.14, "hello", std::string("world")); };
    pool->add_task(func);
    func = test;
    pool->add_task(func);
    delete pool;
    return 0;
}