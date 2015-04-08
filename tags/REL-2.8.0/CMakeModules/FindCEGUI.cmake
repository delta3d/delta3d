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

IF (APPLE)
   FIND_PATH(CEGUI_FRAMEWORK_DIR CEGUI.h 
     HINTS
       ${DELTA3D_EXT_DIR}/Frameworks/CEGUIBase.framework/Headers
       ${DELTA3D_EXT_DIR}/Frameworks/CEGUI.framework/Headers
     PATHS
       ~/Library/Frameworks/CEGUIBase.framework/Headers
       /Library/Frameworks/CEGUIBase.framework/Headers
       ~/Library/Frameworks/CEGUI.framework/Headers
       /Library/Frameworks/CEGUI.framework/Headers
   )
   MARK_AS_ADVANCED(CEGUI_FRAMEWORK_DIR)
ENDIF (APPLE)

FIND_PATH(CEGUI_INCLUDE_DIR NAMES CEGUI/CEGUI.h CEGUI.h
    HINTS
    ${CEGUI_FRAMEWORK_DIR}
    $ENV{CEGUI_DIR}/include
    $ENV{CEGUIDIR}/include
    $ENV{CEGUI_ROOT}/include
    ${DELTA3D_EXT_DIR}/inc
    ${DELTA3D_EXT_DIR}/Frameworks
    $ENV{DELTA_ROOT}/ext/inc
    PATHS
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
MARK_AS_ADVANCED(CEGUI_INCLUDE_DIR)

IF (CEGUI_FRAMEWORK_DIR)
   SET(CEGUI_INCLUDE_DIR ${CEGUI_INCLUDE_DIR} ${CEGUI_FRAMEWORK_DIR})
ELSE (CEGUI_FRAMEWORK_DIR)
   SET(CEGUI_INCLUDE_DIR ${CEGUI_INCLUDE_DIR} ${CEGUI_INCLUDE_DIR}/CEGUI)
ENDIF (CEGUI_FRAMEWORK_DIR)

MACRO(FIND_CEGUI_LIBRARY MYLIBRARY MYLIBRARYNAMES)

    FIND_LIBRARY(${MYLIBRARY}
        NAMES ${MYLIBRARYNAMES}
        HINTS
        $ENV{CEGUI_DIR}/lib
        $ENV{CEGUI_DIR}
        $ENV{CEGUIDIR}/lib
        $ENV{CEGUIDIR}
        $ENV{CEGUI_ROOT}/lib
        ${DELTA3D_EXT_DIR}/lib
        $ENV{DELTA_ROOT}/ext/lib
        PATHS
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

    MARK_AS_ADVANCED(${MYLIBRARY})

ENDMACRO(FIND_CEGUI_LIBRARY LIBRARY LIBRARYNAME)

SET(CEGUIList CEGUIBase CEGUI)
FIND_CEGUI_LIBRARY(CEGUI_LIBRARY "${CEGUIList}")

SET(CEGUIDebugList CEGUIBase_d)
FIND_CEGUI_LIBRARY(CEGUI_LIBRARY_DEBUG "${CEGUIDebugList}")

SET(CEGUIOGLList OpenGLGUIRenderer CEGUIOpenGLRenderer)
FIND_CEGUI_LIBRARY(CEGUIOPENGL_LIBRARY "${CEGUIOGLList}")

SET(CEGUIOGLDebugList OpenGLGUIRenderer_d CEGUIOpenGLRenderer_d)
FIND_CEGUI_LIBRARY(CEGUIOPENGL_LIBRARY_DEBUG "${CEGUIOGLDebugList}")

SET(CEGUI_FOUND "NO")
IF(CEGUI_LIBRARY AND CEGUI_INCLUDE_DIR)
    SET(CEGUI_FOUND "YES")
ENDIF(CEGUI_LIBRARY AND CEGUI_INCLUDE_DIR)
