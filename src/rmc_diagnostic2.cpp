#include "romea_gps_localisation_plugin/rmc_diagnostic2.hpp"

namespace romea {


//-----------------------------------------------------------------------------
DiagnosticRMCTrackAngle2::DiagnosticRMCTrackAngle2(const double & minimalSpeedOverGround):
  report_(),
  minimalSpeedOverGround_(minimalSpeedOverGround)
{

}

//-----------------------------------------------------------------------------
DiagnosticStatus DiagnosticRMCTrackAngle2::evaluate(const RMCFrame & rmcFrame)
{

  if(checkFrameIsComplete_(rmcFrame))
  {
    checkFixIsReliable_(rmcFrame);
  }
  setReportInfos_(rmcFrame);
  return report_.status;
}

//-----------------------------------------------------------------------------
void DiagnosticRMCTrackAngle2::checkFixIsReliable_(const RMCFrame & rmcFrame)
{
  if(*rmcFrame.speedOverGroundInMeterPerSecond<minimalSpeedOverGround_)
  {
    report_.status= DiagnosticStatus::WARN;
    report_.message= "Track angle not reliable.";
  }
  else
  {
    report_.status= DiagnosticStatus::WARN;
    report_.message= "Track angle OK.";
  }
}

//-----------------------------------------------------------------------------
const DiagnosticReport & DiagnosticRMCTrackAngle2::getReport() const
{
  return report_;
}


//-----------------------------------------------------------------------------
bool DiagnosticRMCTrackAngle2::checkFrameIsComplete_(const RMCFrame & rmcFrame)
{
  if(rmcFrame.speedOverGroundInMeterPerSecond&&
     rmcFrame.trackAngleTrue)
  {
    return true;
  }
  else
  {
    report_.status=DiagnosticStatus::ERROR;
    report_.message="frame is incomplete";
    return false;
  }
}

//-----------------------------------------------------------------------------
void DiagnosticRMCTrackAngle2::setReportInfos_(const RMCFrame & rmcFrame)
{
  setReportInfo(report_,"talker",rmcFrame.talkerId);
  setReportInfo(report_,"speed_over_ground",rmcFrame.speedOverGroundInMeterPerSecond);
  setReportInfo(report_,"track_angle",rmcFrame.trackAngleTrue);
  setReportInfo(report_,"magnetic deviation", rmcFrame.magneticDeviation);
}

}// namespace



