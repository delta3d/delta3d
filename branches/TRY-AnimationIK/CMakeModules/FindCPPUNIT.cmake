# Locate cppunit
# This module defines
# FLTK_LIBRARY
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

FIND_LIBRARY(cppunit
    NAMES cppunit
    PATHS
    $ENV{CPPUNIT_DIR}/lib
    $ENV{CPPUNIT_DIR}
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

SET(CPPUNIT_FOUND "NO")
IF(CPPUNIT_LIBRARY AND CPPUNIT_INCLUDE_DIR)
    SET(CPPUNIT_FOUND "YES")
ENDIF(CPPUNIT_LIBRARY AND CPPUNIT_INCLUDE_DIR)