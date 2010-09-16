# Locate OpenThreads
# This module defines
# OPENTHREADS_LIBRARY
# OPENTHREADS_FOUND, if false, do not try to link to OpenThreads 
# OPENTHREADS_INCLUDE_DIR, where to find the headers
#
# $OPENTHREADS_DIR is an environment variable that would
# correspond to the ./configure --prefix=$OPENTHREADS_DIR
#
# Created by Robert Osfield
#   with revisions by the Delta3D team.

FIND_PATH(OPENTHREADS_INCLUDE_DIR OpenThreads/Thread
    ${OPENTHREADS_DIR}/include
    $ENV{OPENTHREADS_DIR}/include
    $ENV{OPENTHREADS_DIR}
    ${DELTA3D_EXT_DIR}/inc
    $ENV{DELTA_ROOT}/ext/inc
    $ENV{DELTA_ROOT}
    $ENV{OSG_DIR}/include
    $ENV{OSG_ROOT}/include
    ~/Library/Frameworks
    /Library/Frameworks
    /usr/local/include
    /usr/include
    /sw/include # Fink
    /opt/local/include # DarwinPorts
    /opt/csw/include # Blastwave
    /opt/include
    [HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment;OSG_ROOT]/include
    /usr/freeware/include
)

 MACRO(FIND_OPENTHREADS_LIBRARY MYLIBRARY MYLIBRARYNAME)
 
FIND_LIBRARY(${MYLIBRARY} 
    NAMES ${MYLIBRARYNAME}
    PATHS
    ${OPENTHREADS_DIR}/lib
    $ENV{OPENTHREADS_DIR}/lib
    $ENV{OPENTHREADS_DIR}/lib64
    $ENV{OPENTHREADS_DIR}
    ${DELTA3D_EXT_DIR}/lib
    ${DELTA3D_EXT_DIR}/lib64
    $ENV{DELTA_ROOT}/ext/lib
    $ENV{DELTA_ROOT}/ext/lib64
    $ENV{DELTA_ROOT}
    $ENV{OSG_DIR}/lib
    $ENV{OSG_DIR}/build/lib
    $ENV{OSG_ROOT}/lib
    $ENV{OSG_ROOT}/build/lib
    ~/Library/Frameworks
    /Library/Frameworks
    /usr/local/lib
    /usr/lib
    /sw/lib
    /opt/local/lib
    /opt/csw/lib
    /opt/lib
    [HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment;OSG_ROOT]/lib
    /usr/freeware/lib64
)

 ENDMACRO(FIND_OPENTHREADS_LIBRARY MYLIBRARY MYLIBRARYNAME)
 
 FIND_OPENTHREADS_LIBRARY(OPENTHREADS_LIBRARY OpenThreads OpenThreadsWin32)
 FIND_OPENTHREADS_LIBRARY(OPENTHREADS_LIBRARY_DEBUG OpenThreadsd)

SET(OPENTHREADS_FOUND "NO")
IF(OPENTHREADS_LIBRARY AND OPENTHREADS_INCLUDE_DIR)
    SET(OPENTHREADS_FOUND "YES")
ENDIF(OPENTHREADS_LIBRARY AND OPENTHREADS_INCLUDE_DIR)


