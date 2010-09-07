# - Locate OpenAL
# This module defines
#  OPENAL_LIBRARY
#  OPENAL_FOUND, if false, do not try to link to OpenAL 
#  OPENAL_INCLUDE_DIR, where to find the headers
#   
# $OPENALDIR is an environment variable that would
# correspond to the ./configure --prefix=$OPENALDIR
# used in building OpenAL.
#   
# Created by Eric Wing. This was influenced by the FindSDL.cmake module.
# On OSX, this will prefer the Framework version (if found) over others.
# People will have to manually change the cache values of 
# OPENAL_LIBRARY to override this selection.
# Tiger will include OpenAL as part of the System.
# But for now, we have to look around.
# Other (Unix) systems should be able to utilize the non-framework paths.
FIND_PATH(OPENAL_INCLUDE_DIR al.h
  HINTS
  $ENV{OPENALDIR}/include/AL
  $ENV{OPENALDIR}/include/OpenAL
  ${DELTA3D_EXT_DIR}/inc
  ${DELTA3D_EXT_DIR}/inc/AL
  ${DELTA3D_EXT_DIR}/inc/OpenAL
  $ENV{DELTA_ROOT}/ext/inc
  $ENV{DELTA_ROOT}/ext/inc/AL
  $ENV{DELTA_ROOT}/ext/inc/OpenAL
  PATHS
  ~/Library/Frameworks/OpenAL.framework/Headers
  /Library/Frameworks/OpenAL.framework/Headers
  /System/Library/Frameworks/OpenAL.framework/Headers # Tiger
  /usr/local/include/AL
  /usr/local/include/OpenAL
  /usr/local/include
  /usr/include/AL
  /usr/include/OpenAL
  /usr/include
  /sw/include/AL # Fink
  /sw/include/OpenAL 
  /sw/include
  /opt/local/include/AL # DarwinPorts
  /opt/local/include/OpenAL
  /opt/local/include
  /opt/csw/include/AL # Blastwave
  /opt/csw/include/OpenAL
  /opt/csw/include
  /opt/include/AL
  /opt/include/OpenAL
  /opt/include
  NO_DEFAULT_PATH #Added so it won't find the system one before the delta3d/ext one.
  )

FIND_PATH(ALUT_INCLUDE_DIR alut.h
  $ENV{OPENALDIR}/include/AL
  $ENV{OPENALDIR}/include/OpenAL
  ${DELTA3D_EXT_DIR}/inc/AL
  ${DELTA3D_EXT_DIR}/inc/OpenAL
  $ENV{DELTA_ROOT}/ext/inc
  $ENV{DELTA_ROOT}/ext/inc/AL
  $ENV{DELTA_ROOT}/ext/inc/OpenAL
  ~/Library/Frameworks/OpenAL.framework/Headers
  /Library/Frameworks/OpenAL.framework/Headers
  /System/Library/Frameworks/OpenAL.framework/Headers # Tiger
  ~/Library/Frameworks/ALUT.framework/Headers
  /Library/Frameworks/ALUT.framework/Headers
  /usr/local/include/AL
  /usr/local/include/OpenAL
  /usr/local/include
  /usr/include/AL
  /usr/include/OpenAL
  /usr/include
  /sw/include/AL # Fink
  /sw/include/OpenAL 
  /sw/include
  /opt/local/include/AL # DarwinPorts
  /opt/local/include/OpenAL
  /opt/local/include
  /opt/csw/include/AL # Blastwave
  /opt/csw/include/OpenAL
  /opt/csw/include
  /opt/include/AL
  /opt/include/OpenAL
  /opt/include
  NO_DEFAULT_PATH
  )
  
