/*
 * Remark:
 * The code provided here is not mandatory, as it can be replaced in board.h
 * with a simple std::initializer_list. The primary purpose of this code is to
 * practice meta/template programming and make the code more generic. Overall,
 * the technique used here can even be efficient for large arrays (not really,
 * because we can't create sequence larger than 500 elements. Maybe we should
 * split it on several sequences while creating), but in this project it is
 * unlikely to make a difference
 */

#ifndef __UTIL_H__
#define __UTIL_H__

#include <algorithm>
#include <array>
#include <concepts>

namespace util {
template <int... Ints> struct index_sequence {
  template <int N> using append = index_sequence<Ints..., N>;
};

template <template <int...> typename Seq, int... Ints>
consteval std::array<int, sizeof...(Ints)>
make_array_from_sequence(Seq<Ints...>) {
  return std::array<int, sizeof...(Ints)>{{Ints...}};
}

// Range for Board64
// The main idea is that we skip [9], [10], [18], [19], [28], [29] and so on
// elements in sequence
template <int Start, int End> struct make_range_sequence_with_two_gaps {
  using type = typename std::conditional_t<
      (End > Start),
      typename std::conditional_t<
          (End % 10 != (Start + 9) % 10) && (End % 10 != (Start + 8) % 10),
          typename make_range_sequence_with_two_gaps<
              Start, End - 1>::type::template append<End>,
          typename make_range_sequence_with_two_gaps<Start, End - 1>::type>,
      index_sequence<>>;
};

template <int Start> struct make_range_sequence_with_two_gaps<Start, Start> {
  using type = index_sequence<Start>;
};

// Range for Board120
// The main idea is to create a sequence in shell

// So, we have the number of columns and rows, which will mean the shell in
// which the sequence is located
static constexpr int rows = 2, cols = 1;

// This will be clearer with an example
// Sequence from 0 to 64; 65 will be shell number

// 65  65  65  65  65  65  65  65  65  65
// 65  65  65  65  65  65  65  65  65  65
// 65   0   1   2   3   4   5   6   7  65
// 65   8   9  10  11  12  13  14  15  65
// 65  16  17  18  19  20  21  22  23  65
// 65  24  25  26  27  28  29  30  31  65
// 65  32  33  34  35  36  37  38  39  65
// 65  40  41  42  43  44  45  46  47  65
// 65  48  49  50  51  52  53  54  55  65
// 65  56  57  58  59  60  61  62  63  65
// 65  65  65  65  65  65  65  65  65  65
// 65  65  65  65  65  65  65  65  65  65

// TODO: Check whether the range (End - Start) is valid, i.e. whether it is
// equal to a prime number. If not, find the two closest integers whose product
// is equal to (End - Start). The obtained numbers will be the number of rows
// and columns of the internal sequence. Try to make a more generalized code for
// different sequences (including bFiles from board.h)
// The actual size of sequence may be less then (End - Start)!
// To fix create size method in sequence and one more conjunct to check
// existance of this method

// Doesn't realy work right now, its more like blank

// Check if number is prime
template <int N, int I = 2> struct is_prime {
  static constexpr bool value = (N % I != 0) && is_prime<N, I + 1>::value;
};

template <int N> struct is_prime<N, N> {
  static constexpr bool value = (N > 1);
};

// Check difference is prime number
template <template <int, int> typename Seq, int Start, int End, typename = void>
struct is_difference_prime : std::false_type {};

template <template <int, int> typename Seq, int Start, int End>
struct is_difference_prime<Seq, Start, End,
                           std::void_t<decltype(is_prime<End - Start>::value)>>
    : std::integral_constant<bool, is_prime<End - Start>::value> {};

template <template <int, int> typename Seq, int Start, int End, typename = void>
struct has_typedef_type : std::false_type {};

template <template <int, int> typename Seq, int Start, int End>
struct has_typedef_type<Seq, Start, End,
                        std::void_t<typename Seq<Start, End>::type>>
    : std::true_type {};

template <template <int, int> typename Seq, int Start, int End>
concept SequenceCheck = has_typedef_type<Seq, Start, End>::value &&
                        !is_difference_prime<Seq, Start, End>::value;

// TODO: main algorithm to create shell
template <template <int Start, int End> typename Seq, int Start, int End,
          int shellNum>
  requires SequenceCheck<Seq, Start, End>
struct make_range_sequence_in_shell {
  using type = typename Seq<Start, End>::type;
};

make_range_sequence_in_shell<make_range_sequence_with_two_gaps, 21, 98, 100>;

// Error: constraints not satisfied for class template
// 28 - 21 = 7 which is prime number
// make_range_sequence_in_shell<make_range_sequence_with_two_gaps, 21, 28, 100>;

template <int Start, int End> struct fake_range {
  using typ = int;      // not type
  virtual void f() = 0; // A workaround so you can't create it
};
// Will case same error, because `type` is not defined
// make_range_sequence_in_shell<fake_range, 21, 98, 100>;

} // namespace util

#endif // __UTIL_H__