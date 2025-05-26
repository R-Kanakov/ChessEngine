/*
 * Remark:
 * The code provided here is not mandatory, as it can be replaced in board.h
 * with a simple initializer lists. The primary purpose of this code is to
 * practice meta/template programming and make the code more "generic". Overall,
 * the technique used here can even be efficient for large arrays (not really
 * for now because of compiler constraint:
 * "recursive template instantiation exceeded maximum depth of 1024")
 */


#ifndef __UTIL_H__
#define __UTIL_H__

#include <algorithm>
#include <array>
#include <concepts>
#include <iostream>

namespace util {
template <size_t... Ints> struct index_sequence {
  template <size_t N> using append = index_sequence<Ints..., N>;
};

template <template <size_t...> typename Seq, size_t... Ints>
consteval std::array<size_t, sizeof...(Ints)>
make_array_from_sequence(Seq<Ints...>) {
  return std::array<size_t, sizeof...(Ints)>{Ints...};
}


// Integer square root
template <size_t N, size_t L = 1, size_t H = N, size_t mid = (L + H + 1) / 2>
struct Sqrt : std::integral_constant <size_t, std::conditional_t<
  (N < mid* mid),
    Sqrt<N, L, mid - 1>,
    Sqrt<N, mid, H>>
  {}>{};

template <size_t N, size_t S>
struct Sqrt<N, S, S, S> : std::integral_constant<size_t, S> {};


// Check if number is prime
template <size_t N, size_t I = 2> struct is_prime {
  static constexpr bool value = (N > I) && (N % I != 0) &&
                                is_prime<N, I + 1>::value;
};
//                                     Sqrt<N>> {
template <size_t N> struct is_prime<N, N> { 
  static constexpr bool value = (N > 1);
};


// Check field (::type) for existance
template <template <size_t, size_t> typename Seq,
          size_t Start, size_t End,
          typename = void>
struct has_typedef_type : std::false_type {};

template <template <size_t, size_t> typename Seq,
          size_t Start, size_t End>
struct has_typedef_type<Seq, Start, End,
                        std::void_t<typename Seq<Start, End>::type>>
    : std::true_type {};


// Size of sequence
template <template <size_t...> typename Seq, size_t... Ints>
consteval size_t seq_size(Seq<Ints...>) {
  return sizeof...(Ints);
}


// Finding two closest numbers which production give size of sequence
template <template <size_t, size_t> typename Seq, size_t Start, size_t End,
          size_t size>
consteval std::pair<size_t, size_t> find_pair() {
  size_t r = 0, c = 0, sqrt = Sqrt<size>::value;

  for (size_t i = sqrt; i > 1; --i)
    if (size % i == 0) {
      r = i;
      c = size / i;
      break;
    }

  if (r < c)
    std::swap(r, c);

  return {r, c};
}


// ----------------------------Sequence for Board64----------------------------
// The main idea is that we skip [9], [10], [19], [20], [29], [30] and so on
// elements in sequence

// Example: sequence from 1 to 18

// 1   2   3   4
// 5   6   7   8
// 11  12  13  14
// 15  16  17  18

template <size_t Start, size_t End> struct range_sequence_with_two_gaps {
  static_assert(Start < End);

  using type = typename std::conditional_t<
    (End > Start),
      typename std::conditional_t<
        (End % 10 != (Start + 9) % 10) && (End % 10 != (Start + 8) % 10),
          typename range_sequence_with_two_gaps<Start, End - 1>::type::template append<End>,
          typename range_sequence_with_two_gaps<Start, End - 1>::type>,
      index_sequence<>>;
};

template <size_t Start> struct range_sequence_with_two_gaps<Start, Start> {
  using type = index_sequence<Start>;
};


// ---------------------------Sequence for Board120---------------------------
// The main idea is to create a shell with a sequence in it

// I think this would be even more complicated to use if I allow user to change
// the number of rows and columns a shellNumber will occupy
// So....
static constexpr int shellRows = 2, shellCols = 1;

// The only constraint is that the number of all its elements
// (which are really used in it) mustn't be a prime number

// This will be clearer with an example:
// Input is a `simple_sequence` (you can use any other) from 1 to 4
// 5 will be shell number

// 5  5  5  5
// 5  5  5  5
// 5  1  2  5
// 5  3  4  5
// 5  5  5  5
// 5  5  5  5

// concept for make_range_sequence_in_shell
template <template <size_t, size_t> typename Seq, size_t Start, size_t End>
concept SequenceCheck = has_typedef_type<Seq, Start, End>::value &&
  !(is_prime<seq_size(typename Seq<Start, End>::type{})>::value);

// Main algorithm to create sequence in a shell
template <template <size_t Start, size_t End> typename Seq,
          size_t Start, size_t End, size_t shellNum>
  requires SequenceCheck<Seq, Start, End>
struct sequence_in_shell {
  static_assert(Start < End);

  using type = typename Seq<Start, End>::type;

  static constexpr size_t size_ = seq_size(type{});

  static constexpr size_t rows = find_pair<Seq, Start, End, size_>().first,
                          cols = find_pair<Seq, Start, End, size_>().second,
                          totalRows = rows + shellRows * 2,
                          totalCols = cols + shellCols * 2,
                          total = totalCols * totalRows;

  static consteval std::array<size_t, total> get() {
    std::array<size_t, size_> seq = make_array_from_sequence(type{});
    std::array<size_t, total> arr{};

    for (size_t i = 0; i < total; ++i) {
      if (i < shellRows * totalCols + shellCols)
        arr[i] = shellNum;
      else {

        constexpr auto is_in_range = [](size_t i) {
          size_t j;
          for (j = 0; j < rows; ++j) {
            size_t lower = j * totalCols + shellRows * totalCols + shellCols;
            size_t upper = lower + cols - 1;
            if (i >= lower && i <= upper)
              return std::make_pair(true, j);
          }
          return std::make_pair(false, size_t{0});
        };

        auto [in_range, j] = is_in_range(i);

        if (in_range) {
          size_t index = i - (shellRows * totalCols + shellCols +
                              j * 2 * shellCols);

          arr[i] = seq[index];
        }
        else {
          arr[i] = shellNum;
        }
      }
    }
    return arr;
  }
};


// TODO: move it to tests
// Concept work check

// sequence_in_shell<range_sequence_with_two_gaps, 21, 35, 100>;

// Error: "constraints not satisfied for class template", because
// if range_sequence_with_two_gaps `A` has numbers [21, 35] \ [29, 30] ->
// -> size(A) = 13

template <int Start, int End> struct fake_range {
  using typ = int;      // not equal to `type`
  virtual void f() = 0; // A workaround so that this object cannot be created
};

// Will cause same error, because `type` is not defined
// sequence_in_shell<fake_range, 21, 98, 100>;


// ----------------------------Sequence for setMask----------------------------
consteval size_t shift(size_t i) { return (1ull << i); }

template <size_t Start, size_t End> struct sequence_with_shift {
  using type = typename sequence_with_shift<Start, End - 1>
                        ::type::template append<shift(End)>;
};

template <size_t Start> struct sequence_with_shift<Start, Start> {
  using type = index_sequence<Start>;
};


// ---------------------------Sequence for clearMask---------------------------
consteval size_t shift_with_tilda(size_t i) { return ~(1ull << i); }

template <size_t Start, size_t End> struct sequence_with_tilda_shift {
  static_assert(Start < End);
  using type = typename sequence_with_shift<
      Start, End - 1>::type::template append<shift_with_tilda(End)>;
};

template <size_t Start> struct sequence_with_tilda_shift<Start, Start> {
  using type = index_sequence<Start>;
};


// ------------------------------Simple sequence------------------------------
template <size_t Start, size_t End> struct simple_sequence {
  static_assert(Start < End);

  using type = typename simple_sequence<Start, End - 1>::type::template append<End>;
};

template <size_t Start> struct simple_sequence<Start, Start> {
  using type = index_sequence<Start>;
};


// --------------------------Sequence for files array--------------------------
// To create it we must concatenate N sequences
template <typename Seq1, typename Seq2> struct concat_sequences;

template <size_t... Ints1, size_t... Ints2>
struct concat_sequences<index_sequence<Ints1...>, index_sequence<Ints2...>> {
  using type = index_sequence<Ints1..., Ints2...>;
};

template <template <size_t, size_t> typename Seq, size_t Start, size_t End,
          size_t N>
struct concat {
  static_assert(Start < End);

  using type =
      typename concat_sequences<typename concat<Seq, Start, End, N - 1>::type,
                                typename Seq<Start, End>::type>::type;
};

template <template <size_t, size_t> typename Seq, size_t Start, size_t End>
struct concat<Seq, Start, End, 1> {
  using type = typename Seq<Start, End>::type;
};


// --------------------------Sequence for rank array--------------------------
template <size_t Num, size_t N> struct unique_sequence {
  using type = unique_sequence<Num, N - 1>::type::template append<Num>;
};

template <size_t Num> struct unique_sequence<Num, 1> {
  using type = index_sequence<Num>;
};

template <size_t From, size_t To, size_t N> struct columnar_sequence {
  static_assert(From < To);

  template <size_t Current> struct generate_columns {
    using type = typename concat_sequences<
        typename generate_columns<Current - 1>::type,
        typename unique_sequence<Current, N>::type>::type;
  };

  template <> struct generate_columns<From> {
    using type = unique_sequence<From, N>::type;
  };

  using type = typename generate_columns<To>::type;
};


// --------------------------Sequence to reset board--------------------------
template <size_t Num, size_t N>
struct zeros_sequence {
  using type = zeros_sequence<Num, N - 1>::type::template append<Num>;
};

template <size_t Num>
struct zeros_sequence<Num, 1> {
  using type = index_sequence<Num>;
};

} // namespace util

#endif // __UTIL_H__