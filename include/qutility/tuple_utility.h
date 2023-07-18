#pragma once

#include <tuple>

#include "c_array.h"

namespace qutility
{
    namespace tuple_utility
    {
        template <typename... Ts>
        auto make_tuple_ptrs(const std::tuple<Ts...> &tuple_data)
        {
            using qutility::c_array::c_array;
            return std::apply(
                [](auto &...args)
                {
                    return (c_array<void *, 1>{(void *)&args} && ...);
                },
                tuple_data);
        }
    }
}
