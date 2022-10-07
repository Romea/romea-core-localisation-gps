#ifndef _romea_CheckRMCTrackAngle_hpp_
#define _romea_CheckRMCTrackAngle_hpp_

//std
#include <mutex>

//romea
#include <romea_core_common/diagnostic/DiagnosticReport.hpp>
#include <romea_core_gps/nmea/RMCFrame.hpp>

namespace romea {

class  CheckupRMCTrackAngle
{

public:

  CheckupRMCTrackAngle(const double & minimalSpeedOverGround);

  DiagnosticStatus evaluate(const RMCFrame & rmcFrame);

  const DiagnosticReport & getReport()const;

  void reset();

private :

  bool checkFrameIsComplete_(const RMCFrame & rmcFrame);
  void checkFixIsReliable_(const RMCFrame & rmcFrame);

  void declareReportInfos_();
  void setReportInfos_(const RMCFrame & rmcFrame);
  void setDiagnostic_(const DiagnosticStatus & status, const std::string & message);

private:

  double minimalSpeedOverGround_;

  mutable std::mutex mutex_;
  DiagnosticReport report_;
};


}// namespace


#endif
