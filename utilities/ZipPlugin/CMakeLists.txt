
SET(LIB_NAME     osgdb_zip)

SET(SOURCE_PATH ${CMAKE_SOURCE_DIR}/utilities/ZipPlugin)

SET( LIB_SOURCES 
    ${SOURCE_PATH}/ReaderWriterZIP.cpp
    ${SOURCE_PATH}/unzip.h
    ${SOURCE_PATH}/unzip.cpp
    ${SOURCE_PATH}/ZipArchive.h
    ${SOURCE_PATH}/ZipArchive.cpp
   )
   

ADD_LIBRARY(${LIB_NAME} MODULE
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

ADD_DEFINITIONS(-DZIP_STD)
# OSG doesn't want the "lib" on the front
SET(CMAKE_SHARED_MODULE_PREFIX "")

IF (MSVC_IDE)
  SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL DT_UTIL_LIBRARY)
ENDIF (MSVC_IDE)

