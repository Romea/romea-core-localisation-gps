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


class GPSLocalisationPlugin
{

public :

  GPSLocalisationPlugin(std::unique_ptr<GPSReceiver> gps,
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

  DiagnosticReport makeDiagnosticReport();

protected:

  std::unique_ptr<GPSReceiver> gps_;
  ENUConverter enuConverter_;

  CheckupRate gga_rate_diagnostic_;
  CheckupRate rmc_rate_diagnostic_;

  CheckupGGAFix gga_fix_diagnostic_;
  CheckupRMCTrackAngle rmc_track_angle_diagnostic_;
};

}

#endif
