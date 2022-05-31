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

add_executable(
  dummy_test
  dummy_test.cpp
)

target_link_libraries(dummy_test
  PRIVATE 
  ${PROJECT_NAME}
  gtest_main
)

include(GoogleTest)
gtest_discover_tests(dummy_test)
