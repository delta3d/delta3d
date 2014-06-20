
SET(LIB_NAME     dtTerrain)

INCLUDE_DIRECTORIES(${GDAL_INCLUDE_DIR})

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/inc/${LIB_NAME})
SET(SOURCE_PATH ${DELTA3D_SOURCE_DIR}/src/${LIB_NAME})
file(GLOB LIB_PUBLIC_HEADERS "${HEADER_PATH}/*.h")
file(GLOB LIB_SOURCES "*.cpp")
list(REMOVE_ITEM LIB_SOURCES ${SOURCE_PATH}/precomp.cpp)

ADD_LIBRARY(${LIB_NAME} SHARED                        
    ${LIB_PUBLIC_HEADERS}                         
    ${LIB_SOURCES}
)

LINK_WITH_VARIABLES(${LIB_NAME}
                        OSG_LIBRARY
                        OSGDB_LIBRARY
                        GDAL_LIBRARY
                    )

TARGET_LINK_LIBRARIES(${LIB_NAME} 
                      ${OPENGL_LIBRARY}
                      ${MATH_LIBRARY}
                      ${DTUTIL_LIBRARY}
                      ${DTCORE_LIBRARY}                     
                      )

DELTA3D_ADD_LIBRARY_PROPERTIES(${LIB_NAME} DT_TERRAIN_LIBRARY ${LIB_PUBLIC_HEADERS})


INCLUDE(ModuleInstall OPTIONAL)

