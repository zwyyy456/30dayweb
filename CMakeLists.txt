cmake_minimum_required(VERSION 3.0)  # 指定所需的CMake最低版本

project(30dayweb)  # 指定项目名称

# 设置包含头文件的目录
include_directories(include)

# 添加可执行文件并指定源文件
add_executable(server day3/server.cpp day3/util.cpp)
add_executable(client day3/client.cpp day3/util.cpp)

# 这里可以添加其他的CMake命令和目标
