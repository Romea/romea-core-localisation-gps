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

#ifndef ROMEA_CORE_LOCALISATION_GPS__CHECKUP_GGA_FIX_HPP_
#define ROMEA_CORE_LOCALISATION_GPS__CHECKUP_GGA_FIX_HPP_

// std
#include <list>
#include <mutex>
#include <string>

// romea
#include "romea_core_common/diagnostic/DiagnosticReport.hpp"
#include "romea_core_gps/nmea/GGAFrame.hpp"

namespace romea
{
namespace core
{
namespace localisation
{

class CheckupGGAFix
{
public:
  explicit CheckupGGAFix(const FixQuality & minimal_fix_quality);

  DiagnosticStatus evaluate(const GGAFrame & gga_frame);

  const DiagnosticReport & get_report() const;

  void reset();

private:
  void declare_report_infos_();
  void set_report_infos_(const GGAFrame & gga_frame);
  void add_diagnostic_(const DiagnosticStatus & status, const std::string & message);

  bool check_frame_is_complete_(const GGAFrame & gga_frame);
  void check_fix_is_reliable_(const GGAFrame & gga_frame);
  bool check_fix_quality_(const GGAFrame & gga_frame);
  bool check_horizontal_dilution_of_precision_(const GGAFrame & gga_frame);
  bool check_number_satellites_used_to_compute_fix_(const GGAFrame & gga_frame);

private:
  FixQuality minimal_fix_quality_;
  double maximalHorizontal_dilution_of_precision_;

  mutable std::mutex mutex_;
  DiagnosticReport report_;
};

}  // namespace localisation
}  // namespace core
}  // namespace romea

#endif  // ROMEA_CORE_LOCALISATION_GPS__CHECKUP_GGA_FIX_HPP_
