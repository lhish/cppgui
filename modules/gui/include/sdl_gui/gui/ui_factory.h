//
// Created by lhy on 25-7-22.
//

#ifndef UI_FACTORY_H
#define UI_FACTORY_H
class UI;

enum class UIType { UI, UIGroup, Button, Shape };

template <UIType>
class UITypeHelper {};

#define UI_FACTORY_REGISTER(ENUM_NAME, CLASS) \
  template <>                                 \
  class UITypeHelper<ENUM_NAME> {             \
   public:                                    \
    using Type = CLASS;                       \
  };

class UIFactory {
 public:
  template <UIType type, typename... Args>
  static std::shared_ptr<UI> CreateUI(Args &&...args);
};

template <UIType type, typename... Args>
std::shared_ptr<UI> UIFactory::CreateUI(Args &&...args) {
  return std::make_shared<typename UITypeHelper<type>::Type>(std::forward<Args>(args)...);
}

#endif  // UI_FACTORY_H
