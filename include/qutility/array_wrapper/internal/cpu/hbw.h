#pragma once

#include "base.h"
#include "hbw_posix_allocator.h"

namespace qutility {
	namespace array_wrapper {
		template <class T, std::size_t A = 64>
		using DArrayHBW = ArrayCPU<T, hbw::allocator<T, A>, A>;

		template <class T, std::size_t A = 64>
		using ArrayHBW = ArrayCPU<T, hbw::allocator<T, A>, A>;
	}
}
