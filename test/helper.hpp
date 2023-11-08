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

#ifndef HELPER_HPP_
#define HELPER_HPP_

#include "romea_core_gps/nmea/GGAFrame.hpp"
#include "romea_core_gps/nmea/HDTFrame.hpp"
#include "romea_core_gps/nmea/RMCFrame.hpp"

romea::GGAFrame minimalGoodGGAFrame()
{
  romea::GGAFrame frame;
  frame.talkerId = romea::TalkerId::GN;
  frame.longitude = romea::Longitude(0.03);
  frame.latitude = romea::Latitude(0.7854);
  frame.geoidHeight = 400.8;
  frame.altitudeAboveGeoid = 53.3;
  frame.horizontalDilutionOfPrecision = 1.2;
  frame.numberSatellitesUsedToComputeFix = 12;
  frame.fixQuality = romea::FixQuality::RTK_FIX;
  frame.horizontalDilutionOfPrecision = 1.2;
  frame.dgpsCorrectionAgeInSecond = 2.5;
  frame.dgpsStationIdNumber = 1;
  return frame;
}

romea::RMCFrame minimalGoodRMCFrame()
{
  romea::RMCFrame frame;
  frame.talkerId = romea::TalkerId::GP;
  frame.speedOverGroundInMeterPerSecond = 3.2;
  frame.trackAngleTrue = 1.54;
  frame.magneticDeviation = 0.0378;
  return frame;
}

romea::HDTFrame minimalGoodHDTFrame()
{
  romea::HDTFrame frame;
  frame.talkerId = romea::TalkerId::GL;
  frame.heading = 0.378;
  return frame;
}

#endif  // HELPER_HPP_
