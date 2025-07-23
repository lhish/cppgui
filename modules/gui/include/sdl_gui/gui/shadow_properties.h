//
// Created by lhy on 25-7-21.
//

#ifndef SHADOW_PROPERTIES_H
#define SHADOW_PROPERTIES_H
#include <array>
#include <map>

struct ShadowSingleLayerProperties {
  float offset_y_;
  float blur_radius_;
  float extend_;
  float alpha_;

  ShadowSingleLayerProperties &operator*=(const float mul) {
    offset_y_ *= mul;
    blur_radius_ *= mul;
    extend_ *= mul;
    return *this;
  }

  ShadowSingleLayerProperties &operator+=(const ShadowSingleLayerProperties &add) {
    offset_y_ += add.offset_y_;
    blur_radius_ += add.blur_radius_;
    extend_ += add.extend_;
    return *this;
  }
};

ShadowSingleLayerProperties operator*(ShadowSingleLayerProperties element, float mul);

ShadowSingleLayerProperties operator+(ShadowSingleLayerProperties element, const ShadowSingleLayerProperties &add);

struct ShadowProperties {
  std::array<ShadowSingleLayerProperties, 3> layer;

  ShadowProperties &operator*=(const float mul) {
    for (auto &layer : layer) {
      layer *= mul;
    }
    return *this;
  }

  ShadowProperties &operator+=(const ShadowProperties &add) {
    for (int i = 0; i < layer.size(); i++) {
      layer[i] += add.layer[i];
    }
    return *this;
  }
};

ShadowProperties operator*(ShadowProperties element, float mul);

ShadowProperties operator+(ShadowProperties element, const ShadowProperties &add);

inline std::map<float, ShadowProperties> shadow_properties_map{
    {0, {ShadowSingleLayerProperties{0.0, 0.0, 0.0, 0.0}, {0.0, 0.0, 0.0, 0.0}, {0.0, 0.0, 0.0, 0.0}}},
    {1, {ShadowSingleLayerProperties{2.0, 1.0, -1.0, 0.14}, {1.0, 1.0, 0.0, 0.12}, {1.0, 3.0, 0.0, 0.2}}},
    {2, {ShadowSingleLayerProperties{3.0, 1.0, -2.0, 0.14}, {2.0, 2.0, 0.0, 0.12}, {1.0, 5.0, 0.0, 0.2}}},
    {3, {ShadowSingleLayerProperties{3.0, 3.0, -2.0, 0.14}, {3.0, 4.0, 0.0, 0.12}, {1.0, 8.0, 0.0, 0.2}}},
    {4, {ShadowSingleLayerProperties{2.0, 4.0, -1.0, 0.14}, {4.0, 5.0, 0.0, 0.12}, {1.0, 10.0, 0.0, 0.2}}},
    {6, {ShadowSingleLayerProperties{3.0, 5.0, -1.0, 0.14}, {6.0, 10.0, 0.0, 0.12}, {1.0, 18.0, 0.0, 0.2}}},
    {8, {ShadowSingleLayerProperties{5.0, 5.0, -3.0, 0.14}, {8.0, 10.0, 1.0, 0.12}, {3.0, 14.0, 2.0, 0.2}}},
    {9, {ShadowSingleLayerProperties{5.0, 6.0, -3.0, 0.14}, {9.0, 12.0, 1.0, 0.12}, {3.0, 16.0, 2.0, 0.2}}},
    {12, {ShadowSingleLayerProperties{7.0, 8.0, -4.0, 0.14}, {12.0, 17.0, 2.0, 0.12}, {5.0, 22.0, 4.0, 0.2}}},
    {16, {ShadowSingleLayerProperties{8.0, 10.0, -5.0, 0.14}, {16.0, 24.0, 2.0, 0.12}, {6.0, 30.0, 5.0, 0.2}}},
    {24, {ShadowSingleLayerProperties{11.0, 15.0, -7.0, 0.14}, {24.0, 38.0, 3.0, 0.12}, {9.0, 46.0, 8.0, 0.2}}}};
#endif  // SHADOW_PROPERTIES_H
