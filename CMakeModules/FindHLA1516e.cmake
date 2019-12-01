# Locate PITCH RTI
# This module defines
# RTI_LIBRARY
# RTI_FOUND, if false, do not try to link to OSG
# RTI_INCLUDE_DIR, where to find the headers
# RTI_USE_HLA1516e set to make it build for 1516e

#This can be used for ANY implementation of HLA 1516e since they are binary compatible and the 
# library names are even standardized. The paths in here are set to work with pitch, but
# other rti implementations' paths can be added.

find_path(RTI1516e_INCLUDE_DIR
   NAMES
      RTI/RTI1516.h
   PATH_SUFFIXES 
      include/1516e
      include
      inc/1516e
      inc
   HINTS
      $ENV{RTI_HOME}
      ${RTIS_ROOT_DIR}
      $ENV{DELTA_ROOT}/../prti1516e
      $ENV{HLA1516_DIR}
      $ENV{PROGRAMFILES}/Pitch/prti1516e
   PATHS
       /Applications/prti1516e
)
MARK_AS_ADVANCED(RTI1516e_INCLUDE_DIR)

if (GCC_VERSION)
  IF (APPLE)
    SET(PATHLIST 
        macintel_g++-${GCC_MAJOR}.${GCC_MINOR}
        macintel_g++-unknown
        macintel64_g++-${GCC_MAJOR}.${GCC_MINOR}
        macintel64_g++-unknown
        darwin_g++-${GCC_MAJOR}.${GCC_MINOR}
        gcc${GCC_MAJOR}${GCC_MINOR}
    )
  ELSE (APPLE)
    IF (UNIX)
       SET(PATHLIST    
          linux_g++-${GCC_MAJOR}.${GCC_MINOR}
          gcc${GCC_MAJOR}${GCC_MINOR}
       )
    ENDIF (UNIX)
  ENDIF (APPLE)
endif(GCC_VERSION)


IF (MSVC)
     SET(PATHLIST 
        winnt_vc++-8.0
        winnt_vc++-7.1
        winnt_vc++-9.0
        winnt_vc++-10.0
     )
ENDIF (MSVC)

find_library(RTI1516e_LIBRARY
   NAMES
      rti1516e
   PATH_SUFFIXES
      ${PATHLIST}
   HINTS
      $ENV{RTI_HOME}/lib
      ${RTIS_ROOT_DIR}/lib
      $ENV{DELTA_ROOT}/../prti1516e/lib
      $ENV{PROGRAMFILES}/Pitch/prti1516e/lib
      $ENV{HLA1516_DIR}/lib
   PATHS
      /Applications/prti1516e
)
MARK_AS_ADVANCED(RTI1516e_LIBRARY)

find_library(RTI1516e_FEDTIME_LIBRARY
   NAMES
      fedtime1516e
   PATH_SUFFIXES
      ${PATHLIST}
   HINTS
      $ENV{RTI_HOME}/lib
      ${RTIS_ROOT_DIR}/lib
      $ENV{DELTA_ROOT}/../prti1516e/lib
      $ENV{PROGRAMFILES}/Pitch/prti1516e/lib
      $ENV{HLA1516_DIR}/lib
   PATHS
      /Applications/prti1516e
)
MARK_AS_ADVANCED(RTI1516e_FEDTIME_LIBRARY)

set(RTI_FOUND "NO")
if(RTI1516e_LIBRARY AND RTI1516e_INCLUDE_DIR)
    set(RTI_FOUND "YES")
    set(RTI_1516E "ON")
    SET(RTI1516e_LIBRARIES
           ${RTI1516e_LIBRARY}
           ${RTI1516e_FEDTIME_LIBRARY}
    )
endif(RTI1516e_LIBRARY AND RTI1516e_INCLUDE_DIR)
