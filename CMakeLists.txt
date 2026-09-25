cmake_minimum_required(VERSION 3.30)
project(compilador C)

set(CMAKE_C_STANDARD 11)

add_executable(compilador main.c
        compilador.h
        beamer.h
        prueba.c)
