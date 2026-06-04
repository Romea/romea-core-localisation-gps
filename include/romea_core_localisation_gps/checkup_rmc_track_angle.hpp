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

#ifndef ROMEA_CORE_LOCALISATION_GPS__CHECKUP_RMC_TRACK_ANGLE_HPP_
#define ROMEA_CORE_LOCALISATION_GPS__CHECKUP_RMC_TRACK_ANGLE_HPP_

// std
#include <mutex>
#include <string>

// romea
#include "romea_core_common/diagnostic/DiagnosticReport.hpp"
#include "romea_core_gps/nmea/RMCFrame.hpp"

namespace romea
{
namespace core
{
namespace localisation
{

class CheckupRMCTrackAngle
{
public:
  explicit CheckupRMCTrackAngle(const double & minimalSpeedOverGround);

  DiagnosticStatus evaluate(const RMCFrame & rmc_frame);

  const DiagnosticReport & get_report() const;

  void reset();

private:
  bool check_frame_is_complete_(const RMCFrame & rmc_frame);
  void check_fix_is_reliable_(const RMCFrame & rmc_frame);

  void declare_report_infos_();
  void set_report_infos_(const RMCFrame & rmcFrame);
  void set_diagnostic_(const DiagnosticStatus & status, const std::string & message);

private:
  double minimal_speed_over_ground_;

  mutable std::mutex mutex_;
  DiagnosticReport report_;
};

}  // namespace localisation
}  // namespace core
}  // namespace romea

#endif  // ROMEA_CORE_LOCALISATION_GPS__CHECKUP_RMC_TRACK_ANGLE_HPP_
