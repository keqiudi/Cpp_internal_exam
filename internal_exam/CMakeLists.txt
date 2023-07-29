cmake_minimum_required(VERSION 3.22.1)
project(internal_exam)

set(CMAKE_CXX_STANDARD 17)

add_executable(internal_exam Src/main.cpp Src/Socket.cpp Inc/Socket.h Inc/Socket.h Src/Socket.cpp Src/packet.cpp Inc/packet.h Inc/packet.h Src/threadPool.cpp Inc/threadPool.h Inc/threadPool.h Src/threadPool.cpp Src/taskQueue.cpp Inc/taskQueue.h Inc/taskQueue.h Src/User.cpp Inc/User.h Inc/User.h Src/bioPacket.cpp Inc/bioPacket.h Src/doorLock.cpp Inc/doorLock.h Inc/doorLockMenu.h Src/doorLockMenu.cpp Src/doorLockMenu.cpp)
