## MOPET Project
## CMakeLists file

cmake_minimum_required(VERSION 2.6)
project(mopet)

# OpenCV support
set(CMAKE_INCLUDE_DIRS_CONFIGCMAKE "${CMAKE_INSTALL_PREFIX}/include/opencv" "${CMAKE_INSTALL_PREFIX}/include")
find_package(OpenCV)
include_directories(${OPENCV_INCLUDE_DIR})

set(CMAKE_CXX_FLAGS "-O2 -Wall")
#set(CMAKE_CXX_FLAGS "-g -Wall")


# Project config
add_executable(
  mopet
  main.cpp
  helper.h
  helper.cpp
  preprocessor.h
  preprocessor.cpp
  VarianceForegroundClassifier.h
  VarianceForegroundClassifier.cpp
  ColorVarianceForegroundClassifier.h
  ColorVarianceForegroundClassifier.cpp
  TestForegroundClassifier.h
  TestForegroundClassifier.cpp
  InPainter.h
  InPainter.cpp
)

target_link_libraries(
  mopet
  opencv_core
  opencv_imgproc
  opencv_highgui
  opencv_features2d
  opencv_calib3d
  opencv_nonfree
  opencv_flann
)

# If your compiler supports OpenMP, it may be a good idea
# to speed things up a little
include(FindOpenMP)
if(OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()
