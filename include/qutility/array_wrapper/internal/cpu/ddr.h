#pragma once

#include "base.h"
#include "boost/align.hpp"

namespace qutility {
	namespace array_wrapper {
		template <class T, std::size_t A = 64>
		using DArrayDDR = ArrayCPU<T, boost::alignment::aligned_allocator<T, A>, A>;

		template <class T, std::size_t A = 64>
		using ArrayDDR = ArrayCPU<T, boost::alignment::aligned_allocator<T, A>, A>;
	}
}
