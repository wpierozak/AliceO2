#ifndef _FT0_EVENTS_PER_BC_CALIB_OBJECT
#define _FT0_EVENTS_PER_BC_CALIB_OBJECT

#include "CommonConstants/LHCConstants.h"
#include <Rtypes.h>

namespace o2::ft0
{
struct EventsPerBc {
  std::array<double, o2::constants::lhc::LHCMaxBunches> histogram;
  int16_t amplThresSideA;
  int16_t amplThresSideC;
  ClassDefNV(EventsPerBc, 1);
};
} // namespace o2::ft0
#endif