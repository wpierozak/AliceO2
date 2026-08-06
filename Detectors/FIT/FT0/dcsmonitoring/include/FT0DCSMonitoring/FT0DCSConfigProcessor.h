#ifndef O2_FT0_DCSCONFIGPROCESSOR_H
#define O2_FT0_DCSCONFIGPROCESSOR_H

#include "FITDCSMonitoring/FITDCSConfigProcessorSpec.h"
#include "DetectorsCalibration/Utils.h"
#include "Framework/WorkflowSpec.h"
#include "Headers/DataHeader.h"

#include <string>
#include <vector>

namespace o2::ft0
{
class FT0DCSConfigProcessor : public o2::fit::FITDCSConfigProcessor
{
 public:
  FT0DCSConfigProcessor(const std::string& detectorName, const o2::header::DataDescription& dataDescriptionDChM)
    : o2::fit::FITDCSConfigProcessor(detectorName, dataDescriptionDChM) {}

  void init(o2::framework::InitContext& ic) final;
  void run(o2::framework::ProcessingContext& pc) final;
  void endOfStream(o2::framework::EndOfStreamContext& ec) final;
};
}
#endif // O2_FT0_DCSCONFIGPROCESSOR_H