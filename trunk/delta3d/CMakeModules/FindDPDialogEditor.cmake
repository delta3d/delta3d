# Locate DPDialogEditor

INCLUDE(delta3d_common)
  
#variable names of the individual Delta3D libraries.  Can be used in application cmakelist.txt files.
FIND_DELTA3D_LIBRARY(DP_DIALOG_EDITOR_LIBRARY       DPDialogEditor)
FIND_DELTA3D_LIBRARY(DP_DIALOG_EDITOR_DEBUG_LIBRARY DPDialogEditord)

IF (NOT DP_DIALOG_EDITOR_DEBUG_LIBRARY)
  SET(DP_DIALOG_EDITOR_DEBUG_LIBRARY ${DP_DIALOG_EDITOR_LIBRARY})
  MESSAGE(STATUS "No debug library was found for DP_DIALOG_EDITOR_DEBUG_LIBRARY")
ENDIF()

#convienent list of libraries to link with when using DPDialogEditor
SET(DP_DIALOG_EDITOR_LIBRARIES
    optimized ${DP_DIALOG_EDITOR_LIBRARY} debug ${DP_DIALOG_EDITOR_DEBUG_LIBRARY}
    )
    
SET(DP_DIALOG_EDITOR_INCLUDE_DIRECTORIES ${DELTA3D_ROOT}/utilities/DirectorPlugins/DPDialogEditor
    )

# handle the QUIETLY and REQUIRED arguments and set DELTA3D_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(DPDialogEditor DEFAULT_MSG DP_DIALOG_EDITOR_INCLUDE_DIRECTORIES DP_DIALOG_EDITOR_LIBRARY DP_DIALOG_EDITOR_DEBUG_LIBRARY
                                  )
