# Locate cppunit
# This module defines
# CPPUNIT_FOUND, if false, do not try to link to cppunit 
# CPPUNIT_INCLUDE_DIR, where to find the headers
#
# $CPPUNIT_DIR is an environment variable that would
# correspond to the ./configure --prefix=$CPPUNIT_DIR
#
# Created by Robert Osfield. 

FIND_PATH(CPPUNIT_INCLUDE_DIR cppunit/extensions/HelperMacros.h
    $ENV{CPPUNIT_DIR}/include
    $ENV{CPPUNIT_DIR}
    ${DELTA3D_EXT_DIR}/inc
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

MACRO(FIND_CPPUNIT_LIBRARY MYLIBRARY MYLIBRARYNAMES)
FIND_LIBRARY(${MYLIBRARY}
    NAMES ${MYLIBRARYNAMES}
    PATHS
    $ENV{CPPUNIT_DIR}/lib
    $ENV{CPPUNIT_DIR}
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
ENDMACRO(FIND_CPPUNIT_LIBRARY MYLIBRARY MYLIBRARYNAMES)

FIND_CPPUNIT_LIBRARY(CPPUNIT_LIBRARY cppunit)
IF (WIN32)
  FIND_CPPUNIT_LIBRARY(CPPUNIT_LIBRARY_DEBUG cppunitd)
ENDIF (WIN32)

SET(CPPUNIT_FOUND "NO")
IF(CPPUNIT_LIBRARY AND CPPUNIT_INCLUDE_DIR)
    SET(CPPUNIT_FOUND "YES")
ENDIF(CPPUNIT_LIBRARY AND CPPUNIT_INCLUDE_DIR)
