// gtest
#include <gtest/gtest.h>

//romea
#include "helper.hpp"
#include "romea_core_localisation_gps/LocalisationGPSPlugin.hpp"

bool boolean(const romea::DiagnosticStatus & status)
{
  return status == romea::DiagnosticStatus::OK;
}

class TestGPSPlugin : public ::testing::Test
{
public:

  TestGPSPlugin():
    stamp(),
    gga_frame(minimalGoodGGAFrame()),
    rmc_frame(minimalGoodRMCFrame()),
    linear_speed(2.0),
    course(),
    position(),
    gps_plugin(nullptr)
  {
  }

  virtual void SetUp() override
  {
    auto gps = std::make_unique<romea::GPSReceiver>();
    gps->setAntennaBodyPosition(Eigen::Vector3d(0.3,0,2.));
    gps->setUERE(romea::FixQuality::RTK_FIX,0.02);

    gps_plugin = std::make_unique<romea::LocalisationGPSPlugin>(
          std::move(gps),romea::FixQuality::RTK_FIX,1.);
  }


  const romea::Diagnostic & diagnostic(const size_t & index)
  {
    return *std::next(std::cbegin(report.diagnostics),index);
  }


  void check(const romea::DiagnosticStatus & finalFixStatus,
             const romea::DiagnosticStatus & finalTrackStatus,
             const romea::DiagnosticStatus & finalLinearSpeedStatus)
  {
    std::string gga_sentence = gga_frame.toNMEA();
    std::string rmc_sentence = rmc_frame.toNMEA();


    if( std::isfinite(linear_speed))
    {
      for(size_t n=0;n<=20;++n)
      {
        romea::Duration stamp = romea::durationFromSecond(n/20.);
        gps_plugin->processLinearSpeed(stamp,linear_speed);
      }
    }

    for(size_t n=0;n<4;++n)
    {
      romea::Duration stamp = romea::durationFromSecond(0.5+n/10.);
      EXPECT_FALSE(gps_plugin->processGGA(stamp,gga_sentence,position));
      EXPECT_FALSE(gps_plugin->processRMC(stamp,rmc_sentence,course));
      report = gps_plugin->makeDiagnosticReport(stamp);

      EXPECT_EQ(diagnostic(1).status,romea::DiagnosticStatus::ERROR);
      EXPECT_EQ(diagnostic(2).status,romea::DiagnosticStatus::STALE);
      EXPECT_EQ(diagnostic(3).status,romea::DiagnosticStatus::ERROR);
      EXPECT_EQ(diagnostic(4).status,romea::DiagnosticStatus::STALE);

    }

    romea::Duration stamp = romea::durationFromSecond(0.5);
    EXPECT_EQ(gps_plugin->processGGA(stamp,gga_sentence,position),boolean(finalFixStatus));
    EXPECT_EQ(gps_plugin->processRMC(stamp,rmc_sentence,course),boolean(finalTrackStatus) && boolean(finalLinearSpeedStatus));
    report = gps_plugin->makeDiagnosticReport(stamp);
    EXPECT_EQ(diagnostic(0).status,finalLinearSpeedStatus);
    EXPECT_EQ(diagnostic(1).status,romea::DiagnosticStatus::OK);
    EXPECT_EQ(diagnostic(2).status,finalFixStatus);
    EXPECT_EQ(diagnostic(3).status,romea::DiagnosticStatus::OK);
    EXPECT_EQ(diagnostic(4).status,finalTrackStatus);
  }

  romea::Duration stamp;
  romea::GGAFrame gga_frame;
  romea::RMCFrame rmc_frame;
  double  linear_speed;

  romea::ObservationCourse course;
  romea::ObservationPosition position;
  std::unique_ptr<romea::LocalisationGPSPlugin> gps_plugin;

  romea::DiagnosticReport report;
};




//-----------------------------------------------------------------------------
TEST_F(TestGPSPlugin, testAllOk)
{
  check(romea::DiagnosticStatus::OK,
        romea::DiagnosticStatus::OK,
        romea::DiagnosticStatus::OK);
}

//-----------------------------------------------------------------------------
TEST_F(TestGPSPlugin, testFixOKTrakAngleWarn)
{
  rmc_frame.speedOverGroundInMeterPerSecond=0.5;
  check(romea::DiagnosticStatus::OK,
        romea::DiagnosticStatus::WARN,
        romea::DiagnosticStatus::OK);
}

//-----------------------------------------------------------------------------
TEST_F(TestGPSPlugin, testFixOKTrakAngleError)
{
  rmc_frame.trackAngleTrue.reset();
  check(romea::DiagnosticStatus::OK,
        romea::DiagnosticStatus::ERROR,
        romea::DiagnosticStatus::OK);
}

//-----------------------------------------------------------------------------
TEST_F(TestGPSPlugin, testFixWarnTrackAngleOK)
{
  gga_frame.numberSatellitesUsedToComputeFix=5;
  check(romea::DiagnosticStatus::WARN,
        romea::DiagnosticStatus::OK,
        romea::DiagnosticStatus::OK);
}

//-----------------------------------------------------------------------------
TEST_F(TestGPSPlugin, testFixErrorTrackAngleOK)
{
  gga_frame.fixQuality.reset();
  check(romea::DiagnosticStatus::ERROR,
        romea::DiagnosticStatus::OK,
        romea::DiagnosticStatus::OK);
}

//-----------------------------------------------------------------------------
TEST_F(TestGPSPlugin, testCannotComputeCourseBecauseLinearSpeedIsMissing)
{
  linear_speed=std::numeric_limits<double>::quiet_NaN();
  check(romea::DiagnosticStatus::OK,
        romea::DiagnosticStatus::OK,
        romea::DiagnosticStatus::ERROR);
}


//-----------------------------------------------------------------------------
int main(int argc, char **argv){
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
