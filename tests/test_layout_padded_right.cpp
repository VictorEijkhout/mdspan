#define MDSPAN_INTERNAL_TEST

#include <mdspan/mdspan.hpp>
#include <gtest/gtest.h>

namespace KokkosEx = MDSPAN_IMPL_STANDARD_NAMESPACE::MDSPAN_IMPL_PROPOSED_NAMESPACE;

// Compile time tests

// For internal traits
struct fake_mapping {
  using layout_type = Kokkos::Experimental::layout_right_padded<5>;
};

static_assert(!Kokkos::Experimental::detail::__is_layout_right_padded_mapping<
              fake_mapping>::value);

static_assert(Kokkos::Experimental::detail::__is_layout_right_padded_mapping<
              Kokkos::Experimental::layout_right_padded<4>::mapping<
                  Kokkos::extents<size_t, 7, 4>>>::value);

// layout_left_padded must be trivial
static_assert(std::is_trivial_v<KokkosEx::layout_right_padded<0>>);
static_assert(std::is_trivial_v<KokkosEx::layout_right_padded<4>>);
static_assert(std::is_trivial_v<KokkosEx::layout_right_padded<Kokkos::dynamic_extent>>);

// actual padding stride
// If extents_type::rank() equals zero or one, then padding_stride.
static_assert(KokkosEx::layout_right_padded<0>::mapping<Kokkos::extents<std::size_t, 0>>::__actual_padding_stride == 0);
static_assert(KokkosEx::layout_right_padded<2>::mapping<Kokkos::extents<std::size_t, 0>>::__actual_padding_stride == 2);
static_assert(KokkosEx::layout_right_padded<2>::mapping<Kokkos::extents<std::size_t, Kokkos::dynamic_extent>>::__actual_padding_stride == 2);
static_assert(KokkosEx::layout_right_padded<2>::mapping<Kokkos::extents<std::size_t>>::__actual_padding_stride == 2);
static_assert(KokkosEx::layout_right_padded<Kokkos::dynamic_extent>::mapping<Kokkos::extents<std::size_t, 3>>::__actual_padding_stride == Kokkos::dynamic_extent);
static_assert(KokkosEx::layout_right_padded<Kokkos::dynamic_extent>::mapping<Kokkos::extents<std::size_t, 0>>::__actual_padding_stride == Kokkos::dynamic_extent);
static_assert(KokkosEx::layout_right_padded<Kokkos::dynamic_extent>::mapping<Kokkos::extents<std::size_t>>::__actual_padding_stride == Kokkos::dynamic_extent);

// Else, if
// - padding_stride does not equal dynamic_extent and
// - extents_type::static_extent(0) does not equal dynamic_extent,
// then the size_t value which is the least multiple of padding_stride that is greater than or equal to extents_type::static_extent(0).
static_assert(KokkosEx::layout_right_padded<2>::mapping<Kokkos::extents<std::size_t, 7, 3>>::__actual_padding_stride == 4);
static_assert(KokkosEx::layout_right_padded<2>::mapping<Kokkos::extents<std::size_t, Kokkos::dynamic_extent, 3>>::__actual_padding_stride == 4);

// Otherwise, dynamic_extent.
static_assert(KokkosEx::layout_right_padded<2>::mapping<Kokkos::extents<std::size_t, 7, Kokkos::dynamic_extent>>::__actual_padding_stride == Kokkos::dynamic_extent);
static_assert(KokkosEx::layout_right_padded<Kokkos::dynamic_extent>::mapping<Kokkos::extents<std::size_t, 7, Kokkos::dynamic_extent>>::__actual_padding_stride == Kokkos::dynamic_extent);
static_assert(KokkosEx::layout_right_padded<Kokkos::dynamic_extent>::mapping<Kokkos::extents<std::size_t, 7, 3>>::__actual_padding_stride == Kokkos::dynamic_extent);

// inner extents type
// If extents_type::rank() equals zero or one, then inner-extents-type names the type extents_type.
namespace
{
template<class LayoutrightPadded, class Extents, class DesiredExtents>
inline constexpr bool has_inner_extents_type = std::is_same_v<
    typename LayoutrightPadded::template mapping<Extents>::__inner_extents_type,
    DesiredExtents >;
template<class LayoutrightPadded, class Extents>
inline constexpr bool is_0_or_1_rank_inner_extents_type = has_inner_extents_type<LayoutrightPadded, Extents, typename LayoutrightPadded::template mapping<Extents>::extents_type>;
}
static_assert(is_0_or_1_rank_inner_extents_type<KokkosEx::layout_right_padded<0>, Kokkos::extents<std::size_t, 0>>);
static_assert(is_0_or_1_rank_inner_extents_type<KokkosEx::layout_right_padded<2>, Kokkos::extents<std::size_t>>);
static_assert(is_0_or_1_rank_inner_extents_type<KokkosEx::layout_right_padded<2>, Kokkos::extents<std::size_t, 3>>);
static_assert(is_0_or_1_rank_inner_extents_type<KokkosEx::layout_right_padded<2>, Kokkos::extents<std::size_t, Kokkos::dynamic_extent>>);
static_assert(is_0_or_1_rank_inner_extents_type<KokkosEx::layout_right_padded<Kokkos::dynamic_extent>, Kokkos::extents<std::size_t, 3>>);
static_assert(is_0_or_1_rank_inner_extents_type<KokkosEx::layout_right_padded<Kokkos::dynamic_extent>, Kokkos::extents<std::size_t, Kokkos::dynamic_extent>>);

// Otherwise, inner-extents-type names the type extents<index_type,actual-padding-stride, extents_type::static_extent(P_right)...>.
static_assert(has_inner_extents_type<KokkosEx::layout_right_padded<0>, Kokkos::extents<std::size_t, 5, 0>, Kokkos::extents<std::size_t, 5, 0>>);
static_assert(has_inner_extents_type<KokkosEx::layout_right_padded<2>, Kokkos::extents<std::size_t, 5, 3>, Kokkos::extents<std::size_t, 5, 4>>);
static_assert(has_inner_extents_type<KokkosEx::layout_right_padded<2>, Kokkos::extents<std::size_t, 5, Kokkos::dynamic_extent>, Kokkos::extents<std::size_t, 5, Kokkos::dynamic_extent>>);
static_assert(has_inner_extents_type<KokkosEx::layout_right_padded<2>, Kokkos::extents<std::size_t, Kokkos::dynamic_extent, 3>, Kokkos::extents<std::size_t, Kokkos::dynamic_extent, 4>>);
static_assert(has_inner_extents_type<KokkosEx::layout_right_padded<Kokkos::dynamic_extent>, Kokkos::extents<std::size_t, 5, 3>, Kokkos::extents<std::size_t, 5, Kokkos::dynamic_extent>>);

