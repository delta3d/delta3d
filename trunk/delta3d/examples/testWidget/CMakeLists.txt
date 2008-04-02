
SET(PROG_NAME     testWidget)

file(GLOB PROG_SOURCES "*.cxx" "*.cpp" "*.h")
list(REMOVE_ITEM PROG_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/precomp.cpp)

ADD_EXECUTABLE(${PROG_NAME}
    ${PROG_SOURCES}
)

LINK_WITH_VARIABLES(${PROG_NAME}
                                         FLTK_LIBRARY
                                         DTCORE_LIBRARY
                                         DTABC_LIBRARY
                                         OSG_LIBRARY)
IF (WIN32)                                             
TARGET_LINK_LIBRARIES(${PROG_NAME} Ws2_32 Comctl32)
ENDIF (WIN32)


INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES(${PROG_NAME} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES(${PROG_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)
