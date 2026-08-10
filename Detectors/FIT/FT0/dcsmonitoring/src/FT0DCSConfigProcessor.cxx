#include "FT0DCSMonitoring/FT0DCSConfigProcessor.h"

namespace o2::ft0
{
void FT0DCSConfigProcessor::init(o2::framework::InitContext& ic)
{
  initDeadChannelMapReader();
  setupDeadChannelMapReader(ic);
  setupFeeConfigurationReader(ic, mFeeConfigurationReader);
}

void FT0DCSConfigProcessor::run(o2::framework::ProcessingContext& pc)
{
  try {
    long dataTime = getValidityTime(pc);

    gsl::span<const char> dataBuffer = pc.inputs().get<gsl::span<char>>("inputConfig");
    std::string configFileName = pc.inputs().get<std::string>("inputConfigFileName");
    LOG(info) << "Got input file " << configFileName << " of size " << dataBuffer.size();

    if (!configFileName.compare(mDeadChannelMapReader->getFileNameDChM())) {
      handleDeadChannelMapUpdate(pc, dataTime, dataBuffer);
    }
    if (mFeeConfigurationReader.matchFilename(configFileName)) {
      Ft0FeeConfiguration feeConfiguration = mFeeConfigurationReader.parseFeeConfiguration(dataBuffer);
      o2::ccdb::CcdbObjectInfo objectInfo = mFeeConfigurationReader.createObjectInfo(feeConfiguration, dataTime, {});
      sendObject(pc.outputs(), feeConfiguration, objectInfo, mFeeConfigurationReader.getDataDescriptor());
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