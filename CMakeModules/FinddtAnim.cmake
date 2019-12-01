# Locate dtAnim
#

INCLUDE(delta3d_common)
  
#variable names of the individual Delta3D libraries.  Can be used in application cmakelist.txt files.
FIND_DELTA3D_LIBRARY(DTANIM_LIBRARY         dtAnim)

if (FIND_dtAnim_REQUIRED)
   FIND_PACKAGE(Cal3d REQUIRED)
elseif(FIND_dtAnim_QUIETLY)
   FIND_PACKAGE(Cal3d QUIETLY)
else()
   FIND_PACKAGE(Cal3d)
endif()

SET(DTANIM_INCLUDE_DIRECTORIES ${DELTA3D_INCLUDE_DIR}
                               ${CAL3D_INCLUDE_DIR}
    )

CREATE_LINK_LINES_FOR_TARGETS(DTANIM_LIBRARIES
        DTANIM_LIBRARY
        CAL3D_LIBRARY
        )

# handle the QUIETLY and REQUIRED arguments and set DELTA3D_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(dtAnim DEFAULT_MSG DELTA3D_INCLUDE_DIR     DTANIM_LIBRARY
                                                     CAL3D_INCLUDE_DIR       CAL3D_LIBRARY
                                  )
