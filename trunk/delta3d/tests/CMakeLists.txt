
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
  TARGET_LINK_LIBRARIES(${PROG_NAME} ${RTIS_LIBRARIES})
ENDIF (BUILD_HLA)

SET_TARGET_PROPERTIES(${PROG_NAME} PROPERTIES PREFIX "../tests/")

IF (WIN32)
  SET_TARGET_PROPERTIES(${PROG_NAME} PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")
ENDIF (WIN32)
