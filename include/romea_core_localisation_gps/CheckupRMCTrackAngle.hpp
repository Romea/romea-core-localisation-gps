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


#ifndef ROMEA_CORE_LOCALISATION_GPS__CHECKUPRMCTRACKANGLE_HPP_
#define ROMEA_CORE_LOCALISATION_GPS__CHECKUPRMCTRACKANGLE_HPP_

// romea
#include <romea_core_common/diagnostic/DiagnosticReport.hpp>
#include <romea_core_gps/nmea/RMCFrame.hpp>

// std
#include <mutex>
#include <string>

namespace romea
{

class CheckupRMCTrackAngle
{
public:
  explicit CheckupRMCTrackAngle(const double & minimalSpeedOverGround);

  DiagnosticStatus evaluate(const RMCFrame & rmcFrame);

  const DiagnosticReport & getReport()const;

  void reset();

private:
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

}  // namespace romea


#endif  // ROMEA_CORE_LOCALISATION_GPS__CHECKUPRMCTRACKANGLE_HPP_
