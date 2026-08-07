#include "FITDCSMonitoring/FITFEEConfigurationReader.h"

namespace o2::fit
{
FITFEEConfigurationReader::FITFEEConfigurationReader()
{
  rapidjson::Document schemaDocument;
  schemaDocument.Parse(configurationSchema.c_str());
  if (schemaDocument.HasParseError()) {
    throw std::runtime_error("Cannot parse FIT FEE JSON schema");
  }
  mSchema = std::make_unique<rapidjson::SchemaDocument>(schemaDocument);
}

void FITFEEConfigurationReader::parseTcmConfig(const rapidjson::Value& root, const char* tcmNodeName, TcmConfig& tcmConfig)
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

void FITFEEConfigurationReader::parsePmConfig(const rapidjson::Value& pmNode, PmConfig& pmConfig)
{
  if (!pmNode.HasMember("or_gate")) {
    throw std::runtime_error("Invalid PM configuration node");
  }
  const auto& orGateNode = pmNode["or_gate"];
  pmConfig.orGate = orGateNode.GetUint();
}

bool FITFEEConfigurationReader::validateSchema(const rapidjson::Document& docs)
{
  rapidjson::SchemaValidator validator(*mSchema);
  if (docs.Accept(validator) == false) {
    return false;
  }
  return true;
}

std::string FITFEEConfigurationReader::configurationSchema = R"SCH(
        {
        "type": "object",
        "properties": {
            "channels": {
                "type": "object",
                "properties": {
                    "time_aligments": {"type": "array"},
                    "cfd_thresholds": {"type": "array"},
                    "cfd_zeros": {"type": "array"},
                    "adc_zeros": {"type": "array"},
                    "adc_delays": {"type": "array"},
                    "channel_mask_data": {"type": "array"},
                    "channel_mask_triggers": {"type": "array"}
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
                "items": {"type": "object"}
            },
            "pm_c": {
                "type": "array",
                "items": {"type": "object"}
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