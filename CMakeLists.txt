cmake_minimum_required(VERSION 3.5)
project(image)

set(CMAKE_CXX_STANDARD 17)

if(APPLE)
    enable_language(OBJC)
endif()
add_subdirectory(vendor)
add_subdirectory(include)
add_subdirectory(src)

