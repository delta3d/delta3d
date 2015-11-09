CMAKE_MINIMUM_REQUIRED(VERSION 2.8.0 FATAL_ERROR)

cmake_policy(VERSION 2.8)

PROJECT(RTIPlugins)

################################################################################
# User Options

# We want to build SONAMES shared librariess
# SET(DELTA32_SONAMES TRUE)

# We have some custom .cmake scripts not in the official distribution.
# Maybe this can be used override existing behavior if needed?
SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../CMakeModules;/usr/local/share/cmake/Modules;${CMAKE_MODULE_PATH}")

# Add macro functions 
INCLUDE(UtilityMacros)

################################################################################
# PATHS and output directories

OPTION(USE_RTIS "Use RTI-s for the RTI implementation" OFF)
OPTION(USE_CERTI "Use CERTI for the RTI implementation" OFF)
OPTION(USE_MAK "Use MAK for the RTI implementation" OFF)
OPTION(USE_PORTICO "Use Portico for the RTI implementation" OFF)
OPTION(USE_HLA_1516e "Use an implementation of RTI 1516e (headers and library names are standardized.)" OFF)


FIND_PACKAGE(Delta3D COMPONENTS dtUtil dtCore dtGame dtHLAGM REQUIRED)
SET(CMAKE_MODULE_PATH "${DELTA_DIR}/CMakeModules;${CMAKE_MODULE_PATH}")

#look for module loaders in delta3d CMakeModules

IF (DELTA3D_FOUND)

   include(PlatformConfiguration)

   INCLUDE_DIRECTORIES(
      ${DTCORE_INCLUDE_DIRECTORIES} 
      ${DTUTIL_INCLUDE_DIRECTORIES}
      ${DTGAME_INCLUDE_DIRECTORIES}
      ${DTHLAGM_INCLUDE_DIRECTORIES}
   )

   # Make the headers visible to everything
   
   IF (USE_HLA_1516e)
      FIND_PACKAGE(HLA1516e REQUIRED)
   else()
      unset(RTI1516e_INCLUDE_DIR CACHE)
      unset(RTI1516e_LIBRARIES CACHE)
   endif()
   
   IF (USE_RTIS)
      FIND_PACKAGE(RTI-s REQUIRED) #for dtHLAGM
   ELSE (USE_RTIS)
      IF (USE_CERTI)
         FIND_PACKAGE(RTICERTI REQUIRED)
      ELSE (USE_CERTI)
         IF (USE_MAK)
           FIND_PACKAGE(RTIMAK REQUIRED)
         ELSE  (USE_MAK)
           IF (USE_PORTICO)
             FIND_PACKAGE(RTIPortico REQUIRED)
           ELSE (USE_PORTICO)
              unset(RTI_INCLUDE_DIR CACHE)
              unset(RTI_LIBRARIES CACHE)
              if (NOT USE_HLA_1516e)
                 MESSAGE("No HLA RTI selected, choose USE_CERTI, USE_RTIS, USE_PORTICO, USE_MAK, or USE_HLA_1516e.  HLA 1516 can be compiled as the same time as one of the others.")
              ENDIF(NOT USE_HLA_1516e)
           ENDIF (USE_PORTICO)
         ENDIF (USE_MAK)
      ENDIF (USE_CERTI)
   ENDIF (USE_RTIS)
   if (RTI_INCLUDE_DIR AND (USE_RTIS OR USE_CERTI OR USE_MAK OR USE_PORTICO))
      ADD_SUBDIRECTORY(rti13)
   endif()
   if (USE_HLA_1516e)
      ADD_SUBDIRECTORY(rti1516e)
   endif()
   
   
   #-----------------------------------------------------------------------------
   ### uninstall target
   #-----------------------------------------------------------------------------
   CONFIGURE_FILE(
     "${CMAKE_CURRENT_SOURCE_DIR}/../CMakeModules/cmake_uninstall.cmake.in"
     "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
     IMMEDIATE @ONLY)
   ADD_CUSTOM_TARGET(uninstall
     "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

ENDIF(DELTA3D_FOUND)




