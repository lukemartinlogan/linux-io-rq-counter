cmake_minimum_required(VERSION 3.10)
project(labstor)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-g")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-g")

include_directories(include)

###BLOCK DEVICES
#add_library(blkdev)

###OBJECT STORES
#add_library(object-store)

###ROOT SCRIPTS
set(FLAMEGRAPH ${CMAKE_SOURCE_DIR}/scripts/FlameGraph)

###TRACE SYSTEM CALLS
set(TRACE_SYSCALLS_ROOT ${CMAKE_SOURCE_DIR}/test/trace-syscalls)
set(TRACE_ALL_SYSCALLS_SH ${TRACE_SYSCALLS_ROOT}/trace-all-syscalls.sh)
set(TRACE_POSIX_SYSCALLS_SH ${TRACE_SYSCALLS_ROOT}/trace-posix-syscalls.sh)
set(TRACE_POSIX_EXEC ${CMAKE_CURRENT_BINARY_DIR}/trace-posix-exec)
add_executable(trace-posix-exec ${TRACE_SYSCALLS_ROOT}/trace-posix-syscalls.cpp)

add_custom_target(trace-all-syscalls)
add_custom_command(TARGET trace-all-syscalls COMMAND bash ${TRACE_ALL_SYSCALLS_SH} 3 ${TRACE_POSIX_EXEC} ${CMAKE_CURRENT_BINARY_DIR}/trace-all-syscalls.txt)
add_custom_target(trace-posix-syscalls)
add_custom_command(TARGET trace-posix-syscalls COMMAND bash ${TRACE_POSIX_SYSCALLS_SH} 0 ${TRACE_POSIX_EXEC} ${CMAKE_CURRENT_BINARY_DIR}/trace-posix-syscalls.txt)

###TIME TRANSFER METHODS TEST CASES
set(TIME_POSIX_IO_ROOT ${CMAKE_SOURCE_DIR}/test/time-posix-io)
set(TIME_POSIX_IO_EXEC ${CMAKE_CURRENT_BINARY_DIR}/time-posix-io-exec)
set(TIME_POSIX_IO_SH ${TIME_POSIX_IO_ROOT}/time-transfer-methods.sh)
set(TIME_TRANSFER_STATS_SH ${TIME_POSIX_IO_ROOT}/time-transfer-stats.sh)
set(TIME_TRANSFER_STATS_PY ${TIME_POSIX_IO_ROOT}/time-transfer-stats.py)
add_executable(time-posix-io-exec ${TIME_POSIX_IO_ROOT}/time-posix-io.cpp)

add_custom_target(time-fwrite-100M COMMAND bash ${TIME_POSIX_IO_SH} ${TIME_POSIX_IO_EXEC} 0 100 20m)
add_custom_target(time-fwrite-flush-100M COMMAND bash ${TIME_POSIX_IO_SH} ${TIME_POSIX_IO_EXEC} 1 100 20m)
add_custom_target(time-fwrite-huge-100M COMMAND bash ${TIME_POSIX_IO_SH} ${TIME_POSIX_IO_EXEC} 2 100 20m)
add_custom_target(time-fwrite-flush-huge-100M COMMAND bash ${TIME_POSIX_IO_SH} ${TIME_POSIX_IO_EXEC} 3 100 20m)
add_custom_target(time-direct-write-100M COMMAND bash ${TIME_POSIX_IO_SH} ${TIME_POSIX_IO_EXEC} 4 100 20m)
add_custom_target(time-direct-write-huge-100M COMMAND bash ${TIME_POSIX_IO_SH} ${TIME_POSIX_IO_EXEC} 5 100 20m)
add_custom_target(time-memcpy-write-100M COMMAND bash ${TIME_POSIX_IO_SH} ${TIME_POSIX_IO_EXEC} 6 100 20m)
add_custom_target(time-memcpy-write-huge-100M COMMAND bash ${TIME_POSIX_IO_SH} ${TIME_POSIX_IO_EXEC} 7 100 20m)
add_custom_target(time-transfer-stats COMMAND bash ${TIME_TRANSFER_STATS_SH} ${TIME_TRANSFER_STATS_PY} 100)

