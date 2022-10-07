// gtest
#include <gtest/gtest.h>

//romea
#include "helper.hpp"
#include "romea_core_localisation_gps/CheckupRMCTrackAngle.hpp"

class TestRMCTrackAngleDiagnostic : public ::testing::Test
{
public:

  TestRMCTrackAngleDiagnostic():
    frame(minimalGoodRMCFrame()),
    diagnostic(1)
  {
  }

  romea::RMCFrame frame;
  romea::CheckupRMCTrackAngle diagnostic;
};


//-----------------------------------------------------------------------------
TEST_F(TestRMCTrackAngleDiagnostic, checkEmptyReport)
{
  EXPECT_TRUE(diagnostic.getReport().diagnostics.empty());
  EXPECT_STREQ(diagnostic.getReport().info.at("talker").c_str(),"");
  EXPECT_STREQ(diagnostic.getReport().info.at("speed_over_ground").c_str(),"");
  EXPECT_STREQ(diagnostic.getReport().info.at("track_angle").c_str(),"");
  EXPECT_STREQ(diagnostic.getReport().info.at("magnetic_deviation").c_str(),"");
}

//-----------------------------------------------------------------------------
TEST_F(TestRMCTrackAngleDiagnostic, checkEmptyReportAfterReset)
{
  diagnostic.evaluate(frame);
  diagnostic.reset();
  EXPECT_TRUE(diagnostic.getReport().diagnostics.empty());
  EXPECT_STREQ(diagnostic.getReport().info.at("talker").c_str(),"");
  EXPECT_STREQ(diagnostic.getReport().info.at("speed_over_ground").c_str(),"");
  EXPECT_STREQ(diagnostic.getReport().info.at("track_angle").c_str(),"");
  EXPECT_STREQ(diagnostic.getReport().info.at("magnetic_deviation").c_str(),"");
}

//-----------------------------------------------------------------------------
TEST_F(TestRMCTrackAngleDiagnostic, checkMinimalGoodFrame)
{
  EXPECT_EQ(diagnostic.evaluate(frame),romea::DiagnosticStatus::OK);
  EXPECT_EQ(diagnostic.getReport().diagnostics.front().status,romea::DiagnosticStatus::OK);
  EXPECT_STREQ(diagnostic.getReport().diagnostics.front().message.c_str(),"RMC track angle OK.");
  EXPECT_STREQ(diagnostic.getReport().info.at("talker").c_str(),"NAVSTAR");
  EXPECT_STREQ(diagnostic.getReport().info.at("speed_over_ground").c_str(),"3.2");
  EXPECT_STREQ(diagnostic.getReport().info.at("track_angle").c_str(),"1.54");
  EXPECT_STREQ(diagnostic.getReport().info.at("magnetic_deviation").c_str(),"0.0378");
}

//-----------------------------------------------------------------------------
void checkMissingData(romea::CheckupRMCTrackAngle & diagnostic,
                      const romea::RMCFrame & frame,
                      const std::string & missingDataName)
{
  EXPECT_EQ(diagnostic.evaluate(frame),romea::DiagnosticStatus::ERROR);
  EXPECT_EQ(diagnostic.getReport().diagnostics.front().status,romea::DiagnosticStatus::ERROR);
  EXPECT_STREQ(diagnostic.getReport().diagnostics.front().message.c_str(),"RMC track angle is incomplete.");
  EXPECT_STREQ(diagnostic.getReport().info.at(missingDataName).c_str(),"");
}

//-----------------------------------------------------------------------------
TEST_F(TestRMCTrackAngleDiagnostic, checkMissingSpeedOverGround)
{
  frame.speedOverGroundInMeterPerSecond.reset();
  checkMissingData(diagnostic,frame,"speed_over_ground");
}

//-----------------------------------------------------------------------------
TEST_F(TestRMCTrackAngleDiagnostic, checkMissingTrackAngle)
{
  frame.trackAngleTrue.reset();
  checkMissingData(diagnostic,frame,"track_angle");
}

//-----------------------------------------------------------------------------
TEST_F(TestRMCTrackAngleDiagnostic, checkUnreliableTrackAngle)
{
  frame.speedOverGroundInMeterPerSecond=0.5;
  EXPECT_EQ(diagnostic.evaluate(frame),romea::DiagnosticStatus::WARN);
  EXPECT_EQ(diagnostic.getReport().diagnostics.front().status,romea::DiagnosticStatus::WARN);
  EXPECT_STREQ(diagnostic.getReport().diagnostics.front().message.c_str(),"RMC track angle is not reliable because vehicle speed is lower than 1 m/s.");

}

//-----------------------------------------------------------------------------
int main(int argc, char **argv){
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
