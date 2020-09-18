#include "romea_localisation_gps/LocalisationGPSPlugin.hpp"

namespace
{
const double DEFAULT_COURSE_ANGLE_STD = 20/180.*M_PI;
}


namespace romea {

//-----------------------------------------------------------------------------
LocalisationGPSPlugin::LocalisationGPSPlugin(std::unique_ptr<GPSReceiver> gps,
                                             const FixQuality & minimalFixQuality,
                                             const double & minimalSpeedOverGround):
  gps_(std::move(gps)),
  enuConverter_(),
  gga_rate_diagnostic_("gga",1.0,0.1),
  rmc_rate_diagnostic_("rmc",1.0,0.1),
  gga_fix_diagnostic_(minimalFixQuality),
  rmc_track_angle_diagnostic_(minimalSpeedOverGround)
{
}

//-----------------------------------------------------------------------------
void LocalisationGPSPlugin::setAnchor(const GeodeticCoordinates & wgs84_anchor)
{
  enuConverter_.setAnchor(wgs84_anchor);
}

//-----------------------------------------------------------------------------
bool LocalisationGPSPlugin::processGGA(const Duration & stamp,
                                       const std::string & ggaSentence,
                                       ObservationPosition & positionObs)
{
  GGAFrame ggaFrame(ggaSentence);
  if(gga_rate_diagnostic_.evaluate(stamp) == DiagnosticStatus::OK &&
     gga_fix_diagnostic_.evaluate(ggaFrame) == DiagnosticStatus::OK)
  {

    GeodeticCoordinates geodeticCoordinates((*ggaFrame.latitude).toDouble(),
                                            (*ggaFrame.longitude).toDouble(),
                                            *ggaFrame.altitudeAboveGeoid+*ggaFrame.geoidHeight);

    Eigen::Vector3d position = enuConverter_.toENU(geodeticCoordinates);
    double fixStd = *ggaFrame.horizontalDilutionOfPrecision*gps_->getUERE(*ggaFrame.fixQuality);
    positionObs.Y(ObservationPosition::POSITION_X)=position.x();
    positionObs.Y(ObservationPosition::POSITION_Y)=position.y();
    positionObs.R()=Eigen::Matrix2d::Identity()*fixStd*fixStd;
    positionObs.levelArm=gps_->getAntennaBodyPosition();
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
bool LocalisationGPSPlugin::processRMC(const Duration & stamp,
                                       const std::string & rmcSentence,
                                       const double & linearSpeed,
                                       ObservationCourse & courseObs)
{
  RMCFrame rmcFrame(rmcSentence);
  if(rmc_rate_diagnostic_.evaluate(stamp) == DiagnosticStatus::OK &&
     rmc_track_angle_diagnostic_.evaluate(rmcFrame) == DiagnosticStatus::OK)
  {
    courseObs.Y() = trackAngleToCourseAngle(*rmcFrame.trackAngleTrue,linearSpeed);
    courseObs.R() = DEFAULT_COURSE_ANGLE_STD *DEFAULT_COURSE_ANGLE_STD;
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
void LocalisationGPSPlugin::processGSV(const std::string & gsvSentence)
{
  if(gps_->updateSatellitesViews(gsvSentence))
  {
//    diagnostics_.updateConstellationReliability(gps_->getReliability());
  }
}

//-----------------------------------------------------------------------------
DiagnosticReport LocalisationGPSPlugin::makeDiagnosticReport()
{
  DiagnosticReport report;
  report += gga_rate_diagnostic_.getReport();
  report += rmc_rate_diagnostic_.getReport();
  report += gga_fix_diagnostic_.getReport();
  report += rmc_track_angle_diagnostic_.getReport();
  return report;
}

//  switch (NMEAParsing::extractSentenceId(msg->sentence))
//  {
//  case NMEAParsing::SentenceID::GGA :
//    processGGA_(msg);
//    break;
//  case NMEAParsing::SentenceID::RMC :
//    processRMC_(msg);
//    break;
//  case NMEAParsing::SentenceID::GSV :
//    processGSV_(msg);
//    break;
//  default :
//    break;

}
