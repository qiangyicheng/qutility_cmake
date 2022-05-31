# CMakeLists files in this project can
# refer to the root source directory of the project as ${PROJECT_SOURCE_DIR} and
# to the root binary directory of the project as ${PROJECT_BINARY_DIR}.

cmake_minimum_required(VERSION 3.14)
set (CMAKE_CXX_STANDARD 17) 
set (CMAKE_CXX_STANDARD_REQUIRED True)

find_package(CUDAToolkit)

if(${CUDAToolkit_FOUND})
    project (qutility LANGUAGES CUDA CXX)
else()
    project (qutility LANGUAGES CXX)
endif()

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE include/)

add_subdirectory(include)

enable_testing()
add_subdirectory(test)