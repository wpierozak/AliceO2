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

#define BOOST_TEST_MODULE Test FITDCSMonitoring FITFEEConfigurationReader
#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <rapidjson/document.h>
#include "FITDCSMonitoring/FITFEEConfigurationReader.h"
#include "DataFormatsFIT/Configuration.h"

using namespace o2::fit;

BOOST_AUTO_TEST_SUITE(o2_fit_dcs_monitoring)
// Dummy structure for trigger data for complete definition of FEE configuration structure
struct SimpleTriggers {
    int32_t triggerA;
};

// Test configuration structure
struct SimpleConfiguration {
  static constexpr int NChannels = 5;
  SimpleTriggers triggers;
  o2::fit::ChannelsConfig<NChannels> channels;
  o2::fit::TcmConfig tcm;
  o2::fit::PmConfig pmA[2];
  o2::fit::PmConfig pmC[2];
};

class SimpleFITFEEConfigurationReader: public FITFEEConfigurationReader<SimpleFITFEEConfigurationReader>{
public:
    // Dummy method to parse dummy triggers payload
    SimpleTriggers parseTriggers(const rapidjson::Value& node, const char* childName, SimpleTriggers& triggers) {
        triggers.triggerA = 32;
    }
    using FITFEEConfigurationReader<SimpleFITFEEConfigurationReader>::parseFeeConfiguration;
    using FITFEEConfigurationReader<SimpleFITFEEConfigurationReader>::parseJsonArray;
    using FITFEEConfigurationReader<SimpleFITFEEConfigurationReader>::parseChannelData;
    using FITFEEConfigurationReader<SimpleFITFEEConfigurationReader>::parseTcmConfig;
    using FITFEEConfigurationReader<SimpleFITFEEConfigurationReader>::parsePmConfig;
    using FITFEEConfigurationReader<SimpleFITFEEConfigurationReader>::parsePmsArray;
    using FITFEEConfigurationReader<SimpleFITFEEConfigurationReader>::validateSchema;
};

BOOST_AUTO_TEST_CASE(shouldParseSinglePmConfig)
{
    const uint8_t orGateValue = 20;
    rapidjson::Document doc;
    doc.SetObject();
    auto& allocator = doc.GetAllocator();

    doc.AddMember("or_gate", orGateValue, allocator);
    SimpleFITFEEConfigurationReader reader;

    PmConfig pmConfig;
    reader.parsePmConfig(doc, pmConfig);

    BOOST_CHECK(pmConfig.orGate == orGateValue);
}

BOOST_AUTO_TEST_CASE(shouldParseTcmConfig) {
    const float phaseDelayAValue = 1.2;
    const float phaseDelayCValue = -1.3;

    rapidjson::Document doc;
    doc.SetObject();
    auto& allocator = doc.GetAllocator();
    
    rapidjson::Value tcmConfigJson(rapidjson::kObjectType);
    tcmConfigJson.AddMember("phase_delay_a", phaseDelayAValue, allocator);
    tcmConfigJson.AddMember("phase_delay_c", phaseDelayCValue, allocator);

    doc.AddMember("tcm_config", tcmConfigJson, allocator);
    TcmConfig tcmConfig;

    SimpleFITFEEConfigurationReader reader;
    reader.parseTcmConfig(doc, "tcm_config", tcmConfig);

    TcmConfig expectedConfig = {.phaseDelayA = phaseDelayAValue, .phaseDelayC = phaseDelayCValue};
    BOOST_CHECK(tcmConfig == expectedConfig);
}

BOOST_AUTO_TEST_CASE(shouldParseArrayOfPmConfigs) {
    uint8_t pmAOrGate[2] = {3, 4};
    uint8_t pmCOrGate[2] = {5, 6};

    rapidjson::Document doc;
    doc.SetObject();
    auto& allocator = doc.GetAllocator();

    rapidjson::Value pmAJson(rapidjson::kArrayType);
    rapidjson::Value pmCJson(rapidjson::kArrayType);

    for(int idx = 0; idx < sizeof(pmAOrGate); idx++) {
        rapidjson::Value pm(rapidjson::kObjectType);
        pm.AddMember("or_gate", pmAOrGate[idx], allocator);
        pmAJson.PushBack(pm, allocator);
    }

    for(int idx = 0; idx < sizeof(pmCOrGate); idx++) {
        rapidjson::Value pm(rapidjson::kObjectType);
        pm.AddMember("or_gate", pmCOrGate[idx], allocator);
        pmCJson.PushBack(pm, allocator);
    }

    doc.AddMember("pm_a", pmAJson, allocator);
    doc.AddMember("pm_c", pmCJson, allocator);

    PmConfig pmAParsed[2];
    PmConfig pmCParsed[2];

    SimpleFITFEEConfigurationReader reader;
    reader.parsePmsArray(doc, "pm_a", pmAParsed);
    reader.parsePmsArray(doc, "pm_c", pmCParsed);

    for(int idx= 0; idx < sizeof(pmAParsed); idx++) {
        BOOST_CHECK(pmAParsed[idx].orGate == pmAOrGate[idx]);
    }

    for(int idx= 0; idx < sizeof(pmAParsed); idx++) {
        BOOST_CHECK(pmCParsed[idx].orGate == pmCOrGate[idx]);
    }
}

