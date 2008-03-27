# Locate gdal
# This module defines
# DTCORE_LIBRARY
# DELTA3D_EXT_DIR
# DELTA3D_FOUND, if false, do not try to link to gdal 
# DELTA3D_INCLUDE_DIR, where to find the headers
#
# $DELTA3D_DIR is an environment variable that would
# correspond to the ./configure --prefix=$DELTA3D
#
# Created by Robert Osfield. 

FIND_PATH(DELTA3D_INCLUDE_DIR dtCore/dt.h
    ${DELTA3D_DIR}/include
    $ENV{DELTA_ROOT}/inc
    ~/Library/Frameworks
    /Library/Frameworks
    /usr/local/include
    /usr/include
    /sw/include # Fink
    /opt/local/include # DarwinPorts
    /opt/csw/include # Blastwave
    /opt/include
    [HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment;DELTA_ROOT]/inc
    /usr/freeware/include
)

FIND_PATH( DELTA3D_EXT_DIR inc/osg/Node
    ${DELTA3D_DIR}/ext
    $ENV{DELTA_ROOT}/ext
)

SET(DTUTIL_LIBRARY dtUtil)
SET(DTUTIL_LIBRARY_DEBUG dtUtild)
SET(DTCORE_LIBRARY dtCore)
SET(DTCORE_LIBRARY_DEBUG dtCored)
SET(DTABC_LIBRARY dtABC)
SET(DTABC_LIBRARY_DEBUG dtABCd)
SET(DTDAL_LIBRARY dtDAL)
SET(DTDAL_LIBRARY_DEBUG dtDALd)
SET(DTAI_LIBRARY dtAI)
SET(DTAI_LIBRARY_DEBUG dtAId)
SET(DTGAME_LIBRARY dtGame)
SET(DTGAME_LIBRARY_DEBUG dtGamed)
SET(DTAUDIO_LIBRARY dtAudio)
SET(DTAUDIO_LIBRARY_DEBUG dtAudiod)
SET(DTANIM_LIBRARY dtAnim)
SET(DTANIM_LIBRARY_DEBUG dtAnimd)

SET(DELTA3D_FOUND "NO")
IF(DTCORE_LIBRARY AND DELTA3D_INCLUDE_DIR AND DELTA3D_EXT_DIR)
    SET(DELTA3D_FOUND "YES")
ENDIF(DTCORE_LIBRARY AND DELTA3D_INCLUDE_DIR AND DELTA3D_EXT_DIR)