// unpadded extent type
namespace
{
template<class LayoutrightPadded, class Extents, class DesiredExtents>
inline constexpr bool has_unpadded_extents_type = std::is_same_v<
    typename LayoutrightPadded::template mapping<Extents>::__unpadded_extent_type,
    DesiredExtents >;
}
// If extents_type::rank() equals zero, then unpadded-extent-type names the type extents<index_type>.
static_assert(has_unpadded_extents_type<KokkosEx::layout_right_padded<0>, Kokkos::extents<std::size_t>, Kokkos::extents<std::size_t>>);
static_assert(has_unpadded_extents_type<KokkosEx::layout_right_padded<2>, Kokkos::extents<std::size_t>, Kokkos::extents<std::size_t>>);
static_assert(has_unpadded_extents_type<KokkosEx::layout_right_padded<Kokkos::dynamic_extent>, Kokkos::extents<std::size_t>, Kokkos::extents<std::size_t>>);

// Otherwise, unpadded-extent-type names the type extents<index_type, extents_type::static_extent(0)>.
static_assert(has_unpadded_extents_type<KokkosEx::layout_right_padded<0>, Kokkos::extents<std::size_t, 0>, Kokkos::extents<std::size_t, 0>>);
static_assert(has_unpadded_extents_type<KokkosEx::layout_right_padded<2>, Kokkos::extents<std::size_t, 3>, Kokkos::extents<std::size_t, 3>>);
static_assert(has_unpadded_extents_type<KokkosEx::layout_right_padded<2>, Kokkos::extents<std::size_t, Kokkos::dynamic_extent>, Kokkos::extents<std::size_t, Kokkos::dynamic_extent>>);
static_assert(has_unpadded_extents_type<KokkosEx::layout_right_padded<0>, Kokkos::extents<std::size_t, 7, 0>, Kokkos::extents<std::size_t, 0>>);
static_assert(has_unpadded_extents_type<KokkosEx::layout_right_padded<2>, Kokkos::extents<std::size_t, 5, 3>, Kokkos::extents<std::size_t, 3>>);
static_assert(has_unpadded_extents_type<KokkosEx::layout_right_padded<2>, Kokkos::extents<std::size_t, 13, Kokkos::dynamic_extent>, Kokkos::extents<std::size_t, Kokkos::dynamic_extent>>);

namespace
{
template<class LayoutrightPadded, class InnerExtents, class TestExtents>
void test_inner_mapping_extent(const InnerExtents &inner_extents, const TestExtents &test_extents)
{
  auto mapping = typename LayoutrightPadded::template mapping<InnerExtents>(inner_extents);
  ASSERT_EQ(mapping.__inner_mapping.extents(), test_extents);

  auto strs = mapping.strides();
  size_t prod = 1;
  for (typename decltype(mapping)::rank_type rrev = 0; rrev < TestExtents::rank(); ++rrev)
  {
    auto r = TestExtents::rank() - 1 - rrev;
    ASSERT_EQ(strs[r], prod);
    prod *= test_extents.extent(r);
  }

  ASSERT_EQ(prod, mapping.required_span_size());
}

template <class LayoutrightPadded, class InnerExtents, class TestExtents, class Size>
void test_inner_mapping_extent(const InnerExtents &inner_extents, const TestExtents &test_extents, Size padding_value)
{
  auto mapping = typename LayoutrightPadded::template mapping<InnerExtents>(inner_extents, padding_value);
  ASSERT_EQ(mapping.__inner_mapping.extents(), test_extents);

  auto strs = mapping.strides();
  size_t prod = 1;
  for (typename decltype(mapping)::rank_type rrev = 0; rrev < TestExtents::rank(); ++rrev)
  {
    auto r = TestExtents::rank() - 1 - rrev;
    ASSERT_EQ(strs[r], prod);
    prod *= test_extents.extent(r);
  }

  ASSERT_EQ(prod, mapping.required_span_size());
}

template <class LayoutRightPadded, class Extents>
void test_0_or_1_rank_inner_mapping_extents(const Extents &extents)
{
  test_inner_mapping_extent<LayoutRightPadded>(extents, extents);
}

template <class LayoutRightPadded, class Extents, class Size>
void test_0_or_1_rank_inner_mapping_extents(const Extents &extents, Size padding_value)
{
  test_inner_mapping_extent<LayoutRightPadded>(extents, extents, padding_value);
}

template <class LayoutRightPadded, class Extents>
void test_default_constructor_equivalence()
{
  using mapping_type = typename LayoutRightPadded::template mapping<Extents>;

  ASSERT_EQ(mapping_type(), mapping_type(Extents{}));
}

template <class LayoutRightPadded, class Extents>
void test_copy_constructor(const Extents &extents)
{
  using mapping_type = typename LayoutRightPadded::template mapping<Extents>;
  auto a = mapping_type(extents);
  auto b = a;

  ASSERT_EQ(a, b);
}

template <class LayoutRightPadded, class Extents, class Size>
void test_copy_constructor(const Extents &extents, Size padding_value)
{
  using mapping_type = typename LayoutRightPadded::template mapping<Extents>;
  auto a = mapping_type(extents, padding_value);
  auto b = a;

  ASSERT_EQ(a, b);
}

template <class LayoutRightPadded, class Extents>
void test_copy_assignment(const Extents &extents)
{
  using mapping_type = typename LayoutRightPadded::template mapping<Extents>;
  auto a = mapping_type(extents);
  mapping_type b;

  b = a;

  ASSERT_EQ(a, b);
}

template <class LayoutRightPadded, class Extents, class Size>
void test_copy_assignment(const Extents &extents, Size padding_value)
{
  using mapping_type = typename LayoutRightPadded::template mapping<Extents>;
  auto a = mapping_type(extents, padding_value);
  mapping_type b;

  b = a;

  ASSERT_EQ(a, b);
}
}

