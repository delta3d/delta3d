# Locate dtAudio
#

INCLUDE(delta3d_common)
  
#variable names of the individual Delta3D libraries.  Can be used in application cmakelist.txt files.
FIND_DELTA3D_LIBRARY(DTAUDIO_LIBRARY         dtAudio)
FIND_DELTA3D_LIBRARY(DTAUDIO_DEBUG_LIBRARY   dtAudioD)

SET(OPENAL_HEADERS al.h AL/al.h)

DELTA3D_FIND_PATH   (OPENAL "${OPENAL_HEADERS}")
SET(OPENAL_NAMES openal al OpenAL32)
DELTA3D_FIND_LIBRARY(OPENAL "${OPENAL_NAMES}")

DELTA3D_FIND_PATH   (ALUT AL/alut.h)
DELTA3D_FIND_LIBRARY(ALUT alut)




IF (NOT DTAUDIO_DEBUG_LIBRARY)
  SET(DTAUDIO_DEBUG_LIBRARY DTAUDIO_LIBRARY)
  MESSAGE(STATUS "No debug library was found for DTAUDIO_DEBUG_LIBRARY")
ENDIF()

#convienent list of libraries to link with when using dtAudio
SET(DTAUDIO_LIBRARIES
    optimized ${DTAUDIO_LIBRARY}      debug ${DTAUDIO_DEBUG_LIBRARY}
    optimized ${OPENAL_LIBRARY}       debug ${OPENAL_LIBRARY_DEBUG}
    optimized ${ALUT_LIBRARY}         debug ${ALUT_LIBRARY_DEBUG}
    )
    
SET(DTAUDIO_INCLUDE_DIRECTORIES ${DELTA3D_INCLUDE_DIR}
                               ${OPENAL_INCLUDE_DIR}
                               ${ALUT_INCLUDE_DIR}
    )


# handle the QUIETLY and REQUIRED arguments and set DELTA3D_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(dtAudio DEFAULT_MSG DELTA3D_INCLUDE_DIR     DTAUDIO_LIBRARY
                                                     OPENAL_INCLUDE_DIR       OPENAL_LIBRARY
                                                     ALUT_INCLUDE_DIR         ALUT_LIBRARY
                                  )
