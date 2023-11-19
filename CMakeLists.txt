cmake_minimum_required(VERSION 3.0)  # 指定所需的CMake最低版本

project(30dayweb)  # 指定项目名称

# 设置包含头文件的目录
include_directories(include day12/inc)

# 添加可执行文件并指定源文件
aux_source_directory(day12/src serv)
add_executable(server ${serv} util/util.cpp day12/mserver.cpp)

add_executable(client client.cpp util/util.cpp)

add_executable(tptest threadpool_test.cpp day12/src/thread_pool.cpp)

add_executable(test test.cpp ${serv} util/util.cpp)

# 这里可以添加其他的CMake命令和目标
