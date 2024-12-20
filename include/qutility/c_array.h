#pragma once

#include <type_traits>
#include <iostream>
#include <limits>

#include "ifmember.h"

namespace qutility
{
	namespace c_array
	{
		template <class T, size_t N>
		struct c_array
		{
			T arr[N];

			constexpr T const &operator[](size_t p) const
			{
				return arr[p];
			}

			constexpr T &operator[](size_t p)
			{
				return arr[p];
			}

			template <typename T2, typename = std::enable_if_t<std::is_convertible_v<T, T2>>>
			constexpr operator c_array<T2, N>() const
			{
				c_array<T2, N> ans{};
				for (std::size_t itr = 0; itr < N; ++itr)
				{
					ans[itr] = arr[itr];
				}
				return ans;
			}

			constexpr T const *begin() const
			{
				return arr + 0;
			}
			constexpr T const *end() const
			{
				return arr + N;
			}

			constexpr size_t size() const
			{
				return N;
			}
		};

		template <class T>
		struct c_array<T, 0>
		{
		};

		template <class T1, class T2, size_t N1, size_t N2>
		constexpr bool operator==(c_array<T1, N1> const &lhs, c_array<T2, N2> const &rhs)
		{
			return false;
		}

		template <class T, size_t N>
		constexpr bool operator==(c_array<T, N> const &lhs, c_array<T, N> const &rhs)
		{
			if constexpr (N > 0)
				for (size_t itr = 0; itr < N; ++itr)
				{
					if (lhs[itr] != rhs[itr])
						return false;
				}
			return true;
		}

		template <class T1, class T2, size_t N1, size_t N2>
		constexpr bool operator!=(c_array<T1, N1> const &lhs, c_array<T2, N2> const &rhs)
		{
			return !(lhs == rhs);
		}

		template <class T, size_t N,
				  typename = typename std::enable_if_t<qutility::ifmember::has_operator_left_shift<std::ostream &, T>::value>>
		std::ostream &operator<<(std::ostream &os, const c_array<T, N> &data)
		{
			os << "{";
			if constexpr (N > 0)
				for (size_t itr = 0; itr < N - 1; ++itr)
				{
					os << data[itr] << ", ";
				}
			os << data[N - 1] << "}";
			return os;
		}

		template <size_t... Is>
		struct seq
		{
		};

		template <size_t N, size_t... Is>
		struct gen_seq : gen_seq<N - 1, N - 1, Is...>
		{
		};

		template <size_t... Is>
		struct gen_seq<0, Is...> : seq<Is...>
		{
		};

		template <class T, size_t N, class U, size_t... Is>
		constexpr c_array<T, N + 1> append_impl(c_array<T, N> const &p, U const &e, seq<Is...>)
		{
			return {{p[Is]..., e}};
		}

		template <class T, size_t N, class U>
		constexpr c_array<T, N + 1> append(c_array<T, N> const &p, U const &e)
		{
			return append_impl(p, e, gen_seq<N>{});
		}

		template <class T, size_t N, size_t... Is>
		constexpr c_array<T, N - 1> pop_impl(c_array<T, N> const &p, seq<Is...>)
		{
			return {{p[Is]...}};
		}

		template <class T, size_t N>
		constexpr c_array<T, N - 1> pop(c_array<T, N> const &p)
		{
			return pop_impl(p, gen_seq<N - 1>{});
		}

		template <class T>
		constexpr c_array<T, 0> pop(c_array<T, 1> const &p)
		{
			return c_array<T, 0>();
		}

		template <class T, size_t N, size_t... Is>
		constexpr c_array<T, N> reverse_impl(c_array<T, N> const &p, seq<Is...>)
		{
			return {{p[N - 1 - Is]...}};
		}

		template <class T, size_t N>
		constexpr c_array<T, N> reverse(c_array<T, N> const &p)
		{
			return reverse_impl(p, gen_seq<N>{});
		}

		template <class T, size_t M, size_t... Is>
		constexpr c_array<T, sizeof...(Is)> pick_last_impl(c_array<T, M> const &p, seq<Is...>)
		{
			return {{p[Is + (M - sizeof...(Is))]...}};
		}

		template <size_t N, class T, size_t M>
		constexpr c_array<T, N> pick_last(c_array<T, M> const &p)
		{
			static_assert((N <= M), "Lenth of sequence must be at least N");
			return pick_last_impl(p, gen_seq<N>{});
		}

