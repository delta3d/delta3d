# Locate cal3d
# This module defines
# CAL3D_LIBRARY
# CAL3D_FOUND, if false, do not try to link to cal3d 
# CAL3D_INCLUDE_DIR, where to find the headers
#
# $CAL3D_DIR is an environment variable that would
# correspond to the ./configure --prefix=$CAL3D_DIR
#
# Created by David Guthrie.  Based on code by Robert Osfield 

FIND_PATH(CAL3D_INCLUDE_DIR cal3d.h
    ${CAL3D_DIR}/include
    $ENV{CAL3D_DIR}/include
    $ENV{CAL3D_DIR}
    $ENV{DELTA_ROOT}/ext/inc
    $ENV{DELTA_ROOT}
    ~/Library/Frameworks
    /Library/Frameworks
    /usr/local/include
    /usr/include
    /usr/include/cal3d
    /sw/include # Fink
    /opt/local/include # DarwinPorts
    /opt/csw/include # Blastwave
    /opt/include
    [HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment;OSG_ROOT]/include
    /usr/freeware/include
)

FIND_LIBRARY(CAL3D_LIBRARY 
    NAMES cal3d
    PATHS
    ${CAL3D_DIR}/lib
    $ENV{CAL3D_DIR}/lib
    $ENV{CAL3D_DIR}
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

SET(CAL3D_FOUND "NO")
IF(CAL3D_LIBRARY AND CAL3D_INCLUDE_DIR)
    SET(CAL3D_FOUND "YES")
ENDIF(CAL3D_LIBRARY AND CAL3D_INCLUDE_DIR)


