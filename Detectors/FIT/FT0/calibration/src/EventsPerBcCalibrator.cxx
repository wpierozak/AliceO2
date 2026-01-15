#include "FT0Calibration/EventsPerBcCalibrator.h"
#include "CommonUtils/MemFileHelper.h"

namespace o2::ft0
{
    void EventsPerBc::print() const
    {
        LOG(info) << entries << " entries";
    }

    void EventsPerBc::fill(const o2::dataformats::TFIDInfo& ti, const gsl::span<const o2::ft0::Digit> data)
    {
        size_t oldEntries = entries;
        for(const auto& digit: data) {
            double isVertex = digit.mTriggers.getVertex();
            if (digit.mTriggers.getAmplA() < mMinAmplitudeSideA || digit.mTriggers.getAmplC() < mMinAmplitudeSideC) {
              continue;
            }
            mTvx[digit.mIntRecord.bc] += isVertex;
            entries += isVertex;
        }
        LOG(debug) << "Container is filled with " << entries - oldEntries << " new VTX events";
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

    EventsPerBcCalibrator::EventsPerBcCalibrator(uint32_t minNumberOfEntries, int32_t minAmplitudeSideA, int32_t minAmplitudeSideC) : mMinNumberOfEntries(minNumberOfEntries), mMinAmplitudeSideA(minAmplitudeSideA), mMinAmplitudeSideC(minAmplitudeSideC)
    {
      LOG(info) << "Defined threshold for number of entires per slot: " << mMinNumberOfEntries;
      LOG(info) << "Defined threshold for side A amplitude for event: " << mMinAmplitudeSideA;
      LOG(info) << "Defined threshold for side C amplitude for event: " << mMinAmplitudeSideC;
    }

    bool EventsPerBcCalibrator::hasEnoughData(const EventsPerBcCalibrator::Slot& slot) const
    { 
        return slot.getContainer()->entries > mMinNumberOfEntries;
    }

    void EventsPerBcCalibrator::finalizeSlot(EventsPerBcCalibrator::Slot& slot)
    {
        LOG(info) << "Finalizing slot from " << slot.getStartTimeMS() << " to " << slot.getEndTimeMS();
        o2::ft0::EventsPerBc* data = slot.getContainer();
        mTvxPerBcs.emplace_back(std::make_unique<TH1F>("EventsPerBc", "FT0 Events per BC", o2::constants::lhc::LHCMaxBunches, 0, o2::constants::lhc::LHCMaxBunches - 1));
        for (int bin = 0; bin < o2::constants::lhc::LHCMaxBunches; bin++) {
            mTvxPerBcs.back()->Fill(bin, data->mTvx[bin]);
        }
        auto clName = o2::utils::MemFileHelper::getClassName(*mTvxPerBcs.back());
        auto flName = o2::ccdb::CcdbApi::generateFileName(clName) + ".root";
        std::map<std::string, std::string> metaData;
        mTvxPerBcInfos.emplace_back(std::make_unique<o2::ccdb::CcdbObjectInfo>("FT0/Calib/EventsPerBc", clName, flName, metaData, slot.getStartTimeMS(), slot.getEndTimeMS()));
        LOG(info) << "Created object valid from " << mTvxPerBcInfos.back()->getStartValidityTimestamp() << " to " << mTvxPerBcInfos.back()->getEndValidityTimestamp();
    }

    EventsPerBcCalibrator::Slot& EventsPerBcCalibrator::emplaceNewSlot(bool front, TFType tstart, TFType tend)
    {
        auto& cont = getSlots();
        auto& slot = front ? cont.emplace_front(tstart, tend) : cont.emplace_back(tstart, tend);
        slot.setContainer(std::make_unique<EventsPerBc>(mMinAmplitudeSideA, mMinAmplitudeSideC));
        return slot;
    }
}