# I'm not sure if I should do a special casing for Apple. It is 
# unlikely that other Unix systems will find the framework path.
# But if they do ([Next|Open|GNU]Step?), 
# do they want the -framework option also?
IF(${OPENAL_INCLUDE_DIR} MATCHES ".framework")
  STRING(REGEX REPLACE "(.*)/.*\\.framework/.*" "\\1" OPENAL_FRAMEWORK_PATH_TMP ${OPENAL_INCLUDE_DIR})
  IF("${OPENAL_FRAMEWORK_PATH_TMP}" STREQUAL "/Library/Frameworks"
      OR "${OPENAL_FRAMEWORK_PATH_TMP}" STREQUAL "/System/Library/Frameworks"
      )
    # String is in default search path, don't need to use -F
    SET (OPENAL_LIBRARY "-framework OpenAL" CACHE STRING "OpenAL framework for OSX")
  ELSE("${OPENAL_FRAMEWORK_PATH_TMP}" STREQUAL "/Library/Frameworks"
      OR "${OPENAL_FRAMEWORK_PATH_TMP}" STREQUAL "/System/Library/Frameworks"
      )
    # String is not /Library/Frameworks, need to use -F
    SET(OPENAL_LIBRARY "-F${OPENAL_FRAMEWORK_PATH_TMP} -framework OpenAL" CACHE STRING "OpenAL framework for OSX")
  ENDIF("${OPENAL_FRAMEWORK_PATH_TMP}" STREQUAL "/Library/Frameworks"
    OR "${OPENAL_FRAMEWORK_PATH_TMP}" STREQUAL "/System/Library/Frameworks"
    )
  # Clear the temp variable so nobody can see it
  SET(OPENAL_FRAMEWORK_PATH_TMP "" CACHE INTERNAL "")

  # if we don't have the integrated alut.h in the framework, we need a library
  IF (NOT ${ALUT_INCLUDE_DIR} MATCHES "OpenAL.framework")
    FIND_LIBRARY(ALUT_LIBRARY 
    NAMES alut
    PATHS
    ~/Library/Frameworks/ALUT.framework/Headers
    /Library/Frameworks/ALUT.framework/Headers
	 ${DELTA3D_EXT_DIR}/Frameworks
	 ${DELTA3D_EXT_DIR}/lib
	$ENV{DELTA_ROOT}/ext/lib
    $ENV{OPENALDIR}/lib
    $ENV{OPENALDIR}/libs
    /usr/local/lib
    /usr/lib
    /sw/lib
    /opt/local/lib
    /opt/csw/lib
    /opt/lib
    )
  ENDIF (NOT ${ALUT_INCLUDE_DIR} MATCHES "OpenAL.framework")

   
ELSE(${OPENAL_INCLUDE_DIR} MATCHES ".framework")
  FIND_LIBRARY(OPENAL_LIBRARY 
    NAMES openal al OpenAL32
    PATHS
    ${DELTA3D_EXT_DIR}/lib
	$ENV{DELTA_ROOT}/ext/lib
    $ENV{OPENALDIR}/lib
    $ENV{OPENALDIR}/libs
    /usr/local/lib
    /usr/lib
    /sw/lib
    /opt/local/lib
    /opt/csw/lib
    /opt/lib
    )
	
   FIND_LIBRARY(ALUT_LIBRARY 
    NAMES alut
       HINTS
          ${DELTA3D_EXT_DIR}/lib
          $ENV{OPENALDIR}/lib
          $ENV{OPENALDIR}/libs
       PATHS
          /usr/local/lib
          /usr/lib
          /sw/lib
          /opt/local/lib
          /opt/csw/lib
          /opt/lib
    )
    
    IF (MSVC)
       FIND_LIBRARY(ALUT_DEBUG_LIBRARY 
       NAMES alutd
       HINTS
          ${DELTA3D_EXT_DIR}/lib
          $ENV{OPENALDIR}/lib
          $ENV{OPENALDIR}/libs
       PATHS
          /usr/local/lib
          /usr/lib
          /sw/lib
          /opt/local/lib
          /opt/csw/lib
          /opt/lib
       )
   ENDIF(MSVC)
ENDIF(${OPENAL_INCLUDE_DIR} MATCHES ".framework")

SET(OPENAL_FOUND "NO")
IF(OPENAL_LIBRARY)
  SET(OPENAL_FOUND "YES")
ENDIF(OPENAL_LIBRARY)


