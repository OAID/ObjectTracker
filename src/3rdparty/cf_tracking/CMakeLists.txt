set(CF_CV_EXT "src/3rdparty/cv_ext")
set(CF_PIOTR_DIR "src/3rdparty/piotr/")

set(CF_HEADER_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/${CF_CV_EXT}
	${CMAKE_CURRENT_SOURCE_DIR}/${CF_PIOTR_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/${CF_PIOTR_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cf_libs/kcf
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cf_libs/dsst
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cf_libs/common
    PARENT_SCOPE)
	
set(CF_PIOTR_SOURCES
    ${CF_PIOTR_DIR}/src/gradientMex.cpp
    ${CF_PIOTR_DIR}/gradientMex.hpp
    ${CF_PIOTR_DIR}/src/sse.hpp
    ${CF_PIOTR_DIR}/src/wrappers.hpp)

set(CF_LIB_COMMON_SOURCES
    src/cf_libs/common/feature_channels.hpp
    src/cf_libs/common/mat_consts.hpp
    src/cf_libs/common/math_helper.hpp
    src/cf_libs/common/math_helper.cpp
    src/cf_libs/common/cf_tracker.hpp
    src/cf_libs/common/tracker_debug.hpp
	src/cf_libs/common/scale_estimator.hpp
    src/cf_libs/common/cv_ext.hpp
	${CF_CV_EXT}/shift.cpp
	${CF_CV_EXT}/shift.hpp
	${CF_CV_EXT}/math_spectrums.cpp
	${CF_CV_EXT}/math_spectrums.hpp
	${CF_PIOTR_SOURCES})

set(CF_SOURCES_
    src/cf_libs/dsst/dsst_debug.hpp
    src/cf_libs/dsst/dsst_tracker.hpp
	src/cf_libs/kcf/kcf_debug.hpp
    src/cf_libs/kcf/kcf_tracker.hpp
    ${CF_LIB_COMMON_SOURCES}
    )

function(PREFIX prefix list_)
    SET(tmp_list)

    foreach(SOURCE_FILE ${${list_}})
        list(APPEND tmp_list "${prefix}${SOURCE_FILE}")
    endforeach(SOURCE_FILE)

    set(${list_} ${tmp_list} PARENT_SCOPE)
endfunction(PREFIX)

PREFIX("${CMAKE_CURRENT_SOURCE_DIR}/" "CF_SOURCES_")
set(CF_SOURCES ${CF_SOURCES_} PARENT_SCOPE)
# message(STATUS ${CF_SOURCES_})


