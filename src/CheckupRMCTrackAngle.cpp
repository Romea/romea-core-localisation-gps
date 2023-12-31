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
#include <sstream>
#include <string>

// local
#include "romea_core_localisation_gps/CheckupRMCTrackAngle.hpp"

namespace romea
{
namespace core
{

//-----------------------------------------------------------------------------
CheckupRMCTrackAngle::CheckupRMCTrackAngle(const double & minimalSpeedOverGround)
: minimalSpeedOverGround_(minimalSpeedOverGround),
  mutex_(),
  report_()
{
  declareReportInfos_();
}


//-----------------------------------------------------------------------------
DiagnosticStatus CheckupRMCTrackAngle::evaluate(const RMCFrame & rmcFrame)
{
  std::lock_guard<std::mutex> lock(mutex_);
  if (checkFrameIsComplete_(rmcFrame)) {
    checkFixIsReliable_(rmcFrame);
  }
  setReportInfos_(rmcFrame);
  return report_.diagnostics.front().status;
}

//-----------------------------------------------------------------------------
void CheckupRMCTrackAngle::checkFixIsReliable_(const RMCFrame & rmcFrame)
{
  if (*rmcFrame.speedOverGroundInMeterPerSecond < minimalSpeedOverGround_) {
    std::stringstream msg;
    msg << "RMC track angle is not reliable ";
    msg << "because vehicle speed is lower than ";
    msg << minimalSpeedOverGround_ << " m/s.";
    setDiagnostic_(DiagnosticStatus::WARN, msg.str());
  } else {
    setDiagnostic_(DiagnosticStatus::OK, "RMC track angle OK.");
  }
}

//-----------------------------------------------------------------------------
const DiagnosticReport & CheckupRMCTrackAngle::getReport() const
{
  std::lock_guard<std::mutex> lock(mutex_);
  return report_;
}

//-----------------------------------------------------------------------------
bool CheckupRMCTrackAngle::checkFrameIsComplete_(const RMCFrame & rmcFrame)
{
  if (rmcFrame.speedOverGroundInMeterPerSecond &&
    rmcFrame.trackAngleTrue)
  {
    return true;
  } else {
    setDiagnostic_(DiagnosticStatus::ERROR, "RMC track angle is incomplete.");
    return false;
  }
}

//-----------------------------------------------------------------------------
void CheckupRMCTrackAngle::setReportInfos_(const RMCFrame & rmcFrame)
{
  setReportInfo(report_, "talker", rmcFrame.talkerId);
  setReportInfo(report_, "speed_over_ground", rmcFrame.speedOverGroundInMeterPerSecond);
  setReportInfo(report_, "track_angle", rmcFrame.trackAngleTrue);
  setReportInfo(report_, "magnetic_deviation", rmcFrame.magneticDeviation);
}

//-----------------------------------------------------------------------------
void CheckupRMCTrackAngle::setDiagnostic_(
  const DiagnosticStatus & status,
  const std::string & message)
{
  report_.diagnostics.clear();
  report_.diagnostics.push_back({status, message});
}

//-----------------------------------------------------------------------------
void CheckupRMCTrackAngle::declareReportInfos_()
{
  setReportInfo(report_, "talker", "");
  setReportInfo(report_, "speed_over_ground", "");
  setReportInfo(report_, "track_angle", "");
  setReportInfo(report_, "magnetic_deviation", "");
}

//-----------------------------------------------------------------------------
void CheckupRMCTrackAngle::reset()
{
  std::lock_guard<std::mutex> lock(mutex_);
  report_.diagnostics.clear();
  declareReportInfos_();
}

}  // namespace core
}  // namespace romea
