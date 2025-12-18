#include "Framework/runDataProcessing.h"
#include "CommonUtils/ConfigurableParam.h"
#include "Framework/ConfigParamSpec.h"
#include <Framework/ConfigContext.h>
#include "Framework/DeviceSpec.h"
#include "Framework/WorkflowSpec.h"
#include "Framework/Task.h"

#include "DataFormatsFT0/Digit.h"
#include "DataFormatsFT0/BcEvents.h"
#include "FT0Calibration/EventsPerBcCalibrator.h"

namespace o2::ft0
{
    class FT0EventsPerBcProcessor final : public o2::framework::Task
    {
        void init(o2::framework::InitContext& ic) final
        {
            mCalibrator = std::make_unique<EventsPerBcCalibrator>();
        }

        void run(o2::framework::ProcessContext& pc) final
        {
            auto digits = pc.inputs().get<gsl::span<o2::ft0::Digit>>("digits");
            mCalibrator->process(digits);
        }

        void endOfStream(o2::framework::EndOfStreamContext& ec) final 
        {
            mCalibrator->chekSlotToFinalize();
            sendOutput(ec.outputs());
            mCalibrator->initOutput();    
        }

        void sendOutput(DataAllocator& output)
        {
            TH1F* tvxHist = mCalibrator->getTvxPerBc();
            CcdbObjectInfo* info = mCalibrator->getTvxPerBcCcdbInfo();
            auto image = o2::ccdb::CcdbApi::createObjectImage(tvxHist, info);
            LOG(info) << "Sending object to CCDB";
            output.snapshot(Output{o2::calibration::Utils::gDataOriginCDBPayload, "EVENTS_PER_BC_INFO", 0}, *image.get());
            output.snapshot(Output{o2::calibration::Utils::gDataOriginCDBWrapper, "EVENTS_PER_BC_INFO", 0}, info);
        }

        private:
        std::unique_ptr<EventsPerBcCalibrator> mCalibrator;
    };
}

WorkflowSpec defineDataProcessing(ConfigContext& const & cfgc)
{
    using namespace o2::framework;
    std::vector<InputSpec> inputs;
    inputs.emplace_back("digits", "FT0", "DIGITSBC");
    std::vector<OutputSpec> outputs;
    outputs.emplace_back("eventsPerBcInfo", "FT0", "EVENTS_PER_BC_INFO")
    DataProcessorSpec dataProcessorSpec{
        "FT0EventsPerBcProcessor",
        inputs,
        outputs,
        AlgorithmSpec(adaptFromTask<o2::ft0::FT0EventsPerBcProcessor>()),
        Options{}
    };

    WorkflowSpec workflow;
    workflow.emplace_back(dataProcessorSpec);
    return workflow;
}