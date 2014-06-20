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
    ${DELTA3D_EXT_DIR}/inc
    $ENV{DELTA_ROOT}/ext/inc
    $ENV{DELTA_ROOT}/../open-dis/cpp
    ~/Library/Frameworks
    /Library/Frameworks
    /sw/include # Fink
    /opt/local/include # DarwinPorts
    /opt/csw/include # Blastwave
    /opt/include
    [HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment;OSG_ROOT]/include
    /usr/freeware/include
    DOC "The folder that contains the DIS/*.h header files"
)
MARK_AS_ADVANCED(DIS_INCLUDE_DIR)

FIND_PATH(DIS_CPPUTILS_INCLUDE_DIR DIS/DataStream.h
    ${DIS_INCLUDE_DIR}/../CppUtils
    ${DIS_DIR}/include
    $ENV{DIS_DIR}/include
    $ENV{DIS_DIR}
    ${DELTA3D_EXT_DIR}/inc
    ${DELTA3D_EXT_DIR}/inc/DIS/CppUtils
    $ENV{DELTA_ROOT}/ext/inc
    $ENV{DELTA_ROOT}/../open-dis/CppUtils
    ~/Library/Frameworks
    /Library/Frameworks
    /sw/include # Fink
    /opt/local/include # DarwinPorts
    /opt/csw/include # Blastwave
    /opt/include
    [HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment;OSG_ROOT]/include
    /usr/freeware/include
		DOC "The folder that contains the DIS/*.h CppUtils utility header files"
)
MARK_AS_ADVANCED(DIS_CPPUTILS_INCLUDE_DIR)

MACRO(FIND_DIS_LIBRARY MYLIBRARY MYLIBRARYNAME)
FIND_LIBRARY(${MYLIBRARY}
    NAMES ${MYLIBRARYNAME}
    PATHS
    ${DIS_INCLUDE_DIR}/../bin
    ${DIS_INCLUDE_DIR}/../lib
    ${DIS_DIR}/lib
    $ENV{DIS_DIR}/lib
    $ENV{DIS_DIR}
    ${DELTA3D_EXT_DIR}/lib
    $ENV{DELTA_ROOT}/ext/lib
    $ENV{DELTA_ROOT}/../open-dis/bin
    ~/Library/Frameworks
    /Library/Frameworks
    /sw/lib
    /opt/local/lib
    /opt/csw/lib
    /opt/lib
    [HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment;OSG_ROOT]/lib
    /usr/freeware/lib64
)
MARK_AS_ADVANCED(${MYLIBRARY})
ENDMACRO(FIND_DIS_LIBRARY MYLIBRARY MYLIBRARYNAME)

FIND_DIS_LIBRARY(DIS_LIBRARY DIS)
FIND_DIS_LIBRARY(DIS_LIBRARY_DEBUG DIS_debug)
  
SET(DIS_FOUND "NO")
IF(DIS_LIBRARY AND DIS_INCLUDE_DIR)
    SET(DIS_FOUND "YES")
ENDIF(DIS_LIBRARY AND DIS_INCLUDE_DIR)



