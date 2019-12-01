# Locate Portico RTI
# This module defines
# RTI_LIBRARY
# RTI_FOUND, if false, do not try to link to OSG
# RTI_INCLUDE_DIR, where to find the headers

find_path(RTI_INCLUDE_DIR
   NAMES
      RTI.hh
   PATH_SUFFIXES 
      /include/ng6
   HINTS
      $ENV{DELTA_ROOT}/../portico-1.0rc1
      $ENV{DELTA_ROOT}/../portico/
      $ENV{PORTICO_DIR}
   PATHS
       $ENV{PROGRAMFILES}/Portico/portico-1.0.2
)
MARK_AS_ADVANCED(RTI_INCLUDE_DIR)

find_library(RTI_LIBRARY
   NAMES
      libRTI-NG RTI-NG
   PATH_SUFFIXES
      lib
   HINTS
      $ENV{DELTA_ROOT}/../portico-1.0rc1
      $ENV{DELTA_ROOT}/../portico
      $ENV{PORTICO_DIR}
   PATHS
      $ENV{PROGRAMFILES}/Portico/portico-1.0.2
)
MARK_AS_ADVANCED(RTI_LIBRARY)

find_library(RTI_FEDTIME_LIBRARY
   NAMES
      libFedTime FedTime
   PATH_SUFFIXES
      lib      
   HINTS
      $ENV{DELTA_ROOT}/../portico-1.0rc1
      $ENV{DELTA_ROOT}/../portico
      $ENV{PORTICO_DIR}
   PATHS
      $ENV{PROGRAMFILES}/Portico/portico-1.0.2      
)
MARK_AS_ADVANCED(RTI_FEDTIME_LIBRARY)

set(RTI_FOUND "NO")
if(RTI_LIBRARY AND RTI_INCLUDE_DIR)
    set(RTI_FOUND "YES")
    SET(RTI_LIBRARIES
           ${RTI_LIBRARY}
           ${RTI_FEDTIME_LIBRARY}
    )
endif(RTI_LIBRARY AND RTI_INCLUDE_DIR)
