# Include Google Test

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY git@github.com:google/googletest.git
    GIT_TAG main
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
include(GoogleTest)

# array_wrapper_cpu
set(CURRENT_TEST_NAME array_wrapper_cpu)
add_executable(
    ${CURRENT_TEST_NAME}_test
    ${CURRENT_TEST_NAME}_test.cpp
)

target_link_libraries(${CURRENT_TEST_NAME}_test
    PRIVATE
    ${PROJECT_NAME}
    gtest_main
)

gtest_discover_tests(${CURRENT_TEST_NAME}_test)

# array_wrapper_gpu
if(${CUDAToolkit_FOUND})
    set(CURRENT_TEST_NAME array_wrapper_gpu)
    add_executable(
        ${CURRENT_TEST_NAME}_test
        ${CURRENT_TEST_NAME}_test.cu
    )

    target_link_libraries(${CURRENT_TEST_NAME}_test
        PRIVATE
        ${PROJECT_NAME}
        gtest_main
    )

    gtest_discover_tests(${CURRENT_TEST_NAME}_test)
endif()

# box
set(CURRENT_TEST_NAME box)
add_executable(
    ${CURRENT_TEST_NAME}_test
    ${CURRENT_TEST_NAME}_test.cpp
)

target_link_libraries(${CURRENT_TEST_NAME}_test
    PRIVATE
    ${PROJECT_NAME}
    gtest_main
)

gtest_discover_tests(${CURRENT_TEST_NAME}_test)

# cml_opt
set(CURRENT_TEST_NAME cml_opt)
add_executable(
    ${CURRENT_TEST_NAME}_test
    ${CURRENT_TEST_NAME}_test.cpp
)

target_link_libraries(${CURRENT_TEST_NAME}_test
    PRIVATE
    ${PROJECT_NAME}
    gtest_main
)

gtest_discover_tests(${CURRENT_TEST_NAME}_test)

# traits
set(CURRENT_TEST_NAME traits)
add_executable(
    ${CURRENT_TEST_NAME}_test
    ${CURRENT_TEST_NAME}_test.cpp
)

target_link_libraries(${CURRENT_TEST_NAME}_test
    PRIVATE
    ${PROJECT_NAME}
    gtest_main
)

gtest_discover_tests(${CURRENT_TEST_NAME}_test)
