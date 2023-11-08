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
#include <limits>
#include <memory>
#include <utility>
#include <string>

// romea
#include "helper.hpp"
#include "romea_core_localisation_gps/LocalisationGPSPlugin.hpp"

bool boolean(const romea::DiagnosticStatus & status)
{
  return status == romea::DiagnosticStatus::OK;
}

class TestDualAntennaGPSPlugin : public ::testing::Test
{
public:
  TestDualAntennaGPSPlugin()
  : stamp(),
    gga_frame(minimalGoodGGAFrame()),
    hdt_frame(minimalGoodHDTFrame()),
    course(),
    position(),
    gps_plugin(nullptr)
  {
  }

  void SetUp() override
  {
    auto gps = std::make_unique<romea::GPSReceiver>();
    gps->setAntennaBodyPosition(Eigen::Vector3d(0.3, 0, 2.));

    gps_plugin = std::make_unique<romea::LocalisationDualAntennaGPSPlugin>(
      std::move(gps), romea::FixQuality::RTK_FIX);
  }


  const romea::Diagnostic & diagnostic(const size_t & index)
  {
    return *std::next(std::cbegin(report.diagnostics), index);
  }


  void check(
    const romea::DiagnosticStatus & finalFixStatus,
    const romea::DiagnosticStatus & finalTrackStatus)
  {
    std::string gga_sentence = gga_frame.toNMEA();
    std::string hdt_sentence = hdt_frame.toNMEA();

    for (size_t n = 0; n < 4; ++n) {
      romea::Duration stamp = romea::durationFromSecond(0.5 + n / 10.);
      EXPECT_FALSE(gps_plugin->processGGA(stamp, gga_sentence, position));
      EXPECT_FALSE(gps_plugin->processHDT(stamp, hdt_sentence, course));
      report = gps_plugin->makeDiagnosticReport(stamp);

      EXPECT_EQ(report.diagnostics.size(), 2);
      // EXPECT_EQ(
      //   diagnostic(0).status, std::isfinite(
      //     linear_speed) ? romea::DiagnosticStatus::OK : romea::DiagnosticStatus::ERROR);
      EXPECT_EQ(diagnostic(0).status, romea::DiagnosticStatus::ERROR);
      EXPECT_EQ(diagnostic(1).status, romea::DiagnosticStatus::ERROR);
    }

    romea::Duration stamp = romea::durationFromSecond(0.5);
    EXPECT_EQ(gps_plugin->processGGA(stamp, gga_sentence, position), boolean(finalFixStatus));
    EXPECT_EQ(gps_plugin->processHDT(stamp, hdt_sentence, course), boolean(finalTrackStatus));
    report = gps_plugin->makeDiagnosticReport(stamp);
    EXPECT_EQ(diagnostic(0).status, romea::DiagnosticStatus::OK);
    EXPECT_EQ(diagnostic(1).status, finalFixStatus);
    EXPECT_EQ(diagnostic(2).status, romea::DiagnosticStatus::OK);
    EXPECT_EQ(diagnostic(3).status, finalTrackStatus);
  }

  romea::Duration stamp;
  romea::GGAFrame gga_frame;
  romea::HDTFrame hdt_frame;

  romea::ObservationCourse course;
  romea::ObservationPosition position;
  std::unique_ptr<romea::LocalisationDualAntennaGPSPlugin> gps_plugin;

  romea::DiagnosticReport report;
};


//-----------------------------------------------------------------------------
TEST_F(TestDualAntennaGPSPlugin, testAllOk)
{
  check(romea::DiagnosticStatus::OK, romea::DiagnosticStatus::OK);
}

//-----------------------------------------------------------------------------
TEST_F(TestDualAntennaGPSPlugin, testFixOKTrakAngleError)
{
  hdt_frame.heading.reset();
  check(romea::DiagnosticStatus::OK, romea::DiagnosticStatus::ERROR);
}

//-----------------------------------------------------------------------------
TEST_F(TestDualAntennaGPSPlugin, testFixWarnTrackAngleOK)
{
  gga_frame.numberSatellitesUsedToComputeFix = 5;
  check(romea::DiagnosticStatus::WARN, romea::DiagnosticStatus::OK);
}

//-----------------------------------------------------------------------------
TEST_F(TestDualAntennaGPSPlugin, testFixErrorTrackAngleOK)
{
  gga_frame.fixQuality.reset();
  check(romea::DiagnosticStatus::ERROR, romea::DiagnosticStatus::OK);
}


//-----------------------------------------------------------------------------
int main(int argc, char ** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
