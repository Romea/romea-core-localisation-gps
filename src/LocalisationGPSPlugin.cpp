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
#include <utility>
#include <string>
#include <limits>

// local
#include "romea_core_localisation_gps/LocalisationGPSPlugin.hpp"


namespace
{
const double DEFAULT_COURSE_ANGLE_STD = 20 / 180. * M_PI;
}


namespace romea
{
namespace core
{


//-----------------------------------------------------------------------------
LocalisationGPSPluginBase::LocalisationGPSPluginBase(
  std::unique_ptr<GPSReceiver> gps,
  const FixQuality & minimalFixQuality)
: gps_(std::move(gps)),
  enuConverter_(),
  ggaRateDiagnostic_("gga", 1.0, 0.1),
  ggaFixDiagnostic_(minimalFixQuality)
{
}

//-----------------------------------------------------------------------------
void LocalisationGPSPluginBase::setAnchor(const GeodeticCoordinates & wgs84_anchor)
{
  enuConverter_.setAnchor(wgs84_anchor);
}

//-----------------------------------------------------------------------------
const ENUConverter & LocalisationGPSPluginBase::getENUConverter()const
{
  return enuConverter_;
}

//-----------------------------------------------------------------------------
bool LocalisationGPSPluginBase::processGGA(
  const Duration & stamp,
  const std::string & ggaSentence,
  ObservationPosition & positionObs)
{
  GGAFrame ggaFrame(ggaSentence);
  if (ggaRateDiagnostic_.evaluate(stamp) == DiagnosticStatus::OK &&
    ggaFixDiagnostic_.evaluate(ggaFrame) == DiagnosticStatus::OK)
  {
    auto geodeticCoordinates = makeGeodeticCoordinates(
      (*ggaFrame.latitude).toDouble(),
      (*ggaFrame.longitude).toDouble(),
      (*ggaFrame.altitudeAboveGeoid +
      *ggaFrame.geoidHeight));

    Eigen::Vector3d position = enuConverter_.toENU(geodeticCoordinates);
    double fixStd = *ggaFrame.horizontalDilutionOfPrecision * gps_->getUERE(*ggaFrame.fixQuality);
    positionObs.Y(ObservationPosition::POSITION_X) = position.x();
    positionObs.Y(ObservationPosition::POSITION_Y) = position.y();
    positionObs.R() = Eigen::Matrix2d::Identity() * fixStd * fixStd;
    positionObs.levelArm = gps_->getAntennaBodyPosition();
    return true;
  }

  return false;
}


//-----------------------------------------------------------------------------
void LocalisationGPSPluginBase::processGSV(const std::string & gsvSentence)
{
  if (gps_->updateSatellitesViews(gsvSentence)) {
    //    diagnostics_.updateConstellationReliability(gps_->getReliability());
  }
}


//-----------------------------------------------------------------------------
DiagnosticReport LocalisationGPSPluginBase::makeDiagnosticReport(const Duration & stamp)
{
  checkHearBeats_(stamp);
  return makeDiagnosticReport_();
}


//-----------------------------------------------------------------------------
LocalisationSingleAntennaGPSPlugin::LocalisationSingleAntennaGPSPlugin(
  std::unique_ptr<GPSReceiver> gps,
  const FixQuality & minimalFixQuality,
  const double & minimalSpeedOverGround)
: LocalisationGPSPluginBase(std::move(gps), minimalFixQuality),
  linearSpeed_(std::numeric_limits<double>::quiet_NaN()),
  linearSpeedRateDiagnostic_("linear_speed", 10.0, 0.1),
  rmcRateDiagnostic_("rmc", 1.0, 0.1),
  rmcTrackAngleDiagnostic_(minimalSpeedOverGround)
{
}

//-----------------------------------------------------------------------------
void LocalisationSingleAntennaGPSPlugin::processLinearSpeed(
  const Duration & stamp,
  const double & linearSpeed)
{
  linearSpeed_.store(linearSpeed);
  linearSpeedRateDiagnostic_.evaluate(stamp);
}


//-----------------------------------------------------------------------------
bool LocalisationSingleAntennaGPSPlugin::processRMC(
  const Duration & stamp,
  const std::string & rmcSentence,
  ObservationCourse & courseObs)
{
  RMCFrame rmcFrame(rmcSentence);
  if (rmcRateDiagnostic_.evaluate(stamp) == DiagnosticStatus::OK &&
    rmcTrackAngleDiagnostic_.evaluate(rmcFrame) == DiagnosticStatus::OK &&
    std::isfinite(linearSpeed_))
  {
    courseObs.Y() = trackAngleToCourseAngle(*rmcFrame.trackAngleTrue, linearSpeed_);
    courseObs.R() = DEFAULT_COURSE_ANGLE_STD * DEFAULT_COURSE_ANGLE_STD;
    return true;
  }

  return false;
}


//-----------------------------------------------------------------------------
void LocalisationSingleAntennaGPSPlugin::checkHearBeats_(const Duration & stamp)
{
  if (!linearSpeedRateDiagnostic_.heartBeatCallback(stamp)) {
    linearSpeed_ = std::numeric_limits<double>::quiet_NaN();
  }

  if (!ggaRateDiagnostic_.heartBeatCallback(stamp)) {
    ggaFixDiagnostic_.reset();
  }

  if (!rmcRateDiagnostic_.heartBeatCallback(stamp)) {
    rmcTrackAngleDiagnostic_.reset();
  }
}

//-----------------------------------------------------------------------------
DiagnosticReport LocalisationSingleAntennaGPSPlugin::makeDiagnosticReport_()
{
  DiagnosticReport report;
  report += linearSpeedRateDiagnostic_.getReport();
  report += ggaRateDiagnostic_.getReport();
  report += ggaFixDiagnostic_.getReport();
  report += rmcRateDiagnostic_.getReport();
  report += rmcTrackAngleDiagnostic_.getReport();
  return report;
}

//-----------------------------------------------------------------------------
LocalisationDualAntennaGPSPlugin::LocalisationDualAntennaGPSPlugin(
  std::unique_ptr<GPSReceiver> gps,
  const FixQuality & minimalFixQuality)
: LocalisationGPSPluginBase(std::move(gps), minimalFixQuality),
  hdtRateDiagnostic_("hdt", 1.0, 0.1),
  hdtTrackAngleDiagnostic_()
{
}

//-----------------------------------------------------------------------------
bool LocalisationDualAntennaGPSPlugin::processHDT(
  const Duration & stamp,
  const std::string & hdtSentence,
  ObservationCourse & courseObs)
{
  HDTFrame hdtFrame(hdtSentence);
  if (hdtRateDiagnostic_.evaluate(stamp) == DiagnosticStatus::OK &&
    hdtTrackAngleDiagnostic_.evaluate(hdtFrame) == DiagnosticStatus::OK)
  {
    courseObs.Y() = headingToCourseAngle(*hdtFrame.heading);
    courseObs.R() = DEFAULT_COURSE_ANGLE_STD * DEFAULT_COURSE_ANGLE_STD;
    return true;
  }

  return false;
}


//-----------------------------------------------------------------------------
void LocalisationDualAntennaGPSPlugin::checkHearBeats_(const Duration & stamp)
{
  if (!ggaRateDiagnostic_.heartBeatCallback(stamp)) {
    ggaFixDiagnostic_.reset();
  }

  if (!hdtRateDiagnostic_.heartBeatCallback(stamp)) {
    hdtTrackAngleDiagnostic_.reset();
  }
}

//-----------------------------------------------------------------------------
DiagnosticReport LocalisationDualAntennaGPSPlugin::makeDiagnosticReport_()
{
  DiagnosticReport report;
  report += ggaRateDiagnostic_.getReport();
  report += ggaFixDiagnostic_.getReport();
  report += hdtRateDiagnostic_.getReport();
  report += hdtTrackAngleDiagnostic_.getReport();
  return report;
}

}  // namespace core
}  // namespace romea
