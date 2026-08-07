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

#ifndef O2_FIT_DCS_BASE_CONFIG_READER_H
#define O2_FIT_DCS_BASE_CONFIG_READER_H

#include "DetectorsCalibration/Utils.h"

namespace o2::fit
{
class FITDCSBaseConfigReader
{
 public:
  template <typename FeeConfigType>
  o2::ccdb::CcdbObjectInfo createObjectInfo(const FeeConfigType& configObject, long startValidityTimestamp, long endValidityTimestamp, const std::map<std::string, std::string>& metadata)
  {
    o2::ccdb::CcdbObjectInfo objectInfo;
    o2::calibration::Utils::prepareCCDBobjectInfo(configObject, objectInfo, mCcdbPath, metadata, startValidityTimestamp, endValidityTimestamp);
    objectInfo.setValidateUpload(mValidateUpload);
    return objectInfo;
  }

  void setCcdbPath(const std::string& path)
  {
    mCcdbPath = path;
  }
  const std::string& getCcdbPath() const
  {
    return mCcdbPath;
  }

  void setFilename(const std::string& filename)
  {
    mFilename = filename;
  }
  const std::string& getFilename() const
  {
    return mFilename;
  }

  bool matchFilename(const std::string& filename)
  {
    return mFilename == filename;
  }

  void setValidateUpload(bool validate)
  {
    mValidateUpload = validate;
  }
  bool getValidateUpload() const
  {
    return mValidateUpload;
  }

  void setDataDescriptor(o2::header::DataDescription descriptor)
  {
    mDataDescriptor = descriptor;
  }
  const o2::header::DataDescription& getDataDescriptor() const
  {
    return mDataDescriptor;
  }

 private:
  std::string mCcdbPath;
  o2::header::DataDescription mDataDescriptor;
  std::string mFilename;
  bool mValidateUpload;
};
} // namespace o2::fit
#endif