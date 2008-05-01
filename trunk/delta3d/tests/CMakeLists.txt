
SET(PROG_NAME     allTests)

INCLUDE_DIRECTORIES( ${DELTA3D_SOURCE_DIR}/examples
                     ${DELTA3D_SOURCE_DIR}/utilities) 

SET(DIRS dtABC dtAI dtActors dtAnim dtAudio dtCore dtDAL dtGUI dtGame dtLMS dtUtil)
IF(DIS_FOUND)
  SET(DIRS ${DIRS} dtDIS)
ENDIF(DIS_FOUND)

file(GLOB TEMP_SOURCES "*.cpp" "*.h")
SET(ALL_SOURCES ${TEMP_SOURCES}
  ${DELTA3D_SOURCE_DIR}/utilities/LMS/WebPackager/package_utils.h
  ${DELTA3D_SOURCE_DIR}/utilities/LMS/WebPackager/package_utils.cpp)

FOREACH(varname ${DIRS}) 
  file(GLOB TEMP_SOURCES "${varname}/*.cpp" "${varname}/*.h")
  SET(ALL_SOURCES ${ALL_SOURCES} ${TEMP_SOURCES})
ENDFOREACH(varname)

IF (BUILD_HLA)
  INCLUDE_DIRECTORIES(${RTIS_INCLUDE_DIR}) 
  file(GLOB TEMP_SOURCES "dtHLAGM/*.cpp" "dtHLAGM/*.h")
  SET(ALL_SOURCES ${ALL_SOURCES} ${TEMP_SOURCES})
ENDIF (BUILD_HLA)

list(REMOVE_ITEM ALL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/precomp.cpp)

ADD_EXECUTABLE(${PROG_NAME}
    ${ALL_SOURCES}
)

LINK_WITH_VARIABLES(${PROG_NAME}
					 DTUTIL_LIBRARY   
                                         DTCORE_LIBRARY
					 DTAUDIO_LIBRARY   
                                         DTAI_LIBRARY
					 DTDAL_LIBRARY   
                                         DTLMS_LIBRARY
					 DTGUI_LIBRARY   
                                         DTACTORS_LIBRARY
					 DTANIM_LIBRARY   
                                         DTABC_LIBRARY
					 TEST_ACTOR_LIBRARY
                                         TEST_GAME_ACTOR_LIBRARY
					 CPPUNIT_LIBRARY)                                         
IF(DIS_FOUND)
LINK_WITH_VARIABLES(${PROG_NAME}
					 DTDIS_LIBRARY
					 DIS_LIBRARY)                                         
ENDIF(DIS_FOUND)
                                              
INCLUDE(ModuleInstall OPTIONAL)

IF (BUILD_HLA)
  LINK_WITH_VARIABLES(${PROG_NAME}  DTHLAGM_LIBRARY)
  TARGET_LINK_LIBRARIES(${PROG_NAME} 
    ${RTIS_NG_LIBRARY}
    ${RTIS_MEMPOOL_LIBRARY}
    ${RTIS_13_LIBRARY}
    ${RTIS_URLREADER_LIBRARY}
    ${RTIS_BITVECTOR_LIBRARY}
    ${RTIS_MERSENNE_LIBRARY}
    ${RTIS_VTERM_LIBRARY}
    ${RTIS_BUFMGR_LIBRARY}
    ${RTIS_MSGSTATS_LIBRARY}
    ${RTIS_RTIFEDTIME_LIBRARY}
    ${RTIS_XREGEX_LIBRARY}
    ${RTIS_FEDTIME_LIBRARY}
    ${RTIS_NETUTIL_LIBRARY}
    ${RTIS_RTIINTERCEPT_LIBRARY}
    ${RTIS_Z_LIBRARY}
    ${RTIS_FEDTIME13_LIBRARY}
    ${RTIS_PARSER_LIBRARY}
    ${RTIS_LIBRARY}
    ${RTIS_IF_LIBRARY}
    ${RTIS_RID_LIBRARY}
    ${RTIS_MCAST_LIBRARY}
    ${RTIS_RTI_LIBRARY}
    ${RTIS_TIMERS_LIBRARY}
   )
ENDIF (BUILD_HLA)

SET_TARGET_PROPERTIES(${PROG_NAME} PROPERTIES PREFIX "../tests/")