TEST(LayoutrightTests, construction)
{
  // Default Constructor
  test_default_constructor_equivalence<KokkosEx::layout_right_padded<4>, Kokkos::extents<std::size_t>>();
  test_default_constructor_equivalence<KokkosEx::layout_right_padded<4>, Kokkos::extents<std::size_t, 3>>();
  test_default_constructor_equivalence<KokkosEx::layout_right_padded<4>, Kokkos::extents<std::size_t, 3, 2>>();
  test_default_constructor_equivalence<KokkosEx::layout_right_padded<4>, Kokkos::extents<std::size_t, 3, 2, 1>>();
  test_default_constructor_equivalence<KokkosEx::layout_right_padded<Kokkos::dynamic_extent>, Kokkos::extents<std::size_t>>();
  test_default_constructor_equivalence<KokkosEx::layout_right_padded<Kokkos::dynamic_extent>, Kokkos::extents<std::size_t, 3>>();
  test_default_constructor_equivalence<KokkosEx::layout_right_padded<Kokkos::dynamic_extent>, Kokkos::extents<std::size_t, 3, 2>>();
  test_default_constructor_equivalence<KokkosEx::layout_right_padded<Kokkos::dynamic_extent>, Kokkos::extents<std::size_t, 3, 2, 1>>();
  test_default_constructor_equivalence<KokkosEx::layout_right_padded<4>, Kokkos::extents<std::size_t, 3, 2, Kokkos::dynamic_extent>>();
  test_default_constructor_equivalence<KokkosEx::layout_right_padded<4>, Kokkos::extents<std::size_t, 3, Kokkos::dynamic_extent, Kokkos::dynamic_extent>>();
  test_default_constructor_equivalence<KokkosEx::layout_right_padded<4>, Kokkos::extents<std::size_t, Kokkos::dynamic_extent, Kokkos::dynamic_extent, Kokkos::dynamic_extent>>();

  // Copy constructor
  test_copy_constructor<KokkosEx::layout_right_padded<4>>(Kokkos::extents<std::size_t>());
  test_copy_constructor<KokkosEx::layout_right_padded<4>>(Kokkos::extents<std::size_t, 3>());
  test_copy_constructor<KokkosEx::layout_right_padded<4>>(Kokkos::extents<std::size_t, 3, 2>());
  test_copy_constructor<KokkosEx::layout_right_padded<4>>(Kokkos::extents<std::size_t, 3, 2, 1>());
  test_copy_constructor<KokkosEx::layout_right_padded<4>>(Kokkos::extents<std::size_t, 3, 2, Kokkos::dynamic_extent>(10));
  test_copy_constructor<KokkosEx::layout_right_padded<4>>(Kokkos::extents<std::size_t, 3, Kokkos::dynamic_extent, Kokkos::dynamic_extent>(9, 10));
  test_copy_constructor<KokkosEx::layout_right_padded<4>>(Kokkos::extents<std::size_t, Kokkos::dynamic_extent, Kokkos::dynamic_extent, Kokkos::dynamic_extent>(8, 9, 10));

  test_copy_constructor<KokkosEx::layout_right_padded<Kokkos::dynamic_extent>>(Kokkos::extents<std::size_t>(), 5);
  test_copy_constructor<KokkosEx::layout_right_padded<Kokkos::dynamic_extent>>(Kokkos::extents<std::size_t, 3>(), 5);
  test_copy_constructor<KokkosEx::layout_right_padded<Kokkos::dynamic_extent>>(Kokkos::extents<std::size_t, 3, 2>(), 5);
  test_copy_constructor<KokkosEx::layout_right_padded<Kokkos::dynamic_extent>>(Kokkos::extents<std::size_t, 3, 2, 1>(), 5);

  // Copy assignment
  test_copy_assignment<KokkosEx::layout_right_padded<4>>(Kokkos::extents<std::size_t>());
  test_copy_assignment<KokkosEx::layout_right_padded<4>>(Kokkos::extents<std::size_t, 3>());
  test_copy_assignment<KokkosEx::layout_right_padded<4>>(Kokkos::extents<std::size_t, 3, 2>());
  test_copy_assignment<KokkosEx::layout_right_padded<4>>(Kokkos::extents<std::size_t, 3, 2, 1>());
  test_copy_assignment<KokkosEx::layout_right_padded<4>>(Kokkos::extents<std::size_t, 3, 2, Kokkos::dynamic_extent>(10));
  test_copy_assignment<KokkosEx::layout_right_padded<4>>(Kokkos::extents<std::size_t, 3, Kokkos::dynamic_extent, Kokkos::dynamic_extent>(9, 10));
  test_copy_assignment<KokkosEx::layout_right_padded<4>>(Kokkos::extents<std::size_t, Kokkos::dynamic_extent, Kokkos::dynamic_extent, Kokkos::dynamic_extent>(8, 9, 10));

  test_copy_assignment<KokkosEx::layout_right_padded<Kokkos::dynamic_extent>>(Kokkos::extents<std::size_t>(), 5);
  test_copy_assignment<KokkosEx::layout_right_padded<Kokkos::dynamic_extent>>(Kokkos::extents<std::size_t, 3>(), 5);
  test_copy_assignment<KokkosEx::layout_right_padded<Kokkos::dynamic_extent>>(Kokkos::extents<std::size_t, 3, 2>(), 5);
  test_copy_assignment<KokkosEx::layout_right_padded<Kokkos::dynamic_extent>>(Kokkos::extents<std::size_t, 3, 2, 1>(), 5);

  // Constructor only taking an extent
  // Direct-non-list-initializes inner-mapping with:
  // - ext, if extents_type::rank() is zero or one; else,
  test_0_or_1_rank_inner_mapping_extents<KokkosEx::layout_right_padded<4>>(Kokkos::extents<std::size_t>{});
  test_0_or_1_rank_inner_mapping_extents<KokkosEx::layout_right_padded<0>>(Kokkos::extents<std::size_t, 0>{});
  test_0_or_1_rank_inner_mapping_extents<KokkosEx::layout_right_padded<4>>(Kokkos::extents<std::size_t, 5>{});
  test_0_or_1_rank_inner_mapping_extents<KokkosEx::layout_right_padded<4>>(Kokkos::extents<std::size_t, Kokkos::dynamic_extent>{7});
  test_0_or_1_rank_inner_mapping_extents<KokkosEx::layout_right_padded<Kokkos::dynamic_extent>>(Kokkos::extents<std::size_t>{});
  test_0_or_1_rank_inner_mapping_extents<KokkosEx::layout_right_padded<Kokkos::dynamic_extent>>(Kokkos::extents<std::size_t, 0>{});
  test_0_or_1_rank_inner_mapping_extents<KokkosEx::layout_right_padded<Kokkos::dynamic_extent>>(Kokkos::extents<std::size_t, 5>{});
  test_0_or_1_rank_inner_mapping_extents<KokkosEx::layout_right_padded<Kokkos::dynamic_extent>>(Kokkos::extents<std::size_t, Kokkos::dynamic_extent>{7});

  // - ext.extent(0), ext.extent(P_right)..., if padding_stride is dynamic_extent
  test_inner_mapping_extent<KokkosEx::layout_right_padded<Kokkos::dynamic_extent>>(Kokkos::extents<std::size_t, 7, 0>{}, Kokkos::extents<std::size_t, 7, Kokkos::dynamic_extent>{ 0 });
  test_inner_mapping_extent<KokkosEx::layout_right_padded<Kokkos::dynamic_extent>>(Kokkos::extents<std::size_t, 7, 5>{}, Kokkos::extents<std::size_t, 7, Kokkos::dynamic_extent>{ 5 });
  test_inner_mapping_extent<KokkosEx::layout_right_padded<Kokkos::dynamic_extent>>(Kokkos::extents<std::size_t, 13, Kokkos::dynamic_extent>{7}, Kokkos::extents<std::size_t, 13, Kokkos::dynamic_extent>{7});

  // - S_right, ext.extent(P_right)..., where S_right is the least multiple of padding_stride greater than or equal to ext.extent(0)
  test_inner_mapping_extent<KokkosEx::layout_right_padded<0>>(Kokkos::extents<std::size_t, 7, 0>{}, Kokkos::extents<std::size_t, 7, 0>{});
  test_inner_mapping_extent<KokkosEx::layout_right_padded<4>>(Kokkos::extents<std::size_t, 7, 5>{}, Kokkos::extents<std::size_t, 7, 8>{});
  test_inner_mapping_extent<KokkosEx::layout_right_padded<4>>(Kokkos::extents<std::size_t, 13, Kokkos::dynamic_extent>{7}, Kokkos::extents<std::size_t, 13, Kokkos::dynamic_extent>{8});

  // Constructor taking an extent and a dynamic value
  // Direct-non-list-initializes inner-mapping with:
  // - ext, if extents_type::rank() is zero or one; else,
  test_0_or_1_rank_inner_mapping_extents<KokkosEx::layout_right_padded<4>>(Kokkos::extents<std::size_t>{}, 4);
  test_0_or_1_rank_inner_mapping_extents<KokkosEx::layout_right_padded<0>>(Kokkos::extents<std::size_t, 0>{}, 0);
  test_0_or_1_rank_inner_mapping_extents<KokkosEx::layout_right_padded<4>>(Kokkos::extents<std::size_t, 5>{}, 4);
  test_0_or_1_rank_inner_mapping_extents<KokkosEx::layout_right_padded<4>>(Kokkos::extents<std::size_t, Kokkos::dynamic_extent>{7}, 4);
  test_0_or_1_rank_inner_mapping_extents<KokkosEx::layout_right_padded<Kokkos::dynamic_extent>>(Kokkos::extents<std::size_t>{}, 3);
  test_0_or_1_rank_inner_mapping_extents<KokkosEx::layout_right_padded<Kokkos::dynamic_extent>>(Kokkos::extents<std::size_t, 0>{}, 3255);
  test_0_or_1_rank_inner_mapping_extents<KokkosEx::layout_right_padded<Kokkos::dynamic_extent>>(Kokkos::extents<std::size_t, 5>{}, 1337);
  test_0_or_1_rank_inner_mapping_extents<KokkosEx::layout_right_padded<Kokkos::dynamic_extent>>(Kokkos::extents<std::size_t, Kokkos::dynamic_extent>{7}, 6323);

  // - S_right, ext.extent(P_right)..., where S_right is the least multiple of padding_value greater than or equal to ext.extent(0)
  test_inner_mapping_extent<KokkosEx::layout_right_padded<0>>(Kokkos::extents<std::size_t, 7, 0>{}, Kokkos::extents<std::size_t, 7, 0>{}, 0);
  test_inner_mapping_extent<KokkosEx::layout_right_padded<4>>(Kokkos::extents<std::size_t, 7, 5>{}, Kokkos::extents<std::size_t, 7, 8>{}, 4);
  test_inner_mapping_extent<KokkosEx::layout_right_padded<4>>(Kokkos::extents<std::size_t, 13, Kokkos::dynamic_extent>{7}, Kokkos::extents<std::size_t, 13, Kokkos::dynamic_extent>{8}, 4);
  test_inner_mapping_extent<KokkosEx::layout_right_padded<Kokkos::dynamic_extent>>(Kokkos::extents<std::size_t, 7, 0>{}, Kokkos::extents<std::size_t, 7, Kokkos::dynamic_extent>{0}, 2);
  test_inner_mapping_extent<KokkosEx::layout_right_padded<Kokkos::dynamic_extent>>(Kokkos::extents<std::size_t, 7, 5>{}, Kokkos::extents<std::size_t, 7, Kokkos::dynamic_extent>{8}, 4);
  test_inner_mapping_extent<KokkosEx::layout_right_padded<Kokkos::dynamic_extent>>(Kokkos::extents<std::size_t, 13, Kokkos::dynamic_extent>{7}, Kokkos::extents<std::size_t, 13, Kokkos::dynamic_extent>{8}, 4);

  // Construct layout_right_padded mapping from layout_right mapping
  ASSERT_EQ(KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t>>(Kokkos::layout_right::mapping<Kokkos::extents<std::size_t>>()).extents(), Kokkos::extents<std::size_t>());
  ASSERT_EQ((KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 3>>(Kokkos::layout_right::mapping<Kokkos::extents<std::size_t, 3>>()).extents()), (Kokkos::extents<std::size_t, 3>()));
  ASSERT_EQ((KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 3>>(Kokkos::layout_right::mapping<Kokkos::extents<std::size_t, 3>>()).__inner_mapping.extents()), (Kokkos::extents<std::size_t, 3>()));
  ASSERT_EQ((KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 7, 4>>(Kokkos::layout_right::mapping<Kokkos::extents<std::size_t, 7, 4>>()).extents()), (Kokkos::extents<std::size_t, 7, 4>()));
  ASSERT_EQ((KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 7, 4>>(Kokkos::layout_right::mapping<Kokkos::extents<std::size_t, 7, 4>>()).__inner_mapping.extents()), (Kokkos::extents<std::size_t, 7, 4>()));
  ASSERT_EQ((KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 7, Kokkos::dynamic_extent>>(Kokkos::layout_right::mapping<Kokkos::extents<std::size_t, 7, 4>>()).extents()), (Kokkos::extents<std::size_t, 7, 4>()));
  ASSERT_EQ((KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 7, Kokkos::dynamic_extent>>(Kokkos::layout_right::mapping<Kokkos::extents<std::size_t, 7, 4>>()).__inner_mapping.extents()), (Kokkos::extents<std::size_t, 7, 4>()));

  ASSERT_EQ(KokkosEx::layout_right_padded<Kokkos::dynamic_extent>::mapping<Kokkos::extents<std::size_t>>(Kokkos::layout_right::mapping<Kokkos::extents<std::size_t>>()).extents(), Kokkos::extents<std::size_t>());
  ASSERT_EQ((KokkosEx::layout_right_padded<Kokkos::dynamic_extent>::mapping<Kokkos::extents<std::size_t, 3>>(Kokkos::layout_right::mapping<Kokkos::extents<std::size_t, 3>>()).extents()), (Kokkos::extents<std::size_t, 3>()));
  ASSERT_EQ((KokkosEx::layout_right_padded<Kokkos::dynamic_extent>::mapping<Kokkos::extents<std::size_t, 3>>(Kokkos::layout_right::mapping<Kokkos::extents<std::size_t, 3>>()).__inner_mapping.extents()), (Kokkos::extents<std::size_t, 3>()));
  ASSERT_EQ((KokkosEx::layout_right_padded<Kokkos::dynamic_extent>::mapping<Kokkos::extents<std::size_t, 7, 4>>(Kokkos::layout_right::mapping<Kokkos::extents<std::size_t, 7, 4>>()).extents()), (Kokkos::extents<std::size_t, 7, 4>()));
  ASSERT_EQ((KokkosEx::layout_right_padded<Kokkos::dynamic_extent>::mapping<Kokkos::extents<std::size_t, 7, 4>>(Kokkos::layout_right::mapping<Kokkos::extents<std::size_t, 7, 4>>()).__inner_mapping.extents()), (Kokkos::extents<std::size_t, 7, 4>()));
  ASSERT_EQ((KokkosEx::layout_right_padded<Kokkos::dynamic_extent>::mapping<Kokkos::extents<std::size_t, 7, Kokkos::dynamic_extent>>(Kokkos::layout_right::mapping<Kokkos::extents<std::size_t, 7, Kokkos::dynamic_extent>>(Kokkos::extents<std::size_t, 7, Kokkos::dynamic_extent>{4})).extents()), (Kokkos::extents<std::size_t, 7, 4>()));
  ASSERT_EQ((KokkosEx::layout_right_padded<Kokkos::dynamic_extent>::mapping<Kokkos::extents<std::size_t, 7, Kokkos::dynamic_extent>>(Kokkos::layout_right::mapping<Kokkos::extents<std::size_t, 7, Kokkos::dynamic_extent>>(Kokkos::extents<std::size_t, 7, Kokkos::dynamic_extent>{4})).__inner_mapping.extents()), (Kokkos::extents<std::size_t, 7, 4>()));

  // Construct layout_right_padded mapping from layout stride
  ASSERT_EQ(KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t>>(Kokkos::layout_stride::mapping<Kokkos::extents<std::size_t>>()).extents(), Kokkos::extents<std::size_t>());
  ASSERT_EQ((KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 3>>(Kokkos::layout_stride::mapping<Kokkos::extents<std::size_t, 3>>({}, std::array<std::size_t, 1>{1})).extents()), (Kokkos::extents<std::size_t, 3>()));
  ASSERT_EQ((KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 3>>(Kokkos::layout_stride::mapping<Kokkos::extents<std::size_t, 3>>({}, std::array<std::size_t, 1>{1})).__inner_mapping.extents()), (Kokkos::extents<std::size_t, 3>()));
  ASSERT_EQ((KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 7, 4>>(Kokkos::layout_stride::mapping<Kokkos::extents<std::size_t, 7, 4>>({}, std::array<std::size_t, 2>{4, 1})).extents()), (Kokkos::extents<std::size_t, 7, 4>()));
  ASSERT_EQ((KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 7, 4>>(Kokkos::layout_stride::mapping<Kokkos::extents<std::size_t, 7, 4>>({}, std::array<std::size_t, 2>{4, 1})).__inner_mapping.extents()), (Kokkos::extents<std::size_t, 7, 4>()));
  ASSERT_EQ((KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 7, Kokkos::dynamic_extent>>(Kokkos::layout_stride::mapping<Kokkos::extents<std::size_t, 7, 4>>({}, std::array<std::size_t, 2>{4, 1})).extents()), (Kokkos::extents<std::size_t, 7, 4>()));
  ASSERT_EQ((KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 7, Kokkos::dynamic_extent>>(Kokkos::layout_stride::mapping<Kokkos::extents<std::size_t, 7, 4>>({}, std::array<std::size_t, 2>{4, 1})).__inner_mapping.extents()), (Kokkos::extents<std::size_t, 7, 4>()));
  ASSERT_EQ((KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 7, 5>>(Kokkos::layout_stride::mapping<Kokkos::extents<std::size_t, 7, 5>>({}, std::array<std::size_t, 2>{8, 1})).extents()), (Kokkos::extents<std::size_t, 7, 5>()));
  ASSERT_EQ((KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 7, 5>>(Kokkos::layout_stride::mapping<Kokkos::extents<std::size_t, 7, 5>>({}, std::array<std::size_t, 2>{8, 1})).__inner_mapping.extents()), (Kokkos::extents<std::size_t, 7, 8>()));
  ASSERT_EQ((KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 7, Kokkos::dynamic_extent>>(Kokkos::layout_stride::mapping<Kokkos::extents<std::size_t, 7, 5>>({}, std::array<std::size_t, 2>{8, 1})).extents()), (Kokkos::extents<std::size_t, 7, 5>()));
  ASSERT_EQ((KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 7, Kokkos::dynamic_extent>>(Kokkos::layout_stride::mapping<Kokkos::extents<std::size_t, 7, 5>>({}, std::array<std::size_t, 2>{8, 1})).__inner_mapping.extents()), (Kokkos::extents<std::size_t, 7, 8>()));

  ASSERT_EQ(KokkosEx::layout_right_padded<Kokkos::dynamic_extent>::mapping<Kokkos::extents<std::size_t>>(Kokkos::layout_stride::mapping<Kokkos::extents<std::size_t>>()).extents(), Kokkos::extents<std::size_t>());
  ASSERT_EQ((KokkosEx::layout_right_padded<Kokkos::dynamic_extent>::mapping<Kokkos::extents<std::size_t, 3>>(Kokkos::layout_stride::mapping<Kokkos::extents<std::size_t, 3>>({}, std::array<std::size_t, 1>{1})).extents()), (Kokkos::extents<std::size_t, 3>()));
  ASSERT_EQ((KokkosEx::layout_right_padded<Kokkos::dynamic_extent>::mapping<Kokkos::extents<std::size_t, 3>>(Kokkos::layout_stride::mapping<Kokkos::extents<std::size_t, 3>>({}, std::array<std::size_t, 1>{1})).__inner_mapping.extents()), (Kokkos::extents<std::size_t, 3>()));
  ASSERT_EQ((KokkosEx::layout_right_padded<Kokkos::dynamic_extent>::mapping<Kokkos::extents<std::size_t, 7, 4>>(Kokkos::layout_stride::mapping<Kokkos::extents<std::size_t, 7, 4>>({}, std::array<std::size_t, 2>{4, 1})).extents()), (Kokkos::extents<std::size_t, 7, 4>()));
  ASSERT_EQ((KokkosEx::layout_right_padded<Kokkos::dynamic_extent>::mapping<Kokkos::extents<std::size_t, 7, 4>>(Kokkos::layout_stride::mapping<Kokkos::extents<std::size_t, 7, 4>>({}, std::array<std::size_t, 2>{4, 1})).__inner_mapping.extents()), (Kokkos::extents<std::size_t, 7, 4>()));
  ASSERT_EQ((KokkosEx::layout_right_padded<Kokkos::dynamic_extent>::mapping<Kokkos::extents<std::size_t, 7, Kokkos::dynamic_extent>>(Kokkos::layout_stride::mapping<Kokkos::extents<std::size_t, 7, 4>>({}, std::array<std::size_t, 2>{4, 1})).extents()), (Kokkos::extents<std::size_t, 7, 4>()));
  ASSERT_EQ((KokkosEx::layout_right_padded<Kokkos::dynamic_extent>::mapping<Kokkos::extents<std::size_t, 7, Kokkos::dynamic_extent>>(Kokkos::layout_stride::mapping<Kokkos::extents<std::size_t, 7, 4>>({}, std::array<std::size_t, 2>{4, 1})).__inner_mapping.extents()), (Kokkos::extents<std::size_t, 7, 4>()));
  ASSERT_EQ((KokkosEx::layout_right_padded<Kokkos::dynamic_extent>::mapping<Kokkos::extents<std::size_t, 7, 5>>(Kokkos::layout_stride::mapping<Kokkos::extents<std::size_t, 7, 5>>({}, std::array<std::size_t, 2>{8, 1})).extents()), (Kokkos::extents<std::size_t, 7, 5>()));
  ASSERT_EQ((KokkosEx::layout_right_padded<Kokkos::dynamic_extent>::mapping<Kokkos::extents<std::size_t, 7, 5>>(Kokkos::layout_stride::mapping<Kokkos::extents<std::size_t, 7, 5>>({}, std::array<std::size_t, 2>{8, 1})).__inner_mapping.extents()), (Kokkos::extents<std::size_t, 7, 8>()));
  ASSERT_EQ((KokkosEx::layout_right_padded<Kokkos::dynamic_extent>::mapping<Kokkos::extents<std::size_t, 7, Kokkos::dynamic_extent>>(Kokkos::layout_stride::mapping<Kokkos::extents<std::size_t, 7, 5>>({}, std::array<std::size_t, 2>{8, 1})).extents()), (Kokkos::extents<std::size_t, 7, 5>()));
  ASSERT_EQ((KokkosEx::layout_right_padded<Kokkos::dynamic_extent>::mapping<Kokkos::extents<std::size_t, 7, Kokkos::dynamic_extent>>(Kokkos::layout_stride::mapping<Kokkos::extents<std::size_t, 7, 5>>({}, std::array<std::size_t, 2>{8, 1})).__inner_mapping.extents()), (Kokkos::extents<std::size_t, 7, 8>()));

  // Construct layout_right_padded from another layout_right_padded
  ASSERT_EQ(KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t>>(KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t>>()).extents(), Kokkos::extents<std::size_t>());
  ASSERT_EQ((KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 3>>(KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 3>>()).extents()), (Kokkos::extents<std::size_t, 3>()));
  ASSERT_EQ((KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 3>>(KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 3>>()).__inner_mapping.extents()), (Kokkos::extents<std::size_t, 3>()));
  ASSERT_EQ((KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 7, 4>>(KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 7, 4>>()).extents()), (Kokkos::extents<std::size_t, 7, 4>()));
  ASSERT_EQ((KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 7, 4>>(KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 7, 4>>()).__inner_mapping.extents()), (Kokkos::extents<std::size_t, 7, 4>()));
  ASSERT_EQ((KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 7, Kokkos::dynamic_extent>>(KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 7, 4>>()).extents()), (Kokkos::extents<std::size_t, 7, 4>()));
  ASSERT_EQ((KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 7, Kokkos::dynamic_extent>>(KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 7, 4>>()).__inner_mapping.extents()), (Kokkos::extents<std::size_t, 7, 4>()));

  ASSERT_EQ(KokkosEx::layout_right_padded<Kokkos::dynamic_extent>::mapping<Kokkos::extents<std::size_t>>(KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t>>()).extents(), Kokkos::extents<std::size_t>());
  ASSERT_EQ((KokkosEx::layout_right_padded<Kokkos::dynamic_extent>::mapping<Kokkos::extents<std::size_t, 3>>(KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 3>>()).extents()), (Kokkos::extents<std::size_t, 3>()));
  ASSERT_EQ((KokkosEx::layout_right_padded<Kokkos::dynamic_extent>::mapping<Kokkos::extents<std::size_t, 3>>(KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 3>>()).__inner_mapping.extents()), (Kokkos::extents<std::size_t, 3>()));
  ASSERT_EQ((KokkosEx::layout_right_padded<Kokkos::dynamic_extent>::mapping<Kokkos::extents<std::size_t, 7, 5>>(KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 7, 5>>()).extents()), (Kokkos::extents<std::size_t, 7, 5>()));
  ASSERT_EQ((KokkosEx::layout_right_padded<Kokkos::dynamic_extent>::mapping<Kokkos::extents<std::size_t, 7, 5>>(KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 7, 5>>()).__inner_mapping.extents()), (Kokkos::extents<std::size_t, 7, 8>()));
  ASSERT_EQ((KokkosEx::layout_right_padded<Kokkos::dynamic_extent>::mapping<Kokkos::extents<std::size_t, 7, Kokkos::dynamic_extent>>(KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 7, 5>>()).extents()), (Kokkos::extents<std::size_t, 7, 5>()));
  ASSERT_EQ((KokkosEx::layout_right_padded<Kokkos::dynamic_extent>::mapping<Kokkos::extents<std::size_t, 7, Kokkos::dynamic_extent>>(KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 7, 5>>()).__inner_mapping.extents()), (Kokkos::extents<std::size_t, 7, 8>()));

  // Construct layout_right_padded mapping from layout_left_padded mapping
  ASSERT_EQ(KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t>>(KokkosEx::layout_left_padded<4>::mapping<Kokkos::extents<std::size_t>>()).extents(), Kokkos::extents<std::size_t>());
  ASSERT_EQ((KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 3>>(KokkosEx::layout_left_padded<4>::mapping<Kokkos::extents<std::size_t, 3>>()).extents()), (Kokkos::extents<std::size_t, 3>()));
  ASSERT_EQ((KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 3>>(KokkosEx::layout_left_padded<4>::mapping<Kokkos::extents<std::size_t, 3>>()).__inner_mapping.extents()), (Kokkos::extents<std::size_t, 3>()));
  ASSERT_EQ((KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 3>>(KokkosEx::layout_left_padded<Kokkos::dynamic_extent>::mapping<Kokkos::extents<std::size_t, 3>>({}, 4)).extents()), (Kokkos::extents<std::size_t, 3>()));
  ASSERT_EQ((KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 3>>(KokkosEx::layout_left_padded<Kokkos::dynamic_extent>::mapping<Kokkos::extents<std::size_t, 3>>({}, 4)).__inner_mapping.extents()), (Kokkos::extents<std::size_t, 3>()));

  ASSERT_EQ(KokkosEx::layout_right_padded<Kokkos::dynamic_extent>::mapping<Kokkos::extents<std::size_t>>(KokkosEx::layout_left_padded<4>::mapping<Kokkos::extents<std::size_t>>()).extents(), Kokkos::extents<std::size_t>());
  ASSERT_EQ((KokkosEx::layout_right_padded<Kokkos::dynamic_extent>::mapping<Kokkos::extents<std::size_t, 3>>(KokkosEx::layout_left_padded<4>::mapping<Kokkos::extents<std::size_t, 3>>()).extents()), (Kokkos::extents<std::size_t, 3>()));
  ASSERT_EQ((KokkosEx::layout_right_padded<Kokkos::dynamic_extent>::mapping<Kokkos::extents<std::size_t, 3>>(KokkosEx::layout_left_padded<4>::mapping<Kokkos::extents<std::size_t, 3>>()).__inner_mapping.extents()), (Kokkos::extents<std::size_t, 3>()));
  ASSERT_EQ((KokkosEx::layout_right_padded<Kokkos::dynamic_extent>::mapping<Kokkos::extents<std::size_t, 3>>(KokkosEx::layout_left_padded<Kokkos::dynamic_extent>::mapping<Kokkos::extents<std::size_t, 3>>({}, 4)).extents()), (Kokkos::extents<std::size_t, 3>()));
  ASSERT_EQ((KokkosEx::layout_right_padded<Kokkos::dynamic_extent>::mapping<Kokkos::extents<std::size_t, 3>>(KokkosEx::layout_left_padded<Kokkos::dynamic_extent>::mapping<Kokkos::extents<std::size_t, 3>>({}, 4)).__inner_mapping.extents()), (Kokkos::extents<std::size_t, 3>()));

  // Construct layout_right mapping from layout_right_padded mapping
  ASSERT_EQ(Kokkos::layout_right::mapping<Kokkos::extents<std::size_t>>(KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t>>()).extents(), Kokkos::extents<std::size_t>());
  ASSERT_EQ((Kokkos::layout_right::mapping<Kokkos::extents<std::size_t, 5>>(KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 5>>()).extents()), (Kokkos::extents<std::size_t, 5>()));
  ASSERT_EQ((Kokkos::layout_right::mapping<Kokkos::extents<std::size_t, 4, 8>>(KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 4, 8>>()).extents()), (Kokkos::extents<std::size_t, 4, 8>()));
  ASSERT_EQ((Kokkos::layout_right::mapping<Kokkos::extents<std::size_t, 3, 4, 8>>(KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 3, 4, 8>>()).extents()), (Kokkos::extents<std::size_t, 3, 4, 8>()));
}

