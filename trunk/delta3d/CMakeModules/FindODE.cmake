# Locate ode
# This module defines
# ODE_LIBRARY
# ODE_FOUND, if false, do not try to link to ode 
# ODE_INCLUDE_DIR, where to find the headers
#
# $ODE_DIR is an environment variable that would
# correspond to the ./configure --prefix=$ODE_DIR
#
# Created by David Guthrie.  Based on code by Robert Osfield 

FIND_PATH(ODE_INCLUDE_DIR ode/ode.h
    ${ODE_DIR}/include
    $ENV{ODE_DIR}/include
    $ENV{ODE_DIR}
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

FIND_LIBRARY(ODE_LIBRARY 
    NAMES cal3d
    PATHS
    ${ODE_DIR}/lib
    $ENV{ODE_DIR}/lib
    $ENV{ODE_DIR}
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

SET(ODE_FOUND "NO")
IF(ODE_LIBRARY AND ODE_INCLUDE_DIR)
    SET(ODE_FOUND "YES")
ENDIF(ODE_LIBRARY AND ODE_INCLUDE_DIR)


