// c++ headers

// gtest headers
#include <gtest/gtest.h>

// current project header
#include "qutility/array_wrapper/array_wrapper_cpu.h"

TEST(ArrayWrapperCPU, DArrayHBWTrivial)
{
    qutility::array_wrapper::DArrayHBW<double> arr(0., 128);
    EXPECT_DOUBLE_EQ(0., arr[0]);
}
