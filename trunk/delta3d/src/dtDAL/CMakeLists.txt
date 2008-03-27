
SET(LIB_NAME     dtDAL)


IF (WIN32)
ELSE (WIN32) 
ENDIF (WIN32)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/inc/dtDAL)
SET(LIB_PUBLIC_HEADERS
   ${HEADER_PATH}/abstractparameter.h
   ${HEADER_PATH}/actorpluginregistry.h
   ${HEADER_PATH}/actorproperty.h
   ${HEADER_PATH}/actorproxy.h
   ${HEADER_PATH}/actorproxyicon.h
   ${HEADER_PATH}/actortype.h
   ${HEADER_PATH}/datatype.h
   ${HEADER_PATH}/directoryresourcetypehandler.h
   ${HEADER_PATH}/dtdal.h
   ${HEADER_PATH}/enginepropertytypes.h
   ${HEADER_PATH}/environmentactor.h
   ${HEADER_PATH}/exceptionenum.h
   ${HEADER_PATH}/export.h
   ${HEADER_PATH}/functor.h
   ${HEADER_PATH}/gameevent.h
   ${HEADER_PATH}/gameeventmanager.h
   ${HEADER_PATH}/genericactorproperty.h
   ${HEADER_PATH}/groupactorproperty.h
   ${HEADER_PATH}/librarymanager.h
   ${HEADER_PATH}/map.h
   ${HEADER_PATH}/mapcontenthandler.h
   ${HEADER_PATH}/mapxml.h
   ${HEADER_PATH}/mapxmlconstants.h
   ${HEADER_PATH}/namedparameter.h
   ${HEADER_PATH}/physicalactorproxy.h
   ${HEADER_PATH}/plugin_export.h
   ${HEADER_PATH}/project.h
   ${HEADER_PATH}/rbodyresourcetypehandler.h
   ${HEADER_PATH}/resourcedescriptor.h
   ${HEADER_PATH}/resourcehelper.h
   ${HEADER_PATH}/resourcetreenode.h
   ${HEADER_PATH}/transformableactorproxy.h
   ${HEADER_PATH}/tree.h
   ${HEADER_PATH}/waypointactorproxy.h
)

ADD_LIBRARY(${LIB_NAME} SHARED
    ${LIB_PUBLIC_HEADERS} 
actorpluginregistry.cpp
actorproxy.cpp
actorproxyicon.cpp
actortype.cpp
datatype.cpp
directoryresourcetypehandler.cpp
enginepropertytypes.cpp
environmentactor.cpp
exceptionenum.cpp
gameevent.cpp
gameeventmanager.cpp
groupactorproperty.cpp
librarymanager.cpp
map.cpp
mapcontenthandler.cpp
mapxml.cpp
mapxmlconstants.cpp
namedparameter.cpp
physicalactorproxy.cpp
project.cpp
rbodyresourcetypehandler.cpp
resourcedescriptor.cpp
resourcehelper.cpp
transformableactorproxy.cpp
waypointactorproxy.cpp
)

TARGET_LINK_LIBRARIES(${LIB_NAME}
												optimized dtUtil debug dtUtilD
												optimized dtAI debug dtAID
												)

#TARGET_LINK_LIBRARIES(${LIB_NAME} dtUtil dtCore dtAI ${OPENGL_LIBRARY} ${MATH_LIBRARY}
  # ${OSG_LIBRARY} ${OSGDB_LIBRARY} ${OSGPARTICLE_LIBRARY} ${OSGUTIL_LIBRARY} 
   #${OPENTHREADS_LIBRARY} ${XERCES_LIBRARY})

SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL DT_DAL_LIBRARY)


INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)
