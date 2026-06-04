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
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <utility>

// local
#include "romea_core_localisation_gps/gps_plugin.hpp"

namespace
{
const double DEFAULT_COURSE_ANGLE_STD = 20 / 180. * M_PI;
}

namespace romea
{
namespace core
{
namespace localisation
{

//-----------------------------------------------------------------------------
GPSPluginBase::GPSPluginBase(std::unique_ptr<GPSReceiver> gps, const FixQuality & minimalFixQuality)
: gps_(std::move(gps)),
  enu_converter_(),
  gga_rate_diagnostic_("gga", 1.0, 0.1),
  gga_fix_diagnostic_(minimalFixQuality)
{
}

//-----------------------------------------------------------------------------
void GPSPluginBase::set_anchor(const GeodeticCoordinates & wgs84_anchor)
{
  enu_converter_.setAnchor(wgs84_anchor);
}

//-----------------------------------------------------------------------------
const ENUConverter & GPSPluginBase::get_enu_converter() const
{
  return enu_converter_;
}

//-----------------------------------------------------------------------------
bool GPSPluginBase::process_gga(
  const Duration & stamp, const std::string & gga_sentence, ObservationPosition & position_obs)
{
  GGAFrame gga_frame(gga_sentence);
  if (
    gga_rate_diagnostic_.evaluate(stamp) == DiagnosticStatus::OK &&
    gga_fix_diagnostic_.evaluate(gga_frame) == DiagnosticStatus::OK) {
    auto geodeticCoordinates = makeGeodeticCoordinates(
      (*gga_frame.latitude).toDouble(),
      (*gga_frame.longitude).toDouble(),
      (*gga_frame.altitudeAboveGeoid + *gga_frame.geoidHeight));

    Eigen::Vector3d position = enu_converter_.toENU(geodeticCoordinates);
    double fixStd = *gga_frame.horizontalDilutionOfPrecision * gps_->getUERE(*gga_frame.fixQuality);
    position_obs.Y(ObservationPosition::POSITION_X) = position.x();
    position_obs.Y(ObservationPosition::POSITION_Y) = position.y();
    position_obs.R() = Eigen::Matrix2d::Identity() * fixStd * fixStd;
    position_obs.lever_arm = gps_->getAntennaBodyPosition();
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
void GPSPluginBase::process_gsv(const std::string & gsvSentence)
{
  if (gps_->updateSatellitesViews(gsvSentence)) {
    //    diagnostics_.updateConstellationReliability(gps_->getReliability());
  }
}

//-----------------------------------------------------------------------------
DiagnosticReport GPSPluginBase::make_diagnostic_report(const Duration & stamp)
{
  check_heart_beats_(stamp);
  return make_diagnostic_report_();
}

//-----------------------------------------------------------------------------
SingleAntennaGPSPlugin::SingleAntennaGPSPlugin(
  std::unique_ptr<GPSReceiver> gps,
  const FixQuality & minimal_fix_quality,
  const double & minimal_speed_over_ground)
: GPSPluginBase(std::move(gps), minimal_fix_quality),
  linear_speed_(std::numeric_limits<double>::quiet_NaN()),
  linear_speed_rate_diagnostic_("linear_speed", 10.0, 0.1),
  rmc_rate_diagnostic_("rmc", 1.0, 0.1),
  rmc_track_angle_diagnostic_(minimal_speed_over_ground)
{
}

//-----------------------------------------------------------------------------
void SingleAntennaGPSPlugin::process_linear_speed(
  const Duration & stamp, const double & linear_speed)
{
  linear_speed_.store(linear_speed);
  linear_speed_rate_diagnostic_.evaluate(stamp);
}

//-----------------------------------------------------------------------------
bool SingleAntennaGPSPlugin::process_rmc(
  const Duration & stamp, const std::string & rmc_sentence, ObservationCourse & course_obs)
{
  RMCFrame rmc_frame(rmc_sentence);
  if (
    rmc_rate_diagnostic_.evaluate(stamp) == DiagnosticStatus::OK &&
    rmc_track_angle_diagnostic_.evaluate(rmc_frame) == DiagnosticStatus::OK &&
    std::isfinite(linear_speed_)) {
    course_obs.Y() = trackAngleToCourseAngle(*rmc_frame.trackAngleTrue, linear_speed_);
    course_obs.R() = DEFAULT_COURSE_ANGLE_STD * DEFAULT_COURSE_ANGLE_STD;
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
void SingleAntennaGPSPlugin::check_heart_beats_(const Duration & stamp)
{
  if (!linear_speed_rate_diagnostic_.heartBeatCallback(stamp)) {
    linear_speed_ = std::numeric_limits<double>::quiet_NaN();
  }

  if (!gga_rate_diagnostic_.heartBeatCallback(stamp)) {
    gga_fix_diagnostic_.reset();
  }

  if (!rmc_rate_diagnostic_.heartBeatCallback(stamp)) {
    rmc_track_angle_diagnostic_.reset();
  }
}

//-----------------------------------------------------------------------------
DiagnosticReport SingleAntennaGPSPlugin::make_diagnostic_report_()
{
  DiagnosticReport report;
  report += linear_speed_rate_diagnostic_.getReport();
  report += gga_rate_diagnostic_.getReport();
  report += gga_fix_diagnostic_.get_report();
  report += rmc_rate_diagnostic_.getReport();
  report += rmc_track_angle_diagnostic_.get_report();
  return report;
}

//-----------------------------------------------------------------------------
DualAntennaGPSPlugin::DualAntennaGPSPlugin(
  std::unique_ptr<GPSReceiver> gps, const FixQuality & minimal_fix_quality)
: GPSPluginBase(std::move(gps), minimal_fix_quality),
  hdt_rate_diagnostic_("hdt", 1.0, 0.1),
  hdt_track_angle_diagnostic_()
{
}

//-----------------------------------------------------------------------------
bool DualAntennaGPSPlugin::process_hdt(
  const Duration & stamp, const std::string & hdt_sentence, ObservationCourse & course_obs)
{
  HDTFrame hdtFrame(hdt_sentence);
  if (
    hdt_rate_diagnostic_.evaluate(stamp) == DiagnosticStatus::OK &&
    hdt_track_angle_diagnostic_.evaluate(hdtFrame) == DiagnosticStatus::OK) {
    course_obs.Y() = headingToCourseAngle(*hdtFrame.heading);
    course_obs.R() = DEFAULT_COURSE_ANGLE_STD * DEFAULT_COURSE_ANGLE_STD;
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
void DualAntennaGPSPlugin::check_heart_beats_(const Duration & stamp)
{
  if (!gga_rate_diagnostic_.heartBeatCallback(stamp)) {
    gga_fix_diagnostic_.reset();
  }

  if (!hdt_rate_diagnostic_.heartBeatCallback(stamp)) {
    hdt_track_angle_diagnostic_.reset();
  }
}

//-----------------------------------------------------------------------------
DiagnosticReport DualAntennaGPSPlugin::make_diagnostic_report_()
{
  DiagnosticReport report;
  report += gga_rate_diagnostic_.getReport();
  report += gga_fix_diagnostic_.get_report();
  report += hdt_rate_diagnostic_.getReport();
  report += hdt_track_angle_diagnostic_.get_report();
  return report;
}

}  // namespace localisation
}  // namespace core
}  // namespace romea
