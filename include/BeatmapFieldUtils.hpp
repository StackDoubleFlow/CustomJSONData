#pragma once

#include "CustomBeatmapSaveDatav3.h"

namespace CustomJSONData {

    using namespace BeatmapSaveDataVersion3;


    constexpr auto& BeatmapSaveDataItem_GetBeat(BeatmapSaveData::BeatmapSaveDataItem const* item) {
        return item->b;
    }

    constexpr auto& BeatmapSaveDataItem_GetBeat(BeatmapSaveData::LightColorBaseData const* item) {
        return item->b;
    }

    //

    constexpr auto& LightColorBaseData_GetTransitionType(BeatmapSaveData::LightColorBaseData const* item) {
        return item->i;
    }

    constexpr auto& LightColorBaseData_GetColorType(BeatmapSaveData::LightColorBaseData const* item) {
        return item->c;
    }

    constexpr auto& LightColorBaseData_GetBrightness(BeatmapSaveData::LightColorBaseData const* item) {
        return item->s;
    }

    constexpr auto& LightColorBaseData_GetStrobeFrequency(BeatmapSaveData::LightColorBaseData const* item) {
        return item->f;
    }

    //

    constexpr auto LightRotationBaseData_GetStrobeFrequency(BeatmapSaveData::LightRotationBaseData const* item) {
        return item->p == 1;
    }

    constexpr auto& LightRotationBaseData_GetRotation(BeatmapSaveData::LightRotationBaseData const* item) {
        return item->r;
    }

    constexpr auto& LightRotationBaseData_GetLoopsCount(BeatmapSaveData::LightRotationBaseData const* item) {
        return item->l;
    }

    //

    constexpr auto& EventBox_GetBeatDistributionParam(BeatmapSaveData::EventBox const* item) {
        return item->w;
    }

    constexpr auto& EventBox_GetBeatDistributionParamType(BeatmapSaveData::EventBox const* item) {
        return item->d;
    }

    //

    constexpr auto& IndexFilter_GetParam0(BeatmapSaveData::IndexFilter const* filter) {
        return filter->p;
    }

    constexpr auto& IndexFilter_GetParam1(BeatmapSaveData::IndexFilter const* filter) {
        return filter->t;
    }

    constexpr auto IndexFilter_GetReversed(BeatmapSaveData::IndexFilter const* filter) {
        return filter->r != 0;
    }

    constexpr auto& IndexFilter_GetFilterType(BeatmapSaveData::IndexFilter const* filter) {
        return filter->f;
    }
    //

}