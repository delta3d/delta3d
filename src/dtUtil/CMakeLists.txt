SET(LIB_NAME     dtUtil)


SET(HEADER_PATH ${CMAKE_SOURCE_DIR}/inc/${LIB_NAME})
SET(SOURCE_PATH ${CMAKE_SOURCE_DIR}/src/${LIB_NAME})

SET( LIB_SOURCES 
    ${SOURCE_PATH}/configproperties.cpp
    ${SOURCE_PATH}/coordinates.cpp
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
    ${SOURCE_PATH}/nodeprintout.cpp
    ${SOURCE_PATH}/noisetexture.cpp
    ${SOURCE_PATH}/polardecomp.cpp
#    ${SOURCE_PATH}/precomp.cpp
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
   
SET( LIB_PUBLIC_HEADERS
    ${HEADER_PATH}/assocvector.h
    ${HEADER_PATH}/barycentric.h
    ${HEADER_PATH}/bits.h
    ${HEADER_PATH}/boundingshapeutils.h
    ${HEADER_PATH}/breakoverride.h
    ${HEADER_PATH}/collectorutil.h
    ${HEADER_PATH}/command.h
    ${HEADER_PATH}/configproperties.h
    ${HEADER_PATH}/coordinates.h
    ${HEADER_PATH}/datapathutils.h
    ${HEADER_PATH}/datastream.h
    ${HEADER_PATH}/datetime.h
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
    ${HEADER_PATH}/geometrycollector.h
    ${HEADER_PATH}/getsetmacros.h
    ${HEADER_PATH}/hash.h
    ${HEADER_PATH}/hashmap.h
    ${HEADER_PATH}/hotspotdefinition.h
    ${HEADER_PATH}/hotspotxml.h
    ${HEADER_PATH}/kdtree.h
    ${HEADER_PATH}/keyframedecoder.h
    ${HEADER_PATH}/librarysharingmanager.h
    ${HEADER_PATH}/log.h
    ${HEADER_PATH}/logobserver.h
    ${HEADER_PATH}/logobserverconsole.h
    ${HEADER_PATH}/logobserverfile.h
    ${HEADER_PATH}/macros.h
    ${HEADER_PATH}/mathdefines.h
    ${HEADER_PATH}/matrixutil.h
    ${HEADER_PATH}/mswin.h
    ${HEADER_PATH}/mswinmacros.h
    ${HEADER_PATH}/nodecollector.h
    ${HEADER_PATH}/nodeprintout.h
    ${HEADER_PATH}/noise1.h
    ${HEADER_PATH}/noise2.h
    ${HEADER_PATH}/noise3.h
    ${HEADER_PATH}/noisetexture.h
    ${HEADER_PATH}/noiseutility.h
    ${HEADER_PATH}/objectfactory.h
    ${HEADER_PATH}/polardecomp.h
    ${HEADER_PATH}/referencedinterface.h
    ${HEADER_PATH}/refstring.h
    ${HEADER_PATH}/resourceloader.h
    ${HEADER_PATH}/resourcemanager.h
    ${HEADER_PATH}/seamlessnoise.h
    ${HEADER_PATH}/serializer.h
    ${HEADER_PATH}/stateattributecollector.h
    ${HEADER_PATH}/stringutils.h
    ${HEADER_PATH}/tangentspacevisitor.h
    ${HEADER_PATH}/templateutility.h
    ${HEADER_PATH}/threadpool.h
    ${HEADER_PATH}/transformation.h
    ${HEADER_PATH}/tree.h
    ${HEADER_PATH}/typetraits.h
    ${HEADER_PATH}/typemanip.h
    ${HEADER_PATH}/version.h
    ${HEADER_PATH}/wrapperosgobject.h
    ${HEADER_PATH}/xercesbininputstreamistream.h
    ${HEADER_PATH}/xerceserrorhandler.h
    ${HEADER_PATH}/xercesparser.h
    ${HEADER_PATH}/xercesutils.h
    ${HEADER_PATH}/xerceswriter.h
   )

SET(LIB_EXTERNAL_DEPS
   OSG_LIBRARY 
   OSGDB_LIBRARY
   OSGUTIL_LIBRARY
   OSGSIM_LIBRARY
   XERCES_LIBRARY
   OPENTHREADS_LIBRARY)

if (MSVC)
   # PCH doesn't help with dtUtil on g++.
   ADD_PRECOMPILED_HEADER(${LIB_NAME} prefix/dtutilprefix.h prefix/dtutilprefix.cpp LIB_SOURCES)
endif (MSVC)

DELTA3D_ADD_LIBRARY(${LIB_NAME} DT_UTIL_LIBRARY)

