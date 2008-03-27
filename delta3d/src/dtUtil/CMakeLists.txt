
# FIXME: For OS X, need flag for Framework or dylib

SET(LIB_NAME     dtUtil)


IF (WIN32)
ELSE (WIN32) 
ENDIF (WIN32)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/inc/dtUtil)
SET(LIB_PUBLIC_HEADERS
   ${HEADER_PATH}/barycentric.h
   ${HEADER_PATH}/bits.h
   ${HEADER_PATH}/breakoverride.h
   ${HEADER_PATH}/celestialmath.h
   ${HEADER_PATH}/celestialmesh.h
   ${HEADER_PATH}/command.h
   ${HEADER_PATH}/coordinates.h
   ${HEADER_PATH}/datastream.h
   ${HEADER_PATH}/deprecationmgr.h
   ${HEADER_PATH}/dtutil.h
   ${HEADER_PATH}/enumeration.h
   ${HEADER_PATH}/exception.h
   ${HEADER_PATH}/export.h
   ${HEADER_PATH}/fileutils.h
   ${HEADER_PATH}/fractal.h
   ${HEADER_PATH}/funbind.h
   ${HEADER_PATH}/funcall.h
   ${HEADER_PATH}/functor.h
   ${HEADER_PATH}/funtraits.h
   ${HEADER_PATH}/generic.h
   ${HEADER_PATH}/hotspotdefinition.h
   ${HEADER_PATH}/hotspotxml.h
   ${HEADER_PATH}/keyframedecoder.h
   ${HEADER_PATH}/librarysharingmanager.h
   ${HEADER_PATH}/log.h
   ${HEADER_PATH}/macros.h
   ${HEADER_PATH}/mathdefines.h
   ${HEADER_PATH}/matrixutil.h
   ${HEADER_PATH}/nodeprintout.h
   ${HEADER_PATH}/noise1.h
   ${HEADER_PATH}/noise2.h
   ${HEADER_PATH}/noise3.h
   ${HEADER_PATH}/noisetexture.h
   ${HEADER_PATH}/noiseutility.h
   ${HEADER_PATH}/objectfactory.h
   ${HEADER_PATH}/polardecomp.h
   ${HEADER_PATH}/resourceloader.h
   ${HEADER_PATH}/resourcemanager.h
   ${HEADER_PATH}/seamlessnoise.h
   ${HEADER_PATH}/serializer.h
   ${HEADER_PATH}/stringutils.h
   ${HEADER_PATH}/templateutility.h
   ${HEADER_PATH}/tree.h
   ${HEADER_PATH}/typetraits.h
   ${HEADER_PATH}/version.h
   ${HEADER_PATH}/xerceserrorhandler.h
   ${HEADER_PATH}/xercesparser.h
   ${HEADER_PATH}/xercesutils.h
   ${HEADER_PATH}/xerceswriter.h
)

ADD_LIBRARY(${LIB_NAME} SHARED
    ${LIB_PUBLIC_HEADERS} 
celestialmath.cpp
celestialmesh.cpp
coordinates.cpp
datastream.cpp
exception.cpp
fileutils.cpp
hotspotxml.cpp
librarysharingmanager.cpp
log.cpp
matrixutil.cpp
nodeprintout.cpp
noisetexture.cpp
polardecomp.cpp
seamlessnoise.cpp
serializer.cpp
stringutils.cpp
version.cpp
xerceserrorhandler.cpp
xercesparser.cpp
xercesutils.cpp
xerceswriter.cpp
)

LINK_WITH_VARIABLES(${LIB_NAME}
											  OSG_LIBRARY 
											  OSGDB_LIBRARY 
											  XERCES_LIBRARY
											  OPENTHREADS_LIBRARY)

SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL DT_UTIL_LIBRARY)
					 
INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)


