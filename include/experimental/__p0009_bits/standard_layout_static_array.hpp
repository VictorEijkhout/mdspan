/*
//@HEADER
// ************************************************************************
//
//                        Kokkos v. 2.0
//              Copyright (2019) Sandia Corporation
//
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Christian R. Trott (crtrott@sandia.gov)
//
// ************************************************************************
//@HEADER
*/

#pragma once

#include "macros.hpp"
#include "no_unique_address.hpp"
#include "trait_backports.hpp" // enable_if

#include <cstddef>
#include <utility> // integer_sequence

namespace std {
namespace experimental {
namespace detail {

//==============================================================================

_MDSPAN_INLINE_VARIABLE constexpr struct
    __construct_partially_static_array_from_sizes_tag_t {
} __construct_partially_static_array_from_sizes_tag = {};

template <size_t _N = 0> struct __construct_psa_from_dynamic_values_tag_t {};

//==============================================================================

template <size_t _I, class _T> using __repeated_with_idxs = _T;

//==============================================================================

/**
 *  PSA = "partially static array"
 *
 * @tparam _T
 * @tparam _ValsSeq
 * @tparam __sentinal
 */
template <class _T, class _ValsSeq, _T __sentinal = dynamic_extent,
          class _IdxsSeq = make_index_sequence<_ValsSeq::size()>>
struct __standard_layout_psa;

//==============================================================================

// Dynamic case
template <class _T, _T __value, _T... __values_or_sentinals, _T __sentinal,
          size_t _Idx, size_t... _Idxs>
struct __standard_layout_psa<
    _T, integer_sequence<_T, __value, __values_or_sentinals...>, __sentinal,
    integer_sequence<size_t, _Idx, _Idxs...>>
    : private __no_unique_address_emulation<__standard_layout_psa<
          _T, integer_sequence<_T, __values_or_sentinals...>, __sentinal,
          integer_sequence<size_t, _Idxs...>>> {
  //--------------------------------------------------------------------------

  using __next_t =
      __standard_layout_psa<_T, integer_sequence<_T, __values_or_sentinals...>,
                            __sentinal, integer_sequence<size_t, _Idxs...>>;
  using __base_t = __no_unique_address_emulation<__next_t>;

  MDSPAN_FORCE_INLINE_FUNCTION __next_t &__next() noexcept {
    return this->__base_t::__ref();
  }
  MDSPAN_FORCE_INLINE_FUNCTION __next_t const &__next() const noexcept {
    return this->__base_t::__ref();
  }

  static constexpr auto __size = sizeof...(_Idxs) + 1;
  static constexpr auto __size_dynamic = __next_t::__size_dynamic;

  //--------------------------------------------------------------------------

  MDSPAN_INLINE_FUNCTION_DEFAULTED
  constexpr __standard_layout_psa() = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  constexpr __standard_layout_psa(__standard_layout_psa const &) noexcept =
      default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  constexpr __standard_layout_psa(__standard_layout_psa &&) noexcept = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  _MDSPAN_CONSTEXPR_14_DEFAULTED __standard_layout_psa &
  operator=(__standard_layout_psa const &) noexcept = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  _MDSPAN_CONSTEXPR_14_DEFAULTED __standard_layout_psa &
  operator=(__standard_layout_psa &&) noexcept = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  ~__standard_layout_psa() noexcept = default;

  //--------------------------------------------------------------------------

  MDSPAN_INLINE_FUNCTION
  constexpr __standard_layout_psa(
      __construct_partially_static_array_from_sizes_tag_t, _T const &__val,
      __repeated_with_idxs<_Idxs, _T> const &... __vals) noexcept
      : __base_t(__base_t{__next_t(
            __construct_partially_static_array_from_sizes_tag, __vals...)}) {}

  // Dynamic idxs only given version, which is probably going to not need to
  // supported by the time mdspan is merged into the standard, but is currently
  // the way this is specified.  Use a repeated tag for the old semantics
  template <class... _Ts>
  MDSPAN_INLINE_FUNCTION constexpr __standard_layout_psa(
      __construct_partially_static_array_from_sizes_tag_t,
      __construct_partially_static_array_from_sizes_tag_t,
      _Ts const &... __vals) noexcept
      : __base_t(__base_t{__next_t(
            __construct_partially_static_array_from_sizes_tag,
            __construct_partially_static_array_from_sizes_tag, __vals...)}) {}

  template <size_t _N>
  MDSPAN_INLINE_FUNCTION constexpr explicit __standard_layout_psa(
      array<_T, _N> const &__vals) noexcept
      : __base_t(__base_t{__next_t(__vals)}) {}

  template <size_t _IDynamic, size_t _NDynamic>
  MDSPAN_INLINE_FUNCTION constexpr explicit __standard_layout_psa(
      __construct_psa_from_dynamic_values_tag_t<_IDynamic> __tag,
      array<_T, _NDynamic> const &__vals) noexcept
      : __base_t(__base_t{__next_t(__tag, __vals)}) {}

  template <class _U, class _UValsSeq, _U __u_sentinal, class _IdxsSeq>
  MDSPAN_INLINE_FUNCTION constexpr __standard_layout_psa(
      __standard_layout_psa<_U, _UValsSeq, __u_sentinal, _IdxsSeq> const
          &__rhs) noexcept
      : __base_t(__base_t{__next_t(__rhs.__next())}) {}

  //--------------------------------------------------------------------------

  // See https://godbolt.org/z/_KSDNX for a summary-by-example of why this is
  // necessary.  The we're using inheritance here instead of an alias template
  // is because we have to deduce __values_or_sentinals in several places, and
  // alias templates doen't permit that in this context.
  MDSPAN_FORCE_INLINE_FUNCTION
  constexpr __standard_layout_psa const &__enable_psa_conversion() const
      noexcept {
    return *this;
  }

  template <size_t _I, enable_if_t<_I != _Idx, int> = 0>
  MDSPAN_FORCE_INLINE_FUNCTION constexpr _T __get_n() const noexcept {
    return this->__base_t::__ref().template __get_n<_I>();
  }
  template <size_t _I, enable_if_t<_I == _Idx, int> = 1>
  MDSPAN_FORCE_INLINE_FUNCTION constexpr _T __get_n() const noexcept {
    return __value;
  }
  template <size_t _I, enable_if_t<_I != _Idx, int> = 0>
  MDSPAN_FORCE_INLINE_FUNCTION _MDSPAN_CONSTEXPR_14 void
  __set_n(_T const &__rhs) noexcept {
    this->__base_t::__ref().__set_value(__rhs);
  }
  template <size_t _I, enable_if_t<_I == _Idx, int> = 1>
  MDSPAN_FORCE_INLINE_FUNCTION _MDSPAN_CONSTEXPR_14 void
  __set_n(_T const &__rhs) noexcept {
    // Don't assert here because that would break constexpr. This better
    // not change anything, though
  }
  template <size_t _I, enable_if_t<_I == _Idx, _T> = __sentinal>
  MDSPAN_FORCE_INLINE_FUNCTION static constexpr _T __get_static_n() noexcept {
    return __value;
  }
  template <size_t _I, enable_if_t<_I != _Idx, _T> __default = __sentinal>
  MDSPAN_FORCE_INLINE_FUNCTION static constexpr _T __get_static_n() noexcept {
    return __next_t::template __get_static_n<_I, __default>();
  }
  MDSPAN_FORCE_INLINE_FUNCTION constexpr _T __get(size_t __n) const noexcept {
    return __value * (_T(_Idx == __n)) + this->__base_t::__ref().__get(__n);
  }

  //--------------------------------------------------------------------------
};

//==============================================================================

// Dynamic case, __next_t may or may not be empty
template <class _T, _T __sentinal, _T... __values_or_sentinals, size_t _Idx,
          size_t... _Idxs>
struct __standard_layout_psa<
    _T, integer_sequence<_T, __sentinal, __values_or_sentinals...>, __sentinal,
    integer_sequence<size_t, _Idx, _Idxs...>> {
  //--------------------------------------------------------------------------

  using __next_t =
      __standard_layout_psa<_T, integer_sequence<_T, __values_or_sentinals...>,
                            __sentinal, integer_sequence<size_t, _Idxs...>>;

  using __value_pair_t = __compressed_pair<_T, __next_t>;
  __value_pair_t __value_pair;
  MDSPAN_FORCE_INLINE_FUNCTION __next_t &__next() noexcept {
    return __value_pair.__second();
  }
  MDSPAN_FORCE_INLINE_FUNCTION __next_t const &__next() const noexcept {
    return __value_pair.__second();
  }

  static constexpr auto __size = sizeof...(_Idxs) + 1;
  static constexpr auto __size_dynamic = 1 + __next_t::__size_dynamic;

  //--------------------------------------------------------------------------

  MDSPAN_INLINE_FUNCTION_DEFAULTED
  constexpr __standard_layout_psa() = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  constexpr __standard_layout_psa(__standard_layout_psa const &) noexcept =
      default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  constexpr __standard_layout_psa(__standard_layout_psa &&) noexcept = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  _MDSPAN_CONSTEXPR_14_DEFAULTED __standard_layout_psa &
  operator=(__standard_layout_psa const &) noexcept = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  _MDSPAN_CONSTEXPR_14_DEFAULTED __standard_layout_psa &
  operator=(__standard_layout_psa &&) noexcept = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  ~__standard_layout_psa() noexcept = default;

  //--------------------------------------------------------------------------

  MDSPAN_INLINE_FUNCTION
  constexpr __standard_layout_psa(
      __construct_partially_static_array_from_sizes_tag_t, _T const &__val,
      __repeated_with_idxs<_Idxs, _T> const &... __vals) noexcept
      : __value_pair(__val,
                     __next_t(__construct_partially_static_array_from_sizes_tag,
                              __vals...)) {}

  // Dynamic idxs only given version, which is probably going to not need to
  // supported by the time mdspan is merged into the standard, but is currently
  // the way this is specified.  Use a repeated tag for the old semantics
  template <class... _Ts>
  MDSPAN_INLINE_FUNCTION constexpr __standard_layout_psa(
      __construct_partially_static_array_from_sizes_tag_t,
      __construct_partially_static_array_from_sizes_tag_t, _T const &__val,
      _Ts const &... __vals) noexcept
      : __value_pair(__val,
                     __next_t(__construct_partially_static_array_from_sizes_tag,
                              __construct_partially_static_array_from_sizes_tag,
                              __vals...)) {}

  template <size_t _N>
  MDSPAN_INLINE_FUNCTION constexpr explicit __standard_layout_psa(
      array<_T, _N> const &__vals) noexcept
      : __value_pair(::std::get<_Idx>(__vals), __vals) {}

  template <size_t _IDynamic, size_t _NDynamic>
  MDSPAN_INLINE_FUNCTION constexpr explicit __standard_layout_psa(
      __construct_psa_from_dynamic_values_tag_t<_IDynamic> __tag,
      array<_T, _NDynamic> const &__vals) noexcept
      : __value_pair(
            ::std::get<_IDynamic>(__vals),
            __next_t(__construct_psa_from_dynamic_values_tag_t<_IDynamic + 1>{},
                     __vals)) {}

  template <class _U, class _UValsSeq, _U __u_sentinal, class _UIdxsSeq>
  MDSPAN_INLINE_FUNCTION constexpr __standard_layout_psa(
      __standard_layout_psa<_U, _UValsSeq, __u_sentinal, _UIdxsSeq> const
          &__rhs) noexcept
      : __value_pair(__rhs.template __get_n<_Idx>(), __rhs.__next()) {}

  //--------------------------------------------------------------------------

  // See comment in the previous partial specialization for why this is
  // necessary.  Or just trust me that it's messy.
  MDSPAN_FORCE_INLINE_FUNCTION
  constexpr __standard_layout_psa const &__enable_psa_conversion() const
      noexcept {
    return *this;
  }

  template <size_t _I, enable_if_t<_I != _Idx, int> = 0>
  MDSPAN_FORCE_INLINE_FUNCTION constexpr _T __get_n() const noexcept {
    return __next().template __get_n<_I>();
  }
  template <size_t _I, enable_if_t<_I == _Idx, int> = 1>
  MDSPAN_FORCE_INLINE_FUNCTION constexpr _T __get_n() const noexcept {
    return __value_pair.__first();
  }
  template <size_t _I, enable_if_t<_I != _Idx, int> = 0>
  MDSPAN_FORCE_INLINE_FUNCTION _MDSPAN_CONSTEXPR_14 void
  __set_n(_T const &__rhs) noexcept {
    __next().__set_value(__rhs);
  }
  template <size_t _I, enable_if_t<_I == _Idx, int> = 1>
  MDSPAN_FORCE_INLINE_FUNCTION _MDSPAN_CONSTEXPR_14 void
  __set_n(_T const &__rhs) noexcept {
    __value_pair.__first() = __rhs;
  }
  template <size_t _I, enable_if_t<_I == _Idx, _T> __default = __sentinal>
  MDSPAN_FORCE_INLINE_FUNCTION static constexpr _T __get_static_n() noexcept {
    return __default;
  }
  template <size_t _I, enable_if_t<_I != _Idx, _T> __default = __sentinal>
  MDSPAN_FORCE_INLINE_FUNCTION static constexpr _T __get_static_n() noexcept {
    return __next_t::template __get_static_n<_I, __default>();
  }
  MDSPAN_FORCE_INLINE_FUNCTION constexpr _T __get(size_t __n) const noexcept {
    return __value_pair.__first() * (_T(_Idx == __n)) + __next().__get(__n);
  }

  //--------------------------------------------------------------------------
};

// empty/terminal case
template <class _T, _T __sentinal>
struct __standard_layout_psa<_T, integer_sequence<_T>, __sentinal,
                             integer_sequence<size_t>> {
  //--------------------------------------------------------------------------

  static constexpr auto __size = 0;
  static constexpr auto __size_dynamic = 0;

  //--------------------------------------------------------------------------

  MDSPAN_INLINE_FUNCTION_DEFAULTED
  constexpr __standard_layout_psa() = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  constexpr __standard_layout_psa(__standard_layout_psa const &) noexcept =
      default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  constexpr __standard_layout_psa(__standard_layout_psa &&) noexcept = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  _MDSPAN_CONSTEXPR_14_DEFAULTED __standard_layout_psa &
  operator=(__standard_layout_psa const &) noexcept = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  _MDSPAN_CONSTEXPR_14_DEFAULTED __standard_layout_psa &
  operator=(__standard_layout_psa &&) noexcept = default;
  MDSPAN_INLINE_FUNCTION_DEFAULTED
  ~__standard_layout_psa() noexcept = default;

  MDSPAN_INLINE_FUNCTION
  constexpr __standard_layout_psa(
      __construct_partially_static_array_from_sizes_tag_t) noexcept {}

  template <class... _Ts>
  MDSPAN_INLINE_FUNCTION constexpr __standard_layout_psa(
      __construct_partially_static_array_from_sizes_tag_t,
      __construct_partially_static_array_from_sizes_tag_t) noexcept {}

  template <size_t _N>
  MDSPAN_INLINE_FUNCTION constexpr explicit __standard_layout_psa(
      array<_T, _N> const &__vals) noexcept {}

  template <size_t _IDynamic, size_t _NDynamic>
  MDSPAN_INLINE_FUNCTION constexpr explicit __standard_layout_psa(
      __construct_psa_from_dynamic_values_tag_t<_IDynamic> __tag,
      array<_T, _NDynamic> const &__vals) noexcept {}

  template <class _U, class _UValsSeq, _U __u_sentinal, class _UIdxsSeq>
  MDSPAN_INLINE_FUNCTION constexpr __standard_layout_psa(
      __standard_layout_psa<_U, _UValsSeq, __u_sentinal, _UIdxsSeq> const
          &__rhs) noexcept {}

  MDSPAN_FORCE_INLINE_FUNCTION constexpr _T __get(size_t /*n*/) const noexcept {
    return 0;
  }
};

template <ptrdiff_t... __values_or_sentinals>
struct __partially_static_sizes
    : __standard_layout_psa<
          ptrdiff_t, integer_sequence<ptrdiff_t, __values_or_sentinals...>> {
  using __psa_impl_t = __standard_layout_psa<
      ptrdiff_t, integer_sequence<ptrdiff_t, __values_or_sentinals...>>;
  using __psa_impl_t::__psa_impl_t;
};

} // namespace detail
} // end namespace experimental
} // namespace std
