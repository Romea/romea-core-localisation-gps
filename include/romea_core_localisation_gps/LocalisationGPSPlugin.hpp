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


// romea core
#include <romea_core_gps/GPSReceiver.hpp>
#include <romea_core_common/geodesy/ENUConverter.hpp>
#include <romea_core_common/diagnostic/CheckupRate.hpp>
#include <romea_core_localisation/ObservationPosition.hpp>
#include <romea_core_localisation/ObservationCourse.hpp>

// std
#include <memory>
#include <string>

// local
#include "CheckupRMCTrackAngle.hpp"
#include "CheckupGGAFix.hpp"

namespace romea {


class LocalisationGPSPlugin
{
public :

  LocalisationGPSPlugin(std::unique_ptr<GPSReceiver> gps,
                        const FixQuality & minimalFixQuality,
                        const double & minimalSpeedOverGround);

  void setAnchor(const GeodeticCoordinates & wgs84_anchor);

  void processLinearSpeed(const Duration & stamp,
                          const double & linearSpeed);

  bool processGGA(const Duration & stamp,
                  const std::string & ggaSentence,
                  ObservationPosition & positionObs);

  bool processRMC(const Duration & stamp,
                  const std::string & rmcSentence,
                  ObservationCourse & courseObs);

  void processGSV(const std::string & gsvSentence);

  const ENUConverter & getENUConverter()const;

  DiagnosticReport makeDiagnosticReport(const Duration & stamp);

private :

  void checkHearBeats_(const Duration & stamp);
  DiagnosticReport makeDiagnosticReport_();

private:
  std::unique_ptr<GPSReceiver> gps_;
  ENUConverter enuConverter_;
  std::atomic<double> linearSpeed_;

  CheckupGreaterThanRate ggaRateDiagnostic_;
  CheckupGreaterThanRate rmcRateDiagnostic_;
  CheckupGreaterThanRate linearSpeedRateDiagnostic_;

  CheckupGGAFix ggaFixDiagnostic_;
  CheckupRMCTrackAngle rmcTrackAngleDiagnostic_;
};

}  // namespace romea

#endif  // ROMEA_CORE_LOCALISATION_GPS__LOCALISATIONGPSPLUGIN_HPP_
