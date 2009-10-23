#used to generate the project files only for the Delta3D Examples

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/../../CMakeModules) #where the Find*.cmake files are

PROJECT(Delta3D_Examples)

FIND_PACKAGE(dtABC REQUIRED)
FIND_PACKAGE(dtActors REQUIRED)
FIND_PACKAGE(dtAI REQUIRED)
FIND_PACKAGE(dtAnim REQUIRED)
FIND_PACKAGE(dtAudio REQUIRED)
FIND_PACKAGE(dtCore REQUIRED)
FIND_PACKAGE(dtDAL REQUIRED)
FIND_PACKAGE(dtGame REQUIRED)
FIND_PACKAGE(dtGUI REQUIRED)
FIND_PACKAGE(dtInputPLIB REQUIRED)
FIND_PACKAGE(dtUtil REQUIRED)
FIND_PACKAGE(dtTerrain REQUIRED)

FIND_PACKAGE(Boost) #dtNetGM apps need this
FIND_PACKAGE(dtNetGM REQUIRED) #dtNetGM needs Boost found first



INCLUDE_DIRECTORIES(
                    ${DTABC_INCLUDE_DIRECTORIES}
                    ${DTACTORS_INCLUDE_DIRECTORIES}
                    ${DTAI_INCLUDE_DIRECTORIES}
                    ${DTANIM_INCLUDE_DIRECTORIES}
                    ${DTAUDIO_INCLUDE_DIRECTORIES}
                    ${DTCORE_INCLUDE_DIRECTORIES}
                    ${DTDAL_INCLUDE_DIRECTORIES}
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
ADD_SUBDIRECTORY(examples/testEffects)
ADD_SUBDIRECTORY(examples/testGameActorComponents)
ADD_SUBDIRECTORY(examples/testGameActorLibrary)
ADD_SUBDIRECTORY(examples/testGameEnvironment)
ADD_SUBDIRECTORY(examples/testGUI)
ADD_SUBDIRECTORY(examples/testHUD)
ADD_SUBDIRECTORY(examples/testInput)
ADD_SUBDIRECTORY(examples/testLights)
ADD_SUBDIRECTORY(examples/testMap)
ADD_SUBDIRECTORY(examples/testMotionModels)
ADD_SUBDIRECTORY(examples/testMultiWin)
ADD_SUBDIRECTORY(examples/testNestedView)
#ADD_SUBDIRECTORY(examples/testNetwork)  #needs dtNet (deprecated?)
ADD_SUBDIRECTORY(examples/testPhysics)
ADD_SUBDIRECTORY(examples/testPreRender)
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
