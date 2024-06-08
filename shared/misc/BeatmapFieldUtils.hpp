#pragma once

#include "BeatmapSaveDataVersion2_6_0AndEarlier/BeatmapSaveDataItem.hpp"
#include "../CustomBeatmapSaveDatav3.h"
#include "../JSONWrapper.h"

namespace CustomJSONData {

static v3::CustomDataOpt JSONObjectOrNull(v3::CustomDataOpt const& val) {
  if (!val || !val->get().IsObject()) {
    return std::nullopt;
  }

  return val;
}
static JSONWrapper* JSONWrapperOrNull(v3::CustomDataOpt const& val) {
  auto* wrapper = JSONWrapper::New_ctor();

  if (!val || !val->get().IsObject()) {
    return wrapper;
  }

  wrapper->value = val;

  return wrapper;
}

static v3::CustomDataOptUTF16 JSONObjectOrNull(v3::CustomDataOptUTF16 const& val) {
  if (!val || !val->get().IsObject()) {
    return std::nullopt;
  }

  return val;
}
static JSONWrapperUTF16* JSONWrapperOrNull(v3::CustomDataOptUTF16 const& val) {
  auto* wrapper = JSONWrapperUTF16::New_ctor();

  if (!val || !val->get().IsObject()) {
    return wrapper;
  }

  wrapper->value = val;

  return wrapper;
}

constexpr float BeatmapSaveDataItem_GetBeat(GlobalNamespace::BeatmapDataItem const* item) {
  if (!item) return 0;
  return const_cast<GlobalNamespace::BeatmapDataItem*>(item)->time;
}

constexpr float
BeatmapSaveDataItem_GetBeat(BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveDataItem const* const item) {
  if (!item) return 0;

  return const_cast < BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveDataItem*>(item)->time;
}

constexpr float BeatmapSaveDataItem_GetBeat(BeatmapSaveDataVersion3::BeatmapSaveDataItem const* const item) {
  if (!item) return 0;

  return item->b;
}


constexpr float BeatmapSaveDataItem_GetBeat(BeatmapSaveDataVersion3::LightColorBaseData const* const item) {
  if (!item) return 0;

  return item->b;
}

//

constexpr auto&
LightColorBaseData_GetTransitionType(BeatmapSaveDataVersion3::LightColorBaseData const* item) {
  return item->i;
}

constexpr auto&
LightColorBaseData_GetColorType(BeatmapSaveDataVersion3::LightColorBaseData const* item) {
  return item->c;
}

constexpr auto&
LightColorBaseData_GetBrightness(BeatmapSaveDataVersion3::LightColorBaseData const* item) {
  return item->s;
}

constexpr auto&
LightColorBaseData_GetStrobeFrequency(BeatmapSaveDataVersion3::LightColorBaseData const* item) {
  return item->f;
}

//

constexpr auto
LightRotationBaseData_GetStrobeFrequency(BeatmapSaveDataVersion3::LightRotationBaseData const* item) {
  return item->p == 1;
}

constexpr auto&
LightRotationBaseData_GetRotation(BeatmapSaveDataVersion3::LightRotationBaseData const* item) {
  return item->r;
}

constexpr auto&
LightRotationBaseData_GetLoopsCount(BeatmapSaveDataVersion3::LightRotationBaseData const* item) {
  return item->l;
}

//

constexpr auto& EventBox_GetBeatDistributionParam(BeatmapSaveDataVersion3::EventBox const* item) {
  return item->w;
}

constexpr auto& EventBox_GetBeatDistributionParamType(BeatmapSaveDataVersion3::EventBox const* item) {
  return item->d;
}

//

constexpr auto& IndexFilter_GetParam0(BeatmapSaveDataVersion3::IndexFilter const* filter) {
  return filter->p;
}

constexpr auto& IndexFilter_GetParam1(BeatmapSaveDataVersion3::IndexFilter const* filter) {
  return filter->t;
}

constexpr auto IndexFilter_GetReversed(BeatmapSaveDataVersion3::IndexFilter const* filter) {
  return filter->r != 0;
}

constexpr auto& IndexFilter_GetFilterType(BeatmapSaveDataVersion3::IndexFilter const* filter) {
  return filter->f;
}
//

} // namespace CustomJSONData