###############################################
# OSGExp max plugin
###############################################
SET(OSG_EXP OSGExp)

SET(SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src/OSGExp)
SET(HEADER_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include/OSGExp)

FILE (GLOB HEADER_FILES "${HEADER_PATH}/*.h")
FILE (GLOB SOURCE_FILES "${SOURCE_PATH}/*.cpp")


#.cpp files need the path to the header files
INCLUDE_DIRECTORIES( 
                      ${3DS_INCLUDE_DIR}
                      ${HEADER_PATH}
                   )

ADD_LIBRARY( ${OSG_EXP} SHARED
             ${HEADER_FILES}
             ${SOURCE_FILES}
             ${SOURCE_PATH}/OSGExp.def
             ${SOURCE_PATH}/OSGExp.rc
)

LINK_WITH_VARIABLES( ${OSG_EXP}
                     OSG_LIBRARY
                     OSGDB_LIBRARY
                     OSGPARTICLE_LIBRARY
                     OSGSIM_LIBRARY
                     OSGUTIL_LIBRARY
                    )

TARGET_LINK_LIBRARIES( ${OSG_EXP}
                       ${3DS_LIBRARIES}
                       ${OPENGL_LIBRARY} 
                       Comctl32
                       Previewer
                      )

#remove the inherited definition
REMOVE_DEFINITIONS(-DNOMINMAX)


#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES(${OSG_EXP} PROPERTIES PREFIX "../../bin/")
  SET_TARGET_PROPERTIES(${OSG_EXP} PROPERTIES IMPORT_PREFIX "../")
  SET_TARGET_PROPERTIES(${OSG_EXP} PROPERTIES SUFFIX ".dle")
ENDIF (MSVC)



################################################################
# OsgExp Previewer library
################################################################
SET(EXP_PRE Previewer)

SET(SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src/Previewer)
SET(HEADER_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include/Previewer)

FILE (GLOB HEADER_FILES "${HEADER_PATH}/*.h")
FILE (GLOB SOURCE_FILES "${SOURCE_PATH}/*.cpp")


#.cpp files need the path to the header files
INCLUDE_DIRECTORIES( ${HEADER_PATH}
                   )
                   

ADD_LIBRARY( ${EXP_PRE} SHARED
             ${HEADER_FILES}
             ${SOURCE_FILES}
)

LINK_WITH_VARIABLES( ${EXP_PRE}
                     OSG_LIBRARY
                     OSGGA_LIBRARY
                     OSGUTIL_LIBRARY
                     OPENTHREADS_LIBRARY
                   )
                      
TARGET_LINK_LIBRARIES( ${EXP_PRE}
                       ${OPENGL_LIBRARY} 
                     )

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES(${EXP_PRE} PROPERTIES PREFIX "../../bin/")
  SET_TARGET_PROPERTIES(${EXP_PRE} PROPERTIES IMPORT_PREFIX "../")
  SET_TARGET_PROPERTIES(${EXP_PRE} PROPERTIES DEFINE_SYMBOL PREVIEWER_LIBRARY)

ENDIF (MSVC)




################################################################
# OsgExp Helper Max plugin
################################################################
SET(EXP_HELP OsgHelper)

SET(SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src/OSGExp/Helpers)
SET(HEADER_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include/OSGExp/Helpers)

FILE (GLOB HEADER_FILES "${HEADER_PATH}/*.h")
FILE (GLOB SOURCE_FILES "${SOURCE_PATH}/*.cpp")


#.cpp files need the path to the header files
INCLUDE_DIRECTORIES( ${HEADER_PATH}
                   )

ADD_LIBRARY( ${EXP_HELP} SHARED
             ${HEADER_FILES}
             ${SOURCE_FILES}
             ${SOURCE_PATH}/OSGHelper.def
             ${SOURCE_PATH}/OSGHelper.rc
)

TARGET_LINK_LIBRARIES( ${EXP_HELP}
                       ${3DS_LIBRARIES}
                       Comctl32
                      )

IF (MSVC)
  SET_TARGET_PROPERTIES(${EXP_HELP} PROPERTIES SUFFIX ".dlo")
ENDIF (MSVC)
