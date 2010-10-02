# Locate Xerces-c
# This module defines
# XERCES_LIBRARY
# XERCES_FOUND, if false, do not try to link to xerces-c 
# XERCES_INCLUDE_DIR, where to find the headers
#
# $XERCES_DIR is an environment variable that would
# correspond to the ./configure --prefix=$XERCES_DIR
#
# Created by Robert Osfield. 

FIND_PATH(XERCES_INCLUDE_DIR xercesc
    ${XERCES_DIR}/include
    $ENV{XERCES_DIR}/include
    $ENV{XERCES_DIR}
    ${DELTA3D_EXT_DIR}/inc
    $ENV{DELTA_ROOT}/ext/inc
    $ENV{DELTA_ROOT}
    $ENV{OSG_ROOT}/include
    ~/Library/Frameworks/Xerces.framework/Headers
    /Library/Frameworks/Xerces.framework/Headers
    /usr/local/include
    /usr/include
    /sw/include # Fink
    /opt/local/include # DarwinPorts
    /opt/csw/include # Blastwave
    /opt/include
    [HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment;OSG_ROOT]/include
    /usr/freeware/include
)

MACRO(FIND_XERCES_LIBRARY MYLIBRARY MYLIBRARYNAME)

FIND_LIBRARY(${MYLIBRARY}
    NAMES ${MYLIBRARYNAME} 
    PATHS
    ${XERCES_DIR}/lib
    $ENV{XERCES_DIR}/lib
    $ENV{XERCES_DIR}
    ${DELTA3D_EXT_DIR}/lib
    $ENV{DELTA_ROOT}/ext/lib
    $ENV{DELTA_ROOT}
    $ENV{OSG_ROOT}/lib
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

ENDMACRO(FIND_XERCES_LIBRARY MYLIBRARY MYLIBRARYNAME)

SET(XERCES_LIST Xerces xerces-c xerces-c_2 xerces-c_3)
FIND_XERCES_LIBRARY(XERCES_LIBRARY "${XERCES_LIST}")
SET(XERCES_DEBUG_LIST Xerces xerces-c_2d xerces-c_3D)
FIND_XERCES_LIBRARY(XERCES_LIBRARY_DEBUG "${XERCES_DEBUG_LIST}")

SET(XERCES_FOUND "NO")
IF(XERCES_LIBRARY AND XERCES_INCLUDE_DIR)
    SET(XERCES_FOUND "YES")
ENDIF(XERCES_LIBRARY AND XERCES_INCLUDE_DIR)


