#include "FT0EventsPerBcSpec.h"
o2::framework::WorkflowSpec defineDataProcessing(o2::framework::ConfigContext const& cfgc)
{
    using namespace o2::framework;
    using o2::calibration::FT0EventsPerBcProcessor;
    std::vector<InputSpec> inputs;
    inputs.emplace_back("digits", "FT0", "DIGITSBC");
    std::vector<OutputSpec> outputs;
    outputs.emplace_back(ConcreteDataTypeMatcher{o2::calibration::Utils::gDataOriginCDBWrapper, "EventsPerBc"}, Lifetime::Sporadic);
    outputs.emplace_back(ConcreteDataTypeMatcher{o2::calibration::Utils::gDataOriginCDBPayload, "EventsPerBc"}, Lifetime::Sporadic);
    DataProcessorSpec dataProcessorSpec{
        "FT0EventsPerBcProcessor",
        inputs,
        outputs,
        AlgorithmSpec(adaptFromTask<FT0EventsPerBcProcessor>()),
        Options{
            {"slot-len-sec", VariantType::UInt32, 3600u, {"Time lenght of slot in seconds"}},
            {"one-object-per-run", VariantType::Bool, false, {"If true, then one calibration object is created per run"}}
        }
    };

    WorkflowSpec workflow;
    workflow.emplace_back(dataProcessorSpec);
    return workflow;
}