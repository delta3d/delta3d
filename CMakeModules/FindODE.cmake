# Locate ode
# This module defines
# ODE_LIBRARY
# ODE_FOUND, if false, do not try to link to ode 
# ODE_INCLUDE_DIR, where to find the headers
#
# $ODE_DIR is an environment variable that would
# correspond to the ./configure --prefix=$ODE_DIR
#
# Created by David Guthrie.  Based on code by Robert Osfield 

FIND_PATH(ODE_INCLUDE_DIR ode/ode.h
    HINTS
    ${ODE_DIR}/include
    $ENV{ODE_DIR}/include
    $ENV{ODE_DIR}
    ${DELTA3D_EXT_DIR}/inc
    $ENV{DELTA_ROOT}/ext/inc
)
MARK_AS_ADVANCED(ODE_INCLUDE_DIR)

MACRO(FIND_ODE_LIBRARY MYLIBRARY MYLIBRARYNAME)

FIND_LIBRARY(${MYLIBRARY}
    NAMES ${MYLIBRARYNAME}
    HINTS
    ${ODE_DIR}/lib
    $ENV{ODE_DIR}/lib
    $ENV{ODE_DIR}
    ${DELTA3D_EXT_DIR}/lib
    $ENV{DELTA_ROOT}/ext/lib
)
MARK_AS_ADVANCED(${MYLIBRARY})
ADD_DEFINITIONS(-DdSINGLE)

ENDMACRO(FIND_ODE_LIBRARY MYLIBRARY MYLIBRARYNAME)

SET(RELEASE_LIST ode ode_single)
FIND_ODE_LIBRARY(ODE_LIBRARY "${RELEASE_LIST}")

SET(DEBUG_LIST oded ode_singled)
FIND_ODE_LIBRARY(ODE_LIBRARY_DEBUG "${DEBUG_LIST}")

SET(ODE_FOUND "NO")
IF(ODE_LIBRARY AND ODE_INCLUDE_DIR)
    SET(ODE_FOUND "YES")
ENDIF(ODE_LIBRARY AND ODE_INCLUDE_DIR)


