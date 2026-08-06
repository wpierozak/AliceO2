#include "FT0DCSMonitoring/FT0DCSConfigReader.h"
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>

namespace
{
template <typename T, int Size>
bool parseJsonArray(const rapidjson::Value& node, const char* childName, T (&array)[Size])
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
      if (!node.IsBool())
        return false;
      array[idx] = node.GetBool();
    } else if constexpr (std::is_floating_point_v<T>) {
      if (!node.IsNumber())
        return false;
      array[idx] = node.GetFloat();
    } else if constexpr (std::is_integral_v<T> && std::is_unsigned_v<T>) {
      if (!node.IsUint())
        return false;
      array[idx] = static_cast<T>(node.GetUint());
    } else if constexpr (std::is_integral_v<T>) {
      if (!node.IsInt())
        return false;
      array[idx] = static_cast<T>(node.GetInt());
    } else {
      static_assert(std::is_same_v<T, void>, "Unsupported type");
    }
  }
  return true;
}
} // namespace

namespace o2::ft0
{
Ft0FeeConfiguration FT0DCSConfigReader::parseFeeConfiguration(gsl::span<const char> buffer)
{
  Ft0FeeConfiguration configuration;
  rapidjson::MemoryStream ms(buffer.data(), buffer.size());
  rapidjson::Document document;
  document.ParseStream(ms);

  if (document.HasMember("channels") == false) {
  }
  const auto& channels = document["channels"];
  const auto& timeAligments = channels["timeAligments"];
  parseJsonArray(channels, "timeAligments", configuration.channels.timeAligments);
  parseJsonArray(channels, "cfdThresholds", configuration.channels.cfdThresholds);
  parseJsonArray(channels, "cfdZeros", configuration.channels.cfdZeros);
  parseJsonArray(channels, "adcZeros", configuration.channels.adcZeros);
  parseJsonArray(channels, "adcDelays", configuration.channels.adcDelays);
  parseJsonArray(channels, "channelMaskData", configuration.channels.channelMaskData);
  parseJsonArray(channels, "channelMaskTriggers", configuration.channels.channelMaskTriggers);
}
} // namespace o2::ft0