		template <typename T, size_t M, size_t N, size_t... Is>
		constexpr c_array<T, M * N> flattern_impl(c_array<c_array<T, M>, N> const &matrix, seq<Is...>)
		{
			return {{(matrix[Is / M][Is % M])...}};
		}

		template <typename T, size_t M, size_t N>
		constexpr c_array<T, M * N> flattern(c_array<c_array<T, M>, N> const &matrix)
		{
			return flattern_impl(matrix, gen_seq<M * N>{});
		}

		template <typename Test, typename U, U... e>
		struct append_case
		{
		};

		template <typename Test, typename U, U e, U... Else>
		struct append_case<Test, U, e, Else...>
		{
			template <typename T, size_t N>
			constexpr static auto apply(c_array<T, N> const &p)
			{
				return append_case<Test, U, Else...>::apply(
					append_case<std::bool_constant<Test::apply(e)>, U, e>::apply(p));
			}
		};

		template <typename Test, typename U, U e>
		struct append_case<Test, U, e>
		{
			template <typename T, size_t N>
			constexpr static auto apply(c_array<T, N> const &p)
			{
				return append_case<std::bool_constant<Test::apply(e)>, U, e>::apply(p);
			}
		};

		template <typename U, U e>
		struct append_case<std::false_type, U, e>
		{
			template <typename T, size_t N>
			constexpr static c_array<T, N> apply(c_array<T, N> const &p) { return p; }
		};

		template <typename U, U e>
		struct append_case<std::true_type, U, e>
		{
			template <typename T, size_t N>
			constexpr static c_array<T, N + 1> apply(c_array<T, N> const &p) { return append(p, e); }
		};

		template <class T, size_t N>
		constexpr T product(c_array<T, N> const &a)
		{
			using AnsT = T;
			AnsT ans = (AnsT)1;
			if constexpr (N != 0)
				for (size_t itr = 0; itr < N; ++itr)
				{
					ans *= a[itr];
				}
			return ans;
		}

		template <class T, size_t N>
		constexpr T sum(c_array<T, N> const &a)
		{
			using AnsT = T;
			AnsT ans = (AnsT)0;
			if constexpr (N != 0)
				for (size_t itr = 0; itr < N; ++itr)
				{
					ans += a[itr];
				}
			return ans;
		}

		template <class T, size_t N>
		constexpr c_array<T, N> prefix_sum(c_array<T, N> const &a)
		{
			using AnsT = c_array<T, N>;
			AnsT ans{};
			if constexpr (N != 0)
			{
				T temp = 0;
				for (size_t itr = 0; itr < N; ++itr)
				{
					ans[itr] = temp;
					temp += a[itr];
				}
			}
			return ans;
		}

		template <class T, size_t N>
		constexpr c_array<T, N> suffix_sum(c_array<T, N> const &a)
		{
			using AnsT = c_array<T, N>;
			AnsT ans{};
			if constexpr (N != 0)
			{
				T temp = 0;
				for (size_t itr = 0; itr < N; ++itr)
				{
					ans[N - 1 - itr] = temp;
					temp += a[N - 1 - itr];
				}
			}
			return ans;
		}

		template <class T, size_t N>
		constexpr c_array<T, N> prefix_product(c_array<T, N> const &a)
		{
			using AnsT = c_array<T, N>;
			AnsT ans{};
			if constexpr (N != 0)
			{
				T temp = 1;
				for (size_t itr = 0; itr < N; ++itr)
				{
					ans[itr] = temp;
					temp *= a[itr];
				}
			}
			return ans;
		}

		template <class T, size_t N>
		constexpr c_array<T, N> suffix_product(c_array<T, N> const &a)
		{
			using AnsT = c_array<T, N>;
			AnsT ans{};
			if constexpr (N != 0)
			{
				T temp = 1;
				for (size_t itr = 0; itr < N; ++itr)
				{
					ans[N - 1 - itr] = temp;
					temp *= a[N - 1 - itr];
				}
			}
			return ans;
		}

		template <class T, class U, size_t N>
		constexpr decltype(std::declval<T>() * std::declval<U>()) inner_product(c_array<T, N> const &a, c_array<U, N> const &b)
		{
			using AnsT = decltype(std::declval<T>() * std::declval<U>());
			AnsT ans = AnsT{};
			if constexpr (N != 0)
				for (size_t itr = 0; itr < N; ++itr)
				{
					ans += a[itr] * b[itr];
				}
			return ans;
		}

