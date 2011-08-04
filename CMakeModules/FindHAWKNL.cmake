# Locate hawkNL
# This module defines
# HAWKNL_LIBRARY
# HAWKNL_FOUND, if false, do not try to link to cal3d 
# HAWKNL_INCLUDE_DIR, where to find the headers
#
# $HAWKNL_DIR is an environment variable that would
# correspond to the ./configure --prefix=$HAWKNL_DIR
#
# Created by David Guthrie.  Based on code by Robert Osfield 

FIND_PATH(HAWKNL_INCLUDE_DIR nl.h
    ${HAWKNL_DIR}/include
    $ENV{HAWKNL_DIR}/include
    $ENV{HAWKNL_DIR}
    ${DELTA3D_EXT_DIR}/inc
    $ENV{DELTA_ROOT}/ext/inc
    ~/Library/Frameworks
    /Library/Frameworks
    /usr/local/include
    /usr/local/include/hawknl
    /usr/include
    /usr/include/hawknl
    /sw/include # Fink
    /opt/local/include # DarwinPorts
    /opt/csw/include # Blastwave
    /opt/include
    [HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment;OSG_ROOT]/include
    /usr/freeware/include
)

MACRO(FIND_HAWKNL_LIBRARY MYLIBRARY MYLIBRARYNAMES)

FIND_LIBRARY(${MYLIBRARY} 
    NAMES ${MYLIBRARYNAMES}	
    PATHS
    ${HAWKNL_DIR}/lib
    $ENV{HAWKNL_DIR}/lib
    $ENV{HAWKNL_DIR}
    ${DELTA3D_EXT_DIR}/lib
    $ENV{DELTA_ROOT}/ext/lib
    ~/Library/Frameworks
    /Library/Frameworks
    /usr/local/lib
    /usr/lib
    /sw/lib
    /opt/local/lib
    /opt/csw/lib
    /opt/lib
    [HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment;OSG_ROOT]/lib
    /usr/freeware/lib64
)

ENDMACRO(FIND_HAWKNL_LIBRARY MYLIBRARY MYLIBRARYNAMES)

SET(NL_RELEASE_STATIC_LIB_NAMES NLstatic NL nl)
SET(NL_DEBUG_STATIC_LIB_NAMES NLstaticD NLD nld)

FIND_HAWKNL_LIBRARY(HAWKNL_LIBRARY "${NL_RELEASE_STATIC_LIB_NAMES}")
FIND_HAWKNL_LIBRARY(HAWKNL_LIBRARY_DEBUG "${NL_DEBUG_STATIC_LIB_NAMES}")

SET(HAWKNL_FOUND "NO")
IF(HAWKNL_LIBRARY AND HAWKNL_INCLUDE_DIR)
    SET(HAWKNL_FOUND "YES")
ENDIF(HAWKNL_LIBRARY AND HAWKNL_INCLUDE_DIR)


