# Locate CERTI RTI
# This module defines
# RTI_LIBRARY
# RTI_FOUND, if false, do not try to link to OSG
# RTI_INCLUDE_DIR, where to find the headers

FIND_PATH(RTI_INCLUDE_DIR
   NAMES
      RTI.hh
   HINTS
      ${DELTA3D_EXT_DIR}/inc/CERTI
      /usr/local/include
      /usr/include
      "$ENV{PROGRAMFILES}/certi 3.3.2/include"
      "$ENV{PROGRAMFILES}/certi 3.3.3/include"
      "$ENV{PROGRAMFILES}/certi 3.4.0/include"
       $ENV{CERTI_HOME}/include
)
MARK_AS_ADVANCED(RTI_INCLUDE_DIR)

SET(CERTI_PATHS       
      ${DELTA3D_EXT_DIR}/lib
      /usr/local/lib 
      /usr/lib 
      "$ENV{PROGRAMFILES}/certi 3.3.2/lib"
      "$ENV{PROGRAMFILES}/certi 3.3.3/lib"
      "$ENV{PROGRAMFILES}/certi 3.4.0/lib"
      $ENV{CERTI_HOME}/release
      $ENV{CERTI_HOME}/lib)


FIND_LIBRARY(RTI_LIBRARY
   NAMES
      RTI
      RTI-NG
   PATHS
      ${CERTI_PATHS}
)
MARK_AS_ADVANCED(RTI_LIBRARY)

FIND_LIBRARY(RTI_RTI_LIBRARY
   NAMES
      CERTI
   PATHS
      ${CERTI_PATHS}
)
MARK_AS_ADVANCED(RTI_RTI_LIBRARY)

IF(UNIX)
FIND_LIBRARY(RTI_PARSER_LIBRARY
   NAMES
      xml2
   PATHS
      /usr/local/lib 
      /usr/lib 
)
MARK_AS_ADVANCED(RTI_PARSER_LIBRARY)
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

# handle the QUIETLY and REQUIRED arguments and set DELTA3D_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(RTICERTI DEFAULT_MSG RTI_INCLUDE_DIR RTI_LIBRARY)
