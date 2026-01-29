#include "FT0EventsPerBcSpec.h"
#include "Framework/Lifetime.h"
#include <limits>

o2::framework::WorkflowSpec defineDataProcessing(o2::framework::ConfigContext const& cfgc)
{
  using namespace o2::framework;
  using o2::calibration::FT0EventsPerBcProcessor;
  std::vector<InputSpec> inputs;
  inputs.emplace_back("digits", "FT0", "DIGITSBC", Lifetime::Timeframe);
  auto ccdbRequest = std::make_shared<o2::base::GRPGeomRequest>(true,  // orbitResetTime
                                                                false, // GRPECS=true
                                                                false,                          // GRPLHCIF
                                                                false,                          // GRPMagField
                                                                false,                          // askMatLUT
                                                                o2::base::GRPGeomRequest::None, // geometry
                                                                inputs);
  std::vector<OutputSpec> outputs;
  outputs.emplace_back(ConcreteDataTypeMatcher{o2::calibration::Utils::gDataOriginCDBWrapper, "EventsPerBc"}, Lifetime::Timeframe);
  outputs.emplace_back(ConcreteDataTypeMatcher{o2::calibration::Utils::gDataOriginCDBPayload, "EventsPerBc"}, Lifetime::Timeframe);
  DataProcessorSpec dataProcessorSpec{
    "FT0EventsPerBcProcessor",
    inputs,
    outputs,
    AlgorithmSpec(adaptFromTask<FT0EventsPerBcProcessor>(ccdbRequest)),
    Options{
      {"slot-len-sec", VariantType::UInt32, 3600u, {"Duration of each slot in seconds"}},
      {"slot-len-tf", VariantType::UInt32, 0u, {"Slot length in Time Frames (TFs)"}},
      {"one-object-per-run", VariantType::Bool, false, {"If set, workflow creates only one calibration object per run"}},
      {"min-entries-number", VariantType::UInt32, 0u, {"Minimum number of entries required for a slot to be valid"}},
      {"min-ampl-side-a", VariantType::Int, 0, {"Amplitude threshold for Side A events"}},
      {"min-ampl-side-c", VariantType::Int, 0, {"Amplitude threshold for Side C events"}}}};

  WorkflowSpec workflow;
  workflow.emplace_back(dataProcessorSpec);
  return workflow;
}