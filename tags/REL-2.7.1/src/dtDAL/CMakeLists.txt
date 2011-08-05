SET(LIB_NAME     dtDAL)


SET(HEADER_PATH ${CMAKE_SOURCE_DIR}/inc/${LIB_NAME})
SET(SOURCE_PATH ${CMAKE_SOURCE_DIR}/src/${LIB_NAME})

SET(LIB_PUBLIC_HEADERS
    ${HEADER_PATH}/abstractenumactorproperty.h
    ${HEADER_PATH}/abstractparameter.h
    ${HEADER_PATH}/actoractorproperty.h
    ${HEADER_PATH}/actoridactorproperty.h
    ${HEADER_PATH}/actorhierarchynode.h
    ${HEADER_PATH}/actorpluginregistry.h
    ${HEADER_PATH}/actorproperty.h
    ${HEADER_PATH}/actorpropertyserializer.h
    ${HEADER_PATH}/actorproxy.h
    ${HEADER_PATH}/actorproxyicon.h
    ${HEADER_PATH}/actortype.h
    ${HEADER_PATH}/arrayactorproperty.h
    ${HEADER_PATH}/arrayactorpropertybase.h
    ${HEADER_PATH}/arrayactorpropertycomplex.h
    ${HEADER_PATH}/baseactorobject.h
    ${HEADER_PATH}/basexml.h
    ${HEADER_PATH}/basexmlhandler.h
    ${HEADER_PATH}/basexmlreaderwriter.h
    ${HEADER_PATH}/bitmaskactorproperty.h
    ${HEADER_PATH}/booleanactorproperty.h
    ${HEADER_PATH}/colorrgbaactorproperty.h
    ${HEADER_PATH}/containeractorproperty.h
    ${HEADER_PATH}/datatype.h
    ${HEADER_PATH}/defaultpropertymanager.h
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
    ${HEADER_PATH}/mapheaderdata.h
    ${HEADER_PATH}/mapheaderhandler.h
    ${HEADER_PATH}/mapxml.h
    ${HEADER_PATH}/mapxmlconstants.h
    ${HEADER_PATH}/namedactorparameter.h
    ${HEADER_PATH}/namedarrayparameter.h
    ${HEADER_PATH}/namedbitmaskparameter.h
    ${HEADER_PATH}/namedbooleanparameter.h
    ${HEADER_PATH}/namedcontainerparameter.h
    ${HEADER_PATH}/nameddoubleparameter.h
    ${HEADER_PATH}/namedenumparameter.h
    ${HEADER_PATH}/namedfloatparameter.h
    ${HEADER_PATH}/namedgameeventparameter.h
    ${HEADER_PATH}/namedgenericparameter.h
    ${HEADER_PATH}/namedgroupparameter.h
    ${HEADER_PATH}/namedgroupparameter.inl
    ${HEADER_PATH}/namedintparameter.h
    ${HEADER_PATH}/namedlongintparameter.h
    ${HEADER_PATH}/namedparameter.h
    ${HEADER_PATH}/namedparameters.h
    ${HEADER_PATH}/namedpodparameter.h
    ${HEADER_PATH}/namedpropertycontainerparameter.h
    ${HEADER_PATH}/namedresourceparameter.h
    ${HEADER_PATH}/namedrgbacolorparameter.h
    ${HEADER_PATH}/namedrgbcolorparameter.h
    ${HEADER_PATH}/namedshortintparameter.h
    ${HEADER_PATH}/namedstringparameter.h
    ${HEADER_PATH}/namedunsignedcharparameter.h
    ${HEADER_PATH}/namedunsignedintparameter.h
    ${HEADER_PATH}/namedunsignedlongintparameter.h
    ${HEADER_PATH}/namedunsignedshortintparameter.h
    ${HEADER_PATH}/namedvectorparameters.h
    ${HEADER_PATH}/objecttype.h
    ${HEADER_PATH}/physicalactorproxy.h
    ${HEADER_PATH}/plugin_export.h
    ${HEADER_PATH}/project.h
    ${HEADER_PATH}/projectconfig.h
    ${HEADER_PATH}/projectconfigreaderwriter.h
    ${HEADER_PATH}/projectconfigxmlhandler.h
    ${HEADER_PATH}/propertycontainer.h
    ${HEADER_PATH}/propertycontaineractorproperty.h
    ${HEADER_PATH}/propertymacros.h
    ${HEADER_PATH}/resourceactorproperty.h
    ${HEADER_PATH}/resourcedescriptor.h
    ${HEADER_PATH}/resourcehelper.h
    ${HEADER_PATH}/resourcetreenode.h
    ${HEADER_PATH}/serializeable.h
    ${HEADER_PATH}/stringactorproperty.h
    ${HEADER_PATH}/stringselectoractorproperty.h
    ${HEADER_PATH}/transformableactorproxy.h
    ${HEADER_PATH}/typetoactorproperty.h
   ${HEADER_PATH}/vectoractorproperties.h
)

SET(LIB_SOURCES
   warndtdal.cpp
)

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
