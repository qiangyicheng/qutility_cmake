# Include Google Test

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/2f80c2ba71c0e8922a03b9b855e5b019ad1f7064.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
include(GoogleTest)

# array_wrapper_cpu
add_executable(
    array_wrapper_cpu_test
    array_wrapper_cpu_test.cpp
)

target_link_libraries(array_wrapper_cpu_test
    PRIVATE 
    ${PROJECT_NAME}
    gtest_main
)

gtest_discover_tests(array_wrapper_cpu_test)

# array_wrapper_gpu
if(${CUDAToolkit_FOUND})
    add_executable(
        array_wrapper_gpu_test
        array_wrapper_gpu_test.cu
    )

    target_link_libraries(array_wrapper_gpu_test
        PRIVATE 
        ${PROJECT_NAME}
        gtest_main
    )

    gtest_discover_tests(array_wrapper_gpu_test)
endif()

# dim
add_executable(
    box_test
    box_test.cpp
)

target_link_libraries(box_test
    PRIVATE 
    ${PROJECT_NAME}
    gtest_main
)

gtest_discover_tests(box_test)
