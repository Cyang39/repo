# cmake 的最低版本要求
cmake_minimum_required(VERSION 3.4.1)
# 生成 compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# 项目名称
project(EMS_POJECT)
# 头文件
include_directories(${PROJECT_SOURCE_DIR}/includes)
# 可执行文件输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
# 需要编译的可执行文件
add_executable(server server.c)
add_executable(client client.c)
