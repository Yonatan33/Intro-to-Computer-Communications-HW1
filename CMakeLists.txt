cmake_minimum_required(VERSION 3.17)
project(Intro_to_Computer_Communications_HW1 C)

set(CMAKE_C_STANDARD 11)

include_directories(Utils)

add_executable(Channel
        Channel/Channel.c
        Common.h
        Common.c
        WinSock_handlers.h
        WinSock_handlers.c)

add_executable(Receiver
        Receiver/Receiver.c
        Common.h
        Common.c
        WinSock_handlers.h
        WinSock_handlers.c)

add_executable(Sender
        Sender/Sender.c
        Common.h
        Common.c
        WinSock_handlers.h
        WinSock_handlers.c)