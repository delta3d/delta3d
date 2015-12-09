SET(LIB_NAME     dtUtil)


SET(HEADER_PATH ${CMAKE_SOURCE_DIR}/inc/${LIB_NAME})
SET(SOURCE_PATH ${CMAKE_SOURCE_DIR}/src/${LIB_NAME})

file(GLOB LIB_PUBLIC_HEADERS
   "${HEADER_PATH}/*.h"
   )

SET( LIB_SOURCES 
    ${SOURCE_PATH}/configproperties.cpp
    ${SOURCE_PATH}/coordinates.cpp
    ${SOURCE_PATH}/cullmask.cpp
    ${SOURCE_PATH}/datapathutils.cpp
    ${SOURCE_PATH}/datastream.cpp
    ${SOURCE_PATH}/datetime.cpp
    ${SOURCE_PATH}/deprecationmgr.cpp
    ${SOURCE_PATH}/enumeration.cpp
    ${SOURCE_PATH}/exception.cpp
    ${SOURCE_PATH}/fileutils.cpp
    ${SOURCE_PATH}/hotspotxml.cpp
    ${SOURCE_PATH}/librarysharingmanager.cpp
    ${SOURCE_PATH}/log.cpp
    ${SOURCE_PATH}/logobserverconsole.cpp
    ${SOURCE_PATH}/logobserverfile.cpp
    ${SOURCE_PATH}/matrixutil.cpp
    ${SOURCE_PATH}/nodecollector.cpp
    ${SOURCE_PATH}/nodemask.cpp
    ${SOURCE_PATH}/nodeprintout.cpp
    ${SOURCE_PATH}/nodetypes.cpp
    ${SOURCE_PATH}/noisetexture.cpp
    ${SOURCE_PATH}/polardecomp.cpp
#    ${SOURCE_PATH}/precomp.cpp
    ${SOURCE_PATH}/readnodethreadpooltask.cpp
    ${SOURCE_PATH}/refstring.cpp
    ${SOURCE_PATH}/seamlessnoise.cpp
    ${SOURCE_PATH}/serializer.cpp
    ${SOURCE_PATH}/stateattributecollector.cpp
    ${SOURCE_PATH}/stringutils.cpp
    ${SOURCE_PATH}/tangentspacevisitor.cpp
    ${SOURCE_PATH}/threadpool.cpp
    ${SOURCE_PATH}/version.cpp
    ${SOURCE_PATH}/xercesbininputstreamistream.cpp
    ${SOURCE_PATH}/xerceserrorhandler.cpp
    ${SOURCE_PATH}/xercesparser.cpp
    ${SOURCE_PATH}/xercesutils.cpp
    ${SOURCE_PATH}/xerceswriter.cpp
   )
   
SET(LIB_EXTERNAL_DEPS
   OSG_LIBRARY 
   OSGDB_LIBRARY
   OSGUTIL_LIBRARY
   OSGSIM_LIBRARY
   XERCES_LIBRARY
   OPENTHREADS_LIBRARY
   COREFOUNDATION_LIBRARY
   COCOA_LIBRARY)

IF(APPLE AND NOT ANDROID)
   ##IF(OSG_BUILD_PLATFORM_IPHONE OR OSG_BUILD_PLATFORM_IPHONE_SIMULATOR)
     # compile datapathutils.cpp as objective-c++
     SET_SOURCE_FILES_PROPERTIES(datapathutils.cpp
       PROPERTIES COMPILE_FLAGS "-x objective-c++"
     )
   ##ENDIF()
ENDIF()

if (MSVC)
   # PCH doesn't help with dtUtil on g++.
   ADD_PRECOMPILED_HEADER(${LIB_NAME} prefix/dtutilprefix.h prefix/dtutilprefix.cpp LIB_SOURCES)
endif (MSVC)

DELTA3D_ADD_LIBRARY(${LIB_NAME} DT_UTIL_LIBRARY)

