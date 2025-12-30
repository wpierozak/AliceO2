#include "FT0Calibration/EventsPerBcCalibrator.h"
#include "CommonUtils/MemFileHelper.h"

namespace o2::ft0
{
    void EventsPerBc::print() const
    {
        LOG(info) << entries << " entries";
    }

    void EventsPerBc::fill(const gsl::span<const o2::ft0::Digit> data)
    {
        for(const auto& digit: data) {
            double isVertex = digit.mTriggers.getVertex();
            mTvx[digit.mIntRecord.bc] += isVertex;
            entries += isVertex;
        }
    }

    void EventsPerBc::merge(const EventsPerBc* prev)
    {
       for(int bc = 0; bc < o2::constants::lhc::LHCMaxBunches; bc++){
            mTvx[bc] += prev->mTvx[bc];    
       }
       entries += prev->entries;
    }

    void EventsPerBcCalibrator::initOutput()
    {
        mTvxPerBcs.clear();
        mTvxPerBcInfos.clear();
    }

    void EventsPerBcCalibrator::finalizeSlot(EventsPerBcCalibrator::Slot& slot)
    {
        o2::ft0::EventsPerBc* data = slot.getContainer();
        mTvxPerBcs.emplace_back(std::make_unique<TH1F>("TvxPerBc", "FT0 TVX per BC", o2::constants::lhc::LHCMaxBunches, 0, o2::constants::lhc::LHCMaxBunches - 1));
        for(int bin = 0; bin < o2::constants::lhc::LHCMaxBunches; bin++) {
            mTvxPerBcs.back()->Fill(bin, data->mTvx[bin]);
        }
        auto clName = o2::utils::MemFileHelper::getClassName(*mTvxPerBcs.back());
        auto flName = o2::ccdb::CcdbApi::generateFileName(clName);
        std::map<std::string, std::string> metaData;
        mTvxPerBcInfos.emplace_back(std::make_unique<o2::ccdb::CcdbObjectInfo>("FT0/Calib/TvxPerBc", clName, flName, metaData, slot.getStartTimeMS(), slot.getEndTimeMS()));
    }

    EventsPerBcCalibrator::Slot& EventsPerBcCalibrator::emplaceNewSlot(bool front, TFType tstart, TFType tend)
    {
        auto& cont = getSlots();
        auto& slot = front ? cont.emplace_front(tstart, tend) : cont.emplace_back(tstart, tend);
        slot.setContainer(std::make_unique<EventsPerBc>());
        return slot;
    }
}