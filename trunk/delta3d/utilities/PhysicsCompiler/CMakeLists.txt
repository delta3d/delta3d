SET(PROG_NAME   PhysicsCompiler)

SET(HEADER_PATH ${CMAKE_CURRENT_SOURCE_DIR})
SET(SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB PROG_PUBLIC_HEADERS 
   "${HEADER_PATH}/*.h"
   )

file(GLOB PROG_SOURCES 
   "${SOURCE_PATH}/*.cpp"
   )

ADD_EXECUTABLE(${PROG_NAME}
    ${PROG_PUBLIC_HEADERS}
    ${PROG_SOURCES}
)

LINK_WITH_VARIABLES(${PROG_NAME}
                                   OSG_LIBRARY
                                   OSGUTIL_LIBRARY
                                   OSGTEXT_LIBRARY
                                   XERCES_LIBRARY
                                   OSGDB_LIBRARY
                                   OPENTHREADS_LIBRARY)

TARGET_LINK_LIBRARIES(${PROG_NAME} 
   ${DTUTIL_LIBRARY}
   ${DTCORE_LIBRARY}
   ${DTGAME_LIBRARY}
   ${DTABC_LIBRARY}
   ${OPENGL_LIBRARY}
   ${AGL_LIBRARY}
   ${DTPHYSICS_LIBRARY}
)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC_IDE)
  SET_TARGET_PROPERTIES(${PROG_NAME} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES(${PROG_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC_IDE)
