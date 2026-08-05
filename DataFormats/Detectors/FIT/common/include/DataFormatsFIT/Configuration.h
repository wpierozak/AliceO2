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

// \file FeeConfiguration.h
/// \brief Utilities to describe FEE configuration
/// \author wiktor.pierozak@cern.ch

#ifndef O2_FIT_FEE_CONFIGURATION
#define O2_FIT_FEE_CONFIGURATION

#include <gsl/span>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>

namespace o2::fit {
namespace config_helpers {
struct DefaultValueType {
    template<typename T>
        requires std::is_arithmetic_v<T>
    constexpr operator T() const noexcept
    {
        if constexpr (std::is_same_v<T, bool>) {
            return false;
        } else {
            return std::numeric_limits<T>::max();
        }
    }
};

inline constexpr DefaultValueType DefaultValue{};

template<typename T, std::size_t N>
constexpr void fillDefaultArray(T (&array)[N]) {
    for (auto& element : array) {
        element = DefaultValue;
    }
}
}

struct TcmConfig {
    float phaseDelayA{config_helpers::DefaultValue};
    float phaseDelayC{config_helpers::DefaultValue};
};

template<int NChannels>
struct ChannelsConfig {
    float timeAligments[NChannels]{};
    uint16_t cfdThresholds[NChannels]{};
    int16_t cfdZeros[NChannels]{};
    int16_t adcZeros[NChannels]{};
    uint16_t adcDelays[NChannels]{};
    bool channelMaskData[NChannels]{};
    bool channelMaskTriggers[NChannels]{};

    constexpr ChannelsConfig() {
        config_helpers::fillDefaultArray(timeAligments);
        config_helpers::fillDefaultArray(cfdThresholds);
        config_helpers::fillDefaultArray(cfdZeros);
        config_helpers::fillDefaultArray(adcZeros);
        config_helpers::fillDefaultArray(adcDelays);
        config_helpers::fillDefaultArray(channelMaskData);
        config_helpers::fillDefaultArray(channelMaskTriggers);
    }

    template<typename T>
    [[nodiscard]] static constexpr bool isDefault(const T& value) {
        return value == static_cast<T>(config_helpers::DefaultValue);
    }

    gsl::span<const float, NChannels> getTimeAligments() const { return timeAligments; }
    gsl::span<const uint16_t, NChannels> getCfdThresholds() const { return cfdThresholds; }
    gsl::span<const int16_t, NChannels> getCfdZeros() const { return cfdZeros; }
    gsl::span<const int16_t, NChannels> getAdcZeros() const { return adcZeros; }
    gsl::span<const uint16_t, NChannels> getAdcDelays() const { return adcDelays; }
    gsl::span<const bool, NChannels> getChannelMaskTriggers() const { return channelMaskTriggers; }
    gsl::span<const bool, NChannels> getChannelMaskData() const { return channelMaskData; }
};

struct PmConfig {
    uint8_t orGate{config_helpers::DefaultValue};
};
}
#endif