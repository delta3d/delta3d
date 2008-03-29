# Locate FLTK
# This module defines
# FLTK_LIBRARY
# FLTK_FOUND, if false, do not try to link to fltk
# FLTK_INCLUDE_DIR, where to find the headers
#
# $FLTK_DIR is an environment variable that would
# correspond to the ./configure --prefix=$FLTK_DIR

FIND_PATH(FLTK_INCLUDE_DIR FL.H
    $ENV{FLTK_DIR}/include
    $ENV{FLTK_DIR}
    $ENV{DELTA_ROOT}/ext/inc/FL
    ~/Library/Frameworks
    /Library/Frameworks
    /usr/local/include
    /usr/include
    /usr/include/fltk
    /sw/include # Fink
    /opt/local/include # DarwinPorts
    /opt/csw/include # Blastwave
    /opt/include
    [HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment;
    /usr/freeware/include
)

MACRO(FIND_FLTK_LIBRARY MYLIBRARY MYLIBRARYNAME)

FIND_LIBRARY(${MYLIBRARY} 
    NAMES ${MYLIBRARYNAME}
    PATHS
    ${FLTK_DIR}/lib
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
    [HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment;
    /usr/freeware/lib64
)

ENDMACRO(FIND_FLTK_LIBRARY MYLIBRARY MYLIBRARYNAME)

FIND_FLTK_LIBRARY(FLTK_LIBRARY fltk)
FIND_FLTK_LIBRARY(FLTK_LIBRARY_DEBUG fltkd)

SET(FLTK_FOUND "NO")
IF(FLTK_LIBRARY AND FLTK_INCLUDE_DIR)
    SET(FLTK_FOUND "YES")
ENDIF(FLTK_LIBRARY AND FLTK_INCLUDE_DIR)


