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


// gtest
#include <gtest/gtest.h>

// std
#include <string>

// romea
#include "helper.hpp"
#include "romea_core_localisation_gps/CheckupRMCTrackAngle.hpp"

class TestRMCTrackAngleDiagnostic : public ::testing::Test
{
public:
  TestRMCTrackAngleDiagnostic()
  : frame(minimalGoodRMCFrame()),
    diagnostic(1)
  {
  }

  romea::core::RMCFrame frame;
  romea::core::CheckupRMCTrackAngle diagnostic;
};


//-----------------------------------------------------------------------------
TEST_F(TestRMCTrackAngleDiagnostic, checkEmptyReport)
{
  EXPECT_TRUE(diagnostic.getReport().diagnostics.empty());
  EXPECT_STREQ(diagnostic.getReport().info.at("talker").c_str(), "");
  EXPECT_STREQ(diagnostic.getReport().info.at("speed_over_ground").c_str(), "");
  EXPECT_STREQ(diagnostic.getReport().info.at("track_angle").c_str(), "");
  EXPECT_STREQ(diagnostic.getReport().info.at("magnetic_deviation").c_str(), "");
}

//-----------------------------------------------------------------------------
TEST_F(TestRMCTrackAngleDiagnostic, checkEmptyReportAfterReset)
{
  diagnostic.evaluate(frame);
  diagnostic.reset();
  EXPECT_TRUE(diagnostic.getReport().diagnostics.empty());
  EXPECT_STREQ(diagnostic.getReport().info.at("talker").c_str(), "");
  EXPECT_STREQ(diagnostic.getReport().info.at("speed_over_ground").c_str(), "");
  EXPECT_STREQ(diagnostic.getReport().info.at("track_angle").c_str(), "");
  EXPECT_STREQ(diagnostic.getReport().info.at("magnetic_deviation").c_str(), "");
}

//-----------------------------------------------------------------------------
TEST_F(TestRMCTrackAngleDiagnostic, checkMinimalGoodFrame)
{
  EXPECT_EQ(diagnostic.evaluate(frame), romea::core::DiagnosticStatus::OK);
  EXPECT_EQ(diagnostic.getReport().diagnostics.front().status, romea::core::DiagnosticStatus::OK);
  EXPECT_STREQ(diagnostic.getReport().diagnostics.front().message.c_str(), "RMC track angle OK.");
  EXPECT_STREQ(diagnostic.getReport().info.at("talker").c_str(), "NAVSTAR");
  EXPECT_STREQ(diagnostic.getReport().info.at("speed_over_ground").c_str(), "3.2");
  EXPECT_STREQ(diagnostic.getReport().info.at("track_angle").c_str(), "1.54");
  EXPECT_STREQ(diagnostic.getReport().info.at("magnetic_deviation").c_str(), "0.0378");
}

//-----------------------------------------------------------------------------
void checkMissingData(
  romea::core::CheckupRMCTrackAngle & diagnostic,
  const romea::core::RMCFrame & frame,
  const std::string & missingDataName)
{
  EXPECT_EQ(diagnostic.evaluate(frame), romea::core::DiagnosticStatus::ERROR);
  EXPECT_EQ(
    diagnostic.getReport().diagnostics.front().status,
    romea::core::DiagnosticStatus::ERROR);
  EXPECT_STREQ(diagnostic.getReport().info.at(missingDataName).c_str(), "");
  EXPECT_STREQ(
    diagnostic.getReport().diagnostics.front().message.c_str(),
    "RMC track angle is incomplete.");
}

//-----------------------------------------------------------------------------
TEST_F(TestRMCTrackAngleDiagnostic, checkMissingSpeedOverGround)
{
  frame.speedOverGroundInMeterPerSecond.reset();
  checkMissingData(diagnostic, frame, "speed_over_ground");
}

//-----------------------------------------------------------------------------
TEST_F(TestRMCTrackAngleDiagnostic, checkMissingTrackAngle)
{
  frame.trackAngleTrue.reset();
  checkMissingData(diagnostic, frame, "track_angle");
}

//-----------------------------------------------------------------------------
TEST_F(TestRMCTrackAngleDiagnostic, checkUnreliableTrackAngle)
{
  frame.speedOverGroundInMeterPerSecond = 0.5;
  EXPECT_EQ(diagnostic.evaluate(frame), romea::core::DiagnosticStatus::WARN);
  EXPECT_EQ(diagnostic.getReport().diagnostics.front().status, romea::core::DiagnosticStatus::WARN);
  EXPECT_STREQ(
    diagnostic.getReport().diagnostics.front().message.c_str(),
    "RMC track angle is not reliable because vehicle speed is lower than 1 m/s.");
}

//-----------------------------------------------------------------------------
int main(int argc, char ** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
