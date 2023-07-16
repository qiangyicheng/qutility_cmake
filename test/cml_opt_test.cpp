// c++ headers
#include <string>

// gtest headers
#include <gtest/gtest.h>

// current project header
#include "qutility/cml_opt.h"

class TestSimOptions : public qutility::cml_opt::SimOptionsBase
{
public:
    QUTILITY_SIM_REGISTER_OPTIONS_INITIALIZE(TestSimOptions);
    QUTILITY_SIM_REGISTER_OPTIONS(TestSimOptions, std::string, str, "str", "test string", "test string");
    QUTILITY_SIM_REGISTER_OPTIONS(TestSimOptions, double, d, "d", 0.1, "test double");
    QUTILITY_SIM_REGISTER_OPTIONS(TestSimOptions, size_t, i, "i", 233, "test double");

};

TEST(CmlOpt, ReadOptions)
{
    constexpr int test_argc=5;
    const char *test_argv[test_argc]={"??not important", "--str=hahaha", "--i=42", "--d=0.233", "--help"};
    TestSimOptions opts;
    
    qutility::cml_opt::obtain_cml_options(test_argc,test_argv, opts);
    EXPECT_EQ(opts.str, std::string{"hahaha"});
    EXPECT_EQ(opts.i, 42);
    EXPECT_EQ(opts.d, 0.233);
}