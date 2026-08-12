#include "FDDDCSMonitoring/FDDFEEConfigurationReader.h"
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>

namespace o2::fdd
{
FddFeeConfiguration FDDFEEConfigurationReader::parseFeeConfiguration(gsl::span<const char> buffer)
{
  return FITFEEConfigurationReader<FDDFEEConfigurationReader>::parseFeeConfiguration<FddFeeConfiguration>(buffer);
}

void FDDFEEConfigurationReader::parseTriggers(const rapidjson::Value& root, const char* triggersNodeName, TriggersConfig& config)
{
  const auto& triggersNode = root["triggers"];
  const auto& vertexTimeLowThresholdNode = triggersNode["vertex_time_low_threshold"];
  const auto& vertexTimeHighThresholdNode = triggersNode["vertex_time_high_threshold"];
  const auto& semicentralANode = triggersNode["semi_central_a"];
  const auto& semicentralCNode = triggersNode["semi_central_c"];
  const auto& centralANode = triggersNode["central_a"];
  const auto& centralCNode = triggersNode["central_c"];

  config.vertexTimeLowThreshold = vertexTimeLowThresholdNode.GetInt();
  config.vertexTimeHighThreshold = vertexTimeHighThresholdNode.GetInt();
  config.semicentralA = semicentralANode.GetUint();
  config.semicentralC = semicentralCNode.GetUint();
  config.centralA = centralANode.GetUint();
  config.centralC = centralCNode.GetUint();
}
} // namespace o2::fdd