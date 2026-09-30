cmake_minimum_required(VERSION 3.30)
project(compilador C)

set(CMAKE_C_STANDARD 11)

add_executable(compilador main.c
        compilador.h
        beamer.h
        beamer.c
        prueba2.c
        prueba3.c
        prueba4.c
        prueba4Config.h
        prueba4Geo.h
        prueba5.c
)
