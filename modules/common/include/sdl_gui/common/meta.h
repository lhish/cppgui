//
// Created by lhy on 24-8-11.
//

#ifndef META_H
#define META_H
#include <functional>
#include <type_traits>

namespace lhy {
template <typename T>
class FunctionProperties {};
template <typename T, typename... Args>
class FunctionProperties<T(Args...)> {
 public:
  using RetType = T;
};
template <typename T, typename... Args>
class FunctionProperties<std::function<T(Args...)>> {
 public:
  using RetType = T;
};
template <size_t... Index>
class IndexSequence {};
template <size_t N, typename = void>
class CreateIndexSequence {
 public:
  using type = typename CreateIndexSequence<N - 1, IndexSequence<N - 1>>::type;
};
template <>
class CreateIndexSequence<1, void> {
 public:
  using type = IndexSequence<0>;
};
template <size_t... Index>
class CreateIndexSequence<1, IndexSequence<Index...>> {
 public:
  using type = IndexSequence<0, Index...>;
};
template <size_t N, size_t... Index>
class CreateIndexSequence<N, IndexSequence<Index...>> {
 public:
  using type = typename CreateIndexSequence<N - 1, IndexSequence<N - 1, Index...>>::type;
};
/// IndexSequence在加了<size_t ...Index>后，就无法识别能不能转换了，因此只能通过继承的方式来进行转换
template <typename... T>
class IndexSequenceFor : public CreateIndexSequence<sizeof...(T)>::type {};

class IndexAddOne {
 public:
  template <int i>
  static constexpr int value = std::integral_constant<int, i + 1>::value;
};
class IndexMinusOne {
 public:
  template <int i>
  static constexpr int value = std::integral_constant<int, i - 1>::value;
};
template <typename T>
concept ConstexprNumericFunc = requires() { T::template value<0>; };
template <size_t... Index>
constexpr void static_for(auto fun, IndexSequence<Index...>) {
  (fun.template operator()<Index>(), ...);
}
template <int Beg, int End, ConstexprNumericFunc IndexChangeFunc = IndexAddOne>
constexpr void static_for(auto fun) {
  fun.template operator()<Beg>();
  if constexpr (Beg != End) {
    static_for<IndexChangeFunc::template value<Beg>, End, IndexChangeFunc>(fun);
  }
}
template <int Beg, int End, ConstexprNumericFunc IndexChangeFunc = IndexAddOne, typename RetType>
constexpr RetType static_for_with_return(auto fun) {
  auto ret = fun.template operator()<Beg>();
  if (ret.has_value()) {
    return ret;
  }
  if constexpr (Beg != End) {
    return static_for_with_return<IndexChangeFunc::template value<Beg>, End, IndexChangeFunc, RetType>(fun);
  }
  return {};
}
}  // namespace lhy

#endif  // META_H
