#include "FV0DCSMonitoring/FV0DCSConfigProcessor.h"
#include "DataFormatsFV0/HvConfiguration.h"
#include "DataFormatsFV0/FeeConfiguration.h"

namespace o2::fv0
{
void FV0DCSConfigProcessor::init(o2::framework::InitContext& ic)
{
  initDeadChannelMapReader();
  setupDeadChannelMapReader(ic);
  setupFeeConfigurationReader(ic, mFeeConfigurationReader);
  setupHvConfigurationReader(ic, mHvConfigurationReader);
}

void FV0DCSConfigProcessor::run(o2::framework::ProcessingContext& pc)
{
  long dataTime = getValidityTime(pc);

  gsl::span<const char> dataBuffer = pc.inputs().get<gsl::span<char>>("inputConfig");
  std::string configFileName = pc.inputs().get<std::string>("inputConfigFileName");
  LOG(info) << "Got input file " << configFileName << " of size " << dataBuffer.size();

  if (!configFileName.compare(mDeadChannelMapReader->getFileNameDChM())) {
    handleDeadChannelMapUpdate(pc, dataTime, dataBuffer);
  } else if (mFeeConfigurationReader.matchFilename(configFileName)) {
    Fv0FeeConfiguration feeConfiguration = mFeeConfigurationReader.parseFeeConfiguration(dataBuffer);
    o2::ccdb::CcdbObjectInfo objectInfo = mFeeConfigCcdbInfo.createObjectInfo(feeConfiguration, dataTime, {});
    sendObject(pc.outputs(), feeConfiguration, objectInfo, getFeeConfigDescription());
  } else if (mHvConfigurationReader.matchFilename(configFileName)) {
    Fv0HvConfiguration hvConfig = mHvConfigurationReader.parseHvConfiguration<Fv0HvConfiguration>(dataBuffer);
    o2::ccdb::CcdbObjectInfo objectInfo = mHvConfigCcdbInfo.createObjectInfo(hvConfig, dataTime, {});
    sendObject(pc.outputs(), hvConfig, objectInfo, getHvConfigDescription());
  } else {
    LOG(error) << "Unknown input file: " << configFileName;
  }
}

void FV0DCSConfigProcessor::endOfStream(o2::framework::EndOfStreamContext& ec)
{
}
} // namespace o2::fv0