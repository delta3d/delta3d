
SET(PROG_NAME     allTests)

INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}/examples
                     ${CMAKE_SOURCE_DIR}/utilities) 

SET(DIRS dtABC dtAI dtActors dtAnim dtAudio dtCore dtDAL dtGUI dtGame dtLMS dtUtil)
IF(DTDIS_AVAILABLE)
  SET(DIRS ${DIRS} dtDIS)
ENDIF(DTDIS_AVAILABLE)

file(GLOB TEMP_SOURCES "*.cpp" "*.h")
SET(ALL_SOURCES ${TEMP_SOURCES}
  ${DELTA3D_SOURCE_DIR}/utilities/LMS/WebPackager/package_utils.h
  ${DELTA3D_SOURCE_DIR}/utilities/LMS/WebPackager/package_utils.cpp)


IF (BUILD_HLA)
  INCLUDE_DIRECTORIES(${RTI_INCLUDE_DIR}) 
  SET (DIRS ${DIRS} dtHLAGM)
ENDIF (BUILD_HLA)

IF (BUILD_DEMOS)
  SET(DIRS ${DIRS} "demos/firefighter")
ENDIF (BUILD_DEMOS)

FOREACH(varname ${DIRS}) 
  file(GLOB TEMP_SOURCES "${varname}/*.cpp" "${varname}/*.h")
  SOURCE_GROUP( ${varname} FILES ${TEMP_SOURCES} )
  SET(ALL_SOURCES ${ALL_SOURCES} ${TEMP_SOURCES})
ENDFOREACH(varname)
list(REMOVE_ITEM ALL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/precomp.cpp)


INCLUDE_DIRECTORIES( ${CEGUI_INCLUDE_DIR} #for dtGUI
                                    ${CAL3D_INCLUDE_DIR} #for dtAnim
                                    )


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

IF(DTDIS_AVAILABLE)
   LINK_WITH_VARIABLES(${PROG_NAME}
                        DTDIS_LIBRARY
                        DIS_LIBRARY)
ENDIF(DTDIS_AVAILABLE)

INCLUDE(ModuleInstall OPTIONAL)

IF (BUILD_HLA)
  LINK_WITH_VARIABLES(${PROG_NAME}  DTHLAGM_LIBRARY)
  TARGET_LINK_LIBRARIES(${PROG_NAME} ${RTI_LIBRARIES})
ENDIF (BUILD_HLA)

IF (BUILD_DEMOS)
    LINK_WITH_VARIABLES( ${PROG_NAME} FIREFIGHTER_DEMO_LIBRARY )
    #fireFighter needs the demos folder added for #include search paths
    INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/demos")  
ENDIF (BUILD_DEMOS)

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/tests) #drop the .exe into the delta3d/tests folder

IF (MSVC)
  SET_TARGET_PROPERTIES(${PROG_NAME} PROPERTIES PREFIX "/../") #little hack to get the .exe into the correct folder
  SET_TARGET_PROPERTIES(${PROG_NAME} PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")#tack on the "d" for debug
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4290") #ignore the C4290 warning
ELSE (MSVC)
  SET_TARGET_PROPERTIES(${PROG_NAME} PROPERTIES PREFIX "../tests/")
ENDIF (MSVC)

IF (PCHSupport_FOUND)
   ADD_PRECOMPILED_HEADER(${PROG_NAME} precomp.cpp prefix/dtgameprefix-src.h true)
ENDIF (PCHSupport_FOUND)

#This will automatically run the unit test executable upon successfull compile/link
IF (AUTO_RUN_TESTS)
  IF (WIN32)
    ADD_CUSTOM_COMMAND(TARGET ${PROG_NAME}
                          POST_BUILD
                          COMMAND "\"$(TargetPath)\""
                          COMMENT "Run the unit tests")  
  ELSE (WIN32)
     ADD_CUSTOM_COMMAND(TARGET ${PROG_NAME}
                          POST_BUILD
                          COMMAND ${CMAKE_BINARY_DIR}/tests/allTests
                          WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                          COMMENT Run the unit tests)
  ENDIF (WIN32)
ENDIF (AUTO_RUN_TESTS)

