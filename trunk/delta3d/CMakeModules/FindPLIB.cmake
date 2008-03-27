# Locate plib
# This module defines
# PLIB_LIBRARY
# PLIB_FOUND, if false, do not try to link to plib
# PLIB_INCLUDE_DIR, where to find the headers
#
# $PLIB_DIR is an environment variable that would
# correspond to the ./configure --prefix=$PLIB_DIR
#
# Created David Guthrie with code by Robert Osfield. 

FIND_PATH(PLIB_INCLUDE_DIR plib/js.h
    $ENV{PLIB_DIR}/include
    $ENV{PLIB_DIR}
    $ENV{PLIB_ROOT}/include
    $ENV{DELTA_ROOT}/ext/inc
    ~/Library/Frameworks
    /Library/Frameworks
    /usr/local/include
    /usr/include
    /sw/include # Fink
    /opt/local/include # DarwinPorts
    /opt/csw/include # Blastwave
    /opt/include
    [HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment;PLIB_ROOT]/include
    /usr/freeware/include
)

MACRO(FIND_PLIB_LIBRARY MYLIBRARY MYLIBRARYNAME)

    FIND_LIBRARY(${MYLIBRARY}
        NAMES ${MYLIBRARYNAME}
        PATHS
        $ENV{PLIB_DIR}/lib
        $ENV{PLIB_DIR}
        $ENV{OSGDIR}/lib
        $ENV{OSGDIR}
        $ENV{PLIB_ROOT}/lib
        $ENV{DELTA_ROOT}/ext/lib
        ~/Library/Frameworks
        /Library/Frameworks
        /usr/local/lib
        /usr/lib
        /sw/lib
        /opt/local/lib
        /opt/csw/lib
        /opt/lib
        [HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment;PLIB_ROOT]/lib
        /usr/freeware/lib64
    )

ENDMACRO(FIND_PLIB_LIBRARY LIBRARY LIBRARYNAME)

FIND_PLIB_LIBRARY(PLIB_JS_LIBRARY js)
FIND_PLIB_LIBRARY(PLIB_JS_LIBRARY_DEBUG js_d)
FIND_PLIB_LIBRARY(PLIB_UL_LIBRARY ul)
FIND_PLIB_LIBRARY(PLIB_UL_LIBRARY_DEBUG ul_d)

SET(PLIB_FOUND "NO")
IF(PLIB_LIBRARY AND PLIB_INCLUDE_DIR)
    SET(PLIB_FOUND "YES")
ENDIF(PLIB_LIBRARY AND PLIB_INCLUDE_DIR)
