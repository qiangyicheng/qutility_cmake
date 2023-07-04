// c++ headers

// gtest headers
#include <gtest/gtest.h>

// current project header
#include "qutility/box.h"

TEST(Box, CountNonUnit)
{
    using qutility::c_array::c_array;
    using arrT = c_array<std::size_t, 4>;

    {
        constexpr auto ans = qutility::box::detail::count_non_unit(arrT{0, 0, 0, 1});
        EXPECT_EQ(ans, 0);
    }
    {
        constexpr auto ans = qutility::box::detail::count_non_unit(arrT{0, 3, 0, 1});
        EXPECT_EQ(ans, 1);
    }
    {
        constexpr auto ans = qutility::box::detail::count_non_unit(arrT{3, 4, 2, 1});
        EXPECT_EQ(ans, 3);
    }
}

TEST(Box, RemoveZero)
{
    using qutility::c_array::c_array;
    using arrT = c_array<std::size_t, 4>;

    {
        constexpr auto ans = qutility::box::detail::remove_zero(arrT{0, 0, 0, 1});
        EXPECT_EQ(ans, (arrT{1, 1, 1, 1}));
    }
    {
        constexpr auto ans = qutility::box::detail::remove_zero(arrT{0, 3, 0, 1});
        EXPECT_EQ(ans, (arrT{1, 3, 1, 1}));
    }
    {
        constexpr auto ans = qutility::box::detail::remove_zero(arrT{3, 4, 2, 1});
        EXPECT_EQ(ans, (arrT{3, 4, 2, 1}));
    }
}

TEST(Box, Compress)
{
    using qutility::c_array::c_array;
    using arrT = c_array<std::size_t, 4>;

    {
        constexpr auto ans = qutility::box::detail::compress(arrT{0, 0, 0, 1});
        EXPECT_EQ(ans, (arrT{0, 0, 0, 1}));
    }
    {
        constexpr auto ans = qutility::box::detail::compress(arrT{0, 3, 0, 1});
        EXPECT_EQ(ans, (arrT{0, 0, 1, 3}));
    }
    {
        constexpr auto ans = qutility::box::detail::compress(arrT{3, 4, 2, 1});
        EXPECT_EQ(ans, (arrT{1, 3, 4, 2}));
    }
}

TEST(Box, ArgCompress)
{
    using qutility::c_array::c_array;
    using arrT = c_array<std::size_t, 4>;

    {
        constexpr auto ans = qutility::box::detail::arg_compress(arrT{0, 0, 0, 1});
        EXPECT_EQ(ans, (arrT{0, 1, 2, 3}));
    }
    {
        constexpr auto ans = qutility::box::detail::arg_compress(arrT{0, 3, 0, 1});
        EXPECT_EQ(ans, (arrT{0, 2, 3, 1}));
    }
    {
        constexpr auto ans = qutility::box::detail::arg_compress(arrT{3, 4, 2, 1});
        EXPECT_EQ(ans, (arrT{3, 0, 1, 2}));
    }
}

TEST(Box, Base)
{
    using BoxBaseT = qutility::box::BoxBase<3>;
    constexpr BoxBaseT box{{1, 2, 3}};
    EXPECT_EQ(6, box.total_size_);
}

TEST(Box, Box)
{
    using qutility::c_array::c_array;
    using BoxT = qutility::box::Box<6>;
    constexpr BoxT box{{4, 1, 6, 0, 1, 3}};
    EXPECT_EQ(72, box.original_box_.total_size_);
    EXPECT_EQ(72, box.compressed_box_.total_size_);
    EXPECT_EQ(48, box.compressed_box_.total_size_hermit_);

    constexpr auto simple_stride = box.compressed_box_.stride_;
    EXPECT_EQ(simple_stride, (c_array<std::size_t, 6>{{72, 72, 72, 18, 3, 1}}));

    constexpr auto lengths_compressed = box.shuffle_as_compress_index<double>({3.0, 6.0, 2.0, 0.1, 0.2, 0.3});
    EXPECT_EQ(lengths_compressed, (c_array<double, 6>{{6.0, 0.1, 0.2, 3.0, 2.0, 0.3}}));
}
