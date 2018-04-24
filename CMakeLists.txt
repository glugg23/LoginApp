cmake_minimum_required(VERSION 3.10)
project(LoginApp)

set(CMAKE_CXX_STANDARD 17)

include(Findsodium.cmake)
find_package(libmongocxx REQUIRED)
find_package(libbsoncxx REQUIRED)

add_executable(LoginApp main.cpp user.cpp user.h PRIVATE.h database.cpp database.h)

find_package(libmongocxx REQUIRED)
find_package(libbsoncxx REQUIRED)
file(GLOB COMMON_LIBRARIES ${LIBMONGOCXX_LIBRARIES} ${LIBBSONCXX_LIBRARIES})

target_include_directories(LoginApp PUBLIC ${LIBMONGOCXX_INCLUDE_DIRS})
target_include_directories(LoginApp PUBLIC ${LIBBSONCXX_INCLUDE_DIRS})
target_link_libraries(LoginApp ${COMMON_LIBRARIES})

target_link_libraries(LoginApp ${sodium_LIBRARY_RELEASE})