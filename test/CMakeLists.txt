find_package(GTest REQUIRED)

add_executable(${PROJECT_NAME}_test_gga_diagnostic test_gga_diagnostic.cpp)
target_link_libraries(${PROJECT_NAME}_test_gga_diagnostic ${PROJECT_NAME} GTest::GTest GTest::Main)
target_compile_options(${PROJECT_NAME}_test_gga_diagnostic PRIVATE -std=c++17)
add_test(test_gga_diagnostic ${PROJECT_NAME}_test_gga_diagnostic)

add_executable(${PROJECT_NAME}_test_hdt_diagnostic test_hdt_diagnostic.cpp)
target_link_libraries(${PROJECT_NAME}_test_hdt_diagnostic ${PROJECT_NAME} GTest::GTest GTest::Main)
target_compile_options(${PROJECT_NAME}_test_hdt_diagnostic PRIVATE -std=c++17)
add_test(test_hdt_diagnostic ${PROJECT_NAME}_test_hdt_diagnostic)

add_executable(${PROJECT_NAME}_test_rmc_diagnostic test_rmc_diagnostic.cpp)
target_link_libraries(${PROJECT_NAME}_test_rmc_diagnostic ${PROJECT_NAME} GTest::GTest GTest::Main)
target_compile_options(${PROJECT_NAME}_test_rmc_diagnostic PRIVATE -std=c++17)
add_test(test_rmc_diagnostic ${PROJECT_NAME}_test_rmc_diagnostic)

add_executable(${PROJECT_NAME}_test_single_antenna_gps_plugin test_single_antenna_gps_plugin.cpp)
target_link_libraries(${PROJECT_NAME}_test_single_antenna_gps_plugin ${PROJECT_NAME} GTest::GTest GTest::Main)
target_compile_options(${PROJECT_NAME}_test_single_antenna_gps_plugin PRIVATE -std=c++17)
add_test(test_single_antenna_gps_plugin ${PROJECT_NAME}_test_single_antenna_gps_plugin)

add_executable(${PROJECT_NAME}_test_dual_antenna_gps_plugin test_dual_antenna_gps_plugin.cpp)
target_link_libraries(${PROJECT_NAME}_test_dual_antenna_gps_plugin ${PROJECT_NAME} GTest::GTest GTest::Main)
target_compile_options(${PROJECT_NAME}_test_dual_antenna_gps_plugin PRIVATE -std=c++17)
add_test(test_dual_antenna_gps_plugin ${PROJECT_NAME}_test_dual_antenna_gps_plugin)
