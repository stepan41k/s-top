cmake_minimum_required(VERSION 3.10)
project(sysmon C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -O2)

include_directories(include)

find_package(Curses REQUIRED)

file(GLOB SOURCES "src/*.c")

add_executable(sysmon ${SOURCES})

target_link_libraries(sysmon PRIVATE ${CURSES_LIBRARIES})