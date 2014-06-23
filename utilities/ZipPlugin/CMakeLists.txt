
SET(LIB_NAME     osgdb_zip)

SET(SOURCE_PATH ${CMAKE_SOURCE_DIR}/utilities/ZipPlugin)

SET( LIB_SOURCES 
    ${SOURCE_PATH}/ReaderWriterZIP.cpp
    ${SOURCE_PATH}/unzip.h
    ${SOURCE_PATH}/unzip.cpp
    ${SOURCE_PATH}/ZipArchive.h
    ${SOURCE_PATH}/ZipArchive.cpp
   )
   

ADD_DEFINITIONS(-DZIP_STD)
# OSG doesn't want the "lib" on the front
SET(CMAKE_SHARED_MODULE_PREFIX "")

SET(LIB_EXTERNAL_DEPS
                        OSG_LIBRARY 
                        OSGDB_LIBRARY
                        OSGUTIL_LIBRARY
                        OSGSIM_LIBRARY
                        XERCES_LIBRARY
                        OPENTHREADS_LIBRARY)

DELTA3D_ADD_LIBRARY(${LIB_NAME} DT_UTIL_LIBRARY MODULE)
