#ifndef ROMEA_CORE_LOCALISATION_GPS_LOCALISATIONGPSPLUGIN_HPP_
#define ROMEA_CORE_LOCALISATION_GPS_LOCALISATIONGPSPLUGIN_HPP_

// std
#include <memory>
#include <string>

// romea
#include <romea_core_gps/GPSReceiver.hpp>
#include <romea_core_common/geodesy/ENUConverter.hpp>
#include <romea_core_common/diagnostic/CheckupRate.hpp>
#include <romea_core_localisation/ObservationPosition.hpp>
#include <romea_core_localisation/ObservationCourse.hpp>
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

#endif  // ROMEA_CORE_LOCALISATION_GPS_LOCALISATIONGPSPLUGIN_HPP_
