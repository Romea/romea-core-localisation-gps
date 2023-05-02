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
#include "romea_core_localisation_gps/CheckupGGAFix.hpp"

namespace
{
const double MAXIMAL_HORIZONTAL_DILUTION_OF_PRECISION = 5;
const unsigned short MINIMAL_NUMBER_OF_SATELLITES_TO_COMPUTE_FIX = 6;
}

namespace romea
{

//-----------------------------------------------------------------------------
CheckupGGAFix::CheckupGGAFix(const FixQuality & minimalFixQuality)
: minimalFixQuality_(minimalFixQuality),
  report_()
{
  declareReportInfos_();
}

//-----------------------------------------------------------------------------
DiagnosticStatus CheckupGGAFix::evaluate(const GGAFrame & ggaFrame)
{
  std::lock_guard<std::mutex> lock(mutex_);
  report_.diagnostics.clear();
  if (checkFrameIsComplete_(ggaFrame)) {
    checkFixIsReliable_(ggaFrame);
  }

  setReportInfos_(ggaFrame);
  return worseStatus(report_.diagnostics);
}


//-----------------------------------------------------------------------------
void CheckupGGAFix::setReportInfos_(const GGAFrame & ggaFrame)
{
  setReportInfo(report_, "talker", ggaFrame.talkerId);
  setReportInfo(report_, "geoid_height", ggaFrame.geoidHeight);
  setReportInfo(report_, "altitude_above_geoid", ggaFrame.altitudeAboveGeoid);
  setReportInfo(report_, "fix_quality", ggaFrame.fixQuality);
  setReportInfo(report_, "number_of_satellites", ggaFrame.numberSatellitesUsedToComputeFix);
  setReportInfo(report_, "hdop", ggaFrame.horizontalDilutionOfPrecision);
  setReportInfo(report_, "correction_age", ggaFrame.dgpsCorrectionAgeInSecond);
  setReportInfo(report_, "base_station_id", ggaFrame.dgpsStationIdNumber);

  if (ggaFrame.latitude) {
    setReportInfo(report_, "latitude", (*ggaFrame.latitude).toDouble());
  } else {
    setReportInfo(report_, "latitude", ggaFrame.latitude);
  }

  if (ggaFrame.longitude) {
    setReportInfo(report_, "longitude", (*ggaFrame.longitude).toDouble());
  } else {
    setReportInfo(report_, "longitude", ggaFrame.longitude);
  }
}

//-----------------------------------------------------------------------------
bool CheckupGGAFix::checkFrameIsComplete_(const GGAFrame & ggaFrame)
{
  if (ggaFrame.latitude &&
    ggaFrame.longitude &&
    ggaFrame.altitudeAboveGeoid &&
    ggaFrame.geoidHeight &&
    ggaFrame.horizontalDilutionOfPrecision &&
    ggaFrame.numberSatellitesUsedToComputeFix &&
    ggaFrame.fixQuality)
  {
    return true;
  } else {
    addDiagnostic_(DiagnosticStatus::ERROR, "GGA fix is incomplete.");
    return false;
  }
}

//-----------------------------------------------------------------------------
void CheckupGGAFix::checkFixIsReliable_(const GGAFrame & ggaFrame)
{
  if (*ggaFrame.fixQuality == FixQuality::SIMULATION_FIX ||
    (checkHorizontalDilutionOfPrecision_(ggaFrame) &
    checkNumberSatellitesUsedToComputeFix_(ggaFrame) &
    checkFixQuality_(ggaFrame)))
  {
    addDiagnostic_(DiagnosticStatus::OK, "GGA fix OK.");
  }
}


//-----------------------------------------------------------------------------
bool CheckupGGAFix::checkHorizontalDilutionOfPrecision_(const GGAFrame & ggaFrame)
{
  if (*ggaFrame.horizontalDilutionOfPrecision <
    MAXIMAL_HORIZONTAL_DILUTION_OF_PRECISION)
  {
    return true;
  } else {
    addDiagnostic_(DiagnosticStatus::WARN, "HDOP is two high.");
    return false;
  }
}

//-----------------------------------------------------------------------------
bool CheckupGGAFix::checkNumberSatellitesUsedToComputeFix_(const GGAFrame & ggaFrame)
{
  if (*ggaFrame.numberSatellitesUsedToComputeFix >=
    MINIMAL_NUMBER_OF_SATELLITES_TO_COMPUTE_FIX)
  {
    return true;
  } else {
    addDiagnostic_(DiagnosticStatus::WARN, "Not enough satellites to compute fix.");
    return false;
  }
}

//-----------------------------------------------------------------------------
bool CheckupGGAFix::checkFixQuality_(const GGAFrame & ggaFrame)
{
  if (*ggaFrame.fixQuality >= minimalFixQuality_) {
    return true;
  } else {
    addDiagnostic_(DiagnosticStatus::WARN, "Fix quality is too low.");
    return false;
  }
}

//-----------------------------------------------------------------------------
void CheckupGGAFix::declareReportInfos_()
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
const DiagnosticReport & CheckupGGAFix::getReport()const
{
  std::lock_guard<std::mutex> lock(mutex_);
  return report_;
}

//-----------------------------------------------------------------------------
void CheckupGGAFix::reset()
{
  std::lock_guard<std::mutex> lock(mutex_);
  report_.diagnostics.clear();
  declareReportInfos_();
}

//-----------------------------------------------------------------------------
void CheckupGGAFix::addDiagnostic_(const DiagnosticStatus & status, const std::string & message)
{
  report_.diagnostics.push_back(Diagnostic());
  report_.diagnostics.back().status = status;
  report_.diagnostics.back().message = message;
}

}  // namespace romea
