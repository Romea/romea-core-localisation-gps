#ifndef _romea_DiagnosticsGGAFix2_hpp_
#define _romea_DiagnosticsGGAFix2_hpp_

//std
#include <list>

//romea
#include <romea_common/diagnostic/DiagnosticReport.hpp>
#include <romea_gps/nmea/GGAFrame.hpp>

namespace romea {

class  CheckupGGAFix
{

public:

  CheckupGGAFix(const FixQuality &minimalFixQuality);

  DiagnosticStatus evaluate(const GGAFrame & ggaFrame);

  const DiagnosticReport & getReport()const;

  void reset();

private :

  void setReportInfos_(const GGAFrame & ggaFrame);
  void addDiagnostic_(const DiagnosticStatus & status, const std::string & message);

  bool checkFrameIsComplete_(const GGAFrame & ggaFrame);
  void checkFixIsReliable_(const GGAFrame & ggaFrame);
  bool checkFixQuality_(const GGAFrame & ggaFrame);
  bool checkHorizontalDilutionOfPrecision_(const GGAFrame & ggaFrame);
  bool checkNumberSatellitesUsedToComputeFix_(const GGAFrame & ggaFrame);

private :

  DiagnosticReport report_;
  FixQuality minimalFixQuality_;
  double maximalHorizontalDilutionOfPrecision_;
};



}// namespace


#endif
