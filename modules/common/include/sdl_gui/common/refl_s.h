//
// Created by 连昊宇 on 25-2-19.
//

#ifndef REFL_S_H
#define REFL_S_H
#include "meta.h"
#include "macro.h"

#include <any>
#include <iostream>
#include <ranges>
#include <string>
#include <unordered_map>
#include <sstream>

namespace lhy {
enum class AccessSpecifier { PUBLIC, PROTECTED, PRIVATE };
const std::unordered_map<AccessSpecifier, std::string> access_specifier_map = {
    {AccessSpecifier::PUBLIC, "public"},
    {AccessSpecifier::PROTECTED, "protected"},
    {AccessSpecifier::PRIVATE, "private"}};
template <typename>
class refl_s {};
template <typename T, typename U>
constexpr T& GetMember(U& object, const std::string& name) {
  return std::any_cast<std::reference_wrapper<T>>(refl_s<U>::GetMember(object, name)).get();
}
template <typename T, typename U>
constexpr const T& GetMember(const U& object, const std::string& name) {
  return std::any_cast<std::reference_wrapper<const T>>(refl_s<U>::GetMember(object, name)).get();
}
template <typename T>
constexpr int GetMemberIndex(const std::string& name) {
  return refl_s<T>::GetMemberIndex(name);
}
template <typename T>
constexpr int GetMemberIsStatic(const std::string& name) {
  return refl_s<T>::GetMemberIsStatic(name);
}

template <typename T, typename U>
constexpr T& GetStaticMember(const std::string& name) {
  return std::any_cast<std::reference_wrapper<T>>(refl_s<U>::GetStaticMember(name)).get();
}
template <typename T>
constexpr AccessSpecifier GetMemberAccessSpecifier(const std::string& name) {
  return refl_s<T>::GetMemberAccessSpecifier(name);
}
template <typename T>
constexpr std::string GetMemberAccessSpecifierInString(const std::string& name) {
  return access_specifier_map.at(GetMemberAccessSpecifier<T>(name));
}
template <typename>
class AccessSpecifierHelper {};
template <typename>
class AccessSpecifierGetter {};
template <typename T>
class SerializeImpl;
class Serialize {
  // 单例类
 public:
  Serialize(const Serialize&) = delete;
  Serialize(Serialize&&) = delete;
  Serialize& operator=(const Serialize&) = delete;
  Serialize& operator=(Serialize&&) = delete;
  ~Serialize() = default;
  template <typename T>
  static std::string operator()(const T& object) {
    return SerializeImpl<T>::operator()(object);
  }
  static Serialize& GetInstance() {
    static Serialize instance;
    return instance;
  }

