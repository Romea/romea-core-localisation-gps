#ifndef __GPSLocalisationPlugin2_HPP__
#define __GPSLocalisationPlugin2_HPP__

//std
#include <memory>

//romea
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

  DiagnosticReport makeGGADiagnosticsReport_(const Duration & stamp);

  DiagnosticReport makeRMCDiagnosticsReport_(const Duration & stamp);

  DiagnosticReport makeLinearSpeedDiagnosticReport_(const Duration & stamp);

private:

  std::unique_ptr<GPSReceiver> gps_;
  ENUConverter enuConverter_;
  std::atomic<double> linearSpeed_;

  CheckupRate ggaRateDiagnostic_;
  CheckupRate rmcRateDiagnostic_;
  CheckupRate linearSpeedRateDiagnostic_;

  CheckupGGAFix ggaFixDiagnostic_;
  CheckupRMCTrackAngle rmcTrackAngleDiagnostic_;
};

}

#endif
