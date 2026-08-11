#include "FT0DCSMonitoring/FT0DCSConfigProcessor.h"
#include "DataFormatsFT0/HvConfiguration.h"

namespace o2::ft0
{
void FT0DCSConfigProcessor::init(o2::framework::InitContext& ic)
{
  initDeadChannelMapReader();
  setupDeadChannelMapReader(ic);
  setupFeeConfigurationReader(ic, mFeeConfigurationReader);
  setupHvConfigurationReader(ic, mHvConfigurationReader);
}

void FT0DCSConfigProcessor::run(o2::framework::ProcessingContext& pc)
{
  try {
    long dataTime = getValidityTime(pc);

    gsl::span<const char> dataBuffer = pc.inputs().get<gsl::span<char>>("inputConfig");
    std::string configFileName = pc.inputs().get<std::string>("inputConfigFileName");
    LOG(info) << "Got input file " << configFileName << " of size " << dataBuffer.size();
    LOG(info) << "FEE filename: " << mFeeConfigurationReader.getFilename();
    LOG(info) << "HV filename: " << mHvConfigurationReader.getFilename();
    if (!configFileName.compare(mDeadChannelMapReader->getFileNameDChM())) {
      handleDeadChannelMapUpdate(pc, dataTime, dataBuffer);
    } else if (mFeeConfigurationReader.matchFilename(configFileName)) {
      Ft0FeeConfiguration feeConfiguration = mFeeConfigurationReader.parseFeeConfiguration(dataBuffer);
      o2::ccdb::CcdbObjectInfo objectInfo = mFeeConfigCcdbInfo.createObjectInfo(feeConfiguration, dataTime, {});
      sendObject(pc.outputs(), feeConfiguration, objectInfo, getFeeConfigDescription());
    } else if (mHvConfigurationReader.matchFilename(configFileName)) {
      Ft0HvConfiguration hvConfig = mHvConfigurationReader.parseHvConfiguration<Ft0HvConfiguration>(dataBuffer);
      o2::ccdb::CcdbObjectInfo objectInfo = mHvConfigCcdbInfo.createObjectInfo(hvConfig, dataTime, {});
      sendObject(pc.outputs(), hvConfig, objectInfo, getHvConfigDescription());
    } else {
      LOG(error) << "Unknown input file: " << configFileName;
    }
  } catch (std::exception& e) {
    LOG(error) << "Exception: " << e.what();
  }
}

void FT0DCSConfigProcessor::endOfStream(o2::framework::EndOfStreamContext& ec)
{
}
} // namespace o2::ft0