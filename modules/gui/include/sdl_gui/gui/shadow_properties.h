//
// Created by lhy on 25-7-21.
//

#ifndef SHADOW_PROPERTIES_H
#define SHADOW_PROPERTIES_H
#include <array>
#include <unordered_map>

enum class ShadowHeights {
  d2, d3, d4, d6, d8, d16, d24
};

struct ShadowSingleLayerProperties {
  float offset_y_;
  float blur_radius_;
  float extend_;
  float alpha_;
};

struct ShadowProperties {
  std::array<ShadowSingleLayerProperties, 3> layer;
};

inline std::unordered_map<ShadowHeights, ShadowProperties> shadow_properties_map{
  {ShadowHeights::d2, {ShadowSingleLayerProperties{2, 2, 0, .14}, {1, 5, 0, .12}, {3, 1, -2, .2}}},
  {ShadowHeights::d3, {ShadowSingleLayerProperties{3, 4, 0, .14}, {1, 8, 0, .12}, {3, 3, -1, .4}}},
  {ShadowHeights::d4, {ShadowSingleLayerProperties{4, 5, 0, .14}, {1, 10, 0, .12}, {2, 4, -1, .4}}},
  {ShadowHeights::d6, {ShadowSingleLayerProperties{6, 10, 0, .14}, {1, 18, 0, .12}, {3, 5, -1, .4}}},
  {ShadowHeights::d8, {ShadowSingleLayerProperties{8, 10, 1, .14}, {3, 14, 2, .12}, {5, 5, -3, .4}}},
  {ShadowHeights::d16, {ShadowSingleLayerProperties{16, 24, 2, .14}, {6, 30, 5, .12}, {8, 10, -5, .4}}},
  {ShadowHeights::d24, {ShadowSingleLayerProperties{9, 46, 8, .14}, {24, 38, 3, .12}, {11, 15, -7, .4}}}
};
#endif //SHADOW_PROPERTIES_H
