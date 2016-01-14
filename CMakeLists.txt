#It may work on an older version, but it hasn't been tested.
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.1 FATAL_ERROR)

# see cmake --help-policy C/0003 for more information
cmake_policy(VERSION 2.8.1)

MACRO(ADD_DELTA3D_SUBFOLDERS)
   #optional external dependencies
   IF (BUILD_ANIM)
      FIND_PACKAGE(Cal3d)   #for dtAnim
      ADD_SUBDIRECTORY(src/dtAnim)
      SET(DTANIM_AVAILABLE "YES")
   ENDIF (BUILD_ANIM)

   IF (BUILD_AUDIO)
      FIND_PACKAGE(OpenAL)  #for dtAudio
      ADD_SUBDIRECTORY(src/dtAudio)
      SET(DTAUDIO_AVAILABLE "YES")
   ENDIF (BUILD_AUDIO)

   IF (BUILD_BINDINGS)
      FIND_PACKAGE(Boost COMPONENTS python REQUIRED)      #for dtNet, dtNetGM, Python bindings
      FIND_PACKAGE(PythonLibs) #for Python bindings
      ADD_SUBDIRECTORY(src/python)
      ADD_SUBDIRECTORY(src/dtScript)
   ENDIF (BUILD_BINDINGS)

   IF (BUILD_DIS)
      FIND_PACKAGE(DIS)     #for dtDIS
      FIND_PACKAGE(HAWKNL)  #for dtNet, dtNetGM, dtDIS
      ADD_SUBDIRECTORY(src/dtDIS)
      SET(DTDIS_AVAILABLE "YES")
   ENDIF (BUILD_DIS)

   IF (BUILD_GUI)
      OPTION(BUILD_WITH_OLD_CEGUI  "ON = build using legacy CEGUI 0.6.x compatability, OFF = build using CEGUI 0.7.x compatibility" OFF)
      MARK_AS_ADVANCED(BUILD_WITH_OLD_CEGUI)
      FIND_PACKAGE(CEGUI)   #for dtGUI
      SET(DTGUI_AVAILABLE "YES")
      ADD_SUBDIRECTORY(src/dtGUI)
      ADD_SUBDIRECTORY(src/dtDirectorGUINodes)
   ENDIF (BUILD_GUI)

   IF (BUILD_HLA)
      ADD_SUBDIRECTORY(src/dtHLAGM)
      SET(DTHLAGM_AVAILABLE "YES")
   ENDIF (BUILD_HLA)

   IF (BUILD_PLIB)
      #FIND_PACKAGE(PLIB) #for dtInputPLIB
      ADD_SUBDIRECTORY(src/dtInputPLIB)
   ENDIF(BUILD_PLIB)

   IF (BUILD_NET)
      FIND_PACKAGE(HAWKNL)  #for dtNet, dtNetGM
      FIND_PACKAGE(GNE)     #for dtNet, dtNetGM
      FIND_PACKAGE(Boost)   #for dtNet, dtNetGM, Python bindings
      ADD_SUBDIRECTORY(src/dtNet)
      ADD_SUBDIRECTORY(src/dtNetGM)
      SET(DTNET_AVAILABLE "YES")
      SET(DTNETGM_AVAILABLE "YES")
   ENDIF ()

   IF (BUILD_TERRAIN)
      FIND_PACKAGE(GDAL)    #for dtTerrain
      MARK_AS_ADVANCED(GDAL_CONFIG)
      MARK_AS_ADVANCED(GDAL_INCLUDE_DIR)
      MARK_AS_ADVANCED(GDAL_LIBRARY)
      ADD_SUBDIRECTORY(src/dtTerrain)
      SET(DTTERRAIN_AVAILABLE "YES")
   ENDIF ()

   IF (BUILD_DTPHYSICS)
      FIND_PACKAGE(Pal)
      ADD_SUBDIRECTORY(src/dtPhysics)
      SET(DTPHYSICS_AVAILABLE "YES")
   ENDIF ()

   IF (BUILD_DTRENDER)
      ADD_SUBDIRECTORY(src/dtRender)
      SET(DTRENDER_AVAILABLE "YES")
   ENDIF ()

   if (BUILD_WITH_OPENVDB)
      FIND_PACKAGE(OpenEXR REQUIRED)
      FIND_PACKAGE(TBB REQUIRED)
      FIND_PACKAGE(OpenVDB REQUIRED)
      ADD_SUBDIRECTORY(src/dtVoxel)
      ADD_SUBDIRECTORY(utilities/OpenVDBCompiler)
      SET(DTVOXEL_AVAILABLE "YES")
   else()
      FIND_PACKAGE(TBB)
   endif()

   ADD_SUBDIRECTORY(src/dtActors)
   ADD_SUBDIRECTORY(src/dtUtil)
   ADD_SUBDIRECTORY(src/dtCore)
   ADD_SUBDIRECTORY(src/dtAI)
   ADD_SUBDIRECTORY(src/dtABC)
   ADD_SUBDIRECTORY(src/dtDirector)
   ADD_SUBDIRECTORY(src/dtDirectorNodes)

   IF (DTANIM_AVAILABLE)
      ADD_SUBDIRECTORY(src/dtDirectorAnimNodes)
      SET(DTDIRECTOR_NODES_AVAILABLE "YES")
   ENDIF ()

   IF (DTAUDIO_AVAILABLE)
      ADD_SUBDIRECTORY(src/dtDirectorAudioNodes)
   ENDIF ()

   ADD_SUBDIRECTORY(src/dtGame)
   #ADD_SUBDIRECTORY(src/dtInputISense)
   ADD_SUBDIRECTORY(src/dtLMS)

   IF (QT5_FOUND)
      ADD_SUBDIRECTORY(src/dtQt)
      SET(DTQT_AVAILABLE "YES")
      ADD_SUBDIRECTORY(src/dtDirectorQt)
      ADD_SUBDIRECTORY(src/dtInspectorQt)
   ENDIF ()

   IF (BUILD_TESTS)
      OPTION(AUTO_RUN_TESTS "Automatically runs the unit tests upon successful compile and link" ON)

      FIND_PACKAGE(CPPUNIT) #for unit tests
      ADD_SUBDIRECTORY(tests)
   ENDIF ()

   ADD_SUBDIRECTORY(demos)
   ADD_SUBDIRECTORY(examples)
   ADD_SUBDIRECTORY(utilities)
