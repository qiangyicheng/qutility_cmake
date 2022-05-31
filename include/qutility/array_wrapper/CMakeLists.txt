set(CURRENT_TARGET_NAME qutility_array_wrapper)
add_library(${CURRENT_TARGET_NAME} INTERFACE)

find_package(Boost 1.61)

find_package(CUDAToolkit)
find_package(hip QUIET)
find_package(rocthrust QUIET)

add_subdirectory(internal/common)

if(${Boost_FOUND})
    message(STATUS "Boost with version>=1.61 is found. CPU array wrappers will be enabled")
    add_subdirectory(internal/cpu)
    target_link_libraries(${CURRENT_TARGET_NAME} INTERFACE qutility_array_wrapper_cpu)
else()
    message(STATUS "Boost with version>=1.61 is not found. CPU array wrappers will be disabled")
    target_compile_definitions(${CURRENT_TARGET_NAME} INTERFACE QUTILITY_ARRAY_WRAPPER_DISABLE_CPU_WRAPPER)
endif()

set(QUTILITY_ARRAY_WRAPPER_USING_GPU_WRAPPER False)
if(${CUDAToolkit_FOUND})
    message(STATUS "CUDA is found. GPU array wrappers will be enabled and use thrust from CUDA directly")
    set(QUTILITY_ARRAY_WRAPPER_USING_GPU_WRAPPER True)
    target_compile_definitions(${CURRENT_TARGET_NAME} INTERFACE QUTILITY_ARRAY_WRAPPER_GPU_USE_CUDA)
else()
    if(${hip_FOUND} AND ${rocthrust_FOUND})
        message(STATUS "CUDA is not found but hip and rocthrust are found instead. GPU array wrappers will be enabled and use hip and rocthrust interface")
        set(QUTILITY_ARRAY_WRAPPER_USING_GPU_WRAPPER True)
        target_compile_definitions(${CURRENT_TARGET_NAME} INTERFACE QUTILITY_ARRAY_WRAPPER_GPU_USE_HIP)
    else()
        message(STATUS "Both CUDA and hip are not found. GPU array wrappers will be disabled")
        target_compile_definitions(${CURRENT_TARGET_NAME} INTERFACE QUTILITY_ARRAY_WRAPPER_DISABLE_GPU_WRAPPER)
    endif()
endif()

if(${QUTILITY_ARRAY_WRAPPER_USING_GPU_WRAPPER})
    add_subdirectory(internal/gpu)
    target_link_libraries(${CURRENT_TARGET_NAME} INTERFACE qutility_array_wrapper_gpu)
endif()

target_sources(${PROJECT_NAME}
    INTERFACE
    array_wrapper_cpu.h
    array_wrapper_gpu.h
)