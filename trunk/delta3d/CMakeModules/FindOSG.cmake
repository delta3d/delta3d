# Locate OSG
# This module defines
# OSG_LIBRARY
# OSG_FOUND, if false, do not try to link to OSG
# OSG_INCLUDE_DIR, where to find the headers
#
# $OSG_DIR is an environment variable that would
# correspond to the ./configure --prefix=$OSG_DIR
#
# Created by Robert Osfield. 

FIND_PATH(OSG_INCLUDE_DIR osg/Node
    $ENV{OSG_DIR}/include
    $ENV{OSG_DIR}
    $ENV{OSGDIR}/include
    $ENV{OSGDIR}
    $ENV{OSG_ROOT}/include
    ${DELTA_DIR}/ext/inc
    $ENV{DELTA_ROOT}/ext/inc
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

MACRO(FIND_OSG_LIBRARY MYLIBRARY MYLIBRARYNAME)

    FIND_LIBRARY(${MYLIBRARY}
        NAMES ${MYLIBRARYNAME}
        PATHS
        $ENV{OSG_DIR}/lib
        $ENV{OSG_DIR}
        $ENV{OSGDIR}/lib
        $ENV{OSGDIR}
        $ENV{OSG_ROOT}/lib
        ${DELTA_DIR}/ext/lib
        ${DELTA_DIR}/ext/lib64
        $ENV{DELTA_ROOT}/ext/lib
        $ENV{DELTA_ROOT}/ext/lib64
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

ENDMACRO(FIND_OSG_LIBRARY LIBRARY LIBRARYNAME)

# Find release (optimized) libs
FIND_OSG_LIBRARY(OSG_LIBRARY osg)
FIND_OSG_LIBRARY(OSGUTIL_LIBRARY osgUtil)
FIND_OSG_LIBRARY(OSGDB_LIBRARY osgDB)
FIND_OSG_LIBRARY(OSGTEXT_LIBRARY osgText)
FIND_OSG_LIBRARY(OSGTERRAIN_LIBRARY osgTerrain)
FIND_OSG_LIBRARY(OSGFX_LIBRARY osgFX)
FIND_OSG_LIBRARY(OSGVIEWER_LIBRARY osgViewer)
FIND_OSG_LIBRARY(OSGGA_LIBRARY osgGA)
FIND_OSG_LIBRARY(OSGPARTICLE_LIBRARY osgParticle)
FIND_OSG_LIBRARY(OSGSIM_LIBRARY osgSim)

# Find debug libs
FIND_OSG_LIBRARY(OSG_LIBRARY_DEBUG osgd)
FIND_OSG_LIBRARY(OSGUTIL_LIBRARY_DEBUG osgUtild)
FIND_OSG_LIBRARY(OSGDB_LIBRARY_DEBUG osgDBd)
FIND_OSG_LIBRARY(OSGTEXT_LIBRARY_DEBUG osgTextd)
FIND_OSG_LIBRARY(OSGTERRAIN_LIBRARY_DEBUG osgTerraind)
FIND_OSG_LIBRARY(OSGFX_LIBRARY_DEBUG osgFXd)
FIND_OSG_LIBRARY(OSGVIEWER_LIBRARY_DEBUG osgViewerd)
FIND_OSG_LIBRARY(OSGGA_LIBRARY_DEBUG osgGAd)
FIND_OSG_LIBRARY(OSGPARTICLE_LIBRARY_DEBUG osgParticled)
FIND_OSG_LIBRARY(OSGSIM_LIBRARY_DEBUG osgSimd)

SET(OSG_FOUND "NO")
IF(OSG_LIBRARY AND OSG_INCLUDE_DIR)
    SET(OSG_FOUND "YES")
ENDIF(OSG_LIBRARY AND OSG_INCLUDE_DIR)
