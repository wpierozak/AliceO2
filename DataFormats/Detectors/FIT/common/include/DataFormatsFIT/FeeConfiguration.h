// Copyright 2019-2020 CERN and copyright holders of ALICE O2.
// See https://alice-o2.web.cern.ch/copyright for details of the copyright holders.
// All rights not expressly granted are reserved.
//
// This software is distributed under the terms of the GNU General Public
// License v3 (GPL Version 3), copied verbatim in the file "COPYING".
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

#ifndef O2_FIT_FEE_CONFIGURATION
#define O2_FIT_FEE_CONFIGURATION

#include <gsl/span>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>

namespace o2::fit::detector_config {
namespace helpers {
template<typename T>
constexpr auto DefaultValue = [] {
    using ValueType = std::remove_all_extents_t<std::remove_cvref_t<T>>;
    if constexpr (std::is_same_v<ValueType, bool>) {
        return false;
    } else {
        return std::numeric_limits<ValueType>::max();
    }
}();

template<typename T, std::size_t N>
constexpr void fillDefaultArray(T (&array)[N]) {
    for (auto& element : array) {
        element = DefaultValue<T>;
    }
}
}

struct Tcm {
    float phaseDelayA{};
    float phaseDelayC{};
};

template<int NChannels>
struct Channels {
    float timeAligments[NChannels]{};
    uint16_t cfdThresholds[NChannels]{};
    int16_t cfdZeros[NChannels]{};
    int16_t adcZeros[NChannels]{};
    uint16_t adcDelays[NChannels]{};
    float scalingFactorAdc0s[NChannels]{};
    float scalingFactorAdc1s[NChannels]{};
    bool channelMaskData[NChannels]{};
    bool channelMaskTriggers[NChannels]{};

    constexpr Channels() {
        helpers::fillDefaultArray(timeAligments);
        helpers::fillDefaultArray(cfdThresholds);
        helpers::fillDefaultArray(cfdZeros);
        helpers::fillDefaultArray(adcZeros);
        helpers::fillDefaultArray(adcDelays);
        helpers::fillDefaultArray(scalingFactorAdc0s);
        helpers::fillDefaultArray(scalingFactorAdc1s);
        helpers::fillDefaultArray(channelMaskData);
        helpers::fillDefaultArray(channelMaskTriggers);
    }

    template<typename T>
    [[nodiscard]] static constexpr bool isDefault(const T& value) {
        return value == helpers::DefaultValue<T>;
    }

    gsl::span<const float, NChannels> getTimeAligments() const { return timeAligments; }
    gsl::span<const uint16_t, NChannels> getCfdThresholds() const { return cfdThresholds; }
    gsl::span<const int16_t, NChannels> getCfdZeros() const { return cfdZeros; }
    gsl::span<const int16_t, NChannels> getAdcZeros() const { return adcZeros; }
    gsl::span<const uint16_t, NChannels> getAdcDelays() const { return adcDelays; }
    gsl::span<const float, NChannels> getScalingFactorAdc0s() const { return scalingFactorAdc0s; }
    gsl::span<const float, NChannels> getScalingFactorAdc1s() const { return scalingFactorAdc1s; }
    gsl::span<const bool, NChannels> getChannelMaskTriggers() const { return channelMaskTriggers; }
    gsl::span<const bool, NChannels> getChannelMaskData() const { return channelMaskData; }
};

struct Pm {
    uint8_t orGate{};
};
}
#endif