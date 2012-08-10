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
      include
      inc
   HINTS
      $ENV{DELTA_ROOT}/../prti1516e
      $ENV{HLA1516_DIR}
      $ENV{PROGRAMFILES}/Pitch/prti1516e
   PATHS
       /Applications/prti1516e
)

SET(LIBSUBDIR )

if (MSVC)
else () 
    if (GCC_VERSION)
      set(LIBSUBDIR gcc${GCC_MAJOR}${GCC_MINOR})
    endif()
endif()


message( ${LIBSUBDIR})

find_library(RTI1516e_LIBRARY
   NAMES
      rti1516e
   PATH_SUFFIXES
      lib
      lib/${LIBSUBDIR}
   HINTS
      $ENV{DELTA_ROOT}/../prti1516e
      $ENV{PROGRAMFILES}/Pitch/prti1516e
      $ENV{HLA1516_DIR}
   PATHS
      /Applications/prti1516e
)

find_library(RTI1516e_FEDTIME_LIBRARY
   NAMES
      fedtime1516e
   PATH_SUFFIXES
      lib
      lib/${LIBSUBDIR}
   HINTS
      $ENV{DELTA_ROOT}/../prti1516e
      $ENV{PROGRAMFILES}/Pitch/prti1516e
      $ENV{HLA1516_DIR}
   PATHS
      /Applications/prti1516e
)

set(RTI_FOUND "NO")
if(RTI1516e_LIBRARY AND RTI1516e_INCLUDE_DIR)
    set(RTI_FOUND "YES")
    set(RTI_1516E "ON")
    SET(RTI1516e_LIBRARIES
           ${RTI1516e_LIBRARY}
           ${RTI1516e_FEDTIME_LIBRARY}
    )
endif(RTI1516e_LIBRARY AND RTI1516e_INCLUDE_DIR)
