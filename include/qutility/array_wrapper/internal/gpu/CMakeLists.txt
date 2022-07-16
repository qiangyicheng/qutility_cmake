set(CURRENT_TARGET_NAME qutility_array_wrapper_gpu)

add_library(${CURRENT_TARGET_NAME} INTERFACE)

if(${CUDAToolkit_FOUND})
    target_sources(${CURRENT_TARGET_NAME}
        INTERFACE
        cuda.h
    )
    target_link_libraries(${CURRENT_TARGET_NAME} INTERFACE CUDA::cudart)
    if(EXISTS "${CUDAToolkit_BIN_DIR}/../samples/common/inc/helper_cuda.h")
        message(STATUS "Header helper_cuda.h is found in ${CUDAToolkit_BIN_DIR}/../samples/common/inc/")
        target_include_directories(${CURRENT_TARGET_NAME} INTERFACE ${CUDAToolkit_BIN_DIR}/../samples/common/inc/)
    else()
        include(FetchContent)

        FetchContent_Declare(cuda-samples
            GIT_REPOSITORY    https://github.com/NVIDIA/cuda-samples.git
            GIT_TAG           master
        )
        
        if(NOT cuda-samples_POPULATED)
            FetchContent_Populate(cuda-samples)
            target_include_directories(${CURRENT_TARGET_NAME} INTERFACE ${cuda-samples_SOURCE_DIR}/common)
        endif()
    endif()
endif()

if(${hip_FOUND} AND ${rocthrust_FOUND})
    target_sources(${CURRENT_TARGET_NAME}
        INTERFACE
        hip.h
        helper_hip.h
    )
    target_link_libraries(${CURRENT_TARGET_NAME} INTERFACE hip::host roc::rocthrust)
endif()

target_sources(${CURRENT_TARGET_NAME}
    INTERFACE
    base.h
    defs.h
)