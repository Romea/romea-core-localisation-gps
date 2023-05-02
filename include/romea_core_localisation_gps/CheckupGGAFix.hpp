// Copyright 2022 INRAE, French National Research Institute for Agriculture, Food and Environment
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


#ifndef  ROMEA_CORE_LOCALISATION_GPS__CHECKUPGGAFIX_HPP_
#define  ROMEA_CORE_LOCALISATION_GPS__CHECKUPGGAFIX_HPP_

// romea
#include <romea_core_common/diagnostic/DiagnosticReport.hpp>
#include <romea_core_gps/nmea/GGAFrame.hpp>

// std
#include <list>
#include <mutex>
#include <string>


namespace romea
{

class CheckupGGAFix
{
public:
  explicit CheckupGGAFix(const FixQuality & minimalFixQuality);

  DiagnosticStatus evaluate(const GGAFrame & ggaFrame);

  const DiagnosticReport & getReport()const;

  void reset();

private:
  void declareReportInfos_();
  void setReportInfos_(const GGAFrame & ggaFrame);
  void addDiagnostic_(const DiagnosticStatus & status, const std::string & message);

  bool checkFrameIsComplete_(const GGAFrame & ggaFrame);
  void checkFixIsReliable_(const GGAFrame & ggaFrame);
  bool checkFixQuality_(const GGAFrame & ggaFrame);
  bool checkHorizontalDilutionOfPrecision_(const GGAFrame & ggaFrame);
  bool checkNumberSatellitesUsedToComputeFix_(const GGAFrame & ggaFrame);

private:
  FixQuality minimalFixQuality_;
  double maximalHorizontalDilutionOfPrecision_;

  mutable std::mutex mutex_;
  DiagnosticReport report_;
};

}  // namespace romea

#endif  // _ROMEA_CORE_LOCALISATION_GPS__CHECKUPGGAFIX_HPP_
