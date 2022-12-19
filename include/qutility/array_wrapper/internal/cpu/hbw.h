#pragma once

#include "boost/predef.h"

#include "base.h"

#if BOOST_ARCH_X86

#include "hbw_posix_allocator.h"

namespace qutility {
	namespace array_wrapper {
		template <class T, std::size_t A = 64>
		using DArrayHBW = ArrayCPU<T, hbw::allocator<T, A>, A>;

		template <class T, std::size_t A = 64>
		using ArrayHBW = ArrayCPU<T, hbw::allocator<T, A>, A>;
	}
}

#elif BOOST_ARCH_ARM

namespace qutility {
	namespace array_wrapper {

		template <class T, std::size_t A = 64>
		using DArrayHBW = ArrayCPU<T, boost::alignment::aligned_allocator<T, A>, A>;

		template <class T, std::size_t A = 64>
		using ArrayHBW = ArrayCPU<T, boost::alignment::aligned_allocator<T, A>, A>;
	}
}

#endif
