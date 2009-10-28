# Locate Portico RTI
# This module defines
# RTI_LIBRARY
# RTI_FOUND, if false, do not try to link to OSG
# RTI_INCLUDE_DIR, where to find the headers

find_path(RTI_INCLUDE_DIR
   NAMES
      RTI.hh
   PATHS
      $ENV{DELTA_ROOT}/../portico-1.0rc1/include/ng6
      $ENV{DELTA_ROOT}/../portico/include/ng6
)

find_library(RTI_LIBRARY
   NAMES
      RTI-NG
   PATHS
      $ENV{DELTA_ROOT}/../portico-1.0rc1/lib
      $ENV{DELTA_ROOT}/../portico/lib
)

find_library(RTI_FEDTIME_LIBRARY
   NAMES
      FedTime
   PATHS
      $ENV{DELTA_ROOT}/../portico-1.0rc1/lib
      $ENV{DELTA_ROOT}/../portico/lib
)

set(RTI_FOUND "NO")
if(RTI_LIBRARY AND RTI_INCLUDE_DIR)
    set(RTI_FOUND "YES")
    SET(RTI_LIBRARIES
           ${RTI_LIBRARY}
           ${RTI_FEDTIME_LIBRARY}
    )
endif(RTI_LIBRARY AND RTI_INCLUDE_DIR)