 private:
  Serialize() = default;
};
inline auto& serialize = Serialize::GetInstance();
template <typename T>
class SerializeImpl {
 public:
  static std::string operator()(const T& object) {
    std::stringstream out;
    out << "{";
    static_for<0, refl_s<T>::member_num - 1, IndexAddOne>([&out, &object]<int I>() {
      auto& [name, num] = refl_s<T>::member_names[I];
      out << "\"";
      out << name;
      out << "\"";
      out << ":";
      if constexpr (std::get<I>(refl_s<T>::member_static)) {
        out << serialize(std::forward<decltype(*std::get<I>(refl_s<T>::members))>(*std::get<I>(refl_s<T>::members)));
      } else {
        out << serialize(
            std::forward<decltype(object.*std::get<I>(refl_s<T>::members))>(object.*std::get<I>(refl_s<T>::members)));
      }
      if (I != refl_s<T>::member_num - 1) {
        out << ",";
      }
    });
    out << "}";
    return out.str();
  }
};
template <typename T>
  requires std::is_pointer_v<T>
class SerializeImpl<T> {};
template <>
class SerializeImpl<bool> {
 public:
  static std::string operator()(const bool& object) { return object ? "true" : "false"; }
};
inline std::string solvequote(const std::string& in) {
  std::string ret;
  for (auto& each : in) {
    if (each == '\"') {
      ret += '\\';
    }
    ret += each;
  }
  return ret;
}
template <>
class SerializeImpl<std::string> {
 public:
  static std::string operator()(const std::string& object) { return "\"" + solvequote(object) + "\""; }
};
template <typename T>
  requires(not std::is_same_v<T, bool> and not std::is_same_v<T, std::string> and requires(T x) { std::cout << x; })
class SerializeImpl<T> {
 public:
  static std::string operator()(const T& object) {
    std::stringstream out;
    out << object;
    return out.str();
  }
};
// 提取字符串中去除多余的空格
inline std::string_view trim(const std::string_view str) {
  const auto start = str.find_first_not_of(" \t\n\r");
  const auto end = str.find_last_not_of(" \t\n\r");
  if (start == std::string::npos || end == std::string::npos) {
    return "";
  }
  return str.substr(start, end - start + 1);
}

inline std::string_view trim_front(std::string_view str, char specific_char) {
  while (true) {
    if (str.front() == '\t' || str.front() == ' ' || str.front() == '\n' || str.front() == '\r') {
      str = str.substr(1, str.length() - 1);
    } else if (str.front() == specific_char) {
      str = str.substr(1, str.length() - 1);
      return str;
    } else if (str.empty()) {
      return str;
    }
  }
}
inline std::string_view trim_end(std::string_view str, char specific_char) {
  while (true) {
    if (str.back() == '\t' || str.back() == ' ' || str.back() == '\n' || str.back() == '\r') {
      str = str.substr(0, str.length() - 1);
    } else if (str.back() == specific_char) {
      str = str.substr(0, str.length() - 1);
      return str;
    } else if (str.empty()) {
      return str;
    }
  }
}
inline std::vector<std::string_view> split(const std::string_view str, char specific_char,
                                           const std::vector<std::pair<char, char>>& ignore_string = {},
                                           int max_split = 1000000) {
  /// 不太记得这个flag是干嘛的了
  std::vector<std::string_view> ret;
  int split_count = 0;
  bool flag = false;
  int start = 0;
  std::vector<char> solve_char;
  for (int i = 0; i < str.length(); i++) {
    auto& each = str[i];
    if (each == '\\') {
      flag = true;
    } else {
      if (flag == false && !solve_char.empty() && each == solve_char.back()) {
        solve_char.pop_back();
      } else if (auto pos = std::ranges::find_if(
                     ignore_string, [&each](const std::pair<char, char>& element) { return element.first == each; });
                 flag == false && pos != ignore_string.end()) {
        solve_char.emplace_back(pos->second);
      } else if (solve_char.empty() && each == specific_char) {
        ret.push_back(str.substr(start, i - start));
        split_count++;
        start = i + 1;
        if (split_count == max_split) {
          ret.emplace_back(str.substr(i + 1, str.length() - i - 1));
          return ret;
        }
        continue;
      }
      flag = false;
    }
  }
  if (start != str.length()) {
    ret.push_back(str.substr(start, str.length() - start));
  }
  if (ret.empty()) {
    ret.push_back(str);
  }
  return ret;
}
template <typename T>
class DeSerializeImpl;
class DeSerialize {
  // 单例类
 public:
  DeSerialize(const DeSerialize&) = delete;
  DeSerialize(DeSerialize&&) = delete;
  DeSerialize& operator=(const DeSerialize&) = delete;
  DeSerialize& operator=(DeSerialize&&) = delete;
  ~DeSerialize() = default;
  template <typename T>
  static void operator()(T& object, std::string_view in) {
    DeSerializeImpl<T>::operator()(object, in);
  }
  static DeSerialize& GetInstance() {
    static DeSerialize instance;
    return instance;
  }

