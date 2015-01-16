# Locate dtGUI
#

INCLUDE(delta3d_common)
  
#variable names of the individual Delta3D libraries.  Can be used in application cmakelist.txt files.
FIND_DELTA3D_LIBRARY(DTGUI_LIBRARY         dtGUI)

if (FIND_dtGUI_REQUIRED)
   FIND_PACKAGE(CEGUI REQUIRED)
elseif(FIND_dtGUI_QUIETLY)
   FIND_PACKAGE(CEGUI QUIETLY)
else()
   FIND_PACKAGE(CEGUI)
endif()


CREATE_LINK_LINES_FOR_TARGETS(DTGUI_LIBRARIES
   DTGUI_LIBRARY
   CEGUI_LIBRARY
   CEGUIOPENGL_LIBRARY
   )

SET(DTGUI_INCLUDE_DIRECTORIES ${DELTA3D_INCLUDE_DIR}
                              ${CEGUI_INCLUDE_DIR}
                              ${CEGUI_INCLUDE_DIR}/CEGUI
    )


# handle the QUIETLY and REQUIRED arguments and set DELTA3D_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(dtGUI DEFAULT_MSG DELTA3D_INCLUDE_DIR DTGUI_LIBRARY       
                                                    CEGUI_INCLUDE_DIR   CEGUI_LIBRARY      
                                                                        CEGUIOPENGL_LIBRARY
                                  )
