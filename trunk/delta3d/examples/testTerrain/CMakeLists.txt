
SET(PROG_NAME     testTerrain)

file(GLOB PROG_SOURCES "*.cpp" "*.h")
list(REMOVE_ITEM PROG_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/precomp.cpp)

ADD_EXECUTABLE(${PROG_NAME}
    ${PROG_SOURCES}
)

LINK_WITH_VARIABLES(${PROG_NAME}
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
TARGET_LINK_LIBRARIES(${PROG_NAME} winmm)
ENDIF (WIN32)                                              

INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES(${PROG_NAME} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES(${PROG_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)