ENDMACRO(ADD_DELTA3D_SUBFOLDERS)



PROJECT(DELTA3D)

SET(DELTA_VERSION_FILE_PATH "inc/dtUtil/version.h")

FILE(READ ${DELTA_VERSION_FILE_PATH} VERSION_FILE_TEXT)
STRING(REGEX MATCH "#define DELTA3D_VERSION_MAJOR[ \t]+([0-9]+)" delta3d_VERSION_MAJOR ${VERSION_FILE_TEXT})
STRING(REGEX MATCH "#define DELTA3D_VERSION_MINOR[ \t]+([0-9]+)" delta3d_VERSION_MINOR ${VERSION_FILE_TEXT})
STRING(REGEX MATCH "#define DELTA3D_VERSION_PATCH[ \t]+([0-9]+)" delta3d_VERSION_PATCH ${VERSION_FILE_TEXT})
IF(NOT delta3d_VERSION_MAJOR OR NOT delta3d_VERSION_MINOR OR NOT delta3d_VERSION_PATCH)
   MESSAGE(SEND_ERROR "Could not read version numbers from ${DELTA_VERSION_FILE_PATH}")
ENDIF()
MESSAGE("delta3d version: " ${delta3d_VERSION_MAJOR}.${delta3d_VERSION_MINOR}.${delta3d_VERSION_PATCH})
STRING(REGEX REPLACE "([^0-9]+)([0-9]+)" "\\2" delta3d_VERSION_MAJOR ${delta3d_VERSION_MAJOR})
STRING(REGEX REPLACE "([^0-9]+)([0-9]+)" "\\2" delta3d_VERSION_MINOR ${delta3d_VERSION_MINOR})
STRING(REGEX REPLACE "([^0-9]+)([0-9]+)" "\\2" delta3d_VERSION_PATCH ${delta3d_VERSION_PATCH})
MESSAGE("delta3d version: " ${delta3d_VERSION_MAJOR}.${delta3d_VERSION_MINOR}.${delta3d_VERSION_PATCH})

################################################################################
# User Options
OPTION(BUILD_STATIC        "Builds delta3d as static libraries.(Experimental)" OFF)

