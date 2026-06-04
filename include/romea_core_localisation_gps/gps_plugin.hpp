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

#ifndef ROMEA_CORE_LOCALISATION_GPS__GPS_PLUGIN_HPP_
#define ROMEA_CORE_LOCALISATION_GPS__GPS_PLUGIN_HPP_

// std
#include <memory>
#include <string>

// romea core
#include "romea_core_common/diagnostic/CheckupRate.hpp"
#include "romea_core_common/geodesy/ENUConverter.hpp"
#include "romea_core_gps/GPSReceiver.hpp"
#include "romea_core_localisation/observation_course.hpp"
#include "romea_core_localisation/observation_position.hpp"
// local
#include "checkup_gga_fix.hpp"
#include "checkup_hdt_track_angle.hpp"
#include "checkup_rmc_track_angle.hpp"

namespace romea
{
namespace core
{
namespace localisation
{

class GPSPluginBase
{
public:
  GPSPluginBase(std::unique_ptr<GPSReceiver> gps, const FixQuality & minimalFixQuality);

  virtual ~GPSPluginBase() = default;

  void set_anchor(const GeodeticCoordinates & wgs84_anchor);

  bool process_gga(
    const Duration & stamp, const std::string & gga_sentence, ObservationPosition & position_obs);

  void process_gsv(const std::string & gsvSentence);

  const ENUConverter & get_enu_converter() const;

  DiagnosticReport make_diagnostic_report(const Duration & stamp);

protected:
  virtual void check_heart_beats_(const Duration & stamp) = 0;
  virtual DiagnosticReport make_diagnostic_report_() = 0;

protected:
  std::unique_ptr<GPSReceiver> gps_;
  ENUConverter enu_converter_;

  CheckupGreaterThanRate gga_rate_diagnostic_;
  CheckupGGAFix gga_fix_diagnostic_;
};

class SingleAntennaGPSPlugin : public GPSPluginBase
{
public:
  SingleAntennaGPSPlugin(
    std::unique_ptr<GPSReceiver> gps,
    const FixQuality & minimal_fix_quality,
    const double & minimal_speed_over_ground);

  void process_linear_speed(const Duration & stamp, const double & linear_speed);

  bool process_rmc(
    const Duration & stamp, const std::string & rmc_sentence, ObservationCourse & course_obs);

private:
  void check_heart_beats_(const Duration & stamp) override;
  DiagnosticReport make_diagnostic_report_() override;

private:
  std::atomic<double> linear_speed_;
  CheckupGreaterThanRate linear_speed_rate_diagnostic_;

  CheckupGreaterThanRate rmc_rate_diagnostic_;
  CheckupRMCTrackAngle rmc_track_angle_diagnostic_;
};

class DualAntennaGPSPlugin : public GPSPluginBase
{
public:
  DualAntennaGPSPlugin(std::unique_ptr<GPSReceiver> gps, const FixQuality & minimalFixQuality);

  bool process_hdt(
    const Duration & stamp, const std::string & hdt_sentence, ObservationCourse & course_obs);

private:
  void check_heart_beats_(const Duration & stamp) override;
  DiagnosticReport make_diagnostic_report_() override;

private:
  CheckupGreaterThanRate hdt_rate_diagnostic_;
  CheckupHDTTrackAngle hdt_track_angle_diagnostic_;
};

}  // namespace localisation
}  // namespace core
}  // namespace romea

#endif  // ROMEA_CORE_LOCALISATION_GPS__GPS_PLUGIN_HPP_
