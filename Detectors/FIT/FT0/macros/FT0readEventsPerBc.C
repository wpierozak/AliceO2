#if !defined(__CLING__) || defined(__ROOTCLING__)

#include <iostream>
#include <array>
#include "CCDB/CcdbApi.h"
#include "TH1F.h"
#endif

#include "Framework/Logger.h"
#include "CommonConstants/LHCConstants.h"
#include "DataFormatsFT0/EventsPerBc.h"

std::unique_ptr<TH1F> hist;
std::unique_ptr<TCanvas> canvas;

void FT0readEventsPerBc(std::string ccdbUrl, long timestamp)
{
  o2::ccdb::CcdbApi ccdbApi;
  ccdbApi.init(ccdbUrl);
  const std::string ccdbPath = "FT0/Calib/EventsPerBc";
  std::map<std::string, std::string> metadata;

  if (timestamp < 0) {
    timestamp = o2::ccdb::getCurrentTimestamp();
  }

  EventsArray* events = ccdbApi.retrieveFromTFileAny<o2::ft0::EventsPerBc>(ccdbPath, metadata, timestamp);

  if (!events) {
    LOGP(fatal, "EventsPerBc object not found in {}/{} for timestamp {}.", ccdbUrl, ccdbPath, timestamp);
    return;
  }

  hist = std::make_unique<TH1F>("eventsPerBcHist", "Events per BC", o2::constants::lhc::LHCMaxBunches, 0, o2::constants::lhc::LHCMaxBunches - 1);
  for (int idx = 0; idx < o2::constants::lhc::LHCMaxBunches; idx++) {
    hist->Fill(idx, events->histogram[idx]);
  }
  canvas = std::make_unique<TCanvas>();
  hist->Draw();
  canvas->Draw();
}