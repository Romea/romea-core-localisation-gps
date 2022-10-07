#ifndef _romea_DiagnosticsGGAFix_hpp_
#define _romea_DiagnosticsGGAFix_hpp_

//std
#include <list>
#include <mutex>

//romea
#include <romea_core_common/diagnostic/DiagnosticReport.hpp>
#include <romea_core_gps/nmea/GGAFrame.hpp>

namespace romea {

class  CheckupGGAFix
{

public:

  CheckupGGAFix(const FixQuality &minimalFixQuality);

  DiagnosticStatus evaluate(const GGAFrame & ggaFrame);

  const DiagnosticReport & getReport()const;

  void reset();

private :

  void declareReportInfos_();
  void setReportInfos_(const GGAFrame & ggaFrame);
  void addDiagnostic_(const DiagnosticStatus & status, const std::string & message);

  bool checkFrameIsComplete_(const GGAFrame & ggaFrame);
  void checkFixIsReliable_(const GGAFrame & ggaFrame);
  bool checkFixQuality_(const GGAFrame & ggaFrame);
  bool checkHorizontalDilutionOfPrecision_(const GGAFrame & ggaFrame);
  bool checkNumberSatellitesUsedToComputeFix_(const GGAFrame & ggaFrame);

private :

  FixQuality minimalFixQuality_;
  double maximalHorizontalDilutionOfPrecision_;

  mutable std::mutex mutex_;
  DiagnosticReport report_;

};



}// namespace


#endif