namespace
{
  template <class PaddedLayout, class Extents>
  void test_extent(const Extents &input_extents)
  {
    auto mapping = typename PaddedLayout::template mapping<Extents>(input_extents);
    ASSERT_EQ(mapping.extents(), input_extents);
  }

  template <class PaddedLayout, class Extents, class Size>
  void test_extent(const Extents &input_extents, Size padding_value)
  {
    auto mapping = typename PaddedLayout::template mapping<Extents>(input_extents, padding_value);
    ASSERT_EQ(mapping.extents(), input_extents);
  }
}

TEST(LayoutrightTests, extents)
{
  test_extent<KokkosEx::layout_right_padded<4>>(Kokkos::extents<std::size_t>{});
  test_extent<KokkosEx::layout_right_padded<0>>(Kokkos::extents<std::size_t, 7, 0>{});
  test_extent<KokkosEx::layout_right_padded<4>>(Kokkos::extents<std::size_t, 7, 5>{});
  test_extent<KokkosEx::layout_right_padded<4>>(Kokkos::extents<std::size_t, 13, Kokkos::dynamic_extent>{7});
  test_extent<KokkosEx::layout_right_padded<Kokkos::dynamic_extent>>(Kokkos::extents<std::size_t, 7, 0>{});
  test_extent<KokkosEx::layout_right_padded<Kokkos::dynamic_extent>>(Kokkos::extents<std::size_t, 7, 5>{});
  test_extent<KokkosEx::layout_right_padded<Kokkos::dynamic_extent>>(Kokkos::extents<std::size_t, 13, Kokkos::dynamic_extent>{7});

  test_extent<KokkosEx::layout_right_padded<0>>(Kokkos::extents<std::size_t, 7, 0>{}, 0);
  test_extent<KokkosEx::layout_right_padded<4>>(Kokkos::extents<std::size_t, 7, 5>{}, 4);
  test_extent<KokkosEx::layout_right_padded<4>>(Kokkos::extents<std::size_t, 13, Kokkos::dynamic_extent>{7}, 4);
  test_extent<KokkosEx::layout_right_padded<Kokkos::dynamic_extent>>(Kokkos::extents<std::size_t, 7, 0>{}, 1);
  test_extent<KokkosEx::layout_right_padded<Kokkos::dynamic_extent>>(Kokkos::extents<std::size_t, 7, 5>{}, 3);
  test_extent<KokkosEx::layout_right_padded<Kokkos::dynamic_extent>>(Kokkos::extents<std::size_t, 13, Kokkos::dynamic_extent>{7}, 5);
}

