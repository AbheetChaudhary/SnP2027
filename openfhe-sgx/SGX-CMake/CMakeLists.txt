cmake_minimum_required(VERSION 4.2.1)
project(sgx_cmake)

option(BUILD_SAMPLE "Build sample" OFF)
if(BUILD_SAMPLE)
    add_subdirectory(sample)
endif()
