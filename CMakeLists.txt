cmake_minimum_required(VERSION 3.10)
project(linux-io-rq-counter)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-O3")

include_directories(${CMAKE_SOURCE_DIR})

add_library(linux-io-rq-counter-um SHARED ${CMAKE_SOURCE_DIR}/linux-io-rq-counter-um.c)

add_executable(mount-counter ${CMAKE_SOURCE_DIR}/mount-counter.c)
add_dependencies(mount-counter linux-io-rq-counter-um)
target_link_libraries(mount-counter linux-io-rq-counter-um)
add_executable(test-counter ${CMAKE_SOURCE_DIR}/test-counter.c)
add_dependencies(test-counter linux-io-rq-counter-um)
target_link_libraries(test-counter linux-io-rq-counter-um)

add_custom_target(build-km COMMAND bash ${CMAKE_SOURCE_DIR}/build-km.sh 1 ${CMAKE_SOURCE_DIR})
add_custom_target(check-km COMMAND lsmod | grep linux_io_rq_counter_km)
add_custom_target(check-klog COMMAND dmesg | grep linux_io_rq_counter_km)
add_custom_target(insert-km COMMAND sudo insmod ${CMAKE_SOURCE_DIR}/linux-io-rq-counter-km.ko)
add_custom_target(remove-km COMMAND sudo rmmod linux_io_rq_counter_km)
add_custom_target(clean-km COMMAND bash ${CMAKE_SOURCE_DIR}/build-km.sh 2 ${CMAKE_SOURCE_DIR})
add_custom_target(clean-klog COMMAND sudo dmesg --clear)
