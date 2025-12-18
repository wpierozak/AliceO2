#include "FT0Calibration/EventsPerBcCalibrator.h"

namespace o2::ft0
{
    void EventsPerBc::print() const
    {

    }

    void EventsPerBc::fill(const gsl::span<const o2::ft0::Digit> data)
    {
        for(const auto& digit: digits) {
            double isVertex = digit.mTriggers.isVertex();
            mTvx[digits.mIntRecord.bc] += isVertex;
            entries += isVertex;
        }
    }

    void EventsPerBc::merge(const EventsPerBc* prev)
    {
       for(int bc = 0; bc < o2::constants::lhc::LHCMaxBunches; bc++){
            mTvx[bc] += prev->mTvx[bc];    
       }
       entries += prev->mEntries;
    }

    void EventsPerBcCalibrator::initOutput() final
    {
        mTvxPerBc.reset();
        mTvxPerBcInfo.reset();
    }

    void EventsPerBcCalibrator::finalizeSlot(EventsPerBcCalibrator::Slot& slot) final
    {
        o2::ft0::EventsPerBc* data = slot.getContainer();
        mTvxPerBc = std::make_unique<TH1F>("TvxPerBc", "FT0 TVX per BC", o2::constants::lhc::LHCMaxBunches, 0, o2::constants::lhc::LHCMaxBunches - 1);
        for(int bin = 0; bin < o2::constants::lhc::LHCMaxBunches; bin++) {
            tvxsHist->fill(bin, data->mTvx[bin]);
        }
        auto clName = o2::utils::MemFileHelper::getClassName(*tvxsHist);
        auto flName = o2::ccdb::CcdbApi::generateFileName(clName);
        std::map<std::string, std::string> metaData;
        mTvxPerBcInfo = std::make_unique<CcdbObjectInfo>("FT0/Calib/TvxPerBc", clName, flName, metaData, slot.getStarTimeMs(), slot.getEndTimeStampMS());
    }

    EventsPerBcCalibrator::Slot& EventsPerBcCalibrator::emplaceNewSlot(bool front, TFType tstart, TFType tend) final
    {
        auto& cont = getSlots();
        auto& slot = front ? cont.emplace_front(tstart, tend) : cont.emplace_back(tstart, tend);
        slot.setContainer(std::make_unique<EventsPerBc>());
        return slot;
    }
}