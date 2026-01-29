#ifndef O2_FT0TVXPERBCID
#define O2_FT0TVXPERBCID

#include <bitset>
#include <array>
#include <limits>
#include <TH1F.h>

#include "CommonDataFormat/FlatHisto2D.h"
#include "CommonConstants/LHCConstants.h"
#include "DataFormatsFT0/SpectraInfoObject.h"
#include "DataFormatsFT0/Digit.h"
#include "DataFormatsFT0/EventsPerBc.h"
#include "DetectorsCalibration/TimeSlotCalibration.h"
#include "DetectorsCalibration/TimeSlot.h"
#include "CommonDataFormat/TFIDInfo.h"
#include "TH1F.h"
#include "Rtypes.h"

namespace o2::ft0
{
<<<<<<< HEAD
struct EventsPerBc {
  EventsPerBc(int32_t minAmplitudeSideA, int32_t minAmplitudeSideC) : mMinAmplitudeSideA(minAmplitudeSideA), mMinAmplitudeSideC(minAmplitudeSideC) {}

  size_t getEntries() const { return entries; }
  void print() const;
  void fill(const o2::dataformats::TFIDInfo& ti, const gsl::span<const o2::ft0::Digit> data);
  void merge(const EventsPerBc* prev);
=======
struct EventsPerBcContainer {
  EventsPerBcContainer(int32_t minAmplitudeSideA, int32_t minAmplitudeSideC) : mMinAmplitudeSideA(minAmplitudeSideA), mMinAmplitudeSideC(minAmplitudeSideC) {}

  size_t getEntries() const { return entries; }
  void print() const;
  void fill(const o2::dataformats::TFIDInfo& ti, const gsl::span<const o2::ft0::Digit> data);
  void merge(const EventsPerBcContainer* prev);
>>>>>>> b9ec63cd4d (Created CCDB object class for EvetnsPerBC calibration)

  const int32_t mMinAmplitudeSideA;
  const int32_t mMinAmplitudeSideC;

  std::array<double, o2::constants::lhc::LHCMaxBunches> mTvx{0.0};
  size_t entries{0};
  long startTimeStamp{0};
  long stopTimeStamp{0};

<<<<<<< HEAD
  ClassDefNV(EventsPerBc, 1);
};

class EventsPerBcCalibrator final : public o2::calibration::TimeSlotCalibration<o2::ft0::EventsPerBc>
{
  using Slot = o2::calibration::TimeSlot<o2::ft0::EventsPerBc>;
  using TFType = o2::calibration::TFType;
  using EventsHistogram = std::array<double, o2::constants::lhc::LHCMaxBunches>;
=======
  ClassDefNV(EventsPerBcContainer, 1);
};

class EventsPerBcCalibrator final : public o2::calibration::TimeSlotCalibration<o2::ft0::EventsPerBcContainer>
{
  using Slot = o2::calibration::TimeSlot<o2::ft0::EventsPerBcContainer>;
  using TFType = o2::calibration::TFType;
  using EventsHistogram = std::array<double, o2::constants::lhc::LHCMaxBunches>;
>>>>>>> b9ec63cd4d (Created CCDB object class for EvetnsPerBC calibration)

 public:
  EventsPerBcCalibrator(uint32_t minNumberOfEntries, int32_t minAmplitudeSideA, int32_t minAmplitudeSideC);

  bool hasEnoughData(const Slot& slot) const override;
  void initOutput() override;
  void finalizeSlot(Slot& slot) override;
  Slot& emplaceNewSlot(bool front, TFType tstart, TFType tend) override;

<<<<<<< HEAD
  const std::vector<EventsHistogram>& getTvxPerBc() { return mTvxPerBcs; }
  std::vector<std::unique_ptr<o2::ccdb::CcdbObjectInfo>>& getTvxPerBcCcdbInfo() { return mTvxPerBcInfos; }
=======
  const std::vector<EventsPerBc>& getTvxPerBc() { return mTvxPerBcs; }
  std::vector<std::unique_ptr<o2::ccdb::CcdbObjectInfo>>& getTvxPerBcCcdbInfo() { return mTvxPerBcInfos; }
>>>>>>> b9ec63cd4d (Created CCDB object class for EvetnsPerBC calibration)

 private:
  const uint32_t mMinNumberOfEntries;
  const int32_t mMinAmplitudeSideA;
  const int32_t mMinAmplitudeSideC;

<<<<<<< HEAD
  std::vector<EventsHistogram> mTvxPerBcs;
  std::vector<std::unique_ptr<o2::ccdb::CcdbObjectInfo>> mTvxPerBcInfos;
=======
  std::vector<EventsPerBc> mTvxPerBcs;
  std::vector<std::unique_ptr<o2::ccdb::CcdbObjectInfo>> mTvxPerBcInfos;
>>>>>>> b9ec63cd4d (Created CCDB object class for EvetnsPerBC calibration)

  ClassDefOverride(EventsPerBcCalibrator, 1);
};
} // namespace o2::ft0

#endif
