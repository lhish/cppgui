//
// Created by lhy on 25-7-21.
//

#include "sdl_gui/gui/shadow_properties.h"

ShadowSingleLayerProperties operator*(ShadowSingleLayerProperties element, const float mul) {
  return element *= mul;
}

ShadowSingleLayerProperties operator+(ShadowSingleLayerProperties element, const ShadowSingleLayerProperties &add) {
  return element += add;
}

ShadowProperties operator*(ShadowProperties element, const float mul) {
  return element *= mul;
}

ShadowProperties operator+(ShadowProperties element, const ShadowProperties &add) {
  return element += add;
}
