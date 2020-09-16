// gtest
#include <gtest/gtest.h>
#
//romea
#include "romea_gps_localisation_plugin/gga_diagnostic2.hpp"

romea::GGAFrame goodGGAFrame()
{
  romea::GGAFrame frame;
  frame.talkerId=romea::TalkerId::GN;
  frame.longitude=romea::Longitude(0.03);
  frame.latitude=romea::Latitude(0.7854);
  frame.geoidHeight=400.8;
  frame.altitudeAboveGeoid=53.3;
  frame.horizontalDilutionOfPrecision=1.2;
  frame.numberSatellitesUsedToComputeFix=12;
  frame.fixQuality = romea::FixQuality::RTK_FIX;
  frame.horizontalDilutionOfPrecision=1.2;
  frame.dgpsCorrectionAgeInSecond=2.5;
  frame.dgpsStationIdNumber=1;
  return frame;
}

class TestGGAFixDiagnostic : public ::testing::Test
{
public:

  TestGGAFixDiagnostic():
    frame(goodGGAFrame()),
    diagnostic(romea::FixQuality::RTK_FIX)
  {
  }

  romea::GGAFrame frame;
  romea::DiagnosticGGAFix2 diagnostic;
};

//-----------------------------------------------------------------------------
TEST_F(TestGGAFixDiagnostic, checkGoodFrame)
{
  EXPECT_EQ(diagnostic.evaluate(frame),romea::DiagnosticStatus::OK);
  EXPECT_EQ(diagnostic.getReport().status,romea::DiagnosticStatus::OK);
  EXPECT_STREQ(diagnostic.getReport().message.c_str(),"GGA fix OK.");
  EXPECT_STREQ(diagnostic.getReport().info.at("talker").c_str(),"GNSS");
  EXPECT_STREQ(diagnostic.getReport().info.at("longitude").c_str(),"0.03");
  EXPECT_STREQ(diagnostic.getReport().info.at("latitude").c_str(),"0.7854");
  EXPECT_STREQ(diagnostic.getReport().info.at("geoid_height").c_str(),"400.8");
  EXPECT_STREQ(diagnostic.getReport().info.at("altitude_above_geoid").c_str(),"53.3");
  EXPECT_STREQ(diagnostic.getReport().info.at("hdop").c_str(),"1.2");
  EXPECT_STREQ(diagnostic.getReport().info.at("number_of_satellites").c_str(),"12");
  EXPECT_STREQ(diagnostic.getReport().info.at("correction_age").c_str(),"2.5");
  EXPECT_STREQ(diagnostic.getReport().info.at("base_station_id").c_str(),"1");
  EXPECT_STREQ(diagnostic.getReport().info.at("fix_quality").c_str(),"rtk fix (4)");
}

//-----------------------------------------------------------------------------
void checkMissingData(romea::DiagnosticGGAFix2 & diagnostic,
                      const romea::GGAFrame & frame,
                      const std::string & missingDataName)
{
  EXPECT_EQ(diagnostic.evaluate(frame),romea::DiagnosticStatus::ERROR);
  EXPECT_EQ(diagnostic.getReport().status,romea::DiagnosticStatus::ERROR);
  EXPECT_STREQ(diagnostic.getReport().message.c_str(),"GGA fix incomplete.");
  EXPECT_STREQ(diagnostic.getReport().info.at(missingDataName).c_str(),"");
}

//-----------------------------------------------------------------------------
void checkUnreliableData(romea::DiagnosticGGAFix2 & diagnostic,
                         const romea::GGAFrame & frame,
                         const std::string & warningMessage)
{
  std::string message = "GGA fix not reliable : "+warningMessage+".";
  EXPECT_EQ(diagnostic.evaluate(frame),romea::DiagnosticStatus::WARN);
  EXPECT_EQ(diagnostic.getReport().status,romea::DiagnosticStatus::WARN);
  EXPECT_STREQ(diagnostic.getReport().message.c_str(),message.c_str());
}


//-----------------------------------------------------------------------------
TEST_F(TestGGAFixDiagnostic, missingLontigude)
{
  frame.longitude.reset();
  checkMissingData(diagnostic,frame,"longitude");
}

//-----------------------------------------------------------------------------
TEST_F(TestGGAFixDiagnostic, missingLatitude)
{
  frame.latitude.reset();
  checkMissingData(diagnostic,frame,"latitude");
}

//-----------------------------------------------------------------------------
TEST_F(TestGGAFixDiagnostic, missingGeoidHeight)
{
  frame.geoidHeight.reset();
  checkMissingData(diagnostic,frame,"geoid_height");
}

//-----------------------------------------------------------------------------
TEST_F(TestGGAFixDiagnostic, missingAltitudeAboveGeoid)
{
  frame.altitudeAboveGeoid.reset();
  checkMissingData(diagnostic,frame,"altitude_above_geoid");
}

//-----------------------------------------------------------------------------
TEST_F(TestGGAFixDiagnostic, missingNumberOfSatellites)
{
  frame.numberSatellitesUsedToComputeFix.reset();
  checkMissingData(diagnostic,frame,"number_of_satellites");
}

//-----------------------------------------------------------------------------
TEST_F(TestGGAFixDiagnostic, missingHDOP)
{
  frame.horizontalDilutionOfPrecision.reset();
  checkMissingData(diagnostic,frame,"hdop");
}

//-----------------------------------------------------------------------------
TEST_F(TestGGAFixDiagnostic, missingFixQuality)
{
  frame.fixQuality.reset();
  checkMissingData(diagnostic,frame,"fix_quality");
}

//-----------------------------------------------------------------------------
TEST_F(TestGGAFixDiagnostic, unreliableNumberOfSatellites)
{
  frame.numberSatellitesUsedToComputeFix=5;
  checkUnreliableData(diagnostic,frame,"not enough satellites to compute fix");
}

//-----------------------------------------------------------------------------
TEST_F(TestGGAFixDiagnostic, unreliableHDOP)
{
  frame.horizontalDilutionOfPrecision=6.;
  checkUnreliableData(diagnostic,frame,"HDOP is two high");
}

//-----------------------------------------------------------------------------
TEST_F(TestGGAFixDiagnostic, unreliableFixQuality)
{
  frame.fixQuality=romea::FixQuality::DGPS_FIX;
  checkUnreliableData(diagnostic,frame,"fix quality is too low");
}

//-----------------------------------------------------------------------------
TEST_F(TestGGAFixDiagnostic, severalUnreliableData)
{
  frame.fixQuality=romea::FixQuality::FLOAT_RTK_FIX;
  frame.horizontalDilutionOfPrecision=5.2;
  checkUnreliableData(diagnostic,frame,"HDOP is two high, fix quality is too low");
}


//-----------------------------------------------------------------------------
int main(int argc, char **argv){
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}