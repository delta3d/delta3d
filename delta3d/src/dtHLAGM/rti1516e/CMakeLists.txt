
if (RTI1516e_INCLUDE_DIR)

SET(LIB_NAME     dtHLAGM_rti1516e)

INCLUDE_DIRECTORIES(${RTI1516e_INCLUDE_DIR})

file(GLOB LIB_PUBLIC_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

SET(LIB_SOURCES
rti1516eambassador.cpp
rti1516eregion.cpp
rti1516eutil.cpp
)

ADD_LIBRARY(${LIB_NAME} SHARED
    ${LIB_PUBLIC_HEADERS}
    ${LIB_SOURCES}
)

LINK_WITH_VARIABLES(${LIB_NAME}
                     OSGSIM_LIBRARY
                     OPENTHREADS_LIBRARY
                   )

TARGET_LINK_LIBRARIES(${LIB_NAME}
                      ${DTUTIL_LIBRARY}
                      ${DTDAL_LIBRARY}
                      ${DTGAME_LIBRARY}
                      ${DTHLAGM_LIBRARY}
                      ${RTI1516e_LIBRARIES}
                     )

IF (WIN32)
   TARGET_LINK_LIBRARIES(${LIB_NAME} wsock32)
ENDIF (WIN32)

#SET_TARGET_PROPERTIES(${LIB_NAME}
#                     PROPERTIES DEFINE_SYMBOL DT_HLAGM_LIBRARY)

INCLUDE(ModuleInstall OPTIONAL)

IF (MSVC_IDE)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4290") #ignore the C4290 warning
ENDIF (MSVC_IDE)

endif (RTI1516e_INCLUDE_DIR)
