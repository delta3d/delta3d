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

FIND_PATH(CAL3D_INCLUDE_DIR cal3d/cal3d.h
    PATHS
    $ENV{CAL3D_DIR}/include
    $ENV{CAL3D_DIR}
    ${DELTA3D_EXT_DIR}/inc
    ${DELTA_DIR}
    $ENV{DELTA_ROOT}/ext/inc
    $ENV{DELTA_ROOT}
    ~/Library/Frameworks
    /Library/Frameworks
    /sw/include # Fink
    /opt/local/include # DarwinPorts
    /opt/csw/include # Blastwave
    /opt/include
    /usr/freeware/include
    PATH_SUFFIXES src
)
MARK_AS_ADVANCED(CAL3D_INCLUDE_DIR)

MACRO(FIND_CAL3D_LIBRARY MYLIBRARY MYLIBRARYNAME)

FIND_LIBRARY(${MYLIBRARY} 
    NAMES ${MYLIBRARYNAME}
    HINTS
    $ENV{CAL3D_DIR}
    ${DELTA3D_EXT_DIR}
    ${DELTA_DIR}
    $ENV{DELTA_ROOT}
	PATHS
    ~/Library/Frameworks
    /Library/Frameworks
    /usr/local/lib
    /usr/lib
    /sw/lib
    /opt/local/lib
    /opt/csw/lib
    /opt/lib    
    /usr/freeware/lib64
	PATH_SUFFIXES
	lib
	bin/Debug
	bin/Release
	ext/lib
)
MARK_AS_ADVANCED(${MYLIBRARY})
ENDMACRO(FIND_CAL3D_LIBRARY MYLIBRARY MYLIBRARYNAME)

FIND_CAL3D_LIBRARY(CAL3D_LIBRARY cal3d)
FIND_CAL3D_LIBRARY(CAL3D_LIBRARY_DEBUG cal3d_d)

SET(CAL3D_FOUND "NO")
IF(CAL3D_LIBRARY AND CAL3D_INCLUDE_DIR)
    SET(CAL3D_FOUND "YES")
ENDIF(CAL3D_LIBRARY AND CAL3D_INCLUDE_DIR)


