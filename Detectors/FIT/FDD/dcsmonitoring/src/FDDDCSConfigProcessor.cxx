#include "FDDDCSMonitoring/FDDDCSConfigProcessor.h"
#include "DataFormatsFDD/HvConfiguration.h"

namespace o2::fdd
{
void FDDDCSConfigProcessor::init(o2::framework::InitContext& ic)
{
  initDeadChannelMapReader();
  setupDeadChannelMapReader(ic);
  setupFeeConfigurationReader(ic, mFeeConfigurationReader);
  setupHvConfigurationReader(ic, mHvConfigurationReader);
}

void FDDDCSConfigProcessor::run(o2::framework::ProcessingContext& pc)
{
  try {
    long dataTime = getValidityTime(pc);

    gsl::span<const char> dataBuffer = pc.inputs().get<gsl::span<char>>("inputConfig");
    std::string configFileName = pc.inputs().get<std::string>("inputConfigFileName");
    LOG(info) << "Got input file " << configFileName << " of size " << dataBuffer.size();

    if (!configFileName.compare(mDeadChannelMapReader->getFileNameDChM())) {
      handleDeadChannelMapUpdate(pc, dataTime, dataBuffer);
    } else if (mFeeConfigurationReader.matchFilename(configFileName)) {
      FddFeeConfiguration feeConfiguration = mFeeConfigurationReader.parseFeeConfiguration(dataBuffer);
      o2::ccdb::CcdbObjectInfo objectInfo = mFeeConfigCcdbInfo.createObjectInfo(feeConfiguration, dataTime, {});
      sendObject(pc.outputs(), feeConfiguration, objectInfo, getFeeConfigDescription());
    } else if (mHvConfigurationReader.matchFilename(configFileName)) {
      FddHvConfiguration hvConfig = mHvConfigurationReader.parseHvConfiguration<FddHvConfiguration>(dataBuffer);
      o2::ccdb::CcdbObjectInfo objectInfo = mHvConfigCcdbInfo.createObjectInfo(hvConfig, dataTime, {});
      sendObject(pc.outputs(), hvConfig, objectInfo, getHvConfigDescription());
    } else {
      LOG(error) << "Unknown input file: " << configFileName;
    }
  } catch (std::exception& e) {
    LOG(error) << "Exception: " << e.what();
  }
}

void FDDDCSConfigProcessor::endOfStream(o2::framework::EndOfStreamContext& ec)
{
}
} // namespace o2::fdd