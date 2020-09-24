#ifndef __GPSLocalisationPlugin2_HPP__
#define __GPSLocalisationPlugin2_HPP__

//std
#include <memory>

//romea
#include <romea_gps/GPSReceiver.hpp>
#include <romea_common/geodesy/ENUConverter.hpp>
#include <romea_common/diagnostic/CheckupRate.hpp>
#include <romea_localisation/ObservationPosition.hpp>
#include <romea_localisation/ObservationCourse.hpp>
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

  bool processGGA(const Duration & stamp,
                  const std::string & ggaSentence,
                  ObservationPosition & positionObs);

  bool processRMC(const Duration & stamp,
                  const std::string & rmcSentence,
                  const double & linearSpeed,
                  ObservationCourse & courseObs);

  void processGSV(const std::string & gsvSentence);

  const ENUConverter & getENUConverter()const;

  DiagnosticReport makeDiagnosticReport();

protected:

  std::unique_ptr<GPSReceiver> gps_;
  ENUConverter enuConverter_;

  CheckupRate ggaRateDiagnostic_;
  CheckupRate rmcRateDiagnostic_;

  CheckupGGAFix ggaFixDiagnostic_;
  CheckupRMCTrackAngle rmcTrackAngleDiagnostic_;
};

}

#endif
