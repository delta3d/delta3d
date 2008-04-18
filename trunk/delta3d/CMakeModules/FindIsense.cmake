# Locate isense
# This module defines
# ISENSE_LIBRARY
# ISENSE_FOUND, if false, do not try to link to gdal 
# ISENSE_INCLUDE_DIR, where to find the headers
#
# $ISENSE_DIR is an environment variable that would
# correspond to the ./configure --prefix=$ISENSE_DIR
#
# Created by David Guthrie with code by Robert Osfield. 

FIND_PATH(ISENSE_INCLUDE_DIR isense.h
    ${ISENSE_DIR}/include
    $ENV{ISENSE_DIR}/include
    $ENV{ISENSE_DIR}
    ${DELTA_DIR}/ext/inc
    $ENV{DELTA_ROOT}/ext/inc
    $ENV{DELTA_ROOT}
    ~/Library/Frameworks
    /Library/Frameworks
    /usr/local/include
    /usr/include
    /usr/include/gdal
    /sw/include # Fink
    /opt/local/include # DarwinPorts
    /opt/csw/include # Blastwave
    /opt/include
    [HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment;OSG_ROOT]/include
    /usr/freeware/include
)

MACRO(FIND_ISENSE_LIBRARY MYLIBRARY MYLIBRARYNAME)

FIND_LIBRARY(${MYLIBRARY} 
    NAMES ${MYLIBRARYNAME}
    PATHS
    ${ISENSE_DIR}/lib
    $ENV{ISENSE_DIR}/lib
    $ENV{ISENSE_DIR}
    ${DELTA_DIR}/ext/lib
    $ENV{DELTA_ROOT}/ext/lib
    $ENV{DELTA_ROOT}
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

ENDMACRO(FIND_ISENSE_LIBRARY MYLIBRARY MYLIBRARYNAME)

FIND_ISENSE_LIBRARY(ISENSE_LIBRARY isense)
FIND_ISENSE_LIBRARY(ISENSE_LIBRARY_DEBUG isensed)

SET(ISENSE_FOUND "NO")
IF(ISENSE_LIBRARY AND ISENSE_INCLUDE_DIR)
    SET(ISENSE_FOUND "YES")
ENDIF(ISENSE_LIBRARY AND ISENSE_INCLUDE_DIR)


