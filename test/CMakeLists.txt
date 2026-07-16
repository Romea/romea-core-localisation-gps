find_package(GTest REQUIRED)
include(romea_add_gtest)

romea_add_gtest(test_gga_diagnostic test_gga_diagnostic.cpp LIBRARIES ${PROJECT_NAME})

romea_add_gtest(test_hdt_diagnostic test_hdt_diagnostic.cpp LIBRARIES ${PROJECT_NAME})

romea_add_gtest(test_rmc_diagnostic test_rmc_diagnostic.cpp LIBRARIES ${PROJECT_NAME})

romea_add_gtest(test_single_antenna_gps_plugin test_single_antenna_gps_plugin.cpp LIBRARIES ${PROJECT_NAME})

romea_add_gtest(test_dual_antenna_gps_plugin test_dual_antenna_gps_plugin.cpp LIBRARIES ${PROJECT_NAME})
