cmake_minimum_required(VERSION 3.10)
project(my_project)

# Add the include directory
include_directories(include)

# Add the executable and specify its source files
add_executable(my_program src/main.c src/utils.c)

