CMAKE_MINIMUM_REQUIRED(VERSION 2.6.2 FATAL_ERROR)

# see cmake --help-policy CMP0003 for more information
IF(COMMAND cmake_policy)
   CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND cmake_policy)

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
      IF (USE_RTIS)
         FIND_PACKAGE(RTI-s REQUIRED) #for dtHLAGM
      ELSE (USE_RTIS)
         IF (USE_CERTI)
            FIND_PACKAGE(RTICERTI REQUIRED)
         ELSE (USE_CERTI)
            IF (USE_MAK)
               FIND_PACKAGE(RTIMAK REQUIRED)
            ELSE (USE_MAK) 
               IF (USE_PORTICO)
                  FIND_PACKAGE(RTIPortico REQUIRED)
               ELSE (USE_PORTICO) 
                  MESSAGE("No HLA RTI selected, choose USE_CERTI, USE_RTIS, USE_PORTICO or USE_MAK")
               ENDIF (USE_PORTICO)
            ENDIF (USE_MAK)
         ENDIF (USE_CERTI)
      ENDIF (USE_RTIS)

      ADD_SUBDIRECTORY(src/dtHLAGM)
      SET(DTHLAGM_AVAILABLE "YES")
   ENDIF (BUILD_HLA)

   IF (BUILD_PLIB)
      FIND_PACKAGE(PLIB) #for dtInputPLIB
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
   ENDIF (BUILD_NET)  

   IF (BUILD_TERRAIN)
      FIND_PACKAGE(GDAL)    #for dtTerrain
      ADD_SUBDIRECTORY(src/dtTerrain)
      SET(DTTERRAIN_AVAILABLE "YES")
   ENDIF (BUILD_TERRAIN)



   ADD_SUBDIRECTORY(src/dtActors)
   ADD_SUBDIRECTORY(src/dtUtil)
   ADD_SUBDIRECTORY(src/dtCore)
   ADD_SUBDIRECTORY(src/dtAI)
   ADD_SUBDIRECTORY(src/dtABC)
   ADD_SUBDIRECTORY(src/dtDAL)
   ADD_SUBDIRECTORY(src/dtDirector)

   IF (DTAUDIO_AVAILABLE)
      ADD_SUBDIRECTORY(src/dtDirectorAudioNodes)
   ENDIF (DTAUDIO_AVAILABLE)

   IF (DTANIM_AVAILABLE)
      ADD_SUBDIRECTORY(src/dtDirectorNodes)
      SET(DTDIRECTOR_NODES_AVAILABLE "YES")
   ENDIF (DTANIM_AVAILABLE)

   ADD_SUBDIRECTORY(src/dtGame)
   ADD_SUBDIRECTORY(src/dtInputISense)
   ADD_SUBDIRECTORY(src/dtLMS)

   IF (QT4_FOUND)
      ADD_SUBDIRECTORY(src/dtQt)
      SET(DTQT_AVAILABLE "YES")
      ADD_SUBDIRECTORY(src/dtInspectorQt)
      ADD_SUBDIRECTORY(src/dtDirectorQt)
   ENDIF (QT4_FOUND)

   IF (BUILD_TESTS)
      OPTION(AUTO_RUN_TESTS "Automatically runs the unit tests upon successful compile and link" ON)

      FIND_PACKAGE(CPPUNIT) #for unit tests
      ADD_SUBDIRECTORY(tests)
   ENDIF (BUILD_TESTS)
   
   ADD_SUBDIRECTORY(demos)
   ADD_SUBDIRECTORY(examples)
   ADD_SUBDIRECTORY(utilities)
ENDMACRO(ADD_DELTA3D_SUBFOLDERS)



PROJECT(DELTA3D)

