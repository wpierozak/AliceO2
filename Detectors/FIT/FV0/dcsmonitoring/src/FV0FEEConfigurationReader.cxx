#include "FV0DCSMonitoring/FV0FEEConfigurationReader.h"

namespace o2::fv0
{
Fv0FeeConfiguration FV0FEEConfigurationReader::parseFeeConfiguration(gsl::span<const char> configBuf)
{
  return FITFEEConfigurationReader<FV0FEEConfigurationReader>::parseFeeConfiguration<Fv0FeeConfiguration>(configBuf);
}

void FV0FEEConfigurationReader::parseTriggers(const rapidjson::Value& root, const char* triggersNodeName, TriggersConfig& config)
{
  const auto& triggersNode = root["triggers"];
  const auto& nChannelsLevelNode = triggersNode["n_channels_level"];
  const auto& innerRingsLevelNode = triggersNode["inner_rings_level"];
  const auto& chargeLevelNode = triggersNode["charge_level"];
  const auto& outerRingsLevelNode = triggersNode["outer_rings_level"];

  config.innerRings = innerRingsLevelNode.GetInt();
  config.nChannels = nChannelsLevelNode.GetInt();
  config.charge = chargeLevelNode.GetInt();
  config.outerRings = chargeLevelNode.GetInt();
}
} // namespace o2::fv0