cmake_minimum_required(VERSION 3.10)
project(LoginApp)

set(CMAKE_CXX_STANDARD 17)

include(Findsodium.cmake)

add_executable(LoginApp main.cpp user.cpp user.h)

target_link_libraries(LoginApp ${sodium_LIBRARY_RELEASE})