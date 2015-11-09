# Locate dtAudio
#

INCLUDE(delta3d_common)
  
#variable names of the individual Delta3D libraries.  Can be used in application cmakelist.txt files.
FIND_DELTA3D_LIBRARY(DTAUDIO_LIBRARY         dtAudio)

if (FIND_dtAudio_REQUIRED)
   FIND_PACKAGE(OpenAL REQUIRED)
elseif(FIND_dtAudio_QUIETLY)
   FIND_PACKAGE(OpenAL QUIETLY)
else()
   FIND_PACKAGE(OpenAL)
endif()
    
SET(DTAUDIO_INCLUDE_DIRECTORIES ${DELTA3D_INCLUDE_DIR}
                               ${OPENAL_INCLUDE_DIR}
                               ${ALUT_INCLUDE_DIR}
    )

CREATE_LINK_LINES_FOR_TARGETS(DTAUDIO_LIBRARIES
        DTAUDIO_LIBRARY
        OPENAL_LIBRARY
        ALUT_LIBRARY
        )

# handle the QUIETLY and REQUIRED arguments and set DELTA3D_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(dtAudio DEFAULT_MSG DELTA3D_INCLUDE_DIR     DTAUDIO_LIBRARY
                                                     OPENAL_INCLUDE_DIR       OPENAL_LIBRARY
                                                     ALUT_INCLUDE_DIR         ALUT_LIBRARY
                                  )
