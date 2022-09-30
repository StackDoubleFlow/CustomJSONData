#pragma once

#include "CustomBeatmapSaveDatav3.h"

namespace CustomJSONData {
    

    inline v3::CustomDataOpt JSONObjectOrNull(v3::CustomDataOpt const& val) {
        if (!val || !val->get().IsObject()) return std::nullopt;

        return val;
    }
    inline JSONWrapper* JSONWrapperOrNull(v3::CustomDataOpt const& val) {
        auto wrapper = JSONWrapper::New_ctor();

        if (!val || !val->get().IsObject()) return wrapper;

        wrapper->value = val;

        return wrapper;
    }

    constexpr auto& BeatmapSaveDataItem_GetBeat(BeatmapSaveDataVersion3::BeatmapSaveData::BeatmapSaveDataItem const* item) {
        return item->b;
    }

    constexpr auto& BeatmapSaveDataItem_GetBeat(BeatmapSaveDataVersion3::BeatmapSaveData::LightColorBaseData const* item) {
        return item->b;
    }

    //

    constexpr auto& LightColorBaseData_GetTransitionType(BeatmapSaveDataVersion3::BeatmapSaveData::LightColorBaseData const* item) {
        return item->i;
    }

    constexpr auto& LightColorBaseData_GetColorType(BeatmapSaveDataVersion3::BeatmapSaveData::LightColorBaseData const* item) {
        return item->c;
    }

    constexpr auto& LightColorBaseData_GetBrightness(BeatmapSaveDataVersion3::BeatmapSaveData::LightColorBaseData const* item) {
        return item->s;
    }

    constexpr auto& LightColorBaseData_GetStrobeFrequency(BeatmapSaveDataVersion3::BeatmapSaveData::LightColorBaseData const* item) {
        return item->f;
    }

    //

    constexpr auto LightRotationBaseData_GetStrobeFrequency(BeatmapSaveDataVersion3::BeatmapSaveData::LightRotationBaseData const* item) {
        return item->p == 1;
    }

    constexpr auto& LightRotationBaseData_GetRotation(BeatmapSaveDataVersion3::BeatmapSaveData::LightRotationBaseData const* item) {
        return item->r;
    }

    constexpr auto& LightRotationBaseData_GetLoopsCount(BeatmapSaveDataVersion3::BeatmapSaveData::LightRotationBaseData const* item) {
        return item->l;
    }

    //

    constexpr auto& EventBox_GetBeatDistributionParam(BeatmapSaveDataVersion3::BeatmapSaveData::EventBox const* item) {
        return item->w;
    }

    constexpr auto& EventBox_GetBeatDistributionParamType(BeatmapSaveDataVersion3::BeatmapSaveData::EventBox const* item) {
        return item->d;
    }

    //

    constexpr auto& IndexFilter_GetParam0(BeatmapSaveDataVersion3::BeatmapSaveData::IndexFilter const* filter) {
        return filter->p;
    }

    constexpr auto& IndexFilter_GetParam1(BeatmapSaveDataVersion3::BeatmapSaveData::IndexFilter const* filter) {
        return filter->t;
    }

    constexpr auto IndexFilter_GetReversed(BeatmapSaveDataVersion3::BeatmapSaveData::IndexFilter const* filter) {
        return filter->r != 0;
    }

    constexpr auto& IndexFilter_GetFilterType(BeatmapSaveDataVersion3::BeatmapSaveData::IndexFilter const* filter) {
        return filter->f;
    }
    //

}