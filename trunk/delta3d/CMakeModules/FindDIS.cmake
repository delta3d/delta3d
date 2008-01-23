# Locate DIS
# This module defines
# DIS_LIBRARY
# DIS_FOUND, if false, do not try to link to DIS 
# DIS_INCLUDE_DIR, where to find the headers
#
# $DIS_DIR is an environment variable that would
# correspond to the ./configure --prefix=$DIS_DIR
#
# Created by David Guthrie.  Based on code by Robert Osfield 

FIND_PATH(DIS_INCLUDE_DIR DIS/Pdu.h
    ${DIS_DIR}/include
    $ENV{DIS_DIR}/include
    $ENV{DIS_DIR}
    $ENV{DELTA_ROOT}/ext/inc
    ~/Library/Frameworks
    /Library/Frameworks
    /usr/local/include
    /usr/include
    /sw/include # Fink
    /opt/local/include # DarwinPorts
    /opt/csw/include # Blastwave
    /opt/include
    [HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment;OSG_ROOT]/include
    /usr/freeware/include
)

FIND_LIBRARY(DIS_LIBRARY 
    NAMES DIS
    PATHS
    ${DIS_DIR}/lib
    $ENV{DIS_DIR}/lib
    $ENV{DIS_DIR}
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

SET(DIS_FOUND "NO")
IF(DIS_LIBRARY AND DIS_INCLUDE_DIR)
    SET(DIS_FOUND "YES")
ENDIF(DIS_LIBRARY AND DIS_INCLUDE_DIR)