################################################################################
# User Options
OPTION(BUILD_ANIM          "Enables the building of dtAnim (requires CAL3D)" ON)
OPTION(BUILD_AUDIO         "Enables the building of dtAudio (requires openAL/ALUT)" ON)
OPTION(BUILD_BINDINGS      "Enabled the building of the bindings (requires Python, Boost.Python)" OFF)
OPTION(BUILD_DIS           "Enables the building of dtDIS (requires DIS-cpp,HawkNL)" ON)
OPTION(BUILD_GUI           "Enables the building of dtGUI (requires CEGUI)" ON)
OPTION(BUILD_HLA           "Enables the building of dtHLA related projects (requires RTI)" OFF)
OPTION(BUILD_PLIB          "Enabes the building of dtInputPLIB (requires PLIB)" ON)
OPTION(BUILD_NET           "Enables the building of dtNet/dtNetGM (requires HawkNL,GNE,Boost)" OFF)
OPTION(BUILD_TERRAIN       "Enables the building of dtTerrain (requires GDAL)" ON)
OPTION(BUILD_TESTS         "Enables the building of the unit tests (requires CPPUNIT)" ON)

OPTION(BUILD_EXAMPLES      "Enables the building of the Delta3D example projects" ON)
OPTION(BUILD_DEMOS         "Enables the building of the Delta3D demo projects" ON)
OPTION(BUILD_WITH_QT       "Enables the building of projects that require Qt" OFF)
OPTION(BUILD_3DSMAX_PLUGIN "Build the Autodesk 3ds Max exporter plugin" OFF)
OPTION(BUILD_WITH_PCH      "Enables use of precomplied headers, experimental" OFF)
MARK_AS_ADVANCED(BUILD_WITH_PCH)

OPTION(BUILD_WITH_MULTITHREAD_FIX_HACK_BREAKS_CEGUI "Fixes a multithreading problem that only affect some machine.  THE FIX BREAKS CEGUI!")
MARK_AS_ADVANCED(BUILD_WITH_MULTITHREAD_FIX_HACK_BREAKS_CEGUI)

IF (BUILD_HLA)
   OPTION(USE_RTIS "Use RTI-s for the RTI implementation" ON)
   OPTION(USE_CERTI "Use CERTI for the RTI implementation" OFF)
   OPTION(USE_MAK "Use MAK for the RTI implementation" OFF)
   OPTION(USE_PORTICO "Use Portico for the RTI implementation" OFF)
ENDIF (BUILD_HLA)


IF (WIN32)
   SET(CMAKE_DEBUG_POSTFIX  "d")

   OPTION(BUILD_WITH_MP "Enables the /MP multi-processor compiler option for Visual Studio 2005 and above" ON)
   MARK_AS_ADVANCED(BUILD_WITH_MP)
   IF(BUILD_WITH_MP)
      SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
   ENDIF(BUILD_WITH_MP)
ENDIF (WIN32)

# We want to build SONAMES shared libraries
SET(DELTA32_SONAMES TRUE)

# We have some custom .cmake scripts not in the official distribution.
# Maybe this can be used override existing behavior if needed?
SET(CMAKE_MODULE_PATH "${DELTA3D_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH}")

SET(DELTA_DIR ${CMAKE_SOURCE_DIR})

FIND_PACKAGE(OpenGL REQUIRED)

# Add macro functions 
INCLUDE(UtilityMacros)
INCLUDE(BuildExample)
INCLUDE(BuildBinding)
INCLUDE(PCHMacros)
include(delta3d_common)

if (BUILD_WITH_PCH)
   ADD_DEFINITIONS(-DDT_USE_PCH)
endif (BUILD_WITH_PCH)

if (BUILD_WITH_MULTITHREAD_FIX_HACK_BREAKS_CEGUI)
   ADD_DEFINITIONS(-DMULTITHREAD_FIX_HACK_BREAKS_CEGUI)
endif (BUILD_WITH_MULTITHREAD_FIX_HACK_BREAKS_CEGUI)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

