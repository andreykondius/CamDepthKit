cmake_minimum_required(VERSION 3.5)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(CamDepthKit VERSION 0.1 LANGUAGES CXX)

set(3RDPARTY_LIB 3rdparty_lib)
add_subdirectory(sources/3rdparty)
add_subdirectory(sources/kit)
