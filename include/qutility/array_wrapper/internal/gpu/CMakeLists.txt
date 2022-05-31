set(CURRENT_TARGET_NAME qutility_array_wrapper_gpu)

add_library(${CURRENT_TARGET_NAME} INTERFACE)

if(${CUDAToolkit_FOUND})
    target_sources(${CURRENT_TARGET_NAME}
        INTERFACE
        cuda.h
    )
    target_link_libraries(${CURRENT_TARGET_NAME} INTERFACE CUDA::cudart)
    target_include_directories(${CURRENT_TARGET_NAME} INTERFACE ${CUDAToolkit_BIN_DIR}/../samples/common/inc/)
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