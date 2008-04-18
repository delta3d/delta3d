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

FIND_PATH(CEGUI_INCLUDE_DIR CEGUI.h 
    $ENV{CEGUI_DIR}/include/CEGUI
    $ENV{CEGUI_DIR}
    $ENV{CEGUIDIR}/include/CEGUI
    $ENV{CEGUIDIR}
    $ENV{CEGUI_ROOT}/include/CEGUI
    ${DELTA_DIR}/ext/inc/CEGUI
    $ENV{DELTA_ROOT}/ext/inc/CEGUI
    ~/Library/Frameworks/CEGUI.framework/Headers/CEGUI
    /Library/Frameworks/CEGUI.framework/Headers/CEGUI
    /usr/local/include/CEGUI
    /usr/include/CEGUI
    /sw/include/CEGUI # Fink
    /opt/local/include/CEGUI # DarwinPorts
    /opt/csw/include/CEGUI # Blastwave
    /opt/include/CEGUI
    [HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment;CEGUI_ROOT]/include/CEGUI
    /usr/freeware/include/CEGUI
)

MACRO(FIND_CEGUI_LIBRARY MYLIBRARY MYLIBRARYNAMES)

    FIND_LIBRARY(${MYLIBRARY}
        NAMES ${MYLIBRARYNAMES}
        PATHS
        $ENV{CEGUI_DIR}/lib
        $ENV{CEGUI_DIR}
        $ENV{CEGUIDIR}/lib
        $ENV{CEGUIDIR}
        $ENV{CEGUI_ROOT}/lib
        ${DELTA_DIR}/ext/lib
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

SET(CEGUIList CEGUIBase CEGUI)
FIND_CEGUI_LIBRARY(CEGUI_LIBRARY "${CEGUIList}")

SET(CEGUIOGLList OpenGLGUIRenderer CEGUIOpenGLRenderer)
FIND_CEGUI_LIBRARY(CEGUIOPENGL_LIBRARY "${CEGUIOGLList}")

FIND_CEGUI_LIBRARY(CEGUI_LIBRARY_DEBUG CEGUIBase_d)
FIND_CEGUI_LIBRARY(CEGUIOPENGL_LIBRARY_DEBUG OpenGLGUIRenderer_d)

SET(CEGUI_FOUND "NO")
IF(CEGUI_LIBRARY AND CEGUI_INCLUDE_DIR)
    SET(CEGUI_FOUND "YES")
ENDIF(CEGUI_LIBRARY AND CEGUI_INCLUDE_DIR)