IF(APPLE)
   FIND_LIBRARY(CARBON_LIBRARY Carbon)
   FIND_LIBRARY(APPSERVICES_LIBRARY ApplicationServices)
   FIND_LIBRARY(IOKIT_LIBRARY IOKit)
   FIND_LIBRARY(COREFOUNDATION_LIBRARY CoreFoundation)
   FIND_LIBRARY(AGL_LIBRARY AGL)
   FIND_LIBRARY(COCOA_LIBRARY Cocoa)

   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wnewline-eof -pipe")

   SET(CMAKE_C_FLAGS "-pipe -Wnewline-eof")

   if (NOT CMAKE_OSX_ARCHITECTURES)
     set(CMAKE_OSX_ARCHITECTURES "i386" CACHE STRING "Build architectures for OSX" FORCE)
   endif()

   if (NOT CMAKE_OSX_SYSROOT)
     set(CMAKE_OSX_SYSROOT "/Developer/SDKs/MacOSX10.5.sdk" CACHE STRING "Build SDK root" FORCE )
   endif()

   if (NOT CMAKE_OSX_DEPLOYMENT_TARGET)
     # use 10.5 because osg atomics won't build on less. 
     set(CMAKE_OSX_DEPLOYMENT_TARGET "10.5" CACHE STRING "Build SDK root" FORCE)
   endif()

   SET(CMAKE_SHARED_LINKER_FLAGS "-Wl,-single_module -compatibility_version 3 -current_version 3" )
   SET(CMAKE_CXX_FLAGS_DEBUG "-gdwarf-2")
   SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -gdwarf-2")
   SET(CMAKE_C_FLAGS_DEBUG "-gdwarf-2")
   SET(CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -gdwarf-2")
   INCLUDE_DIRECTORIES(/usr/include/malloc)
ELSE (APPLE)
   #We only want X11 if we are not running on OSX, but still with a unix-like environment 
   IF(UNIX)
      FIND_PACKAGE(X11)
      FIND_LIBRARY(XXF86VM_LIBRARY Xxf86vm)
      SET(X11_LIBRARIES
          ${X11_LIBRARIES}
          ${XXF86VM_LIBRARY})
   ENDIF(UNIX)
ENDIF(APPLE)

IF(UNIX)
   # Some Unicies need explicit linkage to the Math library or the build fails.
   FIND_LIBRARY(MATH_LIBRARY m)

   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-non-virtual-dtor -Wreturn-type")
ENDIF (UNIX)

# Common global definitions

# Platform specific definitions

IF(WIN32)
   ADD_DEFINITIONS(-D_SCL_SECURE_NO_WARNINGS)
   ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE)
   #needed for net plugin
   SET (DELTA3D_SOCKET_LIBS wsock32)
   ADD_DEFINITIONS(-DNOMINMAX)

########################################################################################################
# the foolowing options are MSVC specific,
# the first DELTA3D_MSVC_VERSIONED_DLL activate a custom build-time layout that should allow to run examples and application 
# fron bin folder without requiring installation step.
# it also prepend "osg${OPENSCENEGRAPH_SOVERSION}-" to only .dll files, leaving .lib files untouched in lib
# it also use a hack to get rid of Debug and Release folder in MSVC projects 
# all the .dll and .pdb are in bin and all the .lib and .exp are in lib
# 
# the second option disable incremental linking in debug build , that is enabled by default by CMake
##########################################################################################################

   IF(MSVC)
      IF(${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} EQUAL 4 AND ${CMAKE_PATCH_VERSION} LESS 7)
         MESSAGE("Warning:  disabling versioned options 2.4.6 exibits inconsintencies in .pdb naming, at least under MSVC, suggested upgrading at least to 2.4.7")
         SET(DELTA3D_MSVC_VERSIONED_DLL OFF)
         SET(DELTA3D_MSVC_DEBUG_INCREMENTAL_LINK ON)
      ELSE(${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} EQUAL 4 AND ${CMAKE_PATCH_VERSION} LESS 7)
         OPTION(DELTA3D_MSVC_VERSIONED_DLL "Set to ON to build OpenSceneGraph with versioned dll names" ON)
         MARK_AS_ADVANCED(DELTA3D_MSVC_VERSIONED_DLL)
         OPTION(DELTA3D_MSVC_DEBUG_INCREMENTAL_LINK "Set to OFF to build OpenSceneGraph without incremental linking in debug (release is off by default)" ON)
         MARK_AS_ADVANCED(DELTA3D_MSVC_DEBUG_INCREMENTAL_LINK)
         IF(NOT DELTA3D_MSVC_DEBUG_INCREMENTAL_LINK)
            SET(CMAKE_MODULE_LINKER_FLAGS_DEBUG "/debug /INCREMENTAL:NO")
            SET(CMAKE_SHARED_LINKER_FLAGS_DEBUG "/debug /INCREMENTAL:NO")
            SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "/debug /INCREMENTAL:NO")
         ENDIF(NOT DELTA3D_MSVC_DEBUG_INCREMENTAL_LINK)
      ENDIF(${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} EQUAL 4 AND ${CMAKE_PATCH_VERSION} LESS 7)
   ENDIF(MSVC)
