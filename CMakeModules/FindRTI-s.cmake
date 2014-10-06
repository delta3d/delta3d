# Locate OSG
# This module defines
# RTIS_LIBRARY
# RTIS_FOUND, if false, do not try to link to OSG
# RTI_INCLUDE_DIR, where to find the headers
# RTI_LIBRARIES, list of all required RTI libraries
#
# $RTI is an environment variable that would
# correspond to the ./configure --prefix=$RTI
# when building the RTI using autoconf.  It should point
# to the directory containing include and lib
#
# Created by David Guthrie. 

OPTION(RTIS_SINGLE_LIBRARY "flags the build to assume that rtis is built as one single library" OFF)

FIND_PATH(RTIS_ROOT_DIR NAMES include/1.3/RTI.hh include/rtis/RTI.hh
   PATHS 
   $ENV{RTI} 
   $ENV{RTI_HOME} 
   ${RTI_INCLUDE_DIR}/..
   /usr/local
)	
MARK_AS_ADVANCED(RTIS_ROOT_DIR)

FIND_PATH(RTI_INCLUDE_DIR RTI.hh
    PATH_SUFFIXES 1.3 rtis
    HINTS
    ${RTIS_ROOT_DIR}/include
    PATHS
    /usr/local/include
    /usr/include
    /opt/include    
)
MARK_AS_ADVANCED(RTI_INCLUDE_DIR)

if (GCC_VERSION)
  IF (APPLE)
    SET(PATHLIST
        macintel_g++-${GCC_MAJOR}.${GCC_MINOR}
        macintel_g++-unknown
        macintel64_g++-${GCC_MAJOR}.${GCC_MINOR}
        macintel64_g++-unknown
        darwin_g++-${GCC_MAJOR}.${GCC_MINOR}
        gcc${GCC_MAJOR}${GCC_MINOR}
    )
  ELSE (APPLE)
    IF (UNIX) 
       SET(PATHLIST
          linux_g++-${GCC_MAJOR}.${GCC_MINOR}
          gcc${GCC_MAJOR}${GCC_MINOR}
       )
    ENDIF (UNIX)
  ENDIF (APPLE) 
endif(GCC_VERSION)

IF (MSVC)
     SET(PATHLIST
        winnt_vc++-8.0
        winnt_vc++-7.1
        winnt_vc++-9.0
        winnt_vc++-10.0
     )
ENDIF (MSVC)

MACRO(FIND_RTIS_LIBRARY MYLIBRARY MYLIBRARYNAME)

    FIND_LIBRARY(${MYLIBRARY}
        NAMES ${MYLIBRARYNAME}
        PATH_SUFFIXES ${PATHLIST}
        HINTS
           ${RTIS_ROOT_DIR}/lib
           ${RTIS_ROOT_DIR}
    )
    MARK_AS_ADVANCED(${MYLIBRARY})
ENDMACRO(FIND_RTIS_LIBRARY LIBRARY LIBRARYNAME)

SET(RTI_13_LIST rti13 librti13)
SET(RTI_FEDTIME13_LIST fedtime13 libfedtime13)

if (NOT RTIS_SINGLE_LIBRARY)
   FIND_RTIS_LIBRARY(RTIS_13_LIBRARY "${RTI_13_LIST}" )
   FIND_RTIS_LIBRARY(RTIS_FEDTIME13_LIBRARY "${RTI_FEDTIME13_LIST}" )
endif (NOT RTIS_SINGLE_LIBRARY)

FIND_RTIS_LIBRARY(RTIS_LIBRARY rtis )

#only for cmake 2.6.0 and above
IF(${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} EQUAL 6 AND ${CMAKE_PATCH_VERSION} GREATER 0)
  INCLUDE(FindPackageHandleStandardArgs)
  FIND_PACKAGE_HANDLE_STANDARD_ARGS(RTIS DEFAULT_MSG RTIS_LIBRARY RTI_INCLUDE_DIR)
ENDIF(${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} EQUAL 6 AND ${CMAKE_PATCH_VERSION} GREATER 0)


SET(RTIS_FOUND "NO")
IF(RTIS_LIBRARY AND RTI_INCLUDE_DIR)
    SET(RTIS_FOUND "YES")

FIND_PATH(RTIS_ROOT_DIR NAMES include/1.3/RTI.hh
                        PATHS $ENV{RTI} $ENV{RTI_HOME} )

MARK_AS_ADVANCED(RTIS_ROOT_DIR)

    if (NOT RTIS_ROOT_DIR)
        MESSAGE("RTIS_ROOT_DIR is not set, but the includes and libraries were found successfully. This unset var may be ignored.")
    endif (NOT RTIS_ROOT_DIR)
    
    if (NOT RTIS_SINGLE_LIBRARY)    
       SET(RTI_LIBRARIES
          ${RTIS_13_LIBRARY}
          ${RTIS_FEDTIME13_LIBRARY}
       )
    else (NOT RTIS_SINGLE_LIBRARY)    
       SET(RTI_LIBRARIES
          ${RTIS_LIBRARY}
       )
    endif (NOT RTIS_SINGLE_LIBRARY)    
ELSE(RTIS_LIBRARY AND RTI_INCLUDE_DIR) 
    MESSAGE("Unable to Find RTI-s.  Try setting the cmake variable RTI_ROOT_DIR to the directory that contains include and lib/bin, or set the environment variable RTI_HOME.")
ENDIF(RTIS_LIBRARY AND RTI_INCLUDE_DIR)

