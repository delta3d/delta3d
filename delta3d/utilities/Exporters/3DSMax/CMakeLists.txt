SET(LIB_NAME OSGExp)

SET(SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src/OSGExp)
SET(HEADER_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include/OSGExp)

FILE (GLOB HEADER_FILES "${HEADER_PATH}/*.h")
FILE (GLOB SOURCE_FILES "${SOURCE_PATH}/*.cpp")

#.cpp files need the path to the header files
INCLUDE_DIRECTORIES( ${HEADER_PATH}
                     ${3DS_INCLUDE_DIR}
                     $
                   )

ADD_LIBRARY( ${LIB_NAME} SHARED
             ${HEADER_FILES}
             ${SOURCE_FILES}
)

LINK_WITH_VARIABLES( ${LIB_NAME}
                     3DS_CORE_LIBRARY
                     3DS_GEOM_LIBRARY
                     3DS_MAXUTIL_LIBRARY
                     3DS_MESH_LIBRARY
                     3DS_PARAMBLK2_LIBRARY
                     3DS_BMM_LIBRARY
                     OSG_LIBRARY
                     OSGDB_LIBRARY
                     OSGPARTICLE_LIBRARY
                     OSGSIM_LIBRARY
                     OSGUTIL_LIBRARY
                    )

TARGET_LINK_LIBRARIES( ${LIB_NAME}
					             ${OPENGL_LIBRARY} 
					             Comctl32
					            )

#remove the inherited definition
REMOVE_DEFINITIONS(-DNOMINMAX)


#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES SUFFIX ".dle")
ENDIF (MSVC)
