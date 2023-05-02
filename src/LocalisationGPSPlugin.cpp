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

//-----------------------------------------------------------------------------
LocalisationGPSPlugin::LocalisationGPSPlugin(
  std::unique_ptr<GPSReceiver> gps,
  const FixQuality & minimalFixQuality,
  const double & minimalSpeedOverGround)
: gps_(std::move(gps)),
  enuConverter_(),
  linearSpeed_(std::numeric_limits<double>::quiet_NaN()),
  ggaRateDiagnostic_("gga", 1.0, 0.1),
  rmcRateDiagnostic_("rmc", 1.0, 0.1),
  linearSpeedRateDiagnostic_("linear_speed", 10.0, 0.1),
  ggaFixDiagnostic_(minimalFixQuality),
  rmcTrackAngleDiagnostic_(minimalSpeedOverGround)
{
}

//-----------------------------------------------------------------------------
void LocalisationGPSPlugin::setAnchor(const GeodeticCoordinates & wgs84_anchor)
{
  enuConverter_.setAnchor(wgs84_anchor);
}

//-----------------------------------------------------------------------------
const ENUConverter & LocalisationGPSPlugin::getENUConverter()const
{
  return enuConverter_;
}

//-----------------------------------------------------------------------------
void LocalisationGPSPlugin::processLinearSpeed(
  const Duration & stamp,
  const double & linearSpeed)
{
  linearSpeed_.store(linearSpeed);
  linearSpeedRateDiagnostic_.evaluate(stamp);
}

//-----------------------------------------------------------------------------
bool LocalisationGPSPlugin::processGGA(
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
bool LocalisationGPSPlugin::processRMC(
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
void LocalisationGPSPlugin::processGSV(const std::string & gsvSentence)
{
  if (gps_->updateSatellitesViews(gsvSentence)) {
    //    diagnostics_.updateConstellationReliability(gps_->getReliability());
  }
}

//-----------------------------------------------------------------------------
void LocalisationGPSPlugin::checkHearBeats_(const Duration & stamp)
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
DiagnosticReport LocalisationGPSPlugin::makeDiagnosticReport_()
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
DiagnosticReport LocalisationGPSPlugin::makeDiagnosticReport(const Duration & stamp)
{
  checkHearBeats_(stamp);
  return makeDiagnosticReport_();
}

}  // namespace romea
