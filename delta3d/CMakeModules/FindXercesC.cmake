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

FIND_PATH(XERCES_INCLUDE_DIR xercesc/parsers/SAXParser.hpp
    ${XERCES_DIR}/include
    $ENV{XERCES_DIR}/include
    $ENV{XERCES_DIR}
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

FIND_LIBRARY(XERCES_LIBRARY 
    NAMES Xerces xerces-c xerces-c_2
    PATHS
    ${XERCES_DIR}/lib
    $ENV{XERCES_DIR}/lib
    $ENV{XERCES_DIR}
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

SET(XERCES_FOUND "NO")
IF(XERCES_LIBRARY AND XERCES_INCLUDE_DIR)
    SET(XERCES_FOUND "YES")
ENDIF(XERCES_LIBRARY AND XERCES_INCLUDE_DIR)


