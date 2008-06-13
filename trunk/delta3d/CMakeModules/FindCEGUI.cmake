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
    PATHS
    $ENV{CEGUI_DIR}/include
    $ENV{CEGUIDIR}/include
    $ENV{CEGUI_ROOT}/include
    ${DELTA3D_EXT_DIR}/inc
    ${DELTA3D_EXT_DIR}/Frameworks
    ~/Library/Frameworks
    /Library/Frameworks
    /usr/local/include
    /usr/include
    /sw/include # Fink
    /opt/local/include # DarwinPorts
    /opt/csw/include # Blastwave
    /opt/include
    /usr/freeware/include
)

IF (APPLE)
   FIND_PATH(CEGUI_FRAMEWORK_DIR CEGUI.h 
     PATHS
       ~/Library/Frameworks/CEGUI.framework/Headers
       /Library/Frameworks/CEGUI.framework/Headers
       ${DELTA3D_EXT_DIR}/Frameworks/CEGUI.framework/Headers
)
ENDIF (APPLE)

IF (CEGUI_FRAMEWORK_DIR)
   SET(CEGUI_INCLUDE_DIR ${CEGUI_INCLUDE_DIR} ${CEGUI_FRAMEWORK_DIR})
ELSE (CEGUI_FRAMEWORK_DIR)
   SET(CEGUI_INCLUDE_DIR ${CEGUI_INCLUDE_DIR} ${CEGUI_INCLUDE_DIR}/CEGUI)
ENDIF (CEGUI_FRAMEWORK_DIR)

MACRO(FIND_CEGUI_LIBRARY MYLIBRARY MYLIBRARYNAMES)

    FIND_LIBRARY(${MYLIBRARY}
        NAMES ${MYLIBRARYNAMES}
        PATHS
        $ENV{CEGUI_DIR}/lib
        $ENV{CEGUI_DIR}
        $ENV{CEGUIDIR}/lib
        $ENV{CEGUIDIR}
        $ENV{CEGUI_ROOT}/lib
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