ENDIF(WIN32)


################################################################################

# required external dependencies for core functionality
FIND_PACKAGE(Delta3D REQUIRED)
FIND_PACKAGE(OpenThreads REQUIRED)
FIND_PACKAGE(OSG REQUIRED)
FIND_PACKAGE(ODE REQUIRED)
FIND_PACKAGE(XercesC REQUIRED)

IF (NOT WIN32)
   FIND_PACKAGE(Curses REQUIRED)
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
   ${ODE_INCLUDE_DIR}
   ${XERCES_INCLUDE_DIR}
)


IF (BUILD_WITH_QT)
   SET(QT_USE_QTOPENGL "true")
   FIND_PACKAGE(Qt4 REQUIRED)     #for STAGE, AnimationViewer, ObjectViewer, ParticleEditor
ENDIF (BUILD_WITH_QT)


#MESSAGE(STATUS "${Boost_INCLUDE_DIR} ${Boost_PYTHON_INCLUDE_DIR} ${PYTHON_INCLUDE_PATH}")


LINK_DIRECTORIES( ${DELTA3D_EXT_DIR}/lib )

################################################################################
# Create bin and lib directories if required

IF("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
   FILE(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin ${CMAKE_BINARY_DIR}/lib )
ENDIF("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")


################################################################################
# Installation stuff


IF(UNIX AND NOT WIN32 AND NOT APPLE)
   IF(CMAKE_SIZEOF_VOID_P MATCHES "8")
      SET(LIB_POSTFIX "64" CACHE STRING "suffix for 32/64 dir placement")
      MARK_AS_ADVANCED(LIB_POSTFIX)
   ENDIF(CMAKE_SIZEOF_VOID_P MATCHES "8")
ENDIF(UNIX AND NOT WIN32 AND NOT APPLE)
IF(NOT DEFINED LIB_POSTFIX)
   SET(LIB_POSTFIX "")
ENDIF(NOT DEFINED LIB_POSTFIX)

SET(OUTPUT_BINDIR ${PROJECT_BINARY_DIR}/bin)
MAKE_DIRECTORY(${OUTPUT_BINDIR})
#SET(EXECUTABLE_OUTPUT_PATH ${OUTPUT_BINDIR})

SET(OUTPUT_LIBDIR ${PROJECT_BINARY_DIR}/lib)
MAKE_DIRECTORY(${OUTPUT_LIBDIR})
#SET(LIBRARY_OUTPUT_PATH ${OUTPUT_LIBDIR})

SET (CMAKE_ARCHIVE_OUTPUT_DIRECTORY  ${OUTPUT_LIBDIR} CACHE PATH "build directory")
SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${OUTPUT_BINDIR} CACHE PATH "build directory")
IF(WIN32)
  SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${OUTPUT_BINDIR} CACHE PATH "build directory")
ELSE(WIN32)
  SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_LIBDIR} CACHE PATH "build directory") 
ENDIF(WIN32)

