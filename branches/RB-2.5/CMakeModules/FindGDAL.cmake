# Locate gdal
# This module defines
# GDAL_LIBRARY
# GDAL_FOUND, if false, do not try to link to gdal 
# GDAL_INCLUDE_DIR, where to find the headers
#
# $GDAL_DIR is an environment variable that would
# correspond to the ./configure --prefix=$GDAL_DIR
#
# Created by Robert Osfield. 

FIND_PATH(GDAL_INCLUDE_DIR gdal.h
    ${GDAL_DIR}/include
    $ENV{GDAL_DIR}/include
    $ENV{GDAL_DIR}
    ${DELTA3D_EXT_DIR}/inc
    $ENV{DELTA_ROOT}/ext/inc
    $ENV{DELTA_ROOT}
    $ENV{OSG_ROOT}/include
    ~/Library/Frameworks
    /Library/Frameworks
    /usr/local/include
    /usr/include
    /usr/include/gdal
    /sw/include # Fink
    /opt/local/include # DarwinPorts
    /opt/csw/include # Blastwave
    /opt/include
    [HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment;OSG_ROOT]/include
    /usr/freeware/include
)

MACRO(FIND_GDAL_LIBRARY MYLIBRARY MYLIBRARYNAME)

FIND_LIBRARY(${MYLIBRARY} 
    NAMES ${MYLIBRARYNAME}
    PATHS
    ${GDAL_DIR}/lib
    $ENV{GDAL_DIR}/lib
    $ENV{GDAL_DIR}
    ${DELTA3D_EXT_DIR}/lib
    $ENV{DELTA_ROOT}/ext/lib
    $ENV{DELTA_ROOT}
    $ENV{OSG_ROOT}/lib
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

ENDMACRO(FIND_GDAL_LIBRARY MYLIBRARY MYLIBRARYNAME)

SET(GDAL_LIST gdal gdal_i gdal1.4.0 gdal1.3.2 gdal1.5.0 gdal1.6.0 GDAL)
FIND_GDAL_LIBRARY(GDAL_LIBRARY "${GDAL_LIST}")

SET(GDAL_LIBRARY_DEBUG ${GDAL_LIBRARY})

# Debug not working?
#FIND_GDAL_LIBRARY(GDAL_LIBRARY_DEBUG gdal_id)

SET(GDAL_FOUND "NO")
IF(GDAL_LIBRARY AND GDAL_INCLUDE_DIR)
    SET(GDAL_FOUND "YES")
ENDIF(GDAL_LIBRARY AND GDAL_INCLUDE_DIR)


