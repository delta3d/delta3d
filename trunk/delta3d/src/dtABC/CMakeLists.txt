
# FIXME: For OS X, need flag for Framework or dylib

SET(LIB_NAME     dtABC)


IF (WIN32)
ELSE (WIN32) 
ENDIF (WIN32)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/inc/dtABC)
SET(LIB_PUBLIC_HEADERS
   ${HEADER_PATH}/action.h
   ${HEADER_PATH}/application.h
   ${HEADER_PATH}/applicationconfigdata.h
   ${HEADER_PATH}/applicationconfighandler.h
   ${HEADER_PATH}/applicationconfigschema.h
   ${HEADER_PATH}/applicationconfigwriter.h
   ${HEADER_PATH}/autotrigger.h
   ${HEADER_PATH}/baseabc.h
   ${HEADER_PATH}/beziercontroller.h
   ${HEADER_PATH}/beziercontrolpoint.h
   ${HEADER_PATH}/beziernode.h
   ${HEADER_PATH}/curvenode.h
   ${HEADER_PATH}/dtabc.h
   ${HEADER_PATH}/event.h
   ${HEADER_PATH}/export.h
   ${HEADER_PATH}/fl_dt_window.h
   ${HEADER_PATH}/motionaction.h
   ${HEADER_PATH}/pathpoint.h
   ${HEADER_PATH}/pathpointconverter.h
   ${HEADER_PATH}/proximitytrigger.h
   ${HEADER_PATH}/state.h
   ${HEADER_PATH}/statemanager.h
   ${HEADER_PATH}/trigger.h
   ${HEADER_PATH}/weather.h
   ${HEADER_PATH}/widget.h
)

ADD_LIBRARY(${LIB_NAME} SHARED
    ${LIB_PUBLIC_HEADERS}
action.cpp
application.cpp
applicationconfigdata.cpp
applicationconfighandler.cpp
applicationconfigschema.cpp
applicationconfigwriter.cpp
autotrigger.cpp
baseabc.cpp
beziercontroller.cpp
event.cpp
motionaction.cpp
proximitytrigger.cpp
state.cpp
statemanager.cpp
trigger.cpp
weather.cpp
widget.cpp
)

TARGET_LINK_LIBRARIES(${LIB_NAME} 
												optimized dtUtil debug dtUtilD
												optimized dtCore debug dtCoreD
												optimized dtDAL debug dtDALD
												 ${FLTK_LIBRARY} ${OPENGL_LIBRARY} ${MATH_LIBRARY})
												 
												 
#TARGET_LINK_LIBRARIES(${LIB_NAME} dtUtil dtCore dtAI dtDAL ${FLTK_LIBRARY} ${OPENGL_LIBRARY} ${MATH_LIBRARY}
 #  ${OSG_LIBRARY} ${OSGDB_LIBRARY} ${OSGGA_LIBRARY} ${OSGPARTICLE_LIBRARY} ${OSGUTIL_LIBRARY}
   #${OSGTEXT_LIBRARY} ${OSGSIM_LIBRARY} ${OSGFX_LIBRARY} ${OSGVIEWER_LIBRARY}
   #${OPENTHREADS_LIBRARY} ${XERCES_LIBRARY} ${CARBON_LIBRARY})

SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL DT_ABC_LIBRARY)


INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)
