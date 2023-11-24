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


#ifndef ROMEA_CORE_LOCALISATION_GPS__LOCALISATIONGPSPLUGIN_HPP_
#define ROMEA_CORE_LOCALISATION_GPS__LOCALISATIONGPSPLUGIN_HPP_


// std
#include <memory>
#include <string>

// romea core
#include "romea_core_gps/GPSReceiver.hpp"
#include "romea_core_common/geodesy/ENUConverter.hpp"
#include "romea_core_common/diagnostic/CheckupRate.hpp"
#include "romea_core_localisation/ObservationPosition.hpp"
#include "romea_core_localisation/ObservationCourse.hpp"

// local
#include "CheckupGGAFix.hpp"
#include "CheckupHDTTrackAngle.hpp"
#include "CheckupRMCTrackAngle.hpp"

namespace romea
{
namespace core
{

class LocalisationGPSPluginBase
{
public:
  LocalisationGPSPluginBase(
    std::unique_ptr<GPSReceiver> gps,
    const FixQuality & minimalFixQuality);

  virtual ~LocalisationGPSPluginBase() = default;

  void setAnchor(const GeodeticCoordinates & wgs84_anchor);

  bool processGGA(
    const Duration & stamp,
    const std::string & ggaSentence,
    ObservationPosition & positionObs);

  void processGSV(const std::string & gsvSentence);

  const ENUConverter & getENUConverter()const;

  DiagnosticReport makeDiagnosticReport(const Duration & stamp);

protected:
  virtual void checkHearBeats_(const Duration & stamp) = 0;
  virtual DiagnosticReport makeDiagnosticReport_() = 0;

protected:
  std::unique_ptr<GPSReceiver> gps_;
  ENUConverter enuConverter_;

  CheckupGreaterThanRate ggaRateDiagnostic_;
  CheckupGGAFix ggaFixDiagnostic_;
};


class LocalisationSingleAntennaGPSPlugin : public LocalisationGPSPluginBase
{
public:
  LocalisationSingleAntennaGPSPlugin(
    std::unique_ptr<GPSReceiver> gps,
    const FixQuality & minimalFixQuality,
    const double & minimalSpeedOverGround);

  void processLinearSpeed(
    const Duration & stamp,
    const double & linearSpeed);

  bool processRMC(
    const Duration & stamp,
    const std::string & rmcSentence,
    ObservationCourse & courseObs);

private:
  void checkHearBeats_(const Duration & stamp) override;
  DiagnosticReport makeDiagnosticReport_() override;

private:
  std::atomic<double> linearSpeed_;
  CheckupGreaterThanRate linearSpeedRateDiagnostic_;

  CheckupGreaterThanRate rmcRateDiagnostic_;
  CheckupRMCTrackAngle rmcTrackAngleDiagnostic_;
};

class LocalisationDualAntennaGPSPlugin : public LocalisationGPSPluginBase
{
public:
  LocalisationDualAntennaGPSPlugin(
    std::unique_ptr<GPSReceiver> gps,
    const FixQuality & minimalFixQuality);


  bool processHDT(
    const Duration & stamp,
    const std::string & hdtSentence,
    ObservationCourse & courseObs);

private:
  void checkHearBeats_(const Duration & stamp) override;
  DiagnosticReport makeDiagnosticReport_() override;

private:
  CheckupGreaterThanRate hdtRateDiagnostic_;
  CheckupHDTTrackAngle hdtTrackAngleDiagnostic_;
};

}  // namespace core
}  // namespace romea

#endif  // ROMEA_CORE_LOCALISATION_GPS__LOCALISATIONGPSPLUGIN_HPP_
