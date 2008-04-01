
SET(LIB_NAME     testTerrain)


IF (WIN32)
ELSE (WIN32) 
ENDIF (WIN32)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/examples/testTerrain)

ADD_LIBRARY(${LIB_NAME} SHARED
     ${HEADER_PATH}/testterrain.cpp
)

#ADD_DEPENDENCIES (${LIB_NAME}  dtGUI dtABC)

LINK_WITH_VARIABLES(${LIB_NAME}         
                                         GDAL_LIBRARY
                                         DTCORE_LIBRARY
                                         DTABC_LIBRARY
                                         DTTERRAIN_LIBRARY
                                         ODE_LIBRARY
                                         OSGDB_LIBRARY
                                         OSGPARTICLE_LIBRARY
                                         OSGTEXT_LIBRARY
                                         PLIB_UL_LIBRARY)
IF (WIN32)                                         
TARGET_LINK_LIBRARIES(${LIB_NAME} winmm)
ENDIF (WIN32)                                              

INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)
