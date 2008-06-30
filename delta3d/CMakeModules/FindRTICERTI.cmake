# Locate CERTI RTI
# This module defines
# RTI_LIBRARY
# RTI_FOUND, if false, do not try to link to OSG
# RTI_INCLUDE_DIR, where to find the headers

find_path(RTI_INCLUDE_DIR
   NAMES
      RTI.hh
   PATHS
      ${DELTA3D_EXT_DIR}/inc/CERTI
      /usr/local/include
      /usr/include
   	$ENV{CERTI_HOME}/include
)

SET(CERTI_PATHS       
      ${DELTA3D_EXT_DIR}/lib
      /usr/local/lib 
      /usr/lib 
  	   $ENV{CERTI_HOME}/release)


find_library(RTI_LIBRARY
   NAMES
      RTI
   PATHS
      ${CERTI_PATHS}
)

find_library(RTI_RTI_LIBRARY
   NAMES
      CERTI
   PATHS
      ${CERTI_PATHS}
)

if(UNIX)
find_library(RTI_PARSER_LIBRARY
   NAMES
      xml2
   PATHS
      /usr/local/lib 
      /usr/lib 
)
endif(UNIX)

set(RTI_FOUND "NO")
if(RTI_LIBRARY AND RTI_INCLUDE_DIR)
    set(RTI_FOUND "YES")
    SET(RTI_LIBRARIES
           ${RTI_LIBRARY}
           ${RTI_RTI_LIBRARY}
           ${RTI_PARSER_LIBRARY}
    )
endif(RTI_LIBRARY AND RTI_INCLUDE_DIR)