 private:
  DeSerialize() = default;
};
inline auto& deserialize = DeSerialize::GetInstance();
// 提取 JSON 键值对
template <typename T>
class DeSerializeImpl {
 public:
  static void operator()(T& object, std::string_view json) {
    json = trim(json);

    // 确保是一个对象（花括号包裹的）
    if (json.front() != '{' || json.back() != '}') {
      throw std::runtime_error("Not a JSON object");
    }
    json = trim_end(json, '}');
    json = trim_front(json, '{');
    int count = 0;
    while (true) {
      json = trim_front(json, '"');
      auto name = split(json, '"')[0];
      json = split(json, ':', {}, 1)[1];
      json = trim(json);
      auto splits = split(json, ',', {{'\"', '\"'}, {'{', '}'}}, 1);
      auto value = trim(splits[0]);
      bool exist{};
      static_for<0, refl_s<T>::member_num - 1, IndexAddOne>([&object, &name, &value, &exist]<int I>() {
        if (refl_s<T>::member_names[I].first == name) {
          if constexpr (std::get<I>(refl_s<T>::member_static)) {
            deserialize(*std::get<I>(refl_s<T>::members), value);
          } else {
            deserialize(object.*std::get<I>(refl_s<T>::members), value);
          }
          exist = true;
        }
      });
      if (!exist) {
        throw std::runtime_error("deserialize with wrong type");
      }
      count++;
      if (splits.size() == 1) {
        break;
      }
      json = trim(splits[1]);
    }
    if (count != refl_s<T>::member_num) {
      throw std::runtime_error("deserialize with wrong type");
    }
  }
};
template <typename T>
  requires std::is_pointer_v<T>
class DeSerializeImpl<T> {};
template <>
class DeSerializeImpl<bool> {
 public:
  static void operator()(bool& object, const std::string_view json) {
    if (json != "true" && json != "false") {
      throw std::runtime_error("deserialize with wrong type");
    }
    object = (json == "true");
  }
};
template <>
class DeSerializeImpl<std::string> {
 public:
  static void operator()(std::string& object, const std::string_view json) {
    if (json.front() != '\"' || json.back() != '\"') {
      throw std::runtime_error("deserialize with wrong type");
    }
    object = json.substr(1, json.length() - 2);
  }
};
template <typename T>
  requires(not std::is_same_v<T, bool> and not std::is_same_v<T, std::string> and requires(T x) { std::cout << x; })
class DeSerializeImpl<T> {
 public:
  static void operator()(T& object, const std::string_view json) {
    std::stringstream in{std::string{json}};
    std::stringstream out;
    in >> object;
    out << object;
    if (out.str() != json) {
      throw std::runtime_error("deserialize with wrong type");
    }
  }
};
}  // namespace lhy
#ifdef __clang__
#define CREATE_SPECIFIC_ACCESS_SPECIFIER_GETTER(Class, KidName) \
  constexpr static bool is_protected_##KidName = AccessSpecifierHelperHelper##Class##KidName<Class>::Check_##KidName();
#define CREATE_SPECIFIC_ACCESS_SPECIFIER_GETTER_HELPER(Class, KidName) \
  template <typename T>                                                \
  class AccessSpecifierHelperHelper##Class##KidName : public T {       \
   public:                                                             \
    template <typename = int>                                          \
    constexpr static bool Check_##KidName() {                          \
      return false;                                                    \
    }                                                                  \
    template <typename = int>                                          \
      requires requires() { T::KidName; }                              \
    constexpr static bool Check_##KidName() {                          \
      return true;                                                     \
    }                                                                  \
  };
#elif defined(__GNUC__)
#define CREATE_SPECIFIC_ACCESS_SPECIFIER_GETTER(Class, KidName) \
  constexpr static bool is_protected_##KidName =                \
      static_cast<AccessSpecifierHelperHelper##Class##KidName<Class>*>(nullptr)->Check_##KidName();
#define CREATE_SPECIFIC_ACCESS_SPECIFIER_GETTER_HELPER(Class, KidName) \
  template <typename T>                                                \
  class AccessSpecifierHelperHelper##Class##KidName : public T {       \
   public:                                                             \
    constexpr bool Check_##KidName() {                                 \
      return requires() { T::KidName; };                               \
    }                                                                  \
  };
#else
#endif
// static对象必须是已经定义的了,不管是类内inline还是类外define
#define CONCEPT_OF_KID_ACCESS(Class, KidName) \
  template <typename T>                       \
  concept kid_access_##Class##_##KidName = requires() { T::KidName; };
