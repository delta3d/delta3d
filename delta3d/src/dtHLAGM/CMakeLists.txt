
SET(LIB_NAME     dtHLAGM)

INCLUDE_DIRECTORIES(${RTI_INCLUDE_DIR})


SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/inc/${LIB_NAME})
SET(SOURCE_PATH ${DELTA3D_SOURCE_DIR}/src/${LIB_NAME})
file(GLOB LIB_PUBLIC_HEADERS "${HEADER_PATH}/*.h")

SET(LIB_SOURCES
   attributetype.cpp
   ddmappspacecalculator.cpp
   ddmcalculatorgeographic.cpp
   ddmcameracalculatorgeographic.cpp
   ddmgeographicregiondata.cpp
   ddmmultienumeratedcalculator.cpp
   ddmregioncalculator.cpp
   ddmregioncalculatorgroup.cpp
   ddmregiondata.cpp
   ddmutil.cpp
   distypes.cpp
   environmentprocessrecordlist.cpp
   exceptionenum.cpp
   hlacomponent.cpp
   hlacomponentconfig.cpp
   hlafomconfigxml.cpp
   objectruntimemappinginfo.cpp
   objecttoactor.cpp
   onetomanymapping.cpp
   rprparametertranslator.cpp
   spatial.cpp
)

list(REMOVE_ITEM LIB_SOURCES ${SOURCE_PATH}/precomp.cpp)

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
                      ${RTI_LIBRARIES} 
                     )

IF (WIN32)
   TARGET_LINK_LIBRARIES(${LIB_NAME} wsock32)
ENDIF (WIN32)

SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL DT_HLAGM_LIBRARY)

INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC_IDE)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4290") #ignore the C4290 warning
ENDIF (MSVC_IDE)
