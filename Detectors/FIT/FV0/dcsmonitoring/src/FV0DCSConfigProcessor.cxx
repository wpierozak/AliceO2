#include "FV0DCSMonitoring/FV0DCSConfigProcessor.h"

namespace o2::fv0 {
void FV0DCSConfigProcessor::init(o2::framework::InitContext& ic)
{
    initDeadChannelMapReader();
    setupDeadChannelMapReader(ic);
}

void FV0DCSConfigProcessor::run(o2::framework::ProcessingContext& pc)
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

void FV0DCSConfigProcessor::endOfStream(o2::framework::EndOfStreamContext& ec)
{
}
}