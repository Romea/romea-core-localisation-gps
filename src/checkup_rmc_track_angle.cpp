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
#include "romea_core_localisation_gps/checkup_rmc_track_angle.hpp"

namespace romea
{
namespace core
{
namespace localisation
{

//-----------------------------------------------------------------------------
CheckupRMCTrackAngle::CheckupRMCTrackAngle(const double & minimal_speed_over_ground)
: minimal_speed_over_ground_(minimal_speed_over_ground), mutex_(), report_()
{
  declare_report_infos_();
}

//-----------------------------------------------------------------------------
DiagnosticStatus CheckupRMCTrackAngle::evaluate(const RMCFrame & rmc_frame)
{
  std::lock_guard<std::mutex> lock(mutex_);
  if (check_frame_is_complete_(rmc_frame)) {
    check_fix_is_reliable_(rmc_frame);
  }
  set_report_infos_(rmc_frame);
  return report_.diagnostics.front().status;
}

//-----------------------------------------------------------------------------
void CheckupRMCTrackAngle::check_fix_is_reliable_(const RMCFrame & rmc_frame)
{
  if (*rmc_frame.speedOverGroundInMeterPerSecond < minimal_speed_over_ground_) {
    std::stringstream msg;
    msg << "RMC track angle is not reliable ";
    msg << "because vehicle speed is lower than ";
    msg << minimal_speed_over_ground_ << " m/s.";
    set_diagnostic_(DiagnosticStatus::WARN, msg.str());
  } else {
    set_diagnostic_(DiagnosticStatus::OK, "RMC track angle OK.");
  }
}

//-----------------------------------------------------------------------------
const DiagnosticReport & CheckupRMCTrackAngle::get_report() const
{
  std::lock_guard<std::mutex> lock(mutex_);
  return report_;
}

//-----------------------------------------------------------------------------
bool CheckupRMCTrackAngle::check_frame_is_complete_(const RMCFrame & rmc_frame)
{
  if (rmc_frame.speedOverGroundInMeterPerSecond && rmc_frame.trackAngleTrue) {
    return true;
  } else {
    set_diagnostic_(DiagnosticStatus::ERROR, "RMC track angle is incomplete.");
    return false;
  }
}

//-----------------------------------------------------------------------------
void CheckupRMCTrackAngle::set_report_infos_(const RMCFrame & rmc_frame)
{
  setReportInfo(report_, "talker", rmc_frame.talkerId);
  setReportInfo(report_, "speed_over_ground", rmc_frame.speedOverGroundInMeterPerSecond);
  setReportInfo(report_, "track_angle", rmc_frame.trackAngleTrue);
  setReportInfo(report_, "magnetic_deviation", rmc_frame.magneticDeviation);
}

//-----------------------------------------------------------------------------
void CheckupRMCTrackAngle::set_diagnostic_(
  const DiagnosticStatus & status, const std::string & message)
{
  report_.diagnostics.clear();
  report_.diagnostics.push_back({status, message});
}

//-----------------------------------------------------------------------------
void CheckupRMCTrackAngle::declare_report_infos_()
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
  declare_report_infos_();
}

}  // namespace localisation
}  // namespace core
}  // namespace romea
