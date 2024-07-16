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
            using qutility::c_array::prefix_product;
            using qutility::c_array::product;
            using qutility::c_array::suffix_product;

            template <typename val_t, std::size_t N>
            constexpr inline auto count_non_unit(const c_array<val_t, N> &arr) -> std::size_t
            {
                static_assert(std::is_integral<val_t>::value, "It makes no sense to using non-integral type as the mesh size.");
                constexpr val_t unity = (val_t)1;
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

            template <typename val_t, std::size_t N>
            constexpr inline auto remove_zero(const c_array<val_t, N> &arr) -> const c_array<val_t, N>
            {
                static_assert(std::is_integral<val_t>::value, "It makes no sense to using non-integral type as the mesh size.");
                constexpr val_t zero = (val_t)0;
                constexpr val_t unity = (val_t)1;
                c_array<val_t, N> ans{};
                for (size_t itr = 0; itr < N; ++itr)
                {
                    ans[itr] = arr[itr] == zero ? unity : arr[itr];
                }
                return ans;
            }

            template <typename val_t, std::size_t N>
            constexpr inline auto compress(const c_array<val_t, N> &arr) -> const c_array<val_t, N>
            {
                static_assert(std::is_integral<val_t>::value, "It makes no sense to using non-integral type as the mesh size.");
                constexpr val_t zero = (val_t)0;
                constexpr val_t unity = (val_t)1;
                c_array<val_t, N> ans{};
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

            template <typename val_t, std::size_t N>
            constexpr inline auto arg_compress(const c_array<val_t, N> &arr) -> const c_array<std::size_t, N>
            {
                static_assert(std::is_integral<val_t>::value, "It makes no sense to using non-integral type as the mesh size.");
                constexpr val_t zero = (val_t)0;
                constexpr val_t unity = (val_t)1;
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

            template <typename val_t, std::size_t N>
            constexpr inline auto hermit(const c_array<val_t, N> &arr) -> c_array<val_t, N>
            {
                static_assert(std::is_integral<val_t>::value, "It makes no sense to using non-integral type as the mesh size.");
                c_array<val_t, N> ans{arr};
                ans[N - 1] = ans[N - 1] / 2 + 1;
                return ans;
            }

            template <typename val_t, std::size_t N>
            constexpr inline auto stride(const c_array<val_t, N> &arr) -> c_array<val_t, N>
            {
                static_assert(std::is_integral<val_t>::value, "It makes no sense to using non-integral type as the mesh size.");
                val_t dist = (val_t)1;
                c_array<val_t, N> ans{};
                for (std::size_t itr = N; itr > 0; --itr)
                {
                    ans[itr - 1] = dist;
                    dist *= arr[itr - 1];
                }
                return ans;
            }

        }

        using qutility::c_array::c_array;
        using qutility::c_array::copy_of;

        template <std::size_t Dim, typename ValT = std::size_t>
        struct BoxBase
        {
            using val_t = ValT;
            static constexpr std::size_t dim_ = Dim;
            constexpr BoxBase(c_array<val_t, dim_> box_size)
                : box_size_(detail::remove_zero(box_size)),
                  stride_(detail::stride(box_size_)),          // here use the non-zero version
                  box_size_hermit_(detail::hermit(box_size_)), // here use the non-zero version
                  stride_hermit_(detail::stride(box_size_hermit_)),
                  outer_size_of_(detail::prefix_product(box_size_)),
                  inner_size_of_(detail::suffix_product(box_size_)),
                  inner_size_hermit_of_(detail::suffix_product(box_size_hermit_)),
                  total_size_(detail::product(box_size_)),
                  total_size_hermit_(detail::product(box_size_hermit_)),
                  last_size_(box_size_[dim_ - 1]),
                  last_size_hermit_(box_size_hermit_[dim_ - 1]),
                  outer_size_(total_size_ / last_size_),
                  outer_size_hermit_(outer_size_)
            {
            }
            /* data */
            const c_array<val_t, dim_> box_size_;
            const c_array<val_t, dim_> stride_;
            // size of box with Hermitian symmetry
            const c_array<val_t, dim_> box_size_hermit_;
            const c_array<val_t, dim_> stride_hermit_;
            const c_array<val_t, dim_> outer_size_of_;
            const c_array<val_t, dim_> inner_size_of_;
            const c_array<val_t, dim_> inner_size_hermit_of_;
            const val_t total_size_;
            const val_t total_size_hermit_;
            const val_t last_size_;
            const val_t last_size_hermit_;
            const val_t outer_size_;
            const val_t outer_size_hermit_;
        };

        /// @brief this class will construct the description of a computational box. 
        ///        Any size of 0 will be considered as 1.
        ///        Two versions of the box will be kept, one for the original one, the other one for the compressed one: any dim<=1 will be dropped.
        ///        The direct members are the compressed ones.
        /// @tparam ValT 
        /// @tparam Dim 
        template <std::size_t Dim, typename ValT = std::size_t>
        struct Box : public BoxBase<Dim, ValT>
        {
            using val_t = ValT;
            static constexpr std::size_t dim_ = Dim;
            using BoxBaseT = BoxBase<dim_, val_t>;
            constexpr Box(c_array<val_t, dim_> box_size)
                : BoxBaseT(detail::compress(box_size)),
                  original_box_(box_size),
                  compressed_box_(detail::compress(box_size)),
                  compress_index_(detail::arg_compress(box_size)),
                  useful_dim_(detail::count_non_unit(box_size))
            {
            }
            /* data */
            const BoxBase<dim_, val_t> original_box_;
            const BoxBase<dim_, val_t> compressed_box_;
            const c_array<std::size_t, dim_> compress_index_;
            const std::size_t useful_dim_;

            template <typename DataT>
            constexpr auto shuffle_as_compress_index(const c_array<DataT, dim_> &in) const -> c_array<DataT, dim_>
            {
                c_array<DataT, dim_> out{};
                for (std::size_t itr = 0; itr < dim_; ++itr)
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
            static constexpr std::size_t dim_ = Dim;
            using BoxT = Box<dim_, val_t>;
            constexpr BoxWithLength(c_array<val_t, dim_> box_size, c_array<length_val_t, dim_> box_length)
                : BoxT(box_size),
                  original_box_length_(box_length),
                  original_box_discretization_(original_box_length_ / this->original_box_.box_size_),
                  compressed_box_length_(this->shuffle_as_compress_index(original_box_length_)),
                  compressed_box_discretization_(compressed_box_length_ / this->compressed_box_.box_size_),
                  box_length_(compressed_box_length_),
                  box_discretization_(compressed_box_discretization_)
            {
            }
            constexpr BoxWithLength(c_array<val_t, dim_> box_size)
                : BoxWithLength(box_size, copy_of<length_val_t, dim_>(1.))
            {
            }
            auto set_length(c_array<length_val_t, dim_> box_length) -> void
            {
                original_box_length_ = box_length;
                original_box_discretization_ = original_box_length_ / this->original_box_.box_size_;
                compressed_box_length_ = this->shuffle_as_compress_index(original_box_length_);
                compressed_box_discretization_ = compressed_box_length_ / this->compressed_box_.box_size_;
                box_length_ = compressed_box_length_;
                box_discretization_ = compressed_box_discretization_;
            }
            /* data */
            c_array<length_val_t, dim_> original_box_length_;
            c_array<length_val_t, dim_> original_box_discretization_;
            c_array<length_val_t, dim_> compressed_box_length_;
            c_array<length_val_t, dim_> compressed_box_discretization_;
            c_array<length_val_t, dim_> box_length_;
            c_array<length_val_t, dim_> box_discretization_;
        };

        /// @brief Box at compile time. Note that the internal int type is fixed to be std::size_t
        /// @tparam Dim
        /// @tparam ...Ns
        template <std::size_t Dim, std::size_t... Ns>
        struct BoxCT
        {
            using val_t = std::size_t;
            constexpr static auto box_ = Box<Dim, val_t>{{Ns...}};
        };

        /// @brief Box at compile time, but with lengths. Note that the internal int/fp type is fixed to be std::size_t/double
        template <std::size_t Dim, std::size_t... Ns>
        struct BoxCTWithLength
        {
            using val_t = std::size_t;
            using length_val_t = double;
            constexpr static auto box_ = Box<Dim, val_t>{{Ns...}};
            static constexpr std::size_t dim_ = Dim;
            constexpr BoxCTWithLength()
                : length_(box_.original_box_.box_size_)
            {
            }
            constexpr BoxCTWithLength(c_array<length_val_t, Dim> length)
                : length_(box_.original_box_.box_size_, length)
            {
            }
            BoxWithLength<dim_, val_t, length_val_t> length_;
        };
    }
}