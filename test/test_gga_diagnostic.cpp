// Copyright 2022 INRAE, French National Research Institute for Agriculture, Food and Environment
// Add license

// gtest
#include <gtest/gtest.h>

// std
#include <string>

// romea
#include "helper.hpp"
#include "romea_core_localisation_gps/CheckupGGAFix.hpp"

class TestGGAFixDiagnostic : public ::testing::Test
{
public:
  TestGGAFixDiagnostic():
    frame(minimalGoodGGAFrame()),
    diagnostic(romea::FixQuality::RTK_FIX)
  {
  }

  romea::GGAFrame frame;
  romea::CheckupGGAFix diagnostic;
};

//-----------------------------------------------------------------------------
TEST_F(TestGGAFixDiagnostic, checkEmptyReport)
{
  EXPECT_TRUE(diagnostic.getReport().diagnostics.empty());
  EXPECT_STREQ(diagnostic.getReport().info.at("talker").c_str(), "");
  EXPECT_STREQ(diagnostic.getReport().info.at("longitude").c_str(), "");
  EXPECT_STREQ(diagnostic.getReport().info.at("latitude").c_str(), "");
  EXPECT_STREQ(diagnostic.getReport().info.at("geoid_height").c_str(), "");
  EXPECT_STREQ(diagnostic.getReport().info.at("altitude_above_geoid").c_str(), "");
  EXPECT_STREQ(diagnostic.getReport().info.at("hdop").c_str(), "");
  EXPECT_STREQ(diagnostic.getReport().info.at("number_of_satellites").c_str(), "");
  EXPECT_STREQ(diagnostic.getReport().info.at("correction_age").c_str(), "");
  EXPECT_STREQ(diagnostic.getReport().info.at("base_station_id").c_str(), "");
  EXPECT_STREQ(diagnostic.getReport().info.at("fix_quality").c_str(), "");
}

//-----------------------------------------------------------------------------
TEST_F(TestGGAFixDiagnostic, checkEmptyReportAfterReset)
{
  diagnostic.evaluate(frame);
  diagnostic.reset();
  EXPECT_TRUE(diagnostic.getReport().diagnostics.empty());
  EXPECT_STREQ(diagnostic.getReport().info.at("talker").c_str(), "");
  EXPECT_STREQ(diagnostic.getReport().info.at("longitude").c_str(), "");
  EXPECT_STREQ(diagnostic.getReport().info.at("latitude").c_str(), "");
  EXPECT_STREQ(diagnostic.getReport().info.at("geoid_height").c_str(), "");
  EXPECT_STREQ(diagnostic.getReport().info.at("altitude_above_geoid").c_str(), "");
  EXPECT_STREQ(diagnostic.getReport().info.at("hdop").c_str(), "");
  EXPECT_STREQ(diagnostic.getReport().info.at("number_of_satellites").c_str(), "");
  EXPECT_STREQ(diagnostic.getReport().info.at("correction_age").c_str(), "");
  EXPECT_STREQ(diagnostic.getReport().info.at("base_station_id").c_str(), "");
  EXPECT_STREQ(diagnostic.getReport().info.at("fix_quality").c_str(), "");
}

//-----------------------------------------------------------------------------
TEST_F(TestGGAFixDiagnostic, checkMinimalGoodFrame)
{
  EXPECT_EQ(diagnostic.evaluate(frame), romea::DiagnosticStatus::OK);
  EXPECT_EQ(diagnostic.getReport().diagnostics.front().status, romea::DiagnosticStatus::OK);
  EXPECT_STREQ(diagnostic.getReport().diagnostics.front().message.c_str(), "GGA fix OK.");
  EXPECT_STREQ(diagnostic.getReport().info.at("talker").c_str(), "GNSS");
  EXPECT_STREQ(diagnostic.getReport().info.at("longitude").c_str(), "0.03");
  EXPECT_STREQ(diagnostic.getReport().info.at("latitude").c_str(), "0.7854");
  EXPECT_STREQ(diagnostic.getReport().info.at("geoid_height").c_str(), "400.8");
  EXPECT_STREQ(diagnostic.getReport().info.at("altitude_above_geoid").c_str(), "53.3");
  EXPECT_STREQ(diagnostic.getReport().info.at("hdop").c_str(), "1.2");
  EXPECT_STREQ(diagnostic.getReport().info.at("number_of_satellites").c_str(), "12");
  EXPECT_STREQ(diagnostic.getReport().info.at("correction_age").c_str(), "2.5");
  EXPECT_STREQ(diagnostic.getReport().info.at("base_station_id").c_str(), "1");
  EXPECT_STREQ(diagnostic.getReport().info.at("fix_quality").c_str(), "rtk fix (4)");
}