OPTION(BUILD_ANIM          "Enables the building of dtAnim (requires CAL3D)" ON)
OPTION(BUILD_AUDIO         "Enables the building of dtAudio (requires openAL/ALUT)" ON)
OPTION(BUILD_BINDINGS      "Enabled the building of the bindings (requires Python, Boost.Python)" OFF)
OPTION(BUILD_DIS           "Enables the building of dtDIS (requires DIS-cpp,HawkNL)" OFF)
OPTION(BUILD_GUI           "Enables the building of dtGUI (requires CEGUI)" ON)
OPTION(BUILD_HLA           "Enables the building of dtHLA related projects (requires RTI)" OFF)
OPTION(BUILD_PLIB          "Enables the building of dtInputPLIB (Uses internal plib code)" ON)
OPTION(BUILD_NET           "Enables the building of dtNet/dtNetGM (requires HawkNL,GNE,Boost)" ON)
OPTION(BUILD_DTPHYSICS     "Enables the building of dtPhysics (requires PAL)" ON)
OPTION(BUILD_DTRENDER      "Enables the building of dtRender for advanced rendering support." ON)
OPTION(BUILD_TERRAIN       "Enables the building of dtTerrain (requires GDAL)" ON)
OPTION(BUILD_TESTS         "Enables the building of the unit tests (requires CPPUNIT)" ON)

OPTION(BUILD_EXAMPLES      "Enables the building of the Delta3D example projects" ON)
OPTION(BUILD_DEMOS         "Enables the building of the Delta3D demo projects" ON)
OPTION(BUILD_WITH_QT       "Enables the building of projects that require Qt" OFF)
OPTION(BUILD_3DSMAX_PLUGIN "Build the Autodesk 3ds Max exporter plugin" OFF)
OPTION(BUILD_WITH_PCH      "Enables use of precomplied headers, experimental" OFF)
OPTION(BUILD_WITH_TBB_MALLOC    "If intel threading buliding blocks is found, it will use their thread safe memory manager for all of delta3d" ON)

include(CMakeDependentOption)
CMAKE_DEPENDENT_OPTION(BUILD_WITH_OPENVDB  "Enables a basic integration with openvdb and builds a utility to convert 3D meshes into voxel grids or octrees" OFF
                       "BUILD_DTPHYSICS" OFF)
MARK_AS_ADVANCED(BUILD_WITH_PCH)

OPTION(BUILD_WITH_MULTITHREAD_FIX_HACK_BREAKS_CEGUI "Fixes a multithreading problem that only affect some machine.  THE FIX BREAKS CEGUI!")
MARK_AS_ADVANCED(BUILD_WITH_MULTITHREAD_FIX_HACK_BREAKS_CEGUI)

# We want to build SONAMES shared libraries
# TODO This does nothing yet.
SET(DELTA32_SONAMES TRUE)

# We have some custom .cmake scripts not in the official distribution.
# Maybe this can be used override existing behavior if needed?
SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH}")

SET(DELTA_DIR ${CMAKE_SOURCE_DIR})

FIND_PACKAGE(OpenGL REQUIRED)

# Add macro functions
INCLUDE(UtilityMacros)
INCLUDE(BuildExample)
INCLUDE(BuildBinding)
INCLUDE(PCHMacros)
INCLUDE(DataInstall)
include(delta3d_common)

if (BUILD_WITH_PCH)
   ADD_DEFINITIONS(-DDT_USE_PCH)
endif (BUILD_WITH_PCH)

if (BUILD_WITH_MULTITHREAD_FIX_HACK_BREAKS_CEGUI)
   ADD_DEFINITIONS(-DMULTITHREAD_FIX_HACK_BREAKS_CEGUI)
endif (BUILD_WITH_MULTITHREAD_FIX_HACK_BREAKS_CEGUI)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

include(PlatformConfiguration)

################################################################################

# required external dependencies for core functionality
FIND_PACKAGE(Delta3DInternal REQUIRED)
FIND_PACKAGE(OpenThreads REQUIRED)
FIND_PACKAGE(OSG REQUIRED)
#FIND_PACKAGE(ODE REQUIRED)
FIND_PACKAGE(XercesC REQUIRED)

IF (NOT WIN32)
   FIND_PACKAGE(Curses REQUIRED)
   MARK_AS_ADVANCED(CURSES_FORM_LIBRARY)
   MARK_AS_ADVANCED(CURSES_CURSES_H_PATH)
   MARK_AS_ADVANCED(CURSES_HAVE_CURSES_H)
ENDIF (NOT WIN32)

IF (NOT WIN32 AND NOT APPLE)
   FIND_PACKAGE(UUID REQUIRED)
ENDIF (NOT WIN32 AND NOT APPLE)

# Make the required external dependency headers visible to everything
INCLUDE_DIRECTORIES(
   ${CMAKE_SOURCE_DIR}/inc
   ${OPENGL_INCLUDE_DIR}
   ${OPENTHREADS_INCLUDE_DIR}
   ${OSG_INCLUDE_DIR}
   #${ODE_INCLUDE_DIR}
   ${XERCES_INCLUDE_DIR}
)

