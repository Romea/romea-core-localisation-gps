#include "romea_core_localisation_gps/CheckupRMCTrackAngle.hpp"

namespace romea {


//-----------------------------------------------------------------------------
CheckupRMCTrackAngle::CheckupRMCTrackAngle(const double & minimalSpeedOverGround):
  report_(),
  minimalSpeedOverGround_(minimalSpeedOverGround)
{
  report_.diagnostics.push_back(Diagnostic());
}

//-----------------------------------------------------------------------------
void CheckupRMCTrackAngle::reset()
{
  report_.diagnostics.clear();
  report_.diagnostics.push_back(Diagnostic());
}

//-----------------------------------------------------------------------------
DiagnosticStatus CheckupRMCTrackAngle::evaluate(const RMCFrame & rmcFrame)
{

  if(checkFrameIsComplete_(rmcFrame))
  {
    checkFixIsReliable_(rmcFrame);
  }
  setReportInfos_(rmcFrame);
  return report_.diagnostics.front().status;
}

//-----------------------------------------------------------------------------
void CheckupRMCTrackAngle::checkFixIsReliable_(const RMCFrame & rmcFrame)
{
  if(*rmcFrame.speedOverGroundInMeterPerSecond<minimalSpeedOverGround_)
  {
    std::stringstream msg;
    msg << "RMC track angle is not reliable ";
    msg << "because vehicle speed is lower than ";
    msg << minimalSpeedOverGround_ <<" m/s.";
    setDiagnostic_(DiagnosticStatus::WARN,msg.str());
  }
  else
  {
    setDiagnostic_(DiagnosticStatus::OK,"RMC track angle OK.");
  }
}

//-----------------------------------------------------------------------------
const DiagnosticReport & CheckupRMCTrackAngle::getReport() const
{
  return report_;
}

//-----------------------------------------------------------------------------
bool CheckupRMCTrackAngle::checkFrameIsComplete_(const RMCFrame & rmcFrame)
{
  if(rmcFrame.speedOverGroundInMeterPerSecond&&
     rmcFrame.trackAngleTrue)
  {
    return true;
  }
  else
  {
    setDiagnostic_(DiagnosticStatus::ERROR,"RMC track angle is incomplete.");
    return false;
  }
}

//-----------------------------------------------------------------------------
void CheckupRMCTrackAngle::setReportInfos_(const RMCFrame & rmcFrame)
{
  setReportInfo(report_,"talker",rmcFrame.talkerId);
  setReportInfo(report_,"speed_over_ground",rmcFrame.speedOverGroundInMeterPerSecond);
  setReportInfo(report_,"track_angle",rmcFrame.trackAngleTrue);
  setReportInfo(report_,"magnetic_deviation", rmcFrame.magneticDeviation);
}

//-----------------------------------------------------------------------------
void CheckupRMCTrackAngle::setDiagnostic_(const DiagnosticStatus & status, const std::string & message)
{
  Diagnostic & diagnostic = report_.diagnostics.front();
  diagnostic.message = message;
  diagnostic.status = status;
}

}// namespace



