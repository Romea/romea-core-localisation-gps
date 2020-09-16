#include "romea_gps_localisation_plugin/CheckupGGAFix.hpp"

namespace
{
const double MAXIMAL_HORIZONTAL_DILUTION_OF_PRECISION=5;
const unsigned short MINIMAL_NUMBER_OF_SATELLITES_TO_COMPUTE_FIX=6;
}

namespace romea {

//-----------------------------------------------------------------------------
CheckupGGAFix::CheckupGGAFix(const FixQuality & minimalFixQuality):
  report_(),
  minimalFixQuality_(minimalFixQuality)
{
}

//-----------------------------------------------------------------------------
DiagnosticStatus CheckupGGAFix::evaluate(const GGAFrame & ggaFrame)
{
  report_.diagnostics.clear();
  if(checkFrameIsComplete_(ggaFrame))
  {
    checkFixIsReliable_(ggaFrame);
  }

  setReportInfos_(ggaFrame);
  return worseStatus(report_.diagnostics);
}


//-----------------------------------------------------------------------------
void CheckupGGAFix::setReportInfos_(const GGAFrame & ggaFrame)
{
  setReportInfo(report_,"talker",ggaFrame.talkerId);
  setReportInfo(report_,"geoid_height",ggaFrame.geoidHeight);
  setReportInfo(report_,"altitude_above_geoid",ggaFrame.altitudeAboveGeoid);
  setReportInfo(report_,"fix_quality",ggaFrame.fixQuality);
  setReportInfo(report_,"number_of_satellites",ggaFrame.numberSatellitesUsedToComputeFix);
  setReportInfo(report_,"hdop",ggaFrame.horizontalDilutionOfPrecision);
  setReportInfo(report_,"correction_age",ggaFrame.dgpsCorrectionAgeInSecond);
  setReportInfo(report_,"base_station_id",ggaFrame.dgpsStationIdNumber);

  if(ggaFrame.latitude)
  {
    setReportInfo(report_,"latitude",(*ggaFrame.latitude).toDouble());
  }
  else
  {
    setReportInfo(report_,"latitude",ggaFrame.latitude);
  }

  if(ggaFrame.longitude)
  {
    setReportInfo(report_,"longitude",(*ggaFrame.longitude).toDouble());
  }
  else
  {
    setReportInfo(report_,"longitude",ggaFrame.longitude);
  }

}

//-----------------------------------------------------------------------------
bool CheckupGGAFix::checkFrameIsComplete_(const GGAFrame & ggaFrame)
{
  if(ggaFrame.latitude &&
     ggaFrame.longitude &&
     ggaFrame.altitudeAboveGeoid &&
     ggaFrame.geoidHeight &&
     ggaFrame.horizontalDilutionOfPrecision &&
     ggaFrame.numberSatellitesUsedToComputeFix &&
     ggaFrame.fixQuality)
  {
    return true;
  }
  else
  {
    addDiagnostic_(DiagnosticStatus::ERROR,"GGA fix is incomplete.");
    return false;
  }
}

//-----------------------------------------------------------------------------
void CheckupGGAFix::checkFixIsReliable_(const GGAFrame & ggaFrame)
{
  if(*ggaFrame.fixQuality==FixQuality::SIMULATION_FIX ||
     (checkHorizontalDilutionOfPrecision_(ggaFrame)&
      checkNumberSatellitesUsedToComputeFix_(ggaFrame)&
      checkFixQuality_(ggaFrame)))
  {
    addDiagnostic_(DiagnosticStatus::OK,"GGA fix OK.");
  }
}


//-----------------------------------------------------------------------------
bool CheckupGGAFix::checkHorizontalDilutionOfPrecision_(const GGAFrame & ggaFrame)
{
  if(*ggaFrame.horizontalDilutionOfPrecision<
     MAXIMAL_HORIZONTAL_DILUTION_OF_PRECISION)
  {
    return true;
  }
  else
  {
    addDiagnostic_(DiagnosticStatus::WARN,"HDOP is two high.");
    return false;
  }
}

//-----------------------------------------------------------------------------
bool CheckupGGAFix::checkNumberSatellitesUsedToComputeFix_(const GGAFrame & ggaFrame)
{
  if(*ggaFrame.numberSatellitesUsedToComputeFix>=
     MINIMAL_NUMBER_OF_SATELLITES_TO_COMPUTE_FIX)
  {
    return true;
  }
  else
  {
    addDiagnostic_(DiagnosticStatus::WARN,"Not enough satellites to compute fix.");
    return false;
  }
}

//-----------------------------------------------------------------------------
bool CheckupGGAFix::checkFixQuality_(const GGAFrame & ggaFrame)
{
  if(*ggaFrame.fixQuality>=minimalFixQuality_)
  {
    return true;
  }
  else
  {
    addDiagnostic_(DiagnosticStatus::WARN,"Fix quality is too low.");
    return false;
  }
}

//-----------------------------------------------------------------------------
const DiagnosticReport & CheckupGGAFix::getReport()const
{
  return report_;
}

//-----------------------------------------------------------------------------
void CheckupGGAFix::addDiagnostic_(const DiagnosticStatus & status, const std::string & message)
{
  report_.diagnostics.push_back(Diagnostic());
  report_.diagnostics.back().status=status;
  report_.diagnostics.back().message=message;
}

}// namespace



