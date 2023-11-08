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
#include "romea_core_localisation_gps/CheckupHDTTrackAngle.hpp"

namespace romea
{

//-----------------------------------------------------------------------------
CheckupHDTTrackAngle::CheckupHDTTrackAngle()
: mutex_(),
  report_()
{
  declareReportInfos_();
}

//-----------------------------------------------------------------------------
DiagnosticStatus CheckupHDTTrackAngle::evaluate(const HDTFrame & hdtFrame)
{
  std::lock_guard<std::mutex> lock(mutex_);
  checkFrameIsComplete_(hdtFrame);

  setReportInfos_(hdtFrame);
  return report_.diagnostics.front().status;
}

//-----------------------------------------------------------------------------
const DiagnosticReport & CheckupHDTTrackAngle::getReport() const
{
  std::lock_guard<std::mutex> lock(mutex_);
  return report_;
}

//-----------------------------------------------------------------------------
bool CheckupHDTTrackAngle::checkFrameIsComplete_(const HDTFrame & hdtFrame)
{
  if (hdtFrame.heading) {
    setDiagnostic_(DiagnosticStatus::OK, "HDT track angle OK.");
    return true;
  } else {
    setDiagnostic_(DiagnosticStatus::ERROR, "HDT track angle is incomplete.");
    return false;
  }
}

//-----------------------------------------------------------------------------
void CheckupHDTTrackAngle::setReportInfos_(const HDTFrame & hdtFrame)
{
  setReportInfo(report_, "talker", hdtFrame.talkerId);
  setReportInfo(report_, "track_angle", hdtFrame.heading);
}

//-----------------------------------------------------------------------------
void CheckupHDTTrackAngle::setDiagnostic_(
  const DiagnosticStatus & status,
  const std::string & message)
{
  report_.diagnostics.clear();
  report_.diagnostics.push_back({status, message});
}

//-----------------------------------------------------------------------------
void CheckupHDTTrackAngle::declareReportInfos_()
{
  setReportInfo(report_, "talker", "");
  setReportInfo(report_, "track_angle", "");
}

//-----------------------------------------------------------------------------
void CheckupHDTTrackAngle::reset()
{
  std::lock_guard<std::mutex> lock(mutex_);
  report_.diagnostics.clear();
  declareReportInfos_();
}

}  // namespace romea
