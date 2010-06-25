
SET(LIB_NAME     dtDAL)


SET(HEADER_PATH ${CMAKE_SOURCE_DIR}/inc/${LIB_NAME})
SET(SOURCE_PATH ${CMAKE_SOURCE_DIR}/src/${LIB_NAME})

SET(LIB_PUBLIC_HEADERS
    ${HEADER_PATH}/abstractenumactorproperty.h
    ${HEADER_PATH}/abstractparameter.h
    ${HEADER_PATH}/actoractorproperty.h
    ${HEADER_PATH}/actoridactorproperty.h
    ${HEADER_PATH}/actorpluginregistry.h
    ${HEADER_PATH}/actorproperty.h
    ${HEADER_PATH}/actorpropertyserializer.h
    ${HEADER_PATH}/actorproxy.h
    ${HEADER_PATH}/actorproxyicon.h
    ${HEADER_PATH}/actortype.h
    ${HEADER_PATH}/arrayactorproperty.h
    ${HEADER_PATH}/arrayactorpropertybase.h
    ${HEADER_PATH}/basexml.h
    ${HEADER_PATH}/basexmlhandler.h
    ${HEADER_PATH}/booleanactorproperty.h
    ${HEADER_PATH}/colorrgbaactorproperty.h
    ${HEADER_PATH}/containeractorproperty.h
    ${HEADER_PATH}/datatype.h
    ${HEADER_PATH}/directoryresourcetypehandler.h
    ${HEADER_PATH}/doubleactorproperty.h
    ${HEADER_PATH}/dtdal.h
    ${HEADER_PATH}/enginepropertytypes.h
    ${HEADER_PATH}/enumactorproperty.h
    ${HEADER_PATH}/environmentactor.h
    ${HEADER_PATH}/exceptionenum.h
    ${HEADER_PATH}/export.h
    ${HEADER_PATH}/floatactorproperty.h
    ${HEADER_PATH}/functor.h
    ${HEADER_PATH}/gameevent.h
    ${HEADER_PATH}/gameeventactorproperty.h
    ${HEADER_PATH}/gameeventmanager.h
    ${HEADER_PATH}/genericactorproperty.h
    ${HEADER_PATH}/groupactorproperty.h
    ${HEADER_PATH}/intactorproperty.h
    ${HEADER_PATH}/librarymanager.h
    ${HEADER_PATH}/longactorproperty.h
    ${HEADER_PATH}/map.h
    ${HEADER_PATH}/mapcontenthandler.h
    ${HEADER_PATH}/mapxml.h
    ${HEADER_PATH}/mapxmlconstants.h
    ${HEADER_PATH}/namedparameter.h
    ${HEADER_PATH}/objecttype.h
    ${HEADER_PATH}/physicalactorproxy.h
    ${HEADER_PATH}/plugin_export.h
    ${HEADER_PATH}/project.h
    ${HEADER_PATH}/propertycontainer.h
    ${HEADER_PATH}/propertymacros.h
    ${HEADER_PATH}/resourceactorproperty.h
    ${HEADER_PATH}/resourcedescriptor.h
    ${HEADER_PATH}/resourcehelper.h
    ${HEADER_PATH}/resourcetreenode.h
    ${HEADER_PATH}/serializeable.h
    ${HEADER_PATH}/stringactorproperty.h
    ${HEADER_PATH}/transformableactorproxy.h
    ${HEADER_PATH}/typetoactorproperty.h
	${HEADER_PATH}/vectoractorproperties.h
)

SET(LIB_SOURCES
    ${SOURCE_PATH}/abstractenumactorproperty.cpp
    ${SOURCE_PATH}/actoractorproperty.cpp
    ${SOURCE_PATH}/actoridactorproperty.cpp
    ${SOURCE_PATH}/actorpluginregistry.cpp
    ${SOURCE_PATH}/actorproperty.cpp
    ${SOURCE_PATH}/actorpropertyserializer.cpp
    ${SOURCE_PATH}/actorproxy.cpp
    ${SOURCE_PATH}/actorproxyicon.cpp
    ${SOURCE_PATH}/actortype.cpp
    ${SOURCE_PATH}/arrayactorpropertybase.cpp
    ${SOURCE_PATH}/basexml.cpp
    ${SOURCE_PATH}/basexmlhandler.cpp
    ${SOURCE_PATH}/booleanactorproperty.cpp
    ${SOURCE_PATH}/colorrgbaactorproperty.cpp
    ${SOURCE_PATH}/containeractorproperty.cpp
    ${SOURCE_PATH}/datatype.cpp
    ${SOURCE_PATH}/directoryresourcetypehandler.cpp
    ${SOURCE_PATH}/doubleactorproperty.cpp
    ${SOURCE_PATH}/enumactorproperty.cpp
    ${SOURCE_PATH}/environmentactor.cpp
    ${SOURCE_PATH}/exceptionenum.cpp
    ${SOURCE_PATH}/floatactorproperty.cpp
    ${SOURCE_PATH}/gameevent.cpp
    ${SOURCE_PATH}/gameeventactorproperty.cpp
    ${SOURCE_PATH}/gameeventmanager.cpp
    ${SOURCE_PATH}/groupactorproperty.cpp
    ${SOURCE_PATH}/intactorproperty.cpp
    ${SOURCE_PATH}/librarymanager.cpp
    ${SOURCE_PATH}/longactorproperty.cpp
    ${SOURCE_PATH}/map.cpp
    ${SOURCE_PATH}/mapcontenthandler.cpp
    ${SOURCE_PATH}/mapxml.cpp
    ${SOURCE_PATH}/mapxmlconstants.cpp
    ${SOURCE_PATH}/namedparameter.cpp
    ${SOURCE_PATH}/objecttype.cpp
    ${SOURCE_PATH}/physicalactorproxy.cpp
    ${SOURCE_PATH}/project.cpp
    ${SOURCE_PATH}/propertycontainer.cpp
    ${SOURCE_PATH}/resourceactorproperty.cpp
    ${SOURCE_PATH}/resourcedescriptor.cpp
    ${SOURCE_PATH}/resourcehelper.cpp
    ${SOURCE_PATH}/stringactorproperty.cpp
    ${SOURCE_PATH}/transformableactorproxy.cpp
	${SOURCE_PATH}/vectoractorproperties.cpp
)

ADD_PRECOMPILED_HEADER(${LIB_NAME} prefix/dtdalprefix.h prefix/dtdalprefix.cpp LIB_SOURCES)

ADD_LIBRARY(${LIB_NAME} SHARED
    ${LIB_PUBLIC_HEADERS}
    ${LIB_SOURCES}
)


TARGET_LINK_LIBRARIES( ${LIB_NAME}
                       ${DTUTIL_LIBRARY}
                       ${DTCORE_LIBRARY}
                     )

INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC_IDE)
  SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL DT_DAL_LIBRARY)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC_IDE)
