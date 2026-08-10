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

/// \file FITDCSConfigProcessorSpec.cxx
/// \brief FIT processor spec for DCS configurations
///
/// \author Andreas Molander <andreas.molander@cern.ch>, University of Jyvaskyla, Finland

#ifndef O2_FIT_DCSCONFIGPROCESSORSPEC_H
#define O2_FIT_DCSCONFIGPROCESSORSPEC_H

#include "CCDB/CcdbApi.h"
#include "DetectorsCalibration/Utils.h"
#include "FITDCSMonitoring/FITDeadChannelMapReader.h"
#include "FITDCSMonitoring/FITFEEConfigurationReader.h"
#include "Framework/ConfigParamRegistry.h"
#include "Framework/Task.h"
#include "Framework/WorkflowSpec.h"

#include <chrono>
#include <gsl/span>
#include <memory>
#include <string>
#include <vector>

namespace o2
{
namespace fit
{

class FITDCSConfigProcessor : public o2::framework::Task
{
 public:
  FITDCSConfigProcessor(const std::string& detectorName, const o2::header::DataDescription& dataDescriptionDChM)
    : mDetectorName(detectorName),
      mDataDescriptionDChM(dataDescriptionDChM) {} // TODO AM: how to pass dd

 protected:
  /// Initializes the DCS config reader.
  /// Can be overriden in case another reader (subclass of o2::fit::FITDeadChannelMapReader) is needed.
  virtual void initDeadChannelMapReader()
  {
    mDeadChannelMapReader = std::make_unique<FITDeadChannelMapReader>(FITDeadChannelMapReader());
  }

  long getValidityTime(o2::framework::ProcessingContext& pc)
  {
    long dataTime = (long)(pc.services().get<o2::framework::TimingInfo>().creation);
    if (dataTime == 0xffffffffffffffff) {                                                                                                     // means it is not set
      dataTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count(); // in ms
    }
    return dataTime;
  }

  void setupDeadChannelMapReader(o2::framework::InitContext& ic)
  {
    mDeadChannelMapReader->setFileNameDChM(ic.options().get<std::string>("filename-dchm"));
    mDeadChannelMapReader->setValidDaysDChM(ic.options().get<uint>("valid-days-dchm"));
    mDeadChannelMapReader->setCcdbPathDChM(mDetectorName + "/Calib/DeadChannelMap");
    bool verbose = ic.options().get<bool>("use-verbose-mode");
    mDeadChannelMapReader->setVerboseMode(verbose);
    bool validateUpload = !ic.options().get<bool>("no-validate");
    mDeadChannelMapReader->setValidateUploadMode(validateUpload);

    LOG(info) << "Verbose mode: " << verbose;
    LOG(info) << "Validate upload: " << validateUpload;
    LOG(info) << "Expected dead channel map file name: " << mDeadChannelMapReader->getFileNameDChM();
    LOG(info) << "Dead channel maps will be valid for " << mDeadChannelMapReader->getValidDaysDChM() << " days";
  }

  template <typename ConfigurationReaderType>
  void setupFeeConfigurationReader(o2::framework::InitContext& ic, FITFEEConfigurationReader<ConfigurationReaderType>& feeConfig)
  {
    feeConfig.setFilename(ic.options().get<std::string>("filename-fee-config"));
    feeConfig.setCcdbPath(mDetectorName + "/Config/FeeConfiguration");
    feeConfig.setValidityPeriodInDays(ic.options().get<uint32_t>("valid-days-fee-config"));
  }

  void handleDeadChannelMapUpdate(o2::framework::ProcessingContext& pc, long dataTime, gsl::span<const char> dataBuffer)
  {
    processDeadChannelMap(dataTime, dataBuffer);
    sendObject(pc.outputs(), mDeadChannelMapReader->getDChM(), mDeadChannelMapReader->getObjectInfoDChM(), mDataDescriptionDChM);
    mDeadChannelMapReader->resetStartValidityDChM();
    mDeadChannelMapReader->resetDChM();
  }

  /// Processing the dead channel map
  void processDeadChannelMap(const long& dataTime, gsl::span<const char> dataBuffer)
  {
    if (!mDeadChannelMapReader->isStartValidityDChMSet()) {
      mDeadChannelMapReader->setStartValidityDChM(dataTime);
    }
    mDeadChannelMapReader->processDChM(dataBuffer);
    mDeadChannelMapReader->updateDChMCcdbObjectInfo();
  }

  template <typename ConfigObjectType>
  void sendObject(o2::framework::DataAllocator& output, const ConfigObjectType& object, o2::ccdb::CcdbObjectInfo& info, const o2::header::DataDescription& descriptor)
  {
    auto image = o2::ccdb::CcdbApi::createObjectImage(&object, &info);
    LOG(info) << "Sending object " << info.getPath() << "/" << info.getFileName() << " of size " << image->size()
              << " bytes, valid for " << info.getStartValidityTimestamp() << " : " << info.getEndValidityTimestamp();
    output.snapshot(o2::framework::Output{o2::calibration::Utils::gDataOriginCDBPayload, descriptor, 0}, *image.get());
    output.snapshot(o2::framework::Output{o2::calibration::Utils::gDataOriginCDBWrapper, descriptor, 0}, info);
  }

  std::unique_ptr<FITDeadChannelMapReader> mDeadChannelMapReader;

 private:
  std::string mDetectorName;                        ///< Detector name
  o2::header::DataDescription mDataDescriptionDChM; ///< DataDescription for the dead channel map
};

} // namespace fit
} // namespace o2

#endif // O2_FIT_DCSCONFIGPROCESSORSPEC_H
