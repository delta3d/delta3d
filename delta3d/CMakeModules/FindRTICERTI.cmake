# Locate CERTI RTI
# This module defines
# RTIS_LIBRARY
# RTIS_FOUND, if false, do not try to link to OSG
# RTIS_INCLUDE_DIR, where to find the headers

find_path(RTIS_INCLUDE_DIR
   NAMES
      RTI.hh
   PATHS
      /usr/local/include
      /usr/include
	$ENV{CERTI_HOME}/include
)

find_library(RTIS_LIBRARY
   NAMES
      RTI
   PATHS
      /usr/local/lib 
      /usr/lib 
	$ENV{CERTI_HOME}/release
)

find_library(RTIS_RTI_LIBRARY
   NAMES
      CERTI
   PATHS
      /usr/local/lib 
      /usr/lib 
	$ENV{CERTI_HOME}/release
)

if(UNIX)
find_library(RTIS_PARSER_LIBRARY
   NAMES
      xml2
   PATHS
      /usr/local/lib 
      /usr/lib 
)
endif(UNIX)

set(RTIS_FOUND "NO")
if(RTIS_LIBRARY AND RTIS_INCLUDE_DIR)
    set(RTIS_FOUND "YES")
    SET(RTIS_LIBRARIES
           ${RTIS_LIBRARY}
           ${RTIS_RTI_LIBRARY}
           ${RTIS_PARSER_LIBRARY}
    )
endif(RTIS_LIBRARY AND RTIS_INCLUDE_DIR)
