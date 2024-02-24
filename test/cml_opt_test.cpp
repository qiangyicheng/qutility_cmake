// c++ headers
#include <string>

// gtest headers
#include <gtest/gtest.h>

// current project header
#include "qutility/cml_opt.h"

class TestOptions : public qutility::cml_opt::OptionsBase
{
public:
    QUTILITY_REGISTER_OPTION_INITIALIZE(TestOptions);
    QUTILITY_REGISTER_OPTION_WITH_DEFAULT(TestOptions, std::string, str, "string", "str", "test string", "test string");
    QUTILITY_REGISTER_OPTION_WITH_DEFAULT(TestOptions, double, d, "double", "d", 0.1, "test double");
    QUTILITY_REGISTER_OPTION_WITH_DEFAULT(TestOptions, size_t, i, "integer", "i", 233, "test integer");
};

TEST(CmlOpt, ReadOptions)
{
    constexpr int test_argc = 4;
    const char *test_argv[test_argc] = {"??not important", "--str=hahaha", "--i=42", "--d=0.233"};
    TestOptions opts;

    qutility::cml_opt::obtain_cml_options(test_argc, test_argv, opts);
    EXPECT_EQ(opts.str, std::string{"hahaha"});
    EXPECT_EQ(opts.i, 42);
    EXPECT_EQ(opts.d, 0.233);
}