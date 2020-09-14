#ifndef __GPSLocalisationPlugin2_HPP__
#define __GPSLocalisationPlugin2_HPP__

#include <memory>
#include <romea_gps/GPSReceiver.hpp>
#include <romea_common/geodesy/ENUConverter.hpp>
#include <romea_common/diagnostic/DiagnosticRate.hpp>
#include <romea_localisation/ObservationPosition.hpp>
#include <romea_localisation/ObservationCourse.hpp>
#include "gga_diagnostic2.hpp"
#include "rmc_diagnostic2.hpp"

namespace romea {


class GPSLocalisationPlugin
{

public :

  GPSLocalisationPlugin(std::unique_ptr<GPSReceiver> gps,
                        const FixQuality & minimalFixQuality,
                        const double & minimalSpeedOverGround);

  bool processGGA(const Duration & stamp,
                  const std::string & ggaSentence,
                  ObservationPosition & positionObs);

  bool processRMC(const Duration & stamp,
                  const std::string & rmcSentence,
                  const double & linearSpeed,
                  ObservationCourse & courseObs);

  void processGSV(const std::string & gsvSentence);

  void setAnchor(const GeodeticCoordinates & wgs84_anchor);

protected:

  std::unique_ptr<GPSReceiver> gps_;
  ENUConverter enuConverter_;

  DiagnosticRate gga_rate_diagnostic_;
  DiagnosticRate rmc_rate_diagnostic_;

  DiagnosticGGAFix2 gga_fix_diagnostic_;
  DiagnosticRMCTrackAngle2 rmc_track_angle_diagnostic_;
};



}

#endif
