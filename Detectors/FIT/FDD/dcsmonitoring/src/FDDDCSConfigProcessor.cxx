#include "FDDDCSMonitoring/FDDDCSConfigProcessor.h"

namespace o2::fdd {
void FDDDCSConfigProcessor::init(o2::framework::InitContext& ic)
{
    initDeadChannelMapReader();
    setupDeadChannelMapReader(ic);
}

void FDDDCSConfigProcessor::run(o2::framework::ProcessingContext& pc)
{
    long dataTime = getValidityTime(pc);

    gsl::span<const char> dataBuffer = pc.inputs().get<gsl::span<char>>("inputConfig");
    std::string configFileName = pc.inputs().get<std::string>("inputConfigFileName");
    LOG(info) << "Got input file " << configFileName << " of size " << dataBuffer.size();

    if (!configFileName.compare(mDeadChannelMapReader->getFileNameDChM())) {
        handleDeadChannelMapUpdate(pc,dataTime, dataBuffer);
    } else {
        LOG(error) << "Unknown input file: " << configFileName;
    }
}

void FDDDCSConfigProcessor::endOfStream(o2::framework::EndOfStreamContext& ec)
{
}
}