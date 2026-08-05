#include "FT0DCSMonitoring/FT0DCSConfigReader.h"
#include <rapidjson/rapidjson.h
namespace o2::ft0 {
    Ft0FeeConfiguration FT0DCSConfigReader::parseFeeConfiguration(gsl::span<const char> buffer) {
        rapidjson::MemoryStream ms(buffer.data(), buffer.size());
        rapidjson::Document document;
        document.ParseStream(ms);
    }
}