//-----------------------------------------------------------------------------
void checkMissingData(romea::CheckupGGAFix & diagnostic,
                      const romea::GGAFrame & frame,
                      const std::string & missingDataName)
{
  EXPECT_EQ(diagnostic.evaluate(frame), romea::DiagnosticStatus::ERROR);
  EXPECT_EQ(diagnostic.getReport().diagnostics.front().status, romea::DiagnosticStatus::ERROR);
  EXPECT_STREQ(diagnostic.getReport().diagnostics.front().message.c_str(),
               "GGA fix is incomplete.");
  EXPECT_STREQ(diagnostic.getReport().info.at(missingDataName).c_str(), "");
}

//-----------------------------------------------------------------------------
void checkUnreliableData(romea::CheckupGGAFix & diagnostic,
                         const romea::GGAFrame & frame,
                         const std::string & message)
{
  EXPECT_EQ(diagnostic.evaluate(frame), romea::DiagnosticStatus::WARN);
  EXPECT_EQ(diagnostic.getReport().diagnostics.front().status, romea::DiagnosticStatus::WARN);
  EXPECT_STREQ(diagnostic.getReport().diagnostics.front().message.c_str(), message.c_str());
}

//-----------------------------------------------------------------------------
TEST_F(TestGGAFixDiagnostic, missingLontigude)
{
  frame.longitude.reset();
  checkMissingData(diagnostic, frame, "longitude");
}

//-----------------------------------------------------------------------------
TEST_F(TestGGAFixDiagnostic, missingLatitude)
{
  frame.latitude.reset();
  checkMissingData(diagnostic, frame, "latitude");
}

//-----------------------------------------------------------------------------
TEST_F(TestGGAFixDiagnostic, missingGeoidHeight)
{
  frame.geoidHeight.reset();
  checkMissingData(diagnostic, frame, "geoid_height");
}

//-----------------------------------------------------------------------------
TEST_F(TestGGAFixDiagnostic, missingAltitudeAboveGeoid)
{
  frame.altitudeAboveGeoid.reset();
  checkMissingData(diagnostic, frame, "altitude_above_geoid");
}

//-----------------------------------------------------------------------------
TEST_F(TestGGAFixDiagnostic, missingNumberOfSatellites)
{
  frame.numberSatellitesUsedToComputeFix.reset();
  checkMissingData(diagnostic, frame, "number_of_satellites");
}

//-----------------------------------------------------------------------------
TEST_F(TestGGAFixDiagnostic, missingHDOP)
{
  frame.horizontalDilutionOfPrecision.reset();
  checkMissingData(diagnostic, frame, "hdop");
}

//-----------------------------------------------------------------------------
TEST_F(TestGGAFixDiagnostic, missingFixQuality)
{
  frame.fixQuality.reset();
  checkMissingData(diagnostic, frame, "fix_quality");
}

//-----------------------------------------------------------------------------
TEST_F(TestGGAFixDiagnostic, unreliableNumberOfSatellites)
{
  frame.numberSatellitesUsedToComputeFix = 5;
  checkUnreliableData(diagnostic, frame, "Not enough satellites to compute fix.");
}

//-----------------------------------------------------------------------------
TEST_F(TestGGAFixDiagnostic, unreliableHDOP)
{
  frame.horizontalDilutionOfPrecision = 6.;
  checkUnreliableData(diagnostic, frame, "HDOP is two high.");
}

//-----------------------------------------------------------------------------
TEST_F(TestGGAFixDiagnostic, unreliableFixQuality)
{
  frame.fixQuality = romea::FixQuality::DGPS_FIX;
  checkUnreliableData(diagnostic, frame, "Fix quality is too low.");
}

//-----------------------------------------------------------------------------
TEST_F(TestGGAFixDiagnostic, severalUnreliableData)
{
  frame.fixQuality = romea::FixQuality::FLOAT_RTK_FIX;
  frame.horizontalDilutionOfPrecision = 5.2;

  EXPECT_EQ(diagnostic.evaluate(frame), romea::DiagnosticStatus::WARN);
  EXPECT_EQ(diagnostic.getReport().diagnostics.front().status, romea::DiagnosticStatus::WARN);
  EXPECT_EQ(diagnostic.getReport().diagnostics.back().status, romea::DiagnosticStatus::WARN);

  EXPECT_STREQ(diagnostic.getReport().diagnostics.front().message.c_str(),
               "HDOP is two high.");
  EXPECT_STREQ(diagnostic.getReport().diagnostics.back().message.c_str(),
               "Fix quality is too low.");
}


//-----------------------------------------------------------------------------
int main(int argc, char **argv){
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
