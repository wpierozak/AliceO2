#ifndef O2_FT0TVXPERBCID
#define O2_FT0TVXPERBCID

#include <bitset>
#include <array>
#include <TH1F.h>

#include "CommonDataFormat/FlatHisto2D.h"
#include "CommonConstants/LHCConstants.h"
#include "DataFormatsFT0/SpectraInfoObject.h"
#include "DataFormatsFT0/Digit.h"
#include "DetectorsCalibration/TimeSlotCalibration.h"
#include "DetectorsCalibration/TimeSlot.h"
#include "TH1F.h"
#include "Rtypes.h"

namespace o2::ft0
{
    struct EventsPerBc
    {
        EventsPerBc() = default;
        
        size_t getEntries() const { return entries; }
        void print() const;
        void fill(const gsl::span<const o2::ft0::Digit> data);
        void merge(const EventsPerBc* prev);

        std::array<double, o2::constants::lhc::LHCMaxBunches> mTvx{0.0};
        size_t entries{0};
        long startTimeStamp{0};
        long stopTimeStamp{0};

        ClassDefNV(EventsPerBc, 1);
    };

    class EventsPerBcCalibrator final : public o2::calibration::TimeSlotCalibration<o2::ft0::EventsPerBc>
    {
        using Slot = o2::calibration::TimeSlot<o2::ft0::EventsPerBc>;
        using TFType = o2::calibration::TFType;

        public:
        EventsPerBcCalibrator() = default;
        
        bool hasEnoughData(const Slot& slot) const final { return true; }
        void initOutput() final;
        void finalizeSlot(Slot& slot) final;
        Slot& emplaceNewSlot(bool front, TFType tstart, TFType tend) final;

        const std::vector<std::unique_ptr<TH1F>>& getTvxPerBc() { return mTvxPerBcs; }
        std::vector<std::unique_ptr<o2::ccdb::CcdbObjectInfo>>& getTvxPerBcCcdbInfo() { return mTvxPerBcInfos; }

        private:
        std::vector<std::unique_ptr<TH1F>> mTvxPerBcs;
        std::vector<std::unique_ptr<o2::ccdb::CcdbObjectInfo>> mTvxPerBcInfos;

        ClassDefOverride(EventsPerBcCalibrator, 1);
    };
}

#endif
