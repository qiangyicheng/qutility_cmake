#pragma once

#include "c_array.h"
#include "traits.h"

namespace qutility
{
    namespace box
    {
        namespace detail
        {
            using qutility::c_array::c_array;
            using qutility::c_array::product;

            template <typename ValT, std::size_t N>
            constexpr inline auto count_non_unit(const c_array<ValT, N> &arr) -> std::size_t
            {
                static_assert(std::is_integral<ValT>::value, "It makes no sense to using non-integral type as the mesh size.");
                constexpr ValT unity = (ValT)1;
                std::size_t count = 0;
                for (const auto &ele : arr)
                {
                    if (ele > unity)
                    {
                        count++;
                    }
                }
                return count;
            }

            template <typename ValT, std::size_t N>
            constexpr inline auto remove_zero(const c_array<ValT, N> &arr) -> const c_array<ValT, N>
            {
                static_assert(std::is_integral<ValT>::value, "It makes no sense to using non-integral type as the mesh size.");
                constexpr ValT zero = (ValT)0;
                constexpr ValT unity = (ValT)1;
                c_array<ValT, N> ans{};
                for (size_t itr = 0; itr < N; ++itr)
                {
                    ans[itr] = arr[itr] == zero ? unity : arr[itr];
                }
                return ans;
            }

            template <typename ValT, std::size_t N>
            constexpr inline auto compress(const c_array<ValT, N> &arr) -> const c_array<ValT, N>
            {
                static_assert(std::is_integral<ValT>::value, "It makes no sense to using non-integral type as the mesh size.");
                constexpr ValT zero = (ValT)0;
                constexpr ValT unity = (ValT)1;
                c_array<ValT, N> ans{};
                std::size_t pos = N;
                std::size_t useless_pos = N - count_non_unit(arr);

                for (std::size_t itr = N; itr > 0; --itr)
                {
                    if (arr[itr - 1] > unity)
                    {
                        ans[pos - 1] = arr[itr - 1];
                        --pos;
                    }
                    else
                    {
                        ans[useless_pos - 1] = arr[itr - 1];
                        --useless_pos;
                    }
                }
                return ans;
            }

            template <typename ValT, std::size_t N>
            constexpr inline auto arg_compress(const c_array<ValT, N> &arr) -> const c_array<std::size_t, N>
            {
                static_assert(std::is_integral<ValT>::value, "It makes no sense to using non-integral type as the mesh size.");
                constexpr ValT zero = (ValT)0;
                constexpr ValT unity = (ValT)1;
                c_array<std::size_t, N> ans{};
                std::size_t pos = N;
                std::size_t useless_pos = N - count_non_unit(arr);

                for (std::size_t itr = N; itr > 0; --itr)
                {
                    if (arr[itr - 1] > unity)
                    {
                        ans[pos - 1] = itr - 1;
                        --pos;
                    }
                    else
                    {
                        ans[useless_pos - 1] = itr - 1;
                        --useless_pos;
                    }
                }
                return ans;
            }

            template <typename ValT, std::size_t N>
            constexpr inline auto hermit(const c_array<ValT, N> &arr) -> c_array<ValT, N>
            {
                static_assert(std::is_integral<ValT>::value, "It makes no sense to using non-integral type as the mesh size.");
                c_array<ValT, N> ans{arr};
                ans[N - 1] = ans[N - 1] / 2 + 1;
                return ans;
            }

            template <typename ValT, std::size_t N>
            constexpr inline auto stride(const c_array<ValT, N> &arr) -> c_array<ValT, N>
            {
                static_assert(std::is_integral<ValT>::value, "It makes no sense to using non-integral type as the mesh size.");
                ValT dist = (ValT)1;
                c_array<ValT, N> ans{};
                for (std::size_t itr = N; itr > 0; --itr)
                {
                    ans[itr - 1] = dist;
                    dist *= arr[itr - 1];
                }
                return ans;
            }

        }

        using qutility::c_array::c_array;

        template <std::size_t Dim, typename ValT = std::size_t>
        struct BoxBase
        {
            using val_t = ValT;
            static constexpr std::size_t dim_ = Dim;
            constexpr BoxBase(c_array<ValT, Dim> box_size)
                : box_size_(detail::remove_zero(box_size)),
                  stride_(detail::stride(box_size_)),          // here use the non-zero version
                  box_size_hermit_(detail::hermit(box_size_)), // here use the non-zero version
                  stride_hermit_(detail::stride(box_size_hermit_)),
                  total_size_(detail::product(box_size_)),
                  total_size_hermit_(detail::product(box_size_hermit_)),
                  last_size_(box_size_[Dim - 1]),
                  last_size_hermit_(box_size_hermit_[Dim - 1]),
                  outer_size_(total_size_ / last_size_),
                  outer_size_hermit_(outer_size_)
            {
            }
            /* data */
            const c_array<ValT, dim_> box_size_;
            const c_array<ValT, dim_> stride_;
            // size of box with Hermitian symmetry
            const c_array<ValT, dim_> box_size_hermit_;
            const c_array<ValT, dim_> stride_hermit_;
            const ValT total_size_;
            const ValT total_size_hermit_;
            const ValT last_size_;
            const ValT last_size_hermit_;
            const ValT outer_size_;
            const ValT outer_size_hermit_;
        };

        /// @brief this class will construct the description of a computational box. \
        Any size of 0 will be considered as 1.\
        Two versions of the box will be kept, one for the original one, the other one for the compressed one: any dim<=1 will be dropped.
        template <std::size_t Dim, typename ValT = std::size_t>
        struct Box
        {
            using val_t = ValT;
            static constexpr std::size_t dim_ = Dim;
            constexpr Box(c_array<ValT, Dim> box_size)
                : original_box_(box_size),
                  compressed_box_(detail::compress(box_size)),
                  compress_index_(detail::arg_compress(box_size)),
                  useful_dim_(detail::count_non_unit(box_size))
            {
            }
            /* data */
            const BoxBase<Dim, ValT> original_box_;
            const BoxBase<Dim, ValT> compressed_box_;
            const c_array<std::size_t, Dim> compress_index_;
            const std::size_t useful_dim_;

            template <typename DataT>
            constexpr auto shuffle_as_compress_index(const c_array<DataT, Dim> &in) const -> c_array<DataT, Dim>
            {
                c_array<DataT, Dim> out{};
                for (std::size_t itr = 0; itr < Dim; ++itr)
                {
                    out[itr] = in[compress_index_[itr]];
                }
                return out;
            }
        };

        template <std::size_t Dim, typename ValT = std::size_t, typename LengthValT = double>
        struct BoxWithLength : public Box<Dim, ValT>
        {
            using val_t = ValT;
            using length_val_t = LengthValT;
            using BoxT = Box<Dim, ValT>;
            static constexpr std::size_t dim_ = Dim;
            constexpr BoxWithLength(c_array<ValT, Dim> box_size, c_array<LengthValT, Dim> box_length)
                : BoxT(box_size),
                  box_length_(box_length),
                  box_discretization_(box_length_ / this->original_box_.box_size_),
                  compressed_box_length_(this->shuffle_as_compress_index(box_length_)),
                  compressed_box_discretization_(compressed_box_length_ / this->compressed_box_.box_size_)
            {
            }
            /* data */
            const c_array<LengthValT, Dim> box_length_;
            const c_array<LengthValT, Dim> box_discretization_;
            const c_array<LengthValT, Dim> compressed_box_length_;
            const c_array<LengthValT, Dim> compressed_box_discretization_;
        };
    }
}