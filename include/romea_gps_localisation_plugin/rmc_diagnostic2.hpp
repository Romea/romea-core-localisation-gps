#ifndef _romea_RMCDiagnostic2_hpp_
#define _romea_RMCDiagnostic2_hpp_

//romea
#include <romea_common/diagnostic/DiagnosticReport.hpp>
#include <romea_gps/nmea/RMCFrame.hpp>

namespace romea {

class  DiagnosticRMCTrackAngle2
{

public:

  DiagnosticRMCTrackAngle2(const double & minimalSpeedOverGround);

  DiagnosticStatus evaluate(const RMCFrame & rmcFrame);

  const DiagnosticReport & getReport()const;

private :

  void setReportInfos_(const RMCFrame & rmcFrame);
  bool checkFrameIsComplete_(const RMCFrame & rmcFrame);
  void checkFixIsReliable_(const RMCFrame & rmcFrame);

private:

  DiagnosticReport report_;
  double minimalSpeedOverGround_;
};


}// namespace


#endif
