# Locate CEGUI
# This module defines
# CEGUI_LIBRARY
# CEGUI_FOUND, if false, do not try to link to CEGUI
# CEGUI_INCLUDE_DIR, where to find the headers
#
# $CEGUI_DIR is an environment variable that would
# correspond to the ./configure --prefix=$CEGUI_DIR
#
# Created by David Guthrie with code from Robert Osfield. 

FIND_PATH(CEGUI_INCLUDE_DIR CEGUI/CEGUI.h 
    $ENV{CEGUI_DIR}/include
    $ENV{CEGUI_DIR}
    $ENV{CEGUIDIR}/include
    $ENV{CEGUIDIR}
    $ENV{CEGUI_ROOT}/include
    $ENV{DELTA_ROOT}/ext/inc
    ~/Library/Frameworks
    /Library/Frameworks
    /usr/local/include
    /usr/include
    /sw/include # Fink
    /opt/local/include # DarwinPorts
    /opt/csw/include # Blastwave
    /opt/include
    [HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment;CEGUI_ROOT]/include
    /usr/freeware/include
)

MACRO(FIND_CEGUI_LIBRARY MYLIBRARY MYLIBRARYNAME)

    FIND_LIBRARY(${MYLIBRARY}
        NAMES ${MYLIBRARYNAME}
        PATHS
        $ENV{CEGUI_DIR}/lib
        $ENV{CEGUI_DIR}
        $ENV{CEGUIDIR}/lib
        $ENV{CEGUIDIR}
        $ENV{CEGUI_ROOT}/lib
        $ENV{DELTA_ROOT}/ext/lib
        ~/Library/Frameworks
        /Library/Frameworks
        /usr/local/lib
        /usr/lib
        /sw/lib
        /opt/local/lib
        /opt/csw/lib
        /opt/lib
        [HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment;CEGUI_ROOT]/lib
        /usr/freeware/lib64
    )

ENDMACRO(FIND_CEGUI_LIBRARY LIBRARY LIBRARYNAME)

FIND_CEGUI_LIBRARY(CEGUI_LIBRARY CEGUI)
FIND_CEGUI_LIBRARY(CEGUIOPENGL_LIBRARY CEGUIOpenGLRenderer)

SET(CEGUI_FOUND "NO")
IF(CEGUI_LIBRARY AND CEGUI_INCLUDE_DIR)
    SET(CEGUI_FOUND "YES")
ENDIF(CEGUI_LIBRARY AND CEGUI_INCLUDE_DIR)
