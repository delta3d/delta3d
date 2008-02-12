# Locate GNE
# This module defines
# GNE_LIBRARY
# GNE_FOUND, if false, do not try to link to cal3d 
# GNE_INCLUDE_DIR, where to find the headers
#
# $GNE_DIR is an environment variable that would
# correspond to the ./configure --prefix=$GNE_DIR
#
# Created by David Guthrie.  Based on code by Robert Osfield 

FIND_PATH(GNE_INCLUDE_DIR gnelib.h
    ${GNE_DIR}/include
    $ENV{GNE_DIR}/include
    $ENV{GNE_DIR}
    $ENV{DELTA_ROOT}/ext/inc
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

FIND_LIBRARY(GNE_LIBRARY 
    NAMES gne
    PATHS
    ${GNE_DIR}/lib
    $ENV{GNE_DIR}/lib
    $ENV{GNE_DIR}
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

SET(GNE_FOUND "NO")
IF(GNE_LIBRARY AND GNE_INCLUDE_DIR)
    SET(GNE_FOUND "YES")
ENDIF(GNE_LIBRARY AND GNE_INCLUDE_DIR)


