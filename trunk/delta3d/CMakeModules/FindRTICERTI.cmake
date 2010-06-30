# Locate CERTI RTI
# This module defines
# RTI_LIBRARY
# RTI_FOUND, if false, do not try to link to OSG
# RTI_INCLUDE_DIR, where to find the headers

FIND_PATH(RTI_INCLUDE_DIR
   NAMES
      RTI.hh
   PATHS
      ${DELTA3D_EXT_DIR}/inc/CERTI
      /usr/local/include
      /usr/include
      "$ENV{PROGRAMFILES}/certi 3.3.2/include"
      "$ENV{PROGRAMFILES}/certi 3.3.3/include"
       $ENV{CERTI_HOME}/include
)

SET(CERTI_PATHS       
      ${DELTA3D_EXT_DIR}/lib
      /usr/local/lib 
      /usr/lib 
      "$ENV{PROGRAMFILES}/certi 3.3.2/lib"
      "$ENV{PROGRAMFILES}/certi 3.3.3/lib"
      $ENV{CERTI_HOME}/release
      ENV{CERTI_HOME}/lib)


FIND_LIBRARY(RTI_LIBRARY
   NAMES
      RTI
      RTI-NG
   PATHS
      ${CERTI_PATHS}
)

FIND_LIBRARY(RTI_RTI_LIBRARY
   NAMES
      CERTI
   PATHS
      ${CERTI_PATHS}
)

IF(UNIX)
FIND_LIBRARY(RTI_PARSER_LIBRARY
   NAMES
      xml2
   PATHS
      /usr/local/lib 
      /usr/lib 
)
ENDIF(UNIX)

SET(RTI_FOUND "NO")
IF(RTI_LIBRARY AND RTI_INCLUDE_DIR)
    SET(RTI_FOUND "YES")
    SET(RTI_LIBRARIES
           ${RTI_LIBRARY}
           ${RTI_RTI_LIBRARY}
           ${RTI_PARSER_LIBRARY}
    )
ENDIF(RTI_LIBRARY AND RTI_INCLUDE_DIR)