BOOST_AUTO_TEST_CASE(shouldThrowOnInconsistentPmsArraySize) {
    uint8_t pmAOrGate[2] = {0};

    rapidjson::Document doc;
    doc.SetObject();
    auto& allocator = doc.GetAllocator();

    rapidjson::Value pmAJson(rapidjson::kArrayType);

    for(int idx = 0; idx < sizeof(pmAOrGate); idx++) {
        rapidjson::Value pm(rapidjson::kObjectType);
        pm.AddMember("or_gate", pmAOrGate[0], allocator);
        pmAJson.PushBack(pm, allocator);
    }

    doc.AddMember("pm_a", pmAJson, allocator);

    PmConfig pmAParsed[3];

    SimpleFITFEEConfigurationReader reader;
    BOOST_CHECK_THROW(reader.parsePmsArray(doc, "pm_a", pmAParsed), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(shouldParseChannelsConfig)
{
  constexpr size_t NTestChannels = 4;

  float timeAligments[NTestChannels] = {1.1f, 2.2f, 3.3f, 4.4f};
  uint16_t cfdThresholds[NTestChannels] = {10, 20, 30, 40};
  int16_t cfdZeros[NTestChannels] = {-1, -2, -3, -4};
  int16_t adcZeros[NTestChannels] = {100, 200, 300, 400};
  uint16_t adcDelays[NTestChannels] = {5, 6, 7, 8};
  bool channelMaskData[NTestChannels] = {true, false, true, false};
  bool channelMaskTriggers[NTestChannels] = {false, true, false, true};

  rapidjson::Document doc;
  doc.SetObject();
  auto& allocator = doc.GetAllocator();

  rapidjson::Value channels(rapidjson::kObjectType);

  rapidjson::Value timeAligmentsJson(rapidjson::kArrayType);
  rapidjson::Value cfdThresholdsJson(rapidjson::kArrayType);
  rapidjson::Value cfdZerosJson(rapidjson::kArrayType);
  rapidjson::Value adcZerosJson(rapidjson::kArrayType);
  rapidjson::Value adcDelaysJson(rapidjson::kArrayType);
  rapidjson::Value channelMaskDataJson(rapidjson::kArrayType);
  rapidjson::Value channelMaskTriggersJson(rapidjson::kArrayType);

  for (size_t idx = 0; idx < NTestChannels; ++idx) {
    timeAligmentsJson.PushBack(timeAligments[idx], allocator);
    cfdThresholdsJson.PushBack(cfdThresholds[idx], allocator);
    cfdZerosJson.PushBack(cfdZeros[idx], allocator);
    adcZerosJson.PushBack(adcZeros[idx], allocator);
    adcDelaysJson.PushBack(adcDelays[idx], allocator);
    channelMaskDataJson.PushBack(channelMaskData[idx], allocator);
    channelMaskTriggersJson.PushBack(channelMaskTriggers[idx], allocator);
  }

  channels.AddMember("time_aligments", timeAligmentsJson, allocator);
  channels.AddMember("cfd_thresholds", cfdThresholdsJson, allocator);
  channels.AddMember("cfd_zeros", cfdZerosJson, allocator);
  channels.AddMember("adc_zeros", adcZerosJson, allocator);
  channels.AddMember("adc_delays", adcDelaysJson, allocator);
  channels.AddMember("channel_mask_data", channelMaskDataJson, allocator);
  channels.AddMember("channel_mask_triggers", channelMaskTriggersJson, allocator);

  doc.AddMember("channels", channels, allocator);

  ChannelsConfig<NTestChannels> parsed{};

  SimpleFITFEEConfigurationReader reader;
  reader.parseChannelData(doc, "channels", parsed);

  for (size_t idx = 0; idx < NTestChannels; ++idx) {
    BOOST_CHECK_EQUAL(parsed.timeAligments[idx], timeAligments[idx]);
    BOOST_CHECK_EQUAL(parsed.cfdThresholds[idx], cfdThresholds[idx]);
    BOOST_CHECK_EQUAL(parsed.cfdZeros[idx], cfdZeros[idx]);
    BOOST_CHECK_EQUAL(parsed.adcZeros[idx], adcZeros[idx]);
    BOOST_CHECK_EQUAL(parsed.adcDelays[idx], adcDelays[idx]);
    BOOST_CHECK_EQUAL(parsed.channelMaskData[idx], channelMaskData[idx]);
    BOOST_CHECK_EQUAL(parsed.channelMaskTriggers[idx],
                      channelMaskTriggers[idx]);
  }
}

BOOST_AUTO_TEST_CASE(shouldAcceptValidFeeConfigurationPayload) {
    rapidjson::Document doc;
    doc.Parse(R"json(
    {
        "channels" : {
            "time_aligments": [1,2,3,4],
            "cfd_thresholds": [10,20,30,40],
            "cfd_zeros": [11,22,33,44],
            "adc_zeros": [21,22,23,24],
            "adc_delays": [31,32,33,34],
            "channel_mask_data": [true, false, true, false],
            "channel_mask_triggers": [true, true, false, false]
        },
        "pm_a": [
            {"or_gate": 1.21},
            {"or_gate": 1.22}
        ],
        "pm_c": [
            {"or_gate": 0.1},
            {"or_gate": 0.2}
        ],
        "tcm": {
            "phase_delay_a": 3.3,
            "phase_delay_c": 4.4
        },
        "triggers": {
            "trigger_a": 123
        }
    }
    )json");

    SimpleFITFEEConfigurationReader reader;
    std::string errorMessage;
    BOOST_CHECK_MESSAGE(reader.validateSchema(doc, errorMessage), errorMessage);
}
BOOST_AUTO_TEST_SUITE_END()