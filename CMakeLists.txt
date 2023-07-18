# cmake 的最低版本要求
cmake_minimum_required(VERSION 3.4.1)
# 生成 compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# 项目名称
project(EMS_POJECT)
# 使用 SQLite3 库
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -lsqlite3")
# 头文件
include_directories(${PROJECT_SOURCE_DIR}/includes)
# 可执行文件输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
# 需要编译的可执行文件
add_executable(server server.c ${PROJECT_SOURCE_DIR}/library/message.c
                               ${PROJECT_SOURCE_DIR}/library/dbhandler.c)
add_executable(client client.c ${PROJECT_SOURCE_DIR}/library/message.c)
# 链接 SQLite3 库
include_directories(/usr/include)
link_directories(/usr/lib)
target_link_libraries(server sqlite3)
