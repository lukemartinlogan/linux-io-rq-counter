cmake_minimum_required(VERSION 3.10)
project(linux-io-rq-counter)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-O3")

include_directories(${CMAKE_SOURCE_DIR})

add_library(linux-io-rq-counter-um ${CMAKE_SOURCE_DIR}/linux-io-rq-counter-um.c)

add_executable(mount-counter ${CMAKE_SOURCE_DIR}/mount-counter.c)
add_dependencies(mount-counter linux-io-rq-counter-um)
target_link_libraries(mount-counter linux-io-rq-counter-um)

add_custom_target(build-km COMMAND bash ${CMAKE_SOURCE_DIR}/build-km.sh ${CMAKE_SOURCE_DIR})
