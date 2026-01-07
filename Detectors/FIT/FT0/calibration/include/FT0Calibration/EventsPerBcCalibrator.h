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
#include "DetectorsCalibration/TimeSlotCalibration.h"
#include "DetectorsCalibration/TimeSlot.h"
#include "CommonDataFormat/TFIDInfo.h"
#include "TH1F.h"
#include "Rtypes.h"

namespace o2::ft0
{
    struct EventsPerBc
    {
        EventsPerBc(int32_t minAmplitudeSideA, int32_t minAmplitudeSideC): mMinAmplitudeSideA(minAmplitudeSideA), mMinAmplitudeSideC(minAmplitudeSideC) {}
        
        size_t getEntries() const { return entries; }
        void print() const;
        void fill(const o2::dataformats::TFIDInfo& ti, const gsl::span<const o2::ft0::Digit> data);
        void merge(const EventsPerBc* prev);

        const int32_t mMinAmplitudeSideA;
        const int32_t mMinAmplitudeSideC;

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
        EventsPerBcCalibrator(uint32_t minNumberOfEntries, int32_t minAmplitudeSideA, int32_t minAmplitudeSideC);
        
        bool hasEnoughData(const Slot& slot) const override;
        void initOutput() override;
        void finalizeSlot(Slot& slot) override;
        Slot& emplaceNewSlot(bool front, TFType tstart, TFType tend) override;

        const std::vector<std::unique_ptr<TH1F>>& getTvxPerBc() { return mTvxPerBcs; }
        std::vector<std::unique_ptr<o2::ccdb::CcdbObjectInfo>>& getTvxPerBcCcdbInfo() { return mTvxPerBcInfos; }

        private:
        const uint32_t mMinNumberOfEntries;
        const int32_t mMinAmplitudeSideA;
        const int32_t mMinAmplitudeSideC;

        std::vector<std::unique_ptr<TH1F>> mTvxPerBcs;
        std::vector<std::unique_ptr<o2::ccdb::CcdbObjectInfo>> mTvxPerBcInfos;

        ClassDefOverride(EventsPerBcCalibrator, 1);
    };
}

#endif
