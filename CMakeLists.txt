cmake_minimum_required(VERSION 3.22)
project(OS C)

include_directories("../tools")
set(CMAKE_C_STANDARD 99)

add_executable(OS main.c ./tools/fileinfo.c ./tools/floppy.c oskernel/init/main.c)
