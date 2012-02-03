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

FIND_PATH(RTI_INCLUDE_DIR RTI.hh
    PATH_SUFFIXES 1.3 rtis
    PATHS
    ${RTIS_ROOT_DIR}/include
    /usr/local/include
    /usr/include
    /opt/include    
)

IF (APPLE)
  SET(PATHLIST 
        ${RTIS_ROOT_DIR}/lib/macintel_g++-4.0
        ${RTIS_ROOT_DIR}/lib/darwin_g++-4.0
        ${RTIS_ROOT_DIR}/lib/darwin_g++-3.3
  )
ELSE (APPLE)
  IF (UNIX)
     SET(PATHLIST 
        ${RTIS_ROOT_DIR}/lib/linux_g++-3.4
	${RTIS_ROOT_DIR}/lib/linux_g++-4.0
        ${RTIS_ROOT_DIR}/lib/linux_g++-4.1
        ${RTIS_ROOT_DIR}/lib/linux_g++-4.2
        ${RTIS_ROOT_DIR}/lib/linux_g++-4.3
     )
  ENDIF (UNIX)
ENDIF (APPLE)

IF (WIN32)
     SET(PATHLIST 
	${RTIS_ROOT_DIR}/lib/winnt_vc++-8.0
	${RTIS_ROOT_DIR}/lib/winnt_vc++-7.1
	${RTIS_ROOT_DIR}/lib/winnt_vc++-9.0
     )
ENDIF (WIN32)

MACRO(FIND_RTIS_LIBRARY MYLIBRARY MYLIBRARYNAME)

    FIND_LIBRARY(${MYLIBRARY}
        NAMES ${MYLIBRARYNAME}
        PATHS ${PATHLIST}
    )

ENDMACRO(FIND_RTIS_LIBRARY LIBRARY LIBRARYNAME)

SET(RTI_13_LIST rti13 librti13)
SET(RTI_FEDTIME13_LIST fedtime13 libfedtime13)
SET(RTI_RTIFEDTIME_LIST rtifedtime librtifedtime)
SET(RTI_FEDTIME_LIST fedtime libfedtime)
SET(RTI_RTI_LIST rti librti)
SET(RTI_Z_LIST z zlib)
SET(RTI_NG_LIST RTI-NG libRTI-NG)

if (NOT RTIS_SINGLE_LIBRARY)
   FIND_RTIS_LIBRARY(RTIS_NG_LIBRARY "${RTI_NG_LIST}" )
   FIND_RTIS_LIBRARY(RTIS_MEMPOOL_LIBRARY mempool )
   FIND_RTIS_LIBRARY(RTIS_13_LIBRARY "${RTI_13_LIST}" )
   FIND_RTIS_LIBRARY(RTIS_URLREADER_LIBRARY urlreader )
   FIND_RTIS_LIBRARY(RTIS_BITVECTOR_LIBRARY bitvector )
   FIND_RTIS_LIBRARY(RTIS_MERSENNE_LIBRARY mersenne )
   FIND_RTIS_LIBRARY(RTIS_VTERM_LIBRARY vterm )
   FIND_RTIS_LIBRARY(RTIS_BUFMGR_LIBRARY bufmgr )
   FIND_RTIS_LIBRARY(RTIS_MSGSTATS_LIBRARY msgstats )
   FIND_RTIS_LIBRARY(RTIS_RTIFEDTIME_LIBRARY "${RTI_RTIFEDTIME_LIST}")
   FIND_RTIS_LIBRARY(RTIS_XREGEX_LIBRARY xregex )
   FIND_RTIS_LIBRARY(RTIS_FEDTIME_LIBRARY "${RTI_FEDTIME_LIST}" )
   FIND_RTIS_LIBRARY(RTIS_NETUTIL_LIBRARY netutil )
   FIND_RTIS_LIBRARY(RTIS_RTIINTERCEPT_LIBRARY rtiintercept ) 
   FIND_RTIS_LIBRARY(RTIS_Z_LIBRARY "${RTI_Z_LIST}" )
   FIND_RTIS_LIBRARY(RTIS_FEDTIME13_LIBRARY "${RTI_FEDTIME13_LIST}" )
   FIND_RTIS_LIBRARY(RTIS_PARSER_LIBRARY parser )
   FIND_RTIS_LIBRARY(RTIS_IF_LIBRARY if )
   FIND_RTIS_LIBRARY(RTIS_RID_LIBRARY rid )
   FIND_RTIS_LIBRARY(RTIS_MCAST_LIBRARY mcast )
   FIND_RTIS_LIBRARY(RTIS_RTI_LIBRARY "${RTI_RTI_LIST}" )
   FIND_RTIS_LIBRARY(RTIS_TIMERS_LIBRARY timers )
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

    if (NOT RTIS_ROOT_DIR)
        MESSAGE("RTIS_ROOT_DIR is not set, but the includes and libraries were found successfully. This unset var may be ignored.")
    endif (NOT RTIS_ROOT_DIR)
    
    if (NOT RTIS_SINGLE_LIBRARY)    
       SET(RTI_LIBRARIES
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
    else (NOT RTIS_SINGLE_LIBRARY)    
       SET(RTI_LIBRARIES
          ${RTIS_LIBRARY}
       )
    endif (NOT RTIS_SINGLE_LIBRARY)    
ELSE(RTIS_LIBRARY AND RTI_INCLUDE_DIR) 
    MESSAGE("Unable to Find RTI-s.  Try setting the cmake variable RTI_ROOT_DIR to the directory that contains include and lib/bin, or set the environment variable RTI_HOME.")
ENDIF(RTIS_LIBRARY AND RTI_INCLUDE_DIR)

