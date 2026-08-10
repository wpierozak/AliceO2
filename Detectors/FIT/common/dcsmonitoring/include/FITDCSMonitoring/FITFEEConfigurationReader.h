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
#include "FITDCSMonitoring/FITDCSBaseConfigReader.h"

namespace o2::fit
{
template <typename ConfigurationReaderType>
class FITFEEConfigurationReader : public FITDCSBaseConfigReader
{
 public:
  FITFEEConfigurationReader()
  {
    rapidjson::Document schemaDocument;
    schemaDocument.Parse(configurationSchema.c_str());
    if (schemaDocument.HasParseError()) {
      throw std::runtime_error("Cannot parse FIT FEE JSON schema");
    }
    mSchema = std::make_unique<rapidjson::SchemaDocument>(schemaDocument);
  }

  template <typename FeeConfigType>
  o2::ccdb::CcdbObjectInfo createObjectInfo(const FeeConfigType& configObject, long startValidityTimestamp, const std::map<std::string, std::string>& metadata)
  {
    return FITDCSBaseConfigReader::createObjectInfo(configObject, startValidityTimestamp, getValidityTimestamp(startValidityTimestamp), metadata);
  }

  long getValidityTimestamp(long startTimestamp)
  {
    return startTimestamp + mValidDays * o2::ccdb::CcdbObjectInfo::DAY;
  }

  void setValidityPeriodInDays(long days)
  {
    mValidDays = days;
  }

  long getValidityPeriodInDays()
  {
    return mValidDays;
  }

 protected:
  template <typename ConfigType>
  ConfigType parseFeeConfiguration(gsl::span<const char> buffer)
  {
    ConfigType configuration;
    rapidjson::MemoryStream ms(buffer.data(), buffer.size());
    rapidjson::Document document;
    document.ParseStream(ms);

    std::string validationErrorMessage;
    if (validateSchema(document, validationErrorMessage) == false) {
      std::string_view bufferView(buffer.data(), buffer.size());
      throw std::runtime_error("Received document does not match FEE configuration schema! Error message: " + validationErrorMessage);
    }

    parseChannelData(document, "channels", configuration.channels);
    parseTcmConfig(document, "tcm", configuration.tcm);
    parsePmsArray(document, "pm_a", configuration.pmA);
    parsePmsArray(document, "pm_c", configuration.pmC);
    static_cast<ConfigurationReaderType*>(this)->parseTriggers(document, "triggers", configuration.triggers);

    return configuration;
  }
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
      throw std::runtime_error(std::format("Array {}. Expected array of size {}, parsed array of size {}", childName, Size, jsonArray.Size()));
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
    const auto& channelsNode = root[channelsNodeName];
    parseJsonArray(channelsNode, "time_aligments", channelsConfiguration.timeAligments);
    parseJsonArray(channelsNode, "cfd_thresholds", channelsConfiguration.cfdThresholds);
    parseJsonArray(channelsNode, "cfd_zeros", channelsConfiguration.cfdZeros);
    parseJsonArray(channelsNode, "adc_zeros", channelsConfiguration.adcZeros);
    parseJsonArray(channelsNode, "adc_delays", channelsConfiguration.adcDelays);
    parseJsonArray(channelsNode, "channel_mask_data", channelsConfiguration.channelMaskData);
    parseJsonArray(channelsNode, "channel_mask_triggers", channelsConfiguration.channelMaskTriggers);
  }

  void parseTcmConfig(const rapidjson::Value& root, const char* tcmNodeName, TcmConfig& tcmConfig)
  {
    if (!root.HasMember(tcmNodeName)) {
      std::runtime_error(std::format("Cannot find {}", tcmNodeName));
    }
    const auto& tcmNode = root[tcmNodeName];
    if (!tcmNode.HasMember("phase_delay_a") || !tcmNode.HasMember("phase_delay_c")) {
      throw std::runtime_error("Invalid TCM configuration node!");
    }
    const auto& phaseDelayANode = tcmNode["phase_delay_a"];
    const auto& phaseDelayCNode = tcmNode["phase_delay_c"];
    tcmConfig.phaseDelayA = phaseDelayANode.GetDouble();
    tcmConfig.phaseDelayC = phaseDelayCNode.GetDouble();
  }

  void parsePmConfig(const rapidjson::Value& pmNode, PmConfig& pmConfig)
  {
    if (!pmNode.HasMember("or_gate")) {
      throw std::runtime_error("Invalid PM configuration node");
    }
    const auto& orGateNode = pmNode["or_gate"];
    pmConfig.orGate = orGateNode.GetUint();
  }

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

  bool validateSchema(const rapidjson::Document& docs, std::string& errorMessage)
  {
    rapidjson::SchemaValidator validator(*mSchema);
    if (docs.Accept(validator) == false) {
      rapidjson::StringBuffer buffer;
      std::ostringstream ss;
      validator.GetInvalidSchemaPointer().StringifyUriFragment(buffer);
      ss << "Invalid schema: " << buffer.GetString() << '\n';
      ss << "Invalid keyword: " << validator.GetInvalidSchemaKeyword() << '\n';

      buffer.Clear();

      validator.GetInvalidDocumentPointer().StringifyUriFragment(buffer);
      ss << "Invalid document: " << buffer.GetString() << '\n';

      errorMessage = ss.str();
      return false;
    }
    return true;
  }

  const std::string& getSchemaString() const
  {
    return configurationSchema;
  }

 private:
  static const std::string configurationSchema;
  std::unique_ptr<rapidjson::SchemaDocument> mSchema;
  long mValidDays{180u};
};

template <class ConfigurationReaderType>
const std::string FITFEEConfigurationReader<ConfigurationReaderType>::configurationSchema = R"SCH(
        {
        "type": "object",
        "properties": {
            "channels": {
                "type": "object",
                "properties": {
                    "time_aligments": {"type": "array", "items": {"type": "integer"}},
                    "cfd_thresholds": {"type": "array", "items": {"type": "integer"}},
                    "cfd_zeros": {"type": "array", "items": {"type": "integer"}},
                    "adc_zeros": {"type": "array", "items": {"type": "integer"}},
                    "adc_delays": {"type": "array", "items": {"type": "integer"}},
                    "channel_mask_data": {"type": "array", "items": {"type": "boolean"}},
                    "channel_mask_triggers": {"type": "array", "items": {"type": "boolean"}}
                },
                "required": ["time_aligments", "cfd_thresholds", "cfd_zeros",
                "adc_zeros", "adc_delays", "channel_mask_data", "channel_mask_triggers"]
            },
            "tcm" : {
                "type": "object",
                "properties": {
                    "phase_delay_a": {"type": "number"},
                    "phase_delay_c": {"type": "number"}
                }
            },
            "pm_a":{
                "type": "array",
                "items": {
                  "type": "object",
                  "properties": {
                    "or_gate": {"type": "number"}
                  }
                }
            },
            "pm_c": {
                "type": "array",
                "items": {
                  "type": "object",
                  "properties": {
                    "or_gate": {"type": "number"}
                  }
                }
            },
            "triggers": {
                "type": "object",
                "additionalProperties" : { "type": "number" }
            }
        },
        "required": ["channels", "tcm", "triggers"]
    }
    )SCH";
} // namespace o2::fit

#endif