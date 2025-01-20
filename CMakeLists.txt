cmake_minimum_required(VERSION 3.10)

set(CMAKE_C_FLAGS "-O3")

project(cWebServer)

include_directories(include/)

add_executable(server src/main.c)

add_library(configReader STATIC src/configReader.c)
add_library(socket STATIC src/socket.c)
add_library(webserver SHARED src/webserver.c)
add_library(threadpool SHARED src/threadpool.c)
add_library(clientConnection STATIC src/clientConnection.c)
add_library(queue SHARED src/queue.c)
add_library(http SHARED src/http.c)
add_library(requestHandler SHARED src/requestHandler.c)
add_library(hashmap SHARED src/hashmap.c)
add_library(staticContentServer STATIC src/staticContentServer.c)
add_library(terminal SHARED src/terminal.c)

target_link_libraries(clientConnection http)
target_link_libraries(requestHandler hashmap staticContentServer)
target_link_libraries(socket clientConnection)
target_link_libraries(webserver socket threadpool clientConnection queue pthread requestHandler)
target_link_libraries(server configReader webserver terminal)

add_subdirectory(test/)