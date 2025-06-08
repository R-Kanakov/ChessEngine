// Licensed after GNU GPL v3

/*
 * Remark:
 * The code provided here is not mandatory, as it can be replaced in board.hpp
 * with a simple initializer lists (std::array doesn't really
 * have constructor from std::initializer_list, only direct init).
 * The primary purpose of this code is to practice meta-template programming
 * and make the code more "genelized".
 * Overall, the technique used here can be efficient for large constexpr arrays.
 * Not really for now because of compiler constraint:
 * "recursive template instantiation exceeded maximum depth of 1024".
 * Can be fixed via -ftemplate-depth=10000 (clang++) flag, but it works really bad.
 * Probably the best option to divide instatitions to the maximum possible step
 * (the big problem is to know how can it be for arbitrary recursive template)
 * and concatenate all of them via `concat_sequences`.
 * One more idea of usage is to create some simple sequence and than do
 * seq -> transform -> transform -> ... -> in_shell
 * in compile time.
 */


#ifndef __UTIL_HPP__
#define __UTIL_HPP__

#include <array>
#include <concepts>

namespace util {
template <std::integral T, T... Nums> struct sequence {
  template <T N> using append = sequence<T, Nums..., N>;

  using value_type = T;

  static constexpr size_t size() {
    return sizeof...(Nums);
  }
};

// Sequence to array
template <std::integral T, T... Nums>
consteval auto stoa(sequence<T, Nums...>) {
  return std::array<T, sizeof...(Nums)>{{Nums...}};
}


// Integer square root
template <std::integral T, T N, T L = 1, T H = N, T mid = (L + H + 1) / 2>
struct Sqrt : std::integral_constant < T,
  std::conditional_t<
    (N < mid* mid),
      Sqrt<T, N, L, mid - 1>,
      Sqrt<T, N, mid, H>
    >{}
  >{};

  template <std::integral T, T N, T S>
  struct Sqrt<T, N, S, S, S> : std::integral_constant<T, S> {};


  // Check if number is prime
  template <size_t N, size_t I = 2> struct is_prime {
    static constexpr bool value = (N > I) && (N % I != 0) &&
                                  is_prime<N, I + 1>::value;
  };
  //                                     Sqrt<N>> {
  template <size_t N> struct is_prime<N, N> {
    static constexpr bool value = (N > 1);
  };


  // Check field `type` for existance
  template <typename Seq,
    typename = void>
  struct has_typedef_type : std::false_type {};

  template <typename Seq>
  struct has_typedef_type<Seq, std::void_t<typename Seq::type>>
    : std::true_type {};


  // Check is T instance of sequence
  template <typename T>
  struct is_instance_of : std::false_type {};

  template <typename T, T... Args>
  struct is_instance_of<sequence<T, Args...>> : std::true_type {};


