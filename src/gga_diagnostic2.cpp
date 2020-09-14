#include "romea_gps_localisation_plugin/gga_diagnostic2.hpp"


namespace
{
const double MAXIMAL_HORIZONTAL_DILUTION_OF_PRECISION=5;
const unsigned short MINIMAL_NUMBER_OF_SATELLITES_TO_COMPUTE_FIX=6;
}

namespace romea {

//-----------------------------------------------------------------------------
DiagnosticGGAFix2::DiagnosticGGAFix2(const FixQuality & minimalFixQuality):
  report_(),
  minimalFixQuality_(minimalFixQuality)
{
}

//-----------------------------------------------------------------------------
DiagnosticStatus DiagnosticGGAFix2::evaluate(const GGAFrame & ggaFrame)
{
  if(checkFrameIsComplete_(ggaFrame))
  {
    checkFixIsReliable_(ggaFrame);
  }

  setReportInfos_(ggaFrame);
  return report_.status;
}


//-----------------------------------------------------------------------------
void DiagnosticGGAFix2::setReportInfos_(const GGAFrame & ggaFrame)
{
  setReportInfo(report_,"talker",ggaFrame.talkerId);
  setReportInfo(report_,"latitude",ggaFrame.latitude);
  setReportInfo(report_,"longitude",ggaFrame.longitude);
  setReportInfo(report_,"geoid height",ggaFrame.geoidHeight);
  setReportInfo(report_,"altitude_above_geoid",ggaFrame.altitudeAboveGeoid);
  setReportInfo(report_,"fix_quality",ggaFrame.fixQuality);
  setReportInfo(report_,"number_of_sattellites",ggaFrame.numberSatellitesUsedToComputeFix);
  setReportInfo(report_,"hdop",ggaFrame.horizontalDilutionOfPrecision);
  setReportInfo(report_,"correction_age",ggaFrame.dgpsCorrectionAgeInSecond);
  setReportInfo(report_,"base_station_id",ggaFrame.dgpsStationIdNumber);
}

//-----------------------------------------------------------------------------
bool DiagnosticGGAFix2::checkFrameIsComplete_(const GGAFrame & ggaFrame)
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
    report_.status=DiagnosticStatus::ERROR;
    report_.message="frame is incomplete";
    return false;
  }
}

//-----------------------------------------------------------------------------
void DiagnosticGGAFix2::checkFixIsReliable_(const GGAFrame & ggaFrame)
{
  warningMessages_.clear();

  if(*ggaFrame.fixQuality==FixQuality::SIMULATION_FIX ||
     (checkHorizontalDilutionOfPrecision_(ggaFrame)&&
      checkNumberSatellitesUsedToComputeFix_(ggaFrame)&&
      checkFixQuality_(ggaFrame)))
  {
    report_.status=DiagnosticStatus::OK;
    report_.message="fix OK.";
  }
  else
  {
    report_.status=DiagnosticStatus::WARN;
    makeReportWarningMessage_();
  }
}


//-----------------------------------------------------------------------------
bool DiagnosticGGAFix2::checkHorizontalDilutionOfPrecision_(const GGAFrame & ggaFrame)
{
  if(*ggaFrame.horizontalDilutionOfPrecision<
     MAXIMAL_HORIZONTAL_DILUTION_OF_PRECISION)
  {
    return true;
  }
  else
  {
    warningMessages_.push_back("HDOP is two high");
    return false;
  }
}

//-----------------------------------------------------------------------------
bool DiagnosticGGAFix2::checkNumberSatellitesUsedToComputeFix_(const GGAFrame & ggaFrame)
{
  if(*ggaFrame.numberSatellitesUsedToComputeFix>=
     MINIMAL_NUMBER_OF_SATELLITES_TO_COMPUTE_FIX)
  {
    return true;
  }
  else
  {
    warningMessages_.push_back("not enough satellites to compute fix");
    return false;
  }
}

//-----------------------------------------------------------------------------
bool DiagnosticGGAFix2::checkFixQuality_(const GGAFrame & ggaFrame)
{
  if(*ggaFrame.fixQuality>=minimalFixQuality_)
  {
    return true;
  }
  else
  {
    warningMessages_.push_back("fix quality is too low");
    return false;
  }
}

//-----------------------------------------------------------------------------
void DiagnosticGGAFix2::makeReportWarningMessage_()
{
  report_.message="fix not reliable : ";
  auto it = std::cbegin(warningMessages_);

  report_.message+=*it;
  while(++it!=std::cend(warningMessages_))
  {
    report_.message+=", ";
    report_.message+= *it;
  }
  report_.message +=".";
}

}// namespace



