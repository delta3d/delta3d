
SET(LIB_NAME     osgdb_zip)

SET(SOURCE_PATH ${CMAKE_SOURCE_DIR}/utilities/ZipPlugin)

SET( LIB_SOURCES 
    ${SOURCE_PATH}/ReaderWriterZip.cpp
    ${SOURCE_PATH}/unzip.h
    ${SOURCE_PATH}/unzip.cpp
    ${SOURCE_PATH}/ZipArchive.h
    ${SOURCE_PATH}/ZipArchive.cpp
   )
   

ADD_LIBRARY(${LIB_NAME} SHARED
    ${LIB_PUBLIC_HEADERS} 
    ${LIB_SOURCES}
)

LINK_WITH_VARIABLES(${LIB_NAME}
                        OSG_LIBRARY 
                        OSGDB_LIBRARY
                        OSGUTIL_LIBRARY
                        OSGSIM_LIBRARY
                        XERCES_LIBRARY
                        OPENTHREADS_LIBRARY)

INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC_IDE)
  SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL DT_UTIL_LIBRARY)

  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC_IDE)

