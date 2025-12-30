#include "Framework/runDataProcessing.h"
#include "CommonUtils/ConfigurableParam.h"
#include "Framework/ConfigParamSpec.h"
#include <Framework/ConfigContext.h>
#include "Framework/DeviceSpec.h"
#include "Framework/WorkflowSpec.h"
#include "Framework/Task.h"
#include "DetectorsCalibration/Utils.h"

#include "DataFormatsFT0/Digit.h"
#include "FT0Calibration/EventsPerBcCalibrator.h"


namespace o2::calibration
{
    class FT0EventsPerBcProcessor final : public o2::framework::Task
    {
        public:
        void init(o2::framework::InitContext& ic) final
        {
            mCalibrator = std::make_unique<o2::ft0::EventsPerBcCalibrator>();
            if(ic.options().hasOption("slot-len-sec")) {
                mSlotLenSec = ic.options().get<uint32_t>("slot-len-sec");
            }
            if(ic.options().hasOption("one-object-per-run")) {
                mOneObjectPerRun = ic.options().get<bool>("one-object-per-run");
            }

            if(mOneObjectPerRun) {
                mCalibrator->setUpdateAtTheEndOfRunOnly();
            } else {
                mCalibrator->setSlotLengthInSeconds(mSlotLenSec);
            }
        }

        void run(o2::framework::ProcessingContext& pc) final
        {
            auto digits = pc.inputs().get<gsl::span<o2::ft0::Digit>>("digits");
            mCalibrator->process(digits);
            if(mOneObjectPerRun == false) {
                sendOutput(pc.outputs());
            }
        }

        void endOfStream(o2::framework::EndOfStreamContext& ec) final 
        {
            mCalibrator->checkSlotsToFinalize();
            sendOutput(ec.outputs());
            mCalibrator->initOutput();    
        }

        void sendOutput(o2::framework::DataAllocator& output)
        {
            using o2::framework::Output;

            const auto& tvxHists = mCalibrator->getTvxPerBc();
            auto& infos = mCalibrator->getTvxPerBcCcdbInfo();
            for(int idx = 0; idx < tvxHists.size(); idx++){
                auto& info =  infos[idx];
                const auto& payload = tvxHists[idx];

                auto image = o2::ccdb::CcdbApi::createObjectImage(payload.get(), info.get());
                LOG(info) << "Sending object " << info->getPath() << "/" << info->getFileName() << " of size " << image->size()
                    << " bytes, valid for " << info->getStartValidityTimestamp() << " : " << info->getEndValidityTimestamp();
                output.snapshot(Output(o2::calibration::Utils::gDataOriginCDBPayload, "EVENTS_PER_BC_INFO", 0), *image.get());
                output.snapshot(Output(o2::calibration::Utils::gDataOriginCDBWrapper, "EVENTS_PER_BC_INFO", 0), *info.get());
            }
        }

        private:
        std::unique_ptr<o2::ft0::EventsPerBcCalibrator> mCalibrator;
        bool mOneObjectPerRun;
        uint32_t mSlotLenSec;
    };
}

namespace o2::framework
{
WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
    using namespace o2::framework;
    using o2::calibration::FT0EventsPerBcProcessor;
    std::vector<InputSpec> inputs;
    inputs.emplace_back("digits", "FT0", "DIGITSBC");
    std::vector<OutputSpec> outputs;
    outputs.emplace_back("eventsPerBcInfo", "FT0", "EVENTS_PER_BC_INFO");
    DataProcessorSpec dataProcessorSpec{
        "FT0EventsPerBcProcessor",
        inputs,
        outputs,
        AlgorithmSpec(adaptFromTask<FT0EventsPerBcProcessor>()),
        Options{
            {"slot-len-sec", VariantType::UInt32, 3600u, "Time lenght of slot in seconds"},
            {"one-object-per-run", VariantType::Bool, false, "If true, then one calibration object is created per run"}
        }
    };

    WorkflowSpec workflow;
    workflow.emplace_back(dataProcessorSpec);
    return workflow;
}
}