// is_always_exhaustive
static_assert(KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t>>{}.is_always_exhaustive());
static_assert(KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 3>>{}.is_always_exhaustive());
static_assert(KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, Kokkos::dynamic_extent>>{Kokkos::extents<std::size_t, Kokkos::dynamic_extent>{5}}.is_always_exhaustive());
static_assert(KokkosEx::layout_right_padded<Kokkos::dynamic_extent>::mapping<Kokkos::extents<std::size_t, 3>>{}.is_always_exhaustive());

static_assert(!KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 4, 3>>{}.is_always_exhaustive());
static_assert(KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 6, 4>>{}.is_always_exhaustive());
static_assert(KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 6, 8>>{}.is_always_exhaustive());
static_assert(!KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 4, Kokkos::dynamic_extent>>{Kokkos::extents<std::size_t, 4, Kokkos::dynamic_extent>{5}}.is_always_exhaustive());
static_assert(!KokkosEx::layout_right_padded<Kokkos::dynamic_extent>::mapping<Kokkos::extents<std::size_t, 5, 4>>{}.is_always_exhaustive());
static_assert(KokkosEx::layout_right_padded<0>::mapping<Kokkos::extents<std::size_t, 6, 0>>{}.is_always_exhaustive());
static_assert(KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 6, 0>>{}.is_always_exhaustive());

