# Locate Delta3D
# This module defines
# DTCORE_LIBRARY
# DELTA3D_EXT_DIR
# DELTA3D_FOUND, if false, do not try to link to gdal 
# DELTA3D_INCLUDE_DIR, where to find the headers
#
# $DELTA3D_DIR is an environment variable that would
# correspond to the ./configure --prefix=$DELTA3D
#
# Created by David Guthrie. 

FIND_PATH(DELTA_DIR NAMES inc/dtCore/transform.h include/dtCore/transform.h
  HINTS
    ${CMAKE_SOURCE_DIR}/../delta3d
    $ENV{DELTA_ROOT}
    $ENV{DELTA_INC}
  PATHS
    /usr/local
  PATH_SUFFIXES
    trunk
)

FIND_PATH(DELTA3D_INCLUDE_DIR NAMES dtCore/transform.h
    HINTS
       ${DELTA_DIR}
    PATH_SUFFIXES
       inc
       include
)

if (DELTA_DIR)
   SET(CMAKE_MODULE_PATH "${DELTA_DIR}/CMakeModules;${CMAKE_MODULE_PATH}")

#where to find the Delta3D "ext" folder.  Look for one of the headers that might be in there.
SET(DELTA3D_EXT_DIR ${DELTA_DIR}/ext CACHE PATH "The root of the Delta3D external dependency folder")

IF(DELTA3D_EXT_DIR)
  #for aiding FIND_FILE() and FIND_PATH() searches
  list(APPEND CMAKE_PREFIX_PATH ${DELTA3D_EXT_DIR})
  list(APPEND CMAKE_FRAMEWORK_PATH ${DELTA3D_EXT_DIR}/Frameworks)
  list(APPEND CMAKE_INCLUDE_PATH ${DELTA3D_EXT_DIR}/inc)
ENDIF(DELTA3D_EXT_DIR)

set(MISSING_PACKAGES )
set(PACKAGES )
foreach(COMPONENT ${Delta3D_FIND_COMPONENTS})
   if (Delta3D_FIND_REQUIRED)
      Find_package(${COMPONENT} REQUIRED)
   elseif(Delta3D_FIND_QUIETLY)
      Find_package(${COMPONENT} QUIETLY)
   else()
      Find_package(${COMPONENT})
   endif()

   string(TOUPPER ${COMPONENT} COMPONENT_uc)

   if (NOT ${COMPONENT_uc}_FOUND)
      set(MISSING_PACKAGES "${MISSING_PACKAGES}\n${COMPONENT}")
   endif()
   set(PACKAGES ${PACKAGES} ${COMPONENT_uc}_INCLUDE_DIRECTORIES ${COMPONENT_uc}_LIBRARY)
endforeach()

if(MISSING_PACKAGES)
   if(Delta3D_FIND_REQUIRED)
      message(SEND_ERROR "Unable to find the requested Delta3D libraries.\n${MISSING_PACKAGES}")
   else()
      if(NOT Delta3D_FIND_QUIETLY)
         # we opt not to automatically output Boost_ERROR_REASON here as
         # it could be quite lengthy and somewhat imposing in its requests
         # Since Boost is not always a required dependency we'll leave this
         # up to the end-user.
        message(STATUS "Could NOT find all Delta3D libraries\n${MISSING_PACKAGES}")
      endif()
   endif()
endif()

endif()
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Delta3D DEFAULT_MSG DELTA3D_INCLUDE_DIR ${PACKAGES})