# See if we need to include Delta3D's version of the osgDB Zip plugin
FIND_OSG_VERSION() #from UtilityMacros.cmake
IF (OSG_VERSION_MAJOR AND OSG_VERSION_MAJOR LESS 3)
  SET(BUILD_ZIP_PLUGIN ON)
ENDIF (OSG_VERSION_MAJOR AND OSG_VERSION_MAJOR LESS 3)

IF (BUILD_WITH_QT)
   FIND_PACKAGE(Qt5)
ENDIF (BUILD_WITH_QT)


LINK_DIRECTORIES( ${DELTA3D_EXT_DIR}/lib )# Create bin and lib directories if required

IF("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
   FILE(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin ${CMAKE_BINARY_DIR}/lib )
ENDIF("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")


################################################################################
# Installation stuff

SET(OUTPUT_BINDIR ${PROJECT_BINARY_DIR}/bin)
MAKE_DIRECTORY(${OUTPUT_BINDIR})

SET(OUTPUT_LIBDIR ${PROJECT_BINARY_DIR}/lib)
MAKE_DIRECTORY(${OUTPUT_LIBDIR})


SET (CMAKE_ARCHIVE_OUTPUT_DIRECTORY  ${OUTPUT_LIBDIR} CACHE PATH "build directory")
SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${OUTPUT_BINDIR} CACHE PATH "build directory")
IF(MSVC_IDE)
  SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${OUTPUT_BINDIR} CACHE PATH "build directory")
ELSE()
  SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_LIBDIR} CACHE PATH "build directory")
ENDIF()

# For each configuration (Debug, Release, MinSizeRel... and/or anything the user chooses)
FOREACH(CONF ${CMAKE_CONFIGURATION_TYPES})
# Go uppercase (DEBUG, RELEASE...)
STRING(TOUPPER "${CONF}" CONF)
SET("CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONF}" "${OUTPUT_LIBDIR}")
SET("CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONF}" "${OUTPUT_BINDIR}")
IF(MSVC_IDE)
  SET("CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONF}" "${OUTPUT_BINDIR}")
ELSE()
  SET("CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONF}" "${OUTPUT_LIBDIR}")
ENDIF()
ENDFOREACH()

# Expose CMAKE_INCLUDE_PATH and CMAKE_LIBARY_PATH to the GUI so users
# may set these values without needing to manipulate the environment.
SET(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} CACHE STRING "You may add additional search paths here. Use ; to separate multiple paths.")
SET(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} CACHE STRING "You may add additional search paths here. Use ; to separate multiple paths.")
# We are proposing that a new variable called CMAKE_PREFIX_PATH be introduced
# to CMake to compliment CMAKE_INCLUDE_PATH and CMAKE_LIBRARY_PATH.
# A formal feature request has been submited to CMake, Bug #4947.
# It is intended for those users who have common prefixes for their INCLUDE
# and LIBRARY locations. So if users have headers in /usr/local/include
# and libraries in /usr/local/lib, the common prefix is /usr/local.
# It should also cover the case where headers and libraries are
# in the same directory.
# Our proposal expects that FIND_* commands will automatically search for
# CMAKE_PREFIX_PATH right after CMAKE_INCLUDE_PATH or CMAKE_LIBRARY_PATH.
# Obviously, since CMake does not currently support this, we must write
# our Find*.cmake modules to explicitly support this. Otherwise, this variable
# will have no impact.
# This is unofficial so this may be removed or changed at anytime.
SET(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} CACHE STRING "(EXPERIMENTAL) You may add additional search paths here. Use ; to separate multiple paths.")


# This needs to be run very last so other parts of the scripts can take
# advantage of this.
#IF(NOT DELTA3D_CONFIG_HAS_BEEN_RUN_BEFORE)
#   SET(DELTA3D_CONFIG_HAS_BEEN_RUN_BEFORE 1 CACHE INTERNAL "Flag to track whether this is the first time running CMake or if CMake has been configured before")
#ENDIF(NOT DELTA3D_CONFIG_HAS_BEEN_RUN_BEFORE)

ADD_DELTA3D_SUBFOLDERS()

#-----------------------------------------------------------------------------
### uninstall target
#-----------------------------------------------------------------------------
CONFIGURE_FILE(
   "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/cmake_uninstall.cmake.in"
   "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
   IMMEDIATE @ONLY)

ADD_CUSTOM_TARGET(uninstall
   "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

include(delta3dCPack)
