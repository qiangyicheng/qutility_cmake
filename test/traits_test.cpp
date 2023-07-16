// c++ headers

// gtest headers
#include <gtest/gtest.h>

// current project header
#include "qutility/traits.h"

void test_func1(double *, int *, double, std::size_t);
double *test_func2(const double *, int *, double, std::size_t);
double *test_func3(const double *, int *, double &, const std::size_t &);

TEST(Traits, Function)
{
    using ArgsT1 = qutility::traits::func_args_tuple<decltype(test_func1)>;
    using ArgsT2 = qutility::traits::func_args_tuple<decltype(test_func2)>;
    using ArgsT3 = qutility::traits::func_args_tuple<decltype(test_func3)>;
}