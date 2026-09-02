cmake_minimum_required(VERSION 3.25)
project(HighEfficiencyQueue)

set(CMAKE_CXX_STANDARD 20)

add_executable(
    HighEfficiencyQueue
    main.cpp
    IMUData.hpp
    utils.hpp
)