# For each configuration (Debug, Release, MinSizeRel... and/or anything the user chooses) 
FOREACH(CONF ${CMAKE_CONFIGURATION_TYPES}) 
# Go uppercase (DEBUG, RELEASE...) 
STRING(TOUPPER "${CONF}" CONF) 
SET("CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONF}" "${OUTPUT_LIBDIR}") 
SET("CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONF}" "${OUTPUT_BINDIR}") 
IF(WIN32) 
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




# This is for an advanced option to give aggressive warnings 
# under different compilers. If yours is not implemented, this option
# will not be made available.
IF(CMAKE_COMPILER_IS_GNUCXX)
   # To be complete, we might also do GNUCC flags, 
   # but everything here is C++ code.
   # -Wshadow and -Woverloaded-virtual are also interesting flags, but OSG
   # returns too many hits.
   # FYI, if we do implement GNUCC, then -Wmissing-prototypes in another 
   # interesting C-specific flag.
   # Also, there is a bug in gcc 4.0. Under C++, -pedantic will create
   # errors instead of warnings for certain issues, including superfluous
   # semicolons and commas, and the use of long long. -fpermissive seems
   # to be the workaround. 
   SET(DELTA3D_AGGRESSIVE_WARNING_FLAGS "-Wall -Wparentheses -Wformat=2 -Wno-long-long -Wno-import -pedantic -Wreturn-type -Wmissing-braces -Wunknown-pragmas -Wunused -fpermissive")
ELSE(CMAKE_COMPILER_IS_GNUCXX)
   IF(MSVC)
      # FIXME: What are good aggressive warning flags for Visual Studio?
      # And do we need to further subcase this for different versions of VS?
      # CMake variables: MSVC60, MSVC70, MSVC71, MSVC80, CMAKE_COMPILER_2005
      SET(DELTA3D_AGGRESSIVE_WARNING_FLAGS "/Wall /W4")
   ELSE(MSVC)
      # CMake lacks an elseif, so other non-gcc, non-VS compilers need
      # to be listed below. If unhandled, DELTA3D_AGGRESSIVE_WARNING_FLAGS should 
      # remain unset.
   ENDIF(MSVC)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

# This part is for the CMake menu option to toggle the warnings on/off.
# This will only be made available if we set values for DELTA3D_AGGRESSIVE_WARNING_FLAGS.
IF(DELTA3D_AGGRESSIVE_WARNING_FLAGS)
   OPTION(DELTA3D_USE_AGGRESSIVE_WARNINGS "Enable to activate aggressive warnings" OFF)
   MARK_AS_ADVANCED(DELTA3D_USE_AGGRESSIVE_WARNINGS)

   IF(DELTA3D_USE_AGGRESSIVE_WARNINGS)
      IF(NOT "${OLD_CMAKE_CXX_FLAGS_WAS_SET}")
         SET(OLD_CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" CACHE INTERNAL "Old CXX flags")
         SET(OLD_CMAKE_CXX_FLAGS_WAS_SET 1 CACHE INTERNAL "Old CXX flags was set")
         SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${DELTA3D_AGGRESSIVE_WARNING_FLAGS}" CACHE STRING "Flags used by the compiler during all build types." FORCE)
      ENDIF(NOT "${OLD_CMAKE_CXX_FLAGS_WAS_SET}")
   ELSE(DELTA3D_USE_AGGRESSIVE_WARNINGS)
      # FIXME: This will lose any changes made after OLD_CMAKE_CXX_FLAGS was 
      # set. The better way would be to parse the string and remove each
      # option explicitly.
      IF("${OLD_CMAKE_CXX_FLAGS_WAS_SET}")
         SET(CMAKE_CXX_FLAGS "${OLD_CMAKE_CXX_FLAGS}" CACHE STRING "Flags used by the compiler during all build types." FORCE)
         SET(OLD_CMAKE_CXX_FLAGS_WAS_SET 0 CACHE INTERNAL "Old CXX flags was set")
      ENDIF("${OLD_CMAKE_CXX_FLAGS_WAS_SET}")
   ENDIF(DELTA3D_USE_AGGRESSIVE_WARNINGS)
ENDIF(DELTA3D_AGGRESSIVE_WARNING_FLAGS)

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
