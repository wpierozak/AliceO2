#ifndef O2_FT0TVXPERBCID
#define O2_FT0TVXPERBCID

#include <bitset>
#include <array>

#include "CommonDataFormat/FlatHisto2D.h"
#include "CommonConstants/LHCConstants.h"
#include "DataFormatsFT0/SpectraInfoObject.h"
#include "DetectorsCalibration/TimeSlotCalibration.h"
#include "DetectorsCalibration/TimeSlot.h"
#include "DataFormatsFT0/BcEvents.h"

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
    };

    class EventsPerBcCalibrator final : public o2::calibration::TimeSlotCalibration<o2::ft0::EventsPerBc>
    {
        using Slot = o2::calibration::TimeSlot<o2::ft0::EventsPerBc>;
        using TFType = o2::calibration::TFType;

        public:
        EventsPerBcCalibrator()
        {
            setUpdateAtTheEndOfRunOnly();
        }
        
        bool hasEnoughData(const Slot& slot) const final { return true; }
        void initOutput() final;
        void finalizeSlot(Slot& slot) final;
        Slot& emplaceNewSlot(bool front, TFType tstart, TFType tend) final;

        const TH1F* getTvxPerBc() { return mTvxPerBc.get(); }
        const CcdbObjectInfo* getTvxPerBcCcdbInfo() { return mTvxPerBcInfo.get(); }

        private:
        std::unique_ptr<TH1F> mTvxPerBc;
        std::unique_ptr<CcdbObjectInfo> mTvxPerBcInfo;
    };
}

#endif