		template <class T, class U, size_t N>
		constexpr inline auto operator+(c_array<T, N> const &a, c_array<U, N> const &b)
		{
			using AnsValT = decltype(std::declval<T>() + std::declval<U>());
			using AnsT = c_array<AnsValT, N>;
			AnsT ans{};
			if constexpr (N != 0)
				for (size_t itr = 0; itr < N; ++itr)
				{
					ans[itr] = a[itr] + b[itr];
				}
			return ans;
		}

		template <class T, class U, size_t N>
		constexpr inline auto operator-(c_array<T, N> const &a, c_array<U, N> const &b)
		{
			using AnsValT = decltype(std::declval<T>() - std::declval<U>());
			using AnsT = c_array<AnsValT, N>;
			AnsT ans{};
			if constexpr (N != 0)
				for (size_t itr = 0; itr < N; ++itr)
				{
					ans[itr] = a[itr] - b[itr];
				}
			return ans;
		}

		template <class T, class U, size_t N>
		constexpr inline auto operator*(c_array<T, N> const &a, c_array<U, N> const &b)
		{
			using AnsValT = decltype(std::declval<T>() * std::declval<U>());
			using AnsT = c_array<AnsValT, N>;
			AnsT ans{};
			if constexpr (N != 0)
				for (size_t itr = 0; itr < N; ++itr)
				{
					ans[itr] = a[itr] * b[itr];
				}
			return ans;
		}

		template <class T, class U, size_t N>
		constexpr inline auto operator/(c_array<T, N> const &a, c_array<U, N> const &b)
		{
			using AnsValT = decltype(std::declval<T>() / std::declval<U>());
			using AnsT = c_array<AnsValT, N>;
			AnsT ans{};
			if constexpr (N != 0)
				for (size_t itr = 0; itr < N; ++itr)
				{
					ans[itr] = a[itr] / b[itr];
				}
			return ans;
		}

		// CAUTION: operator&& is used as join to array, instead of operator "and"
		template <typename T, size_t M, size_t N>
		constexpr inline c_array<T, M + N> operator&&(c_array<T, M> const &lhs, c_array<T, N> const &rhs)
		{
			c_array<T, M + N> ans{};
			if constexpr (M != 0)
				for (size_t itr = 0; itr < M; ++itr)
				{
					ans[itr] = lhs[itr];
				}
			if constexpr (N != 0)
				for (size_t itr = 0; itr < N; ++itr)
				{
					ans[itr + M] = rhs[itr];
				}
			return ans;
		}

		template <typename T, size_t... Ms>
		constexpr inline c_array<T, (Ms + ...)> join(c_array<T, Ms> const &...arrs)
		{
			return (arrs && ...);
		}

		template <typename T, size_t M>
		constexpr inline T maximum(c_array<T, M> const &arr)
		{
			T ans = std::numeric_limits<T>::lowest();
			for (size_t itr = 0; itr < M; ++itr)
			{
				ans = ans > arr[itr] ? ans : arr[itr];
			}
			return ans;
		}

		template <size_t N, typename T, size_t M>
		constexpr inline c_array<T, N> add_padding(c_array<T, M> const &arr)
		{
			c_array<T, N> ans{};
			for (size_t itr = 0; itr < M; ++itr)
			{
				ans[itr] = arr[itr];
			}
			return ans;
		}

		template <typename T, size_t M>
		constexpr inline c_array<T, M> copy_of(T const &val)
		{
			c_array<T, M> ans{};
			for (size_t itr = 0; itr < M; ++itr)
			{
				ans[itr] = val;
			}
			return ans;
		}

		template <typename opT, typename T1, size_t M, typename T2>
		constexpr inline size_t pos_first(c_array<T1, M> const &arr, T2 var)
		{
			for (size_t itr = 0; itr < M; ++itr)
			{
				if (opT()(arr[itr], var))
					return itr;
			}
			return M;
		}

		template <typename T, size_t N, typename funcT>
		constexpr c_array<T, N> make_array(funcT f)
		{
			c_array<T, N> ans{};
			if constexpr (N > 0)
			{
				for (size_t itr = 0; itr < N; ++itr)
				{
					ans[itr] = f(itr);
				}
			}
			return ans;
		}
	}
}