#define CREATE_ACCESS_SPECIFIER_GETTER(Class, ...)                        \
  template <>                                                             \
  class AccessSpecifierHelper<Class> {                                    \
   public:                                                                \
    FOR2EACH(CREATE_SPECIFIC_ACCESS_SPECIFIER_GETTER, Class, __VA_ARGS__) \
  };
#define CREATE_SPECIFIC_ACCESS_SPECIFIER(Class, KidName)                                       \
  static constexpr auto access_specifier_##KidName =                                           \
      lhy::detail::kid_access_##Class##_##KidName<Class>                                       \
          ? AccessSpecifier::PUBLIC                                                            \
          : (AccessSpecifierHelper<Class>::is_protected_##KidName ? AccessSpecifier::PROTECTED \
                                                                  : AccessSpecifier::PRIVATE);
#define CREATE_ACCESS_SPECIFIER(Class, ...)                        \
  template <>                                                      \
  class AccessSpecifierGetter<Class> {                             \
   public:                                                         \
    FOR2EACH(CREATE_SPECIFIC_ACCESS_SPECIFIER, Class, __VA_ARGS__) \
  };
#define CREATE_ROBBER_ANOTHER1(Class, KidName) template <typename T
#define CREATE_ROBBER_ANOTHER2(Class, KidName)                                                                         \
  T* M > class Class##KidName##RetType<M> {                                                                            \
   public:                                                                                                             \
    using RealType = T;                                                                                                \
    using PointerType = T*;                                                                                            \
    friend constexpr auto GetPtr##Class##KidName##Type() { return RealType{}; };                                       \
    friend constexpr auto GetPtr##Class##KidName##PointerType() { return PointerType{}; };                             \
    constexpr static bool is_static = true;                                                                            \
    friend constexpr auto Get##Class##KidName##IsStatic() { return is_static; };                                       \
  };                                                                                                                   \
  template class Class##KidName##RetType<&Class::KidName>;                                                             \
  using Ret##Class##KidName##Type = FunctionProperties<decltype(GetPtr##Class##KidName##Type)>::RetType;               \
  using Ret##Class##KidName##PointerType = FunctionProperties<decltype(GetPtr##Class##KidName##PointerType)>::RetType; \
  constexpr bool Class##KidName##_is_static = Get##Class##KidName##IsStatic();                                         \
  constexpr Ret##Class##KidName##PointerType GetPtr##Class##KidName();                                                 \
  template <Ret##Class##KidName##PointerType M>                                                                        \
  struct Rob##Class##KidName {                                                                                         \
    friend constexpr Ret##Class##KidName##PointerType GetPtr##Class##KidName() { return M; }                           \
  };                                                                                                                   \
  template struct Rob##Class##KidName<&Class::KidName>;
#define CREATE_ROBBER1(Class, KidName)                  \
  template <auto T>                                     \
  class Class##KidName##RetType {};                     \
  constexpr auto GetPtr##Class##KidName##Type();        \
  constexpr auto GetPtr##Class##KidName##PointerType(); \
  constexpr auto Get##Class##KidName##IsStatic();       \
  template <typename T
#define CREATE_ROBBER2(Class, KidName)                                                     \
  T Class::* M > class Class##KidName##RetType<M> {                                        \
   public:                                                                                 \
    using RealType = T;                                                                    \
    using PointerType = T Class::*;                                                        \
    friend constexpr auto GetPtr##Class##KidName##Type() { return RealType{}; };           \
    friend constexpr auto GetPtr##Class##KidName##PointerType() { return PointerType{}; }; \
    constexpr static bool is_static = false;                                               \
    friend constexpr auto Get##Class##KidName##IsStatic() { return is_static; };           \
  };
#define GET_KID(Class, KidName) GetPtr##Class##KidName()
#define GET_KID_TYPE(Class, KidName) decltype(GET_KID(Class, KidName))
#define GET_KID_REAL_TYPE(Class, KidName) Ret##Class##KidName##Type
#define GET_KID_NAME_AND_NUM1(KidName, num) \
  {                                         \
    #KidName
#define GET_KID_NAME_AND_NUM2(KidName, num) \
  num                                       \
  }
