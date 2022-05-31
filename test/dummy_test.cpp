// c++ headers

// gtest headers
#include <gtest/gtest.h>

// current project header
#include "qutility/c_array.h"
#include "qutility/array_wrapper/array_wrapper_cpu.h"

TEST(DummyTest, DummyTest0)
{
    EXPECT_EQ(0, 0);

    qutility::array_wrapper::DArrayHBW<double> arr(128);
}
