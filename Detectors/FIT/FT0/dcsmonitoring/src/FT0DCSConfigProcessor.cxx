#include "FT0DCSMonitoring/FT0DCSConfigProcessor.h"

namespace o2::ft0 {
void FT0DCSConfigProcessor::init(o2::framework::InitContext& ic)
{
    initDeadChannelMapReader();
    setupDeadChannelMapReader(ic);
}

void FT0DCSConfigProcessor::run(o2::framework::ProcessingContext& pc)
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

void FT0DCSConfigProcessor::endOfStream(o2::framework::EndOfStreamContext& ec)
{
}
}