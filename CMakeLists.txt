# CMakeLists files in this project can
# refer to the root source directory of the project as ${PROJECT_SOURCE_DIR} and
# to the root binary directory of the project as ${PROJECT_BINARY_DIR}.

cmake_minimum_required(VERSION 3.18)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(CheckLanguage)
check_language(CUDA)
if(CMAKE_CUDA_COMPILER)
    message(STATUS "CUDA is enabled for qutility")
    project(qutility LANGUAGES CUDA CXX)
else()
    message(STATUS "CUDA is disabled for qutility")
    project(qutility LANGUAGES CXX)
endif()
find_package(CUDAToolkit)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE include/)

add_subdirectory(include)

enable_testing()
add_subdirectory(test)