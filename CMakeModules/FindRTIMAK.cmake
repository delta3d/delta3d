# Locate Mak RTI
# This module defines
# RTI_LIBRARY
# RTI_FOUND, if false, do not try to link to OSG
# RTI_INCLUDE_DIR, where to find the headers

find_path(RTI_INCLUDE_DIR
   NAMES
      RTI.hh
   PATHS
      $ENV{DELTA_ROOT}/../makRti3.3.1/include
)

find_library(RTI_LIBRARY
   NAMES
      RTI-NG
   PATHS
      $ENV{DELTA_ROOT}/../makRti3.3.1/lib
)

set(RTI_FOUND "NO")
if(RTI_LIBRARY AND RTI_INCLUDE_DIR)
    set(RTI_FOUND "YES")
    SET(RTI_LIBRARIES
           ${RTI_LIBRARY}
    )
endif(RTI_LIBRARY AND RTI_INCLUDE_DIR)
