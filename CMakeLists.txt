cmake_minimum_required(VERSION 3.20)
project(os_lab4 LANGUAGES C)

add_executable(child1 inc/child1.c inc/fileMapping.c)
add_executable(child2 inc/child2.c inc/fileMapping.c)
add_executable(parent inc/parent.c inc/fileMapping.c)
