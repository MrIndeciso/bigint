cmake_minimum_required(VERSION 3.5)

project(bigint LANGUAGES C)

add_executable(
        bigint
        src/test.c
        src/bigint.h
        src/bigint.c
        src/util.h
        src/util.c
)

add_compile_options(-Wall -Wextra)

add_compile_options(-O2 -march=native)
