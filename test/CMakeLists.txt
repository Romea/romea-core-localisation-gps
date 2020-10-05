find_package(GTest REQUIRED)

add_executable(${PROJECT_NAME}_test_gga_diagnostic test_gga_diagnostic.cpp )
target_link_libraries(${PROJECT_NAME}_test_gga_diagnostic ${PROJECT_NAME} ${catkin_LIBRARIES} GTest::GTest GTest::Main)
target_compile_options(${PROJECT_NAME}_test_gga_diagnostic PRIVATE -std=c++17)
add_test(test_gga_diagnostic ${PROJECT_NAME}_test_gga_diagnostic)

add_executable(${PROJECT_NAME}_test_rmc_diagnostic test_rmc_diagnostic.cpp )
target_link_libraries(${PROJECT_NAME}_test_rmc_diagnostic ${PROJECT_NAME} ${catkin_LIBRARIES} GTest::GTest GTest::Main)
target_compile_options(${PROJECT_NAME}_test_rmc_diagnostic PRIVATE -std=c++17)
add_test(test_rmc_diagnostic ${PROJECT_NAME}_test_rmc_diagnostic)

add_executable(${PROJECT_NAME}_test_gps_plugin test_gps_plugin.cpp )
target_link_libraries(${PROJECT_NAME}_test_gps_plugin ${PROJECT_NAME} ${catkin_LIBRARIES} GTest::GTest GTest::Main)
target_compile_options(${PROJECT_NAME}_test_gps_plugin PRIVATE -std=c++17)
add_test(test_gps_plugin ${PROJECT_NAME}_test_gps_plugin)
