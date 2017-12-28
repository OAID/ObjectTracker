
link_directories(${OpenCV_LIB_DIR})

include_directories(main
    ../libopentld/imacq
    ../libopentld/mftracker
    ../libopentld/tld
    ${CF_HEADER_DIRS}
    ${OpenCV_INCLUDE_DIRS})

if(NOT USE_SYSTEM_LIBS)
    include_directories(../3rdparty/libconfig)
endif(NOT USE_SYSTEM_LIBS)

#-------------------------------------------------------------------------------
# main
add_library(main
    main/Config.cpp
    main/Gui.cpp
    main/Main.cpp
    main/Settings.cpp
    main/Trajectory.cpp
    main/Config.h
    main/Gui.h
    main/Main.h
    main/Settings.h
    main/Trajectory.h
    ../3rdparty/cf_tracking/src/cf_libs/common/cf_tracker.hpp)

target_link_libraries(main libopentld config++ ${OpenCV_LIBS})

#-------------------------------------------------------------------------------
# opentld
add_executable(cftld
    OpenTLD.cpp)

target_link_libraries(cftld main libopentld config++ ${OpenCV_LIBS})

install(TARGETS cftld DESTINATION bin)

#-------------------------------------------------------------------------------