###TIME DIRECT LINUX DRIVER IO
set(TIME_LINUX_DRIVER_IO_ROOT ${CMAKE_SOURCE_DIR}/test/time-linux-driver-io)
set(TIME_LINUX_DRIVER_IO_EXEC ${TIME_LINUX_DRIVER_IO_ROOT}/time-linux-driver-io.ko)
set(BUILD_LINUX_DRIVER_IO_SH ${TIME_LINUX_DRIVER_IO_ROOT}/build-linux-driver-io.sh)
set(TIME_LINUX_DRIVER_IO_SH ${TIME_LINUX_DRIVER_IO_ROOT}/time-linux-driver-io.sh)

add_custom_target(build-linux-driver-io COMMAND bash ${BUILD_LINUX_DRIVER_IO_SH} ${TIME_LINUX_DRIVER_IO_ROOT})
add_custom_target(time-linux-driver-io COMMAND bash ${TIME_LINUX_DRIVER_IO_SH} ${TIME_LINUX_DRIVER_IO_EXEC})

###TRACE TRANSFER METHODS TEST CASES
set(TRACE_TRANSFER_METHODS_ROOT ${CMAKE_SOURCE_DIR}/test/trace-transfer-methods)
set(TRACE_TRANSFER_EXEC ${CMAKE_CURRENT_BINARY_DIR}/trace-transfer-methods-exec)
set(TRACE_TRANSFER_METHODS_SH ${TRACE_TRANSFER_METHODS_ROOT}/trace-transfer-methods.sh)
set(PARSE_TRANSFER_METHODS_SH ${TRACE_TRANSFER_METHODS_ROOT}/parse.sh)
set(PRUNE_TRANSFER_METHODS_SH ${TRACE_TRANSFER_METHODS_ROOT}/prune.sh)
add_executable(trace-transfer-methods-exec ${TRACE_TRANSFER_METHODS_ROOT}/trace-transfer-methods.c)

add_custom_target(trace-fwrite-8K COMMAND bash ${TRACE_TRANSFER_METHODS_SH} ${TRACE_TRANSFER_EXEC} 0 8k)
add_custom_target(trace-fwrite-huge-8K COMMAND bash ${TRACE_TRANSFER_METHODS_SH} ${TRACE_TRANSFER_EXEC} 1 8k)
add_custom_target(trace-direct-write-8K COMMAND bash ${TRACE_TRANSFER_METHODS_SH} ${TRACE_TRANSFER_EXEC} 2 8k)
add_custom_target(trace-direct-write-huge-8K COMMAND bash ${TRACE_TRANSFER_METHODS_SH} ${TRACE_TRANSFER_EXEC} 3 8k)

add_custom_target(parse-fwrite-8K COMMAND bash ${PARSE_TRANSFER_METHODS_SH} ${TRACE_TRANSFER_METHODS_ROOT} 0)
add_custom_target(parse-fwrite-huge-8K COMMAND bash ${PARSE_TRANSFER_METHODS_SH} ${TRACE_TRANSFER_METHODS_ROOT} 1)
add_custom_target(parse-direct-write-8K COMMAND bash ${PARSE_TRANSFER_METHODS_SH} ${TRACE_TRANSFER_METHODS_ROOT} 2)
add_custom_target(parse-direct-write-huge-8K COMMAND bash ${PARSE_TRANSFER_METHODS_SH} ${TRACE_TRANSFER_METHODS_ROOT} 3)

add_custom_target(prune-fwrite-8K COMMAND bash ${PRUNE_TRANSFER_METHODS_SH} ${TRACE_TRANSFER_METHODS_ROOT} 0)
add_custom_target(prune-fwrite-huge-8K COMMAND bash ${PRUNE_TRANSFER_METHODS_SH} ${TRACE_TRANSFER_METHODS_ROOT} 1)
add_custom_target(prune-direct-write-8K COMMAND bash ${PRUNE_TRANSFER_METHODS_SH} ${TRACE_TRANSFER_METHODS_ROOT} 2)
add_custom_target(prune-direct-write-huge-8K COMMAND bash ${PRUNE_TRANSFER_METHODS_SH} ${TRACE_TRANSFER_METHODS_ROOT} 3)