TEST(LayoutrightTests, properties)
{
  // is_exhaustive
  // Sanity check -- if it is always exhaustive it should be exhaustive ^-^
  ASSERT_TRUE((KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t>>{}.is_exhaustive()));
  ASSERT_TRUE((KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 3>>{}.is_exhaustive()));
  ASSERT_TRUE((KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, Kokkos::dynamic_extent>>{}.is_exhaustive()));
  ASSERT_TRUE((KokkosEx::layout_right_padded<Kokkos::dynamic_extent>::mapping<Kokkos::extents<std::size_t, 3>>{}.is_exhaustive()));
  ASSERT_TRUE((KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 6, 4>>{}.is_exhaustive()));
  ASSERT_TRUE((KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 6, 8>>{}.is_exhaustive()));
  ASSERT_TRUE((KokkosEx::layout_right_padded<0>::mapping<Kokkos::extents<std::size_t, 6, 0>>{}.is_exhaustive()));
  ASSERT_TRUE((KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 6, 0>>{}.is_exhaustive()));

  // is_exhaustive with dynamic values
  ASSERT_TRUE((KokkosEx::layout_right_padded<Kokkos::dynamic_extent>::mapping<Kokkos::extents<std::size_t, 6, 4>>{Kokkos::extents<std::size_t, 6, 4>{}, 4}.is_exhaustive()));
  ASSERT_TRUE((KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 6, Kokkos::dynamic_extent>>{Kokkos::extents<std::size_t, 6, Kokkos::dynamic_extent>{8}}.is_exhaustive()));
  ASSERT_FALSE((KokkosEx::layout_right_padded<Kokkos::dynamic_extent>::mapping<Kokkos::extents<std::size_t, 6, 6>>{Kokkos::extents<std::size_t, 6, 6>{}, 4}.is_exhaustive()));
  ASSERT_FALSE((KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 6, Kokkos::dynamic_extent>>{Kokkos::extents<std::size_t, 6, Kokkos::dynamic_extent>{7}}.is_exhaustive()));

  // Equality
  ASSERT_EQ((KokkosEx::layout_right_padded<0>::mapping<Kokkos::extents<std::size_t, 0>>{}), (KokkosEx::layout_right_padded<0>::mapping<Kokkos::extents<std::size_t, 0>>{}));
  ASSERT_EQ((KokkosEx::layout_right_padded<0>::mapping<Kokkos::extents<std::size_t, 0>>{}), (KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 0>>{}));
  ASSERT_EQ((KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 3>>{}), (KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 3>>{}));
  ASSERT_NE((KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 3>>{}), (KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 5>>{}));
  ASSERT_EQ((KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, Kokkos::dynamic_extent>>{Kokkos::extents<std::size_t, Kokkos::dynamic_extent>{5}}),
            (KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, Kokkos::dynamic_extent>>{Kokkos::extents<std::size_t, Kokkos::dynamic_extent>{5}}));
  ASSERT_NE((KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, Kokkos::dynamic_extent>>{Kokkos::extents<std::size_t, Kokkos::dynamic_extent>{3}}),
            (KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, Kokkos::dynamic_extent>>{Kokkos::extents<std::size_t, Kokkos::dynamic_extent>{5}}));
  ASSERT_EQ((KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 7, 3>>{}), (KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 7, 3>>{}));
  ASSERT_NE((KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 7, 3>>{}), (KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 4, 3>>{}));
  ASSERT_NE((KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 7, 3>>{}), (KokkosEx::layout_right_padded<8>::mapping<Kokkos::extents<std::size_t, 7, 3>>{}));
  ASSERT_EQ((KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 3, Kokkos::dynamic_extent>>{Kokkos::extents<std::size_t, 3, Kokkos::dynamic_extent>{5}}),
            (KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 3, Kokkos::dynamic_extent>>{Kokkos::extents<std::size_t, 3, Kokkos::dynamic_extent>{5}}));
  ASSERT_NE((KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 3, Kokkos::dynamic_extent>>{Kokkos::extents<std::size_t, 3, Kokkos::dynamic_extent>{3}}),
            (KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 3, Kokkos::dynamic_extent>>{Kokkos::extents<std::size_t, 3, Kokkos::dynamic_extent>{5}}));
  ASSERT_EQ((KokkosEx::layout_right_padded<Kokkos::dynamic_extent>::mapping<Kokkos::extents<std::size_t, 7, 3>>{{}, 4}), (KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 7, 3>>{}));
  ASSERT_EQ((KokkosEx::layout_right_padded<Kokkos::dynamic_extent>::mapping<Kokkos::extents<std::size_t, 7, 3>>{{}, 4}), (KokkosEx::layout_right_padded<Kokkos::dynamic_extent>::mapping<Kokkos::extents<std::size_t, 7, 3>>{{}, 4}));
  ASSERT_NE((KokkosEx::layout_right_padded<Kokkos::dynamic_extent>::mapping<Kokkos::extents<std::size_t, 7, 3>>{{}, 4}), (KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 4, 3>>{}));
  ASSERT_NE((KokkosEx::layout_right_padded<Kokkos::dynamic_extent>::mapping<Kokkos::extents<std::size_t, 7, 3>>{{}, 4}), (KokkosEx::layout_right_padded<Kokkos::dynamic_extent>::mapping<Kokkos::extents<std::size_t, 7, 3>>{{}, 8}));
}

TEST(LayoutRightTests, stride)
{
  ASSERT_EQ((KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 7, 5>>().stride(0)), 8);
  ASSERT_EQ((KokkosEx::layout_right_padded<4>::mapping<Kokkos::extents<std::size_t, 7, 5>>().stride(1)), 1);
}
