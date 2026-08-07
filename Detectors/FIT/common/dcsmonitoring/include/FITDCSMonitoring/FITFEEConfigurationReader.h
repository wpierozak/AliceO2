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

/// \file FITDCSConfigurationDataParser.h
/// \brief DCS configuration reader for FIT
///
/// \author Andreas Molander <andreas.molander@cern.ch>, University of Jyvaskyla, Finland

#ifndef O2_FIT_DCS_CONFIGURATION_DATA_READER_H
#define O2_FIT_DCS_CONFIGURATION_DATA_READER_H

#include <rapidjson/document.h>
#include <rapidjson/schema.h>
#include "DataFormatsFIT/Configuration.h"
#include "DetectorsCalibration/Utils.h"

namespace o2::fit
{
class FITFEEConfigurationReader
{
 public:
  FITFEEConfigurationReader();

  template <typename FeeConfigType>
  o2::ccdb::CcdbObjectInfo createObjectInfo(const FeeConfigType& configObject, long startValidityTimestamp, const std::map<std::string, std::string> metadata)
  {
    o2::ccdb::CcdbObjectInfo objectInfo;
    o2::calibration::Utils::prepareCCDBobjectInfo(configObject, objectInfo, mCcdbPath, metadata, startValidityTimestamp, o2::ccdb::CcdbObjectInfo::INFINITE_TIMESTAMP);
    return objectInfo;
  }

  void setCcdbPath(const std::string& path)
  {
    mCcdbPath = path;
  }

 protected:
  template <typename T, int Size>
  void parseJsonArray(const rapidjson::Value& node, const char* childName, T (&array)[Size])
  {
    if (node.HasMember(childName) == false) {
      throw std::runtime_error(std::string("Failed to find node of name ") + childName);
    }
    const auto& childNode = node[childName];
    if (childNode.IsArray() == false) {
      throw std::runtime_error(std::format("Node {} is not an array!", childName));
    }
    auto jsonArray = childNode.GetArray();
    if (jsonArray.Size() != Size) {
      throw std::runtime_error(std::format("Expected array of size {}, parsed array of size {}", Size, jsonArray.Size()));
    }
    for (int idx = 0; idx < Size; idx++) {
      const auto& node = jsonArray[idx];
      if constexpr (std::is_same_v<T, bool>) {
        if (!node.IsBool()) {
          throw std::runtime_error(std::format("{} is not a bool array", childName));
        }
        array[idx] = node.GetBool();
      } else if constexpr (std::is_floating_point_v<T>) {
        if (!node.IsNumber()) {
          throw std::runtime_error(std::format("{} is not an floating point array", childName));
        }
        array[idx] = node.GetFloat();
      } else if constexpr (std::is_integral_v<T> && std::is_unsigned_v<T>) {
        if (!node.IsUint()) {
          throw std::runtime_error(std::format("{} is not an unsigned integer array", childName));
        }
        array[idx] = static_cast<T>(node.GetUint());
      } else if constexpr (std::is_integral_v<T>) {
        if (!node.IsInt()) {
          throw std::runtime_error(std::format("{} is not an integer array", childName));
        }
        array[idx] = static_cast<T>(node.GetInt());
      } else {
        static_assert(std::is_same_v<T, void>, "Unsupported type");
      }
    }
  }

  template <int Size>
  void parseChannelData(const rapidjson::Value& root, const char* channelsNodeName, ChannelsConfig<Size>& channelsConfiguration)
  {
    const auto& channelsNode = root["channels"];
    parseJsonArray(channelsNode, "time_aligments", channelsConfiguration.timeAligments);
    parseJsonArray(channelsNode, "cfd_thresholds", channelsConfiguration.cfdThresholds);
    parseJsonArray(channelsNode, "cfd_zeros", channelsConfiguration.cfdZeros);
    parseJsonArray(channelsNode, "adc_zeros", channelsConfiguration.adcZeros);
    parseJsonArray(channelsNode, "adc_delays", channelsConfiguration.adcDelays);
    parseJsonArray(channelsNode, "channel_mask_data", channelsConfiguration.channelMaskData);
    parseJsonArray(channelsNode, "channel_mask_triggers", channelsConfiguration.channelMaskTriggers);
  }

  void parseTcmConfig(const rapidjson::Value& root, const char* tcmNodeName, TcmConfig& tcmConfig);
  void parsePmConfig(const rapidjson::Value& pmNode, PmConfig& pmConfig);

  template <int Size>
  void parsePmsArray(const rapidjson::Value& root, const char* pmArrayNodeName, PmConfig (&pmConfig)[Size])
  {
    if (root.HasMember(pmArrayNodeName) == false) {
      throw std::runtime_error(std::format("Failed to find {} node", pmArrayNodeName));
    }
    const auto& pmArrayNode = root[pmArrayNodeName];
    if (pmArrayNode.IsArray() == false) {
      std::runtime_error(std::format("{} node is not an array!", pmArrayNodeName));
    }
    const auto& pmArray = pmArrayNode.GetArray();
    if (pmArray.Size() != Size) {
      throw std::runtime_error(std::format("Received data for {} PMs, but expected {}", pmArray.Size(), Size));
    }
    for (int idx = 0; idx < pmArray.Size(); idx++) {
      const auto& pm = pmArray[idx];
      if (pm.IsObject() == false) {
        throw std::runtime_error("Encountered non-object element in PM array");
      }
      parsePmConfig(pm, pmConfig[idx]);
    }
  }

  bool validateSchema(const rapidjson::Document& docs);

  const std::string& getSchemaString() const
  {
    return configurationSchema;
  }

 private:
  static std::string configurationSchema;

  std::string mCcdbPath;
  std::unique_ptr<rapidjson::SchemaDocument> mSchema;
};
} // namespace o2::fit

#endif