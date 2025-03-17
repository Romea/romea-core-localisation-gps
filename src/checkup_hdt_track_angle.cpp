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
#include "romea_core_localisation_gps/checkup_hdt_track_angle.hpp"

namespace romea
{
namespace core
{
namespace localisation
{

//-----------------------------------------------------------------------------
CheckupHDTTrackAngle::CheckupHDTTrackAngle()
: mutex_(),
  report_()
{
  declare_report_infos_();
}

//-----------------------------------------------------------------------------
DiagnosticStatus CheckupHDTTrackAngle::evaluate(const HDTFrame & hdt_frame)
{
  std::lock_guard<std::mutex> lock(mutex_);
  check_frame_is_complete_(hdt_frame);

  set_report_infos_(hdt_frame);
  return report_.diagnostics.front().status;
}

//-----------------------------------------------------------------------------
const DiagnosticReport & CheckupHDTTrackAngle::get_report() const
{
  std::lock_guard<std::mutex> lock(mutex_);
  return report_;
}

//-----------------------------------------------------------------------------
bool CheckupHDTTrackAngle::check_frame_is_complete_(const HDTFrame & hdt_frame)
{
  if (hdt_frame.heading) {
    set_diagnostic_(DiagnosticStatus::OK, "HDT track angle OK.");
    return true;
  } else {
    set_diagnostic_(DiagnosticStatus::ERROR, "HDT track angle is incomplete.");
    return false;
  }
}

//-----------------------------------------------------------------------------
void CheckupHDTTrackAngle::set_report_infos_(const HDTFrame & hdt_frame)
{
  setReportInfo(report_, "talker", hdt_frame.talkerId);
  setReportInfo(report_, "track_angle", hdt_frame.heading);
}

//-----------------------------------------------------------------------------
void CheckupHDTTrackAngle::set_diagnostic_(
  const DiagnosticStatus & status,
  const std::string & message)
{
  report_.diagnostics.clear();
  report_.diagnostics.push_back({status, message});
}

//-----------------------------------------------------------------------------
void CheckupHDTTrackAngle::declare_report_infos_()
{
  setReportInfo(report_, "talker", "");
  setReportInfo(report_, "track_angle", "");
}

//-----------------------------------------------------------------------------
void CheckupHDTTrackAngle::reset()
{
  std::lock_guard<std::mutex> lock(mutex_);
  report_.diagnostics.clear();
  declare_report_infos_();
}

}  // namespace localisation
}  // namespace core
}  // namespace romea
