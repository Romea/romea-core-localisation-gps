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
#include "romea_core_localisation_gps/CheckupHDTTrackAngle.hpp"

class TestHDTTrackAngleDiagnostic : public ::testing::Test
{
public:
  TestHDTTrackAngleDiagnostic()
  : frame(minimalGoodHDTFrame()),
    diagnostic()
  {
  }

  romea::core::HDTFrame frame;
  romea::core::CheckupHDTTrackAngle diagnostic;
};


//-----------------------------------------------------------------------------
TEST_F(TestHDTTrackAngleDiagnostic, checkEmptyReport)
{
  EXPECT_TRUE(diagnostic.getReport().diagnostics.empty());
  EXPECT_STREQ(diagnostic.getReport().info.at("talker").c_str(), "");
  EXPECT_STREQ(diagnostic.getReport().info.at("track_angle").c_str(), "");
}

//-----------------------------------------------------------------------------
TEST_F(TestHDTTrackAngleDiagnostic, checkMinimalGoodFrame)
{
  EXPECT_EQ(diagnostic.evaluate(frame), romea::core::DiagnosticStatus::OK);
  EXPECT_EQ(diagnostic.getReport().diagnostics.front().status, romea::core::DiagnosticStatus::OK);
  EXPECT_STREQ(diagnostic.getReport().diagnostics.front().message.c_str(), "HDT track angle OK.");
  EXPECT_STREQ(diagnostic.getReport().info.at("talker").c_str(), "GLONASS");
  EXPECT_STREQ(diagnostic.getReport().info.at("track_angle").c_str(), "0.378");
}

//-----------------------------------------------------------------------------
TEST_F(TestHDTTrackAngleDiagnostic, checkEmptyReportAfterReset)
{
  diagnostic.evaluate(frame);
  diagnostic.reset();
  EXPECT_TRUE(diagnostic.getReport().diagnostics.empty());
  EXPECT_STREQ(diagnostic.getReport().info.at("talker").c_str(), "");
  EXPECT_STREQ(diagnostic.getReport().info.at("track_angle").c_str(), "");
}

//-----------------------------------------------------------------------------
TEST_F(TestHDTTrackAngleDiagnostic, checkMissingTrackAngle)
{
  frame.heading.reset();
  EXPECT_EQ(diagnostic.evaluate(frame), romea::core::DiagnosticStatus::ERROR);
  EXPECT_EQ(
    diagnostic.getReport().diagnostics.front().status,
    romea::core::DiagnosticStatus::ERROR);
  EXPECT_STREQ(diagnostic.getReport().info.at("track_angle").c_str(), "");
  EXPECT_STREQ(
    diagnostic.getReport().diagnostics.front().message.c_str(), "HDT track angle is incomplete.");
}

//-----------------------------------------------------------------------------
int main(int argc, char ** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
