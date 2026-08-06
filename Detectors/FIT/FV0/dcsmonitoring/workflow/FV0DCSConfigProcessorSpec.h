// Copyright 2019-2020 CERN and copyright holders of ALICE O2.
// See https://alice-o2.web.cern.ch/copyright for details of the copyright holders.
// All rights not expressly granted are reserved.
//
// This software is distributed under the terms of the GNU General Public
// License v3 (GPL Version 3), copied verbatim in the file "COPYING".
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file FV0DCSConfigProcessorSpec.cxx
/// \brief FV0 processor spec for DCS configurations
///
/// \author Andreas Molander <andreas.molander@cern.ch>, University of Jyvaskyla, Finland

#ifndef O2_FV0_DCSCONFIGPROCESSOR_H
#define O2_FV0_DCSCONFIGPROCESSOR_H

#include "FITDCSMonitoring/FITDCSConfigProcessorSpec.h"
#include "DetectorsCalibration/Utils.h"
#include "Framework/WorkflowSpec.h"
#include "Headers/DataHeader.h"

#include <string>
#include <vector>

namespace o2
{

namespace framework
{

class FV0DCSConfigProcessor : public o2::fit::FITDCSConfigProcessor
{
 public:
  FV0DCSConfigProcessor(const std::string& detectorName, const o2::header::DataDescription& dataDescriptionDChM)
    : o2::fit::FITDCSConfigProcessor(detectorName, dataDescriptionDChM) {}

  void init(o2::framework::InitContext& ic) final
  {
    initDeadChannelMapReader();
    setupDeadChannelMapReader(ic);
  }

  void run(o2::framework::ProcessingContext& pc) final
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

  void endOfStream(o2::framework::EndOfStreamContext& ec) final
  {
  }
};

DataProcessorSpec getFV0DCSConfigProcessorSpec()
{
  o2::header::DataDescription ddDChM = "FV0_DCHM";
  std::vector<OutputSpec> outputs;
  outputs.emplace_back(ConcreteDataTypeMatcher{o2::calibration::Utils::gDataOriginCDBPayload, ddDChM}, Lifetime::Sporadic);
  outputs.emplace_back(ConcreteDataTypeMatcher{o2::calibration::Utils::gDataOriginCDBWrapper, ddDChM}, Lifetime::Sporadic);

  return DataProcessorSpec{
    "fv0-dcs-config-processor",
    Inputs{{"inputConfig", o2::header::gDataOriginFV0, "DCS_CONFIG_FILE", Lifetime::Sporadic},
           {"inputConfigFileName", o2::header::gDataOriginFV0, "DCS_CONFIG_NAME", Lifetime::Sporadic}},
    outputs,
    AlgorithmSpec{adaptFromTask<FV0DCSConfigProcessor>("FV0", ddDChM)},
    Options{{"use-verbose-mode", VariantType::Bool, false, {"Use verbose mode"}},
            {"filename-dchm", VariantType::String, "FV0-deadchannels.txt", {"Dead channel map file name"}},
            {"valid-days-dchm", VariantType::UInt32, 180u, {"Dead channel map validity in days"}},
            {"no-validate", VariantType::Bool, false, {"Don't validate the CCDB uploads"}}}};
}

} // namespace framework
} // namespace o2

#endif // O2_FV0_DCSCONFIGPROCESSOR_H