  // Finding two closest numbers which production give size of sequence
  template <size_t size>
  consteval std::pair<size_t, size_t> find_pair() {
    size_t r = 0, c = 0, sqrt = Sqrt<size_t, size>::value;

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

  // Sequence for Board64
  // The main idea is that we skip [8], [9, [18], [19], [28], [29] and so on
  // elements in sequence

  // Example: sequence from 1 to 18
  // 1   2   3   4   5   6   7   8
  // 11  12  13  14  15  16  17  18

  template <std::integral T, T Start, T End> struct sequence_with_two_gaps {
    static_assert(Start < End);
    using type = typename std::conditional_t<
      (End > Start),
        typename std::conditional_t<
          (End % 10 != (Start + 9) % 10) && (End % 10 != (Start + 8) % 10),
          typename sequence_with_two_gaps<T, Start, End - 1>::type::template append<End>,
          typename sequence_with_two_gaps<T, Start, End - 1>::type>,
        sequence<T>>;
  };

  template <std::integral T, T Start>
  struct sequence_with_two_gaps<T, Start, Start> {
    using type = sequence<T, Start>;
  };

  // Sequence for Board120
  // The main idea is to create a shell with a sequence in it

  // I think this would be even more complicated to use if I allow user to change
  // the number of rows and columns a shellNumber will occupy
  // So....
  static constexpr int shellRows = 2, shellCols = 1;

  // The only constraint is that the number of all its elements
  // (which are really used in it) mustn't be a prime number

  // This will be clearer with an example:
  // Input is a `сonsecutive_sequence` (or any other) from 1 to 4,
  // 5 will be shell number

  // 5  5  5  5
  // 5  5  5  5
  // 5  1  2  5
  // 5  3  4  5
  // 5  5  5  5
  // 5  5  5  5

  // concept for sequence_in_shell
  template <typename T, typename Seq>
  concept SequenceCheck = is_instance_of<Seq>::value &&
                          !(is_prime<Seq::size()>::value);

  // Main algorithm to create sequence in a shell
  template <std::integral T, typename Seq, T shellNum>
    requires SequenceCheck<T, Seq>
  struct sequence_in_shell {
    static constexpr size_t size_ = Seq::size();

    static constexpr size_t rows = find_pair<size_>().first,
                            cols = find_pair<size_>().second,
                            totalRows = rows + shellRows * 2,
                            totalCols = cols + shellCols * 2,
                            total = totalCols * totalRows;

    static consteval auto get() {
      std::array<T, size_> seq = stoa(Seq{});
      std::array<T, total> arr{};

      for (size_t i = 0; i < total; ++i) {
        if (i < shellRows * totalCols + shellCols)
          arr[i] = shellNum;
        else {
          constexpr auto is_in_range = [](size_t i) {
            for (size_t j = 0; j < rows; ++j) {
              size_t lower = j * totalCols + shellRows * totalCols + shellCols;
              size_t upper = lower + cols - 1;
              if (lower <= i && i <= upper)
                return std::make_pair(true, j);
            }
            return std::make_pair(false, size_t {0});
          };

          auto [in_range, j] = is_in_range(i);

          if (in_range) {
            size_t index = i - (shellRows * totalCols + shellCols +
                                j * 2 * shellCols);
            arr[i] = seq[index];
          }
          else
            arr[i] = shellNum;
        }
      }
      return arr;
    }
  };


  // Sequence that will transform each element it have with transform function
  template <std::integral T, auto transform, T Start, T End>
  struct sequence_transform {
    static_assert(Start < End);
    using type = typename sequence_transform<T, transform, Start, End - 1>
                          ::type::template append<transform(End)>;
  };

  template <std::integral T, auto transform, T Start>
  struct sequence_transform<T, transform, Start, Start> {
    using type = sequence<T, transform(Start)>;
  };


  // Function for setMask
  template <std::integral T>
  constexpr auto shift = [](T i) { return static_cast<T>(0ull | (1ull << i)); };

  // Function for clearMask
  template <std::integral T>
  constexpr auto shift_with_tilda = [](T i) { return ~(static_cast<T>(0ull | (1ull << i))); };


  // Consecutive sequence
  template <std::integral T, T Start, T End>
  struct сonsecutive_sequence {
    static_assert(Start < End);
    using type = сonsecutive_sequence<T, Start, End - 1>::type::template append<End>;
  };

  template <std::integral T, T Start>
  struct сonsecutive_sequence<T, Start, Start> {
    using type = sequence<T, Start>;
  };

  // Concatenation of N sequences
  template <typename Seq1, typename Seq2> struct concat_sequences;

  template <std::integral T, T... Ints1, T... Ints2>
  struct concat_sequences<sequence<T, Ints1...>, sequence<T, Ints2...>> {
    using type = sequence<T, Ints1..., Ints2...>;
  };

  template <typename Seq, size_t N>
  struct n_sequences {};

  template <std::integral T, T... Nums, size_t N>
  struct n_sequences<sequence<T, Nums...>, N> {
    static_assert(N != 0ull);
    using type = typename concat_sequences<
      typename n_sequences<sequence<T, Nums...>, N - 1>::type,
      typename sequence<T, Nums...>>::type;
  };

  template <std::integral T, T... Nums>
  struct n_sequences<sequence<T, Nums...>, 1ull> {
    using type = sequence<T, Nums...>;
  };

  // Sequence with contains N single number
  template <std::integral T, T Num, size_t N>
  struct unique_sequence {
    using type = unique_sequence<T, Num, N - 1>::type::template append<Num>;
  };

  template <std::integral T, T Num>
  struct unique_sequence<T, Num, 1> {
    using type = sequence<T, Num>;
  };

  // Sequence in which each number in an interval [From, To] is repeated N times
  template <std::integral T, T From, T To, size_t N>
  struct columnar_sequence {
    static_assert(From < To);
  private:
    template <T Current>
    struct generate_columns {
      using type = typename concat_sequences<
        typename generate_columns<Current - 1>::type,
        typename unique_sequence<T, Current, N>::type>::type;
    };

    template <> struct generate_columns<From> {
      using type = unique_sequence<T, From, N>::type;
    };
  public:
    using type = typename generate_columns<To>::type;
  };

  // template <std::integral T, T Num, T... Except>

} // namespace util

#endif // __UTIL_HPP__