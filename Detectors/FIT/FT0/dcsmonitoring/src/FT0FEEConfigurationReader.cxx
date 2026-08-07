#include "FT0DCSMonitoring/FT0FEEConfigurationReader.h"
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>

namespace o2::ft0
{
Ft0FeeConfiguration FT0FEEConfigurationReader::parseFeeConfiguration(gsl::span<const char> buffer)
{
  Ft0FeeConfiguration configuration;
  rapidjson::MemoryStream ms(buffer.data(), buffer.size());
  rapidjson::Document document;
  document.ParseStream(ms);

  parseChannelData(document, "channels", configuration.channels);
  parseTcmConfig(document, "tcm", configuration.tcm);
  parsePmsArray(document, "pm_a", configuration.pmA);
  parsePmsArray(document, "pm_c", configuration.pmC);
  parseFT0TriggersConfiguration(document, "triggers", configuration.triggers);
}

void FT0FEEConfigurationReader::parseFT0TriggersConfiguration(const rapidjson::Value& root, const char* triggersNodeName, TriggersConfig& config) {
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
} // namespace o2::ft0