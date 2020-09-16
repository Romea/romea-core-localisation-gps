cmake_minimum_required(VERSION 2.8.3)
project(romea_gps_localisation_plugin2)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++17")


#######################
## Find dependencies ##
#######################
set(${PROJECT_NAME}_CATKIN_DEPS romea_gps romea_localisation)
find_package(catkin REQUIRED COMPONENTS ${${PROJECT_NAME}_CATKIN_DEPS})
find_package(romea_filtering REQUIRED)
find_package( PkgConfig )
pkg_check_modules( EIGEN3 REQUIRED eigen3 )

###################################
## catkin specific configuration ##
###################################
catkin_package(
  INCLUDE_DIRS include
  LIBRARIES ${PROJECT_NAME}
  CATKIN_DEPENDS ${${PROJECT_NAME}_CATKIN_DEPS})

###########
## build ##
###########
include_directories(
    include
    /usr/include/eigen3
    SYSTEM ${catkin_INCLUDE_DIRS}
#    SYSTEM ${EIGEN3_INCLUDE_DIRS},
    SYSTEM ${romea_filtering_INCLUDE_DIRS})

add_library(romea_gps_localisation_plugin2
    src/CheckupGGAFix.cpp
    src/CheckupRMCTrackAngle.cpp
    src/gps_localisation_plugin.cpp)

target_link_libraries(romea_gps_localisation_plugin2 ${catkin_LIBRARIES})

if(CATKIN_ENABLE_TESTING)
  add_subdirectory(test)
endif(CATKIN_ENABLE_TESTING)
