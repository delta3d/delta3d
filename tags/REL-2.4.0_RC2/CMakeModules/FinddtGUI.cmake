# Locate dtGUI
#

INCLUDE(delta3d_common)
  
#variable names of the individual Delta3D libraries.  Can be used in application cmakelist.txt files.
FIND_DELTA3D_LIBRARY(DTGUI_LIBRARY         dtGUI)
FIND_DELTA3D_LIBRARY(DTGUI_DEBUG_LIBRARY   dtGUID)

DELTA3D_FIND_PATH(CEGUI CEGUI/CEGUI.h)

SET(CEGUILIST CEGUI CEGUIBase)
DELTA3D_FIND_LIBRARY(CEGUI "${CEGUILIST}")

SET(CEGUIOGLList OpenGLGUIRenderer CEGUIOpenGLRenderer)
DELTA3D_FIND_LIBRARY(CEGUIOPENGL "${CEGUIOGLList}")


IF (NOT DTGUI_DEBUG_LIBRARY)
  SET(DTGUI_DEBUG_LIBRARY DTGUI_LIBRARY)
  MESSAGE(STATUS "No debug library was found for DTGUI_DEBUG_LIBRARY")
ENDIF()

#convienent list of libraries to link with when using dtGUI
SET(DTGUI_LIBRARIES
    optimized ${DTGUI_LIBRARY}      debug ${DTGUI_DEBUG_LIBRARY}
    optimized ${CEGUI_LIBRARY}      debug ${CEGUI_LIBRARY_DEBUG}
    optimized ${CEGUIOPENGL_LIBRARY} debug ${CEGUIOPENGL_LIBRARY_DEBUG}
    )
    
SET(DTGUI_INCLUDE_DIRECTORIES ${DELTA3D_INCLUDE_DIR}
                              ${CEGUI_INCLUDE_DIR}
                              ${CEGUI_INCLUDE_DIR}/CEGUI
    )


# handle the QUIETLY and REQUIRED arguments and set DELTA3D_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(dtGUI DEFAULT_MSG DELTA3D_INCLUDE_DIR DTGUI_LIBRARY       DTGUI_DEBUG_LIBRARY
                                                    CEGUI_INCLUDE_DIR   CEGUI_LIBRARY       CEGUI_LIBRARY_DEBUG
                                                                        CEGUIOPENGL_LIBRARY CEGUIOPENGL_LIBRARY_DEBUG
                                  )
