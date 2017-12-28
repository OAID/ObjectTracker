# Open The CMake GUI
# specify the source directory and the binary directory
# press configure.
#
# UNIX Makefile:
# 1) go to the binary folder and type "make" to build the project
#
# Microsoft Visual C++:
# 1) open the .sln file
# 2) build the project "ALL_BUILD" to build the opentld project


project(CF_TLD)
cmake_minimum_required(VERSION 2.6)

# add c++11 support
if(CMAKE_COMPILER_IS_GNUCC)
    ADD_DEFINITIONS ( -std=c++11 )
endif(CMAKE_COMPILER_IS_GNUCC)

set(OpenCV_DIR "/usr/local/AID/opencv3.3.0")
#set(OpenCV_DIR "/home/firefly/opencv-3.3.0/build")

#find_package(OpenCV REQUIRED highgui video videoio imgproc imgcodecs)

# find_package(OpenCV) does not account for proper path setup
# on windows; proper path setup requires path "OPENCV_DIR"
# pointing to ie. \build_opencv_3\install\x64\vc12
set(OPENCV_DIR_HINT "/usr/local/AID/opencv3.3.0")

if(WIN32)
    get_filename_component(OPENCV_DIR_PLATFORM $ENV{OPENCV_DIR} DIRECTORY)
    get_filename_component(OPENCV_DIR_HINT ${OPENCV_DIR_PLATFORM} DIRECTORY)
endif(WIN32)

set(OpenCV_STATIC OFF)
find_package(OpenCV REQUIRED HINTS ${OPENCV_DIR_HINT})
# OpenCV 3.0 does currently not include its' includes correctly
include_directories(${OpenCV_DIR}/include)


#-------------------------------------------------------------------------------
#version
set(TLD_VERSION_MAJOR 1)
set(TLD_VERSION_MINOR 4)
set(TLD_VERSION_PATCH 0)
set(TLD_VERSION ${TLD_VERSION_MAJOR}.${TLD_VERSION_MINOR}.${TLD_VERSION_PATCH})

#------------------------------------------------------------------------------
#build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING
        "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
        FORCE)
endif(NOT CMAKE_BUILD_TYPE)

option(USE_SYSTEM_LIBS "Use the installed version of libconfig++." OFF)
option(WITH_OPENMP "Use OpenMP." OFF)

if(WITH_OPENMP)
    find_package(OpenMP REQUIRED)

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif(WITH_OPENMP)

if(WIN32)
    add_definitions(-DLIBCONFIGXX_STATIC -DLIBCONFIG_STATIC) #Needed when linking libconfig statically
endif(WIN32)

if(APPLE)
    add_definitions(-DHAVE_XLOCALE_H)
endif(APPLE)

if(NOT MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif(NOT MSVC)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

#-------------------------------------------------------------------------------
#add subdirectories
add_subdirectory(src/3rdparty/cf_tracking)

if(NOT USE_SYSTEM_LIBS)
    add_subdirectory(src/3rdparty/libconfig)
endif(NOT USE_SYSTEM_LIBS)

add_subdirectory(src/libopentld)
add_subdirectory(src/opentld)

configure_file("${PROJECT_SOURCE_DIR}/OpenTLDConfig.cmake.in" "${PROJECT_BINARY_DIR}/OpenTLDConfig.cmake" @ONLY)
