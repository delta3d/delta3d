# Locate dtABC
#
# Defines:
# DTABC_LIBRARY, DTABC_DEBUG_LIBRARY : Path to the library files
# DTABC_INCLUDE_DIRECTORIES : All required include directories for this library
# DTABC_LIBRARIES : All required library files

INCLUDE(delta3d_common)
  
#variable names of the individual Delta3D libraries.  Can be used in application cmakelist.txt files.
FIND_DELTA3D_LIBRARY(DTABC_LIBRARY         dtABC)
FIND_DELTA3D_LIBRARY(DTABC_DEBUG_LIBRARY   dtABCD)


IF (NOT DTABC_DEBUG_LIBRARY)
  SET(DTABC_DEBUG_LIBRARY DTABC_LIBRARY)
  MESSAGE(STATUS "No debug library was found for DTABC_DEBUG_LIBRARY")
ENDIF()

#convienent list of libraries to link with when using dtABC
SET(DTABC_LIBRARIES
    optimized ${DTABC_LIBRARY}      debug ${DTABC_DEBUG_LIBRARY}
    )
    
SET(DTABC_INCLUDE_DIRECTORIES ${DELTA3D_INCLUDE_DIR}
    )


# handle the QUIETLY and REQUIRED arguments and set DELTA3D_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(dtABC DEFAULT_MSG DELTA3D_INCLUDE_DIR     DTABC_LIBRARY
                                  )
