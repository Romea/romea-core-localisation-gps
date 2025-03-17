// Copyright 2022 INRAE, French National Research Institute for Agriculture, Food and Environment
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


// std
#include <string>

// local
#include "romea_core_localisation_gps/checkup_gga_fix.hpp"

namespace
{
const double MAXIMAL_HORIZONTAL_DILUTION_OF_PRECISION = 5;
const uint16_t MINIMAL_NUMBER_OF_SATELLITES_TO_COMPUTE_FIX = 6;
}

namespace romea
{
namespace core
{
namespace localisation
{

//-----------------------------------------------------------------------------
CheckupGGAFix::CheckupGGAFix(const FixQuality & minimal_fix_quality)
: minimal_fix_quality_(minimal_fix_quality),
  report_()
{
  declare_report_infos_();
}

//-----------------------------------------------------------------------------
DiagnosticStatus CheckupGGAFix::evaluate(const GGAFrame & gga_frame)
{
  std::lock_guard<std::mutex> lock(mutex_);
  report_.diagnostics.clear();
  if (check_frame_is_complete_(gga_frame)) {
    check_fix_is_reliable_(gga_frame);
  }

  set_report_infos_(gga_frame);
  return worseStatus(report_.diagnostics);
}


//-----------------------------------------------------------------------------
void CheckupGGAFix::set_report_infos_(const GGAFrame & gga_frame)
{
  setReportInfo(report_, "talker", gga_frame.talkerId);
  setReportInfo(report_, "geoid_height", gga_frame.geoidHeight);
  setReportInfo(report_, "altitude_above_geoid", gga_frame.altitudeAboveGeoid);
  setReportInfo(report_, "fix_quality", gga_frame.fixQuality);
  setReportInfo(report_, "number_of_satellites", gga_frame.numberSatellitesUsedToComputeFix);
  setReportInfo(report_, "hdop", gga_frame.horizontalDilutionOfPrecision);
  setReportInfo(report_, "correction_age", gga_frame.dgpsCorrectionAgeInSecond);
  setReportInfo(report_, "base_station_id", gga_frame.dgpsStationIdNumber);

  if (gga_frame.latitude) {
    setReportInfo(report_, "latitude", (*gga_frame.latitude).toDouble());
  } else {
    setReportInfo(report_, "latitude", gga_frame.latitude);
  }

  if (gga_frame.longitude) {
    setReportInfo(report_, "longitude", (*gga_frame.longitude).toDouble());
  } else {
    setReportInfo(report_, "longitude", gga_frame.longitude);
  }
}

//-----------------------------------------------------------------------------
bool CheckupGGAFix::check_frame_is_complete_(const GGAFrame & gga_frame)
{
  if (gga_frame.latitude &&
    gga_frame.longitude &&
    gga_frame.altitudeAboveGeoid &&
    gga_frame.geoidHeight &&
    gga_frame.horizontalDilutionOfPrecision &&
    gga_frame.numberSatellitesUsedToComputeFix &&
    gga_frame.fixQuality)
  {
    return true;
  } else {
    add_diagnostic_(DiagnosticStatus::ERROR, "GGA fix is incomplete.");
    return false;
  }
}

//-----------------------------------------------------------------------------
void CheckupGGAFix::check_fix_is_reliable_(const GGAFrame & gga_frame)
{
  if (*gga_frame.fixQuality == FixQuality::SIMULATION_FIX ||
    (check_horizontal_dilution_of_precision_(gga_frame) &
    check_number_satellites_used_to_compute_fix_(gga_frame) &
    check_fix_quality_(gga_frame)))
  {
    add_diagnostic_(DiagnosticStatus::OK, "GGA fix OK.");
  }
}


//-----------------------------------------------------------------------------
bool CheckupGGAFix::check_horizontal_dilution_of_precision_(const GGAFrame & gga_frame)
{
  if (*gga_frame.horizontalDilutionOfPrecision <
    MAXIMAL_HORIZONTAL_DILUTION_OF_PRECISION)
  {
    return true;
  } else {
    add_diagnostic_(DiagnosticStatus::WARN, "HDOP is two high.");
    return false;
  }
}

//-----------------------------------------------------------------------------
bool CheckupGGAFix::check_number_satellites_used_to_compute_fix_(const GGAFrame & gga_frame)
{
  if (*gga_frame.numberSatellitesUsedToComputeFix >=
    MINIMAL_NUMBER_OF_SATELLITES_TO_COMPUTE_FIX)
  {
    return true;
  } else {
    add_diagnostic_(DiagnosticStatus::WARN, "Not enough satellites to compute fix.");
    return false;
  }
}

//-----------------------------------------------------------------------------
bool CheckupGGAFix::check_fix_quality_(const GGAFrame & gga_frame)
{
  if (*gga_frame.fixQuality >= minimal_fix_quality_) {
    return true;
  } else {
    add_diagnostic_(DiagnosticStatus::WARN, "Fix quality is too low.");
    return false;
  }
}

//-----------------------------------------------------------------------------
void CheckupGGAFix::declare_report_infos_()
{
  setReportInfo(report_, "talker", "");
  setReportInfo(report_, "geoid_height", "");
  setReportInfo(report_, "altitude_above_geoid", "");
  setReportInfo(report_, "fix_quality", "");
  setReportInfo(report_, "number_of_satellites", "");
  setReportInfo(report_, "hdop", "");
  setReportInfo(report_, "correction_age", "");
  setReportInfo(report_, "base_station_id", "");
  setReportInfo(report_, "latitude", "");
  setReportInfo(report_, "longitude", "");
}

//-----------------------------------------------------------------------------
const DiagnosticReport & CheckupGGAFix::get_report()const
{
  std::lock_guard<std::mutex> lock(mutex_);
  return report_;
}

//-----------------------------------------------------------------------------
void CheckupGGAFix::reset()
{
  std::lock_guard<std::mutex> lock(mutex_);
  report_.diagnostics.clear();
  declare_report_infos_();
}

//-----------------------------------------------------------------------------
void CheckupGGAFix::add_diagnostic_(const DiagnosticStatus & status, const std::string & message)
{
  report_.diagnostics.push_back(Diagnostic());
  report_.diagnostics.back().status = status;
  report_.diagnostics.back().message = message;
}

}  // namespace localisation
}  // namespace core
}  // namespace romea