#define GET_KID_STATIC(Class, KidName) Class##KidName##_is_static
#define GET_KID_ACCESS_SPECIFIER(Class, KidName) AccessSpecifierGetter<Class>::access_specifier_##KidName
// 很奇怪，在mac上就可以用std::array<std::string>但是，ubuntu上就不行
#define REFL(Class, ...)                                                                                         \
  namespace lhy {                                                                                                \
  namespace detail {                                                                                             \
  FOR2EACH(CONCEPT_OF_KID_ACCESS, Class, __VA_ARGS__)                                                            \
  }                                                                                                              \
  FOR2EACH(CREATE_SPECIFIC_ACCESS_SPECIFIER_GETTER_HELPER, Class, __VA_ARGS__)                                   \
  CREATE_ACCESS_SPECIFIER_GETTER(Class, __VA_ARGS__)                                                             \
  CREATE_ACCESS_SPECIFIER(Class, __VA_ARGS__)                                                                    \
  FOR22EACH(CREATE_ROBBER1, CREATE_ROBBER2, Class, __VA_ARGS__)                                                  \
  FOR22EACH(CREATE_ROBBER_ANOTHER1, CREATE_ROBBER_ANOTHER2, Class, __VA_ARGS__)                                  \
  template <>                                                                                                    \
  class refl_s<Class> {                                                                                          \
   public:                                                                                                       \
    constexpr static std::tuple<FOR_COMMA2_EACH(GET_KID_TYPE, Class, __VA_ARGS__)> members{                      \
        FOR_COMMA2_EACH(GET_KID, Class, __VA_ARGS__)};                                                           \
    constexpr static std::tuple<FOR_COMMA2_EACH(GET_KID_TYPE, Class, __VA_ARGS__)> members_real_type{};          \
    constexpr static std::array<std::pair<const char*, int>, COUNT_ARGS(__VA_ARGS__)> member_names{              \
        {FOR_COMMA22num_EACH(GET_KID_NAME_AND_NUM1, GET_KID_NAME_AND_NUM2, __VA_ARGS__)}};                       \
    constexpr static std::array<bool, COUNT_ARGS(__VA_ARGS__)> member_static{                                    \
        FOR_COMMA2_EACH(GET_KID_STATIC, Class, __VA_ARGS__)};                                                    \
    constexpr static std::array<AccessSpecifier, COUNT_ARGS(__VA_ARGS__)> member_access_specifier{               \
        FOR_COMMA2_EACH(GET_KID_ACCESS_SPECIFIER, Class, __VA_ARGS__)};                                          \
    constexpr static int member_num{COUNT_ARGS(__VA_ARGS__)};                                                    \
    template <int Index>                                                                                         \
    constexpr static decltype(auto) GetStaticMemberReal() {                                                      \
      if constexpr (std::get<Index>(member_static)) {                                                            \
        return *std::get<Index>(members);                                                                        \
      } else {                                                                                                   \
        throw std::runtime_error("member isn't static");                                                         \
        static Class a;                                                                                          \
        return a.*std::get<Index>(members);                                                                      \
      }                                                                                                          \
    }                                                                                                            \
    template <int Index>                                                                                         \
    constexpr static decltype(auto) GetMemberReal(Class& object) {                                               \
      if constexpr (std::get<Index>(member_static)) {                                                            \
        return *std::get<Index>(members);                                                                        \
      } else {                                                                                                   \
        return object.*std::get<Index>(members);                                                                 \
      }                                                                                                          \
    }                                                                                                            \
    template <int Index>                                                                                         \
    constexpr static decltype(auto) GetMemberReal(const Class& object) {                                         \
      if constexpr (std::get<Index>(member_static)) {                                                            \
        return *std::get<Index>(members);                                                                        \
      } else {                                                                                                   \
        return object.*std::get<Index>(members);                                                                 \
      }                                                                                                          \
    }                                                                                                            \
    constexpr static bool GetMemberIsStatic(const std::string& name) {                                           \
      const int member_index = GetMemberIndex(name);                                                             \
      return *static_for_with_return<0, MINUS(COUNT_ARGS(__VA_ARGS__))(), IndexAddOne, std::optional<bool>>(     \
          [&member_index]<int I>() -> std::optional<bool> {                                                      \
            if (I == member_index) {                                                                             \
              return std::get<I>(member_static);                                                                 \
            }                                                                                                    \
            return {};                                                                                           \
          });                                                                                                    \
    }                                                                                                            \
    constexpr static int GetMemberIndex(const std::string& name) {                                               \
      const std::array<std::pair<const char*, int>, COUNT_ARGS(__VA_ARGS__)>::const_iterator result =            \
          std::ranges::find_if(member_names, [&name](const std::pair<const char*, int>& element) {               \
            return std::string(element.first) == name;                                                           \
          });                                                                                                    \
      if (result == member_names.end()) {                                                                        \
        throw std::runtime_error("member not found");                                                            \
      }                                                                                                          \
      return result->second;                                                                                     \
    }                                                                                                            \
    constexpr static std::any GetStaticMember(const std::string& name) {                                         \
      const int member_index = GetMemberIndex(name);                                                             \
      if (member_static[member_index] == 0) {                                                                    \
        throw std::runtime_error("member isn't static");                                                         \
      }                                                                                                          \
      return static_for_with_return<0, MINUS(COUNT_ARGS(__VA_ARGS__))(), IndexAddOne, std::any>(                 \
          [&member_index]<int I>() -> std::any {                                                                 \
            if (I == member_index) {                                                                             \
              return std::reference_wrapper<std::decay_t<decltype(GetStaticMemberReal<I>())>>(                   \
                  GetStaticMemberReal<I>());                                                                     \
            }                                                                                                    \
            return {};                                                                                           \
          });                                                                                                    \
    }                                                                                                            \
    constexpr static std::any GetMember(Class& object, const std::string& name) {                                \
      const int member_index = GetMemberIndex(name);                                                             \
      return static_for_with_return<0, MINUS(COUNT_ARGS(__VA_ARGS__))(), IndexAddOne, std::any>(                 \
          [&member_index, &object]<int I>() -> std::any {                                                        \
            if (I == member_index) {                                                                             \
              return std::reference_wrapper<std::decay_t<decltype(GetMemberReal<I>(object))>>(                   \
                  GetMemberReal<I>(object));                                                                     \
            }                                                                                                    \
            return {};                                                                                           \
          });                                                                                                    \
    }                                                                                                            \
    constexpr static std::any GetMember(const Class& object, const std::string& name) {                          \
      const int member_index = GetMemberIndex(name);                                                             \
      return static_for_with_return<0, MINUS(COUNT_ARGS(__VA_ARGS__))(), IndexAddOne, std::any>(                 \
          [&member_index, &object]<int I>() -> std::any {                                                        \
            if (I == member_index) {                                                                             \
              return std::reference_wrapper<std::add_const_t<std::decay_t<decltype(GetMemberReal<I>(object))>>>( \
                  GetMemberReal<I>(object));                                                                     \
            }                                                                                                    \
            return {};                                                                                           \
          });                                                                                                    \
    }                                                                                                            \
    constexpr static AccessSpecifier GetMemberAccessSpecifier(const std::string& name) {                         \
      const int member_index = GetMemberIndex(name);                                                             \
      return *static_for_with_return<0, MINUS(COUNT_ARGS(__VA_ARGS__))(), IndexAddOne,                           \
                                     std::optional<AccessSpecifier>>(                                            \
          [&member_index]<int I>() -> std::optional<AccessSpecifier> {                                           \
            if (I == member_index) {                                                                             \
              return std::get<I>(member_access_specifier);                                                       \
            }                                                                                                    \
            return {};                                                                                           \
          });                                                                                                    \
    }                                                                                                            \
  };                                                                                                             \
  }

#endif  // REFL_S_H
