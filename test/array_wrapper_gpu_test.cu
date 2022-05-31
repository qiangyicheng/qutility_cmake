// c++ headers

// gtest headers
#include <gtest/gtest.h>

// current project header
#include "qutility/array_wrapper/array_wrapper_gpu.h"

TEST(ArrayWrapperGPU, DArrayGPUTrivial)
{
    int device = 0;
    qutility::array_wrapper::DArrayGPU<double> arr(0., 128, device);
    EXPECT_DOUBLE_EQ(0., arr[0]);
}
