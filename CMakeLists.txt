cmake_minimum_required(VERSION 3.20)
project(Intro_to_Computer_Communications_HW1 C)

set(CMAKE_C_STANDARD 11)

include_directories(Utils)

add_executable(Channel
        Channel/Channel.c
        Utils/Common.h
        Utils/Common.c
        Utils/WinSock_handlers.h
        Utils/WinSock_handlers.c)

add_executable(Receiver
        Receiver/Receiver.c
        Utils/Common.h
        Utils/Common.c
        Utils/WinSock_handlers.h
        Utils/WinSock_handlers.c)

add_executable(Sender
        Sender/Sender.c
        Utils/Common.h
        Utils/Common.c
        Utils/WinSock_handlers.h
        Utils/WinSock_handlers.c)