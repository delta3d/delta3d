#used to generate the project files only for the Delta3D Examples

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
cmake_policy(VERSION 2.8)

SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/../../CMakeModules) #where the Find*.cmake files are

PROJECT(Delta3D_Examples)

FIND_PACKAGE(Delta3D COMPONENTS dtUtil dtCore dtGame dtABC dtAudio dtAnim dtInputPLIB dtGUI dtDirector dtNetGM REQUIRED)
FIND_PACKAGE(Delta3D COMPONENTS dtDIS)
SET(CMAKE_MODULE_PATH "${DELTA_DIR}/CMakeModules;${CMAKE_MODULE_PATH}")

FIND_PACKAGE(Qt4)
IF (QT4_FOUND)
   FIND_PACKAGE(Delta3D COMPONENTS dtQt REQUIRED)
ENDIF (QT4_FOUND)

#look for module loaders in delta3d CMakeModules

IF (DELTA3D_FOUND)

   include(PlatformConfiguration)

ENDIF(DELTA3D_FOUND)



INCLUDE_DIRECTORIES(
                    ${DTABC_INCLUDE_DIRECTORIES}
                    ${DTACTORS_INCLUDE_DIRECTORIES}
                    ${DTAI_INCLUDE_DIRECTORIES}
                    ${DTANIM_INCLUDE_DIRECTORIES}
                    ${DTAUDIO_INCLUDE_DIRECTORIES}
                    ${DTCORE_INCLUDE_DIRECTORIES}
                    ${DTDIRECTOR_INCLUDE_DIRECTORIES}
                    ${DTGAME_INCLUDE_DIRECTORIES}
                    ${DTGUI_INCLUDE_DIRECTORIES}
                    ${DTINPUT_PLIB_INCLUDE_DIRECTORIES}
                    ${DTNETGM_INCLUDE_DIRECTORIES}
                    ${DTUTIL_INCLUDE_DIRECTORIES}
                    ${DTTERRAIN_INCLUDE_DIRECTORIES}
                    )

#ADD_SUBDIRECTORY(examples/testAAR) #needs GameStart
ADD_SUBDIRECTORY(examples/testActorLibrary)
ADD_SUBDIRECTORY(examples/testAI)
#ADD_SUBDIRECTORY(examples/testAnim) #needs GameStart
ADD_SUBDIRECTORY(examples/testApp)
ADD_SUBDIRECTORY(examples/testAudio)
ADD_SUBDIRECTORY(examples/testBumpMap)

IF(Boost_FOUND)
  ADD_SUBDIRECTORY(examples/testClient) #needs Boost header files
  ADD_SUBDIRECTORY(examples/testEchoServer)
ENDIF()
  
ADD_SUBDIRECTORY(examples/testClouds)

IF (DTDIS_FOUND)
  ADD_SUBDIRECTORY(examples/testDIS)
ENDIF (DTDIS_FOUND)

#ADD_SUBDIRECTORY(examples/testDirector)  #needs GameStart
ADD_SUBDIRECTORY(examples/testEffects)
ADD_SUBDIRECTORY(examples/testGameActorComponents)
ADD_SUBDIRECTORY(examples/testGameActorLibrary)
ADD_SUBDIRECTORY(examples/testGameEnvironment)
ADD_SUBDIRECTORY(examples/testGUI)
ADD_SUBDIRECTORY(examples/testInput)
ADD_SUBDIRECTORY(examples/testLights)
ADD_SUBDIRECTORY(examples/testMap)
ADD_SUBDIRECTORY(examples/testMotionModels)
ADD_SUBDIRECTORY(examples/testMultiWin)
ADD_SUBDIRECTORY(examples/testNestedView)
#ADD_SUBDIRECTORY(examples/testNetwork)  #needs dtNet (deprecated?)
ADD_SUBDIRECTORY(examples/testPackage)
ADD_SUBDIRECTORY(examples/testPhysics)
ADD_SUBDIRECTORY(examples/testPreRender)
IF (DTQT_FOUND)
   ADD_SUBDIRECTORY(examples/testQt)
ENDIF (DTQT_FOUND)
#ADD_SUBDIRECTORY(examples/testProceduralAnimation)  #needs GameStart
#ADD_SUBDIRECTORY(examples/testPython)  #needs Python, dtScript
ADD_SUBDIRECTORY(examples/testRecorder)
ADD_SUBDIRECTORY(examples/testShaders)
ADD_SUBDIRECTORY(examples/testSound)
ADD_SUBDIRECTORY(examples/testStateManager)
ADD_SUBDIRECTORY(examples/testTerrain)
ADD_SUBDIRECTORY(examples/testTrigger)
ADD_SUBDIRECTORY(examples/testWeather)
ADD_SUBDIRECTORY(examples/testWin)
