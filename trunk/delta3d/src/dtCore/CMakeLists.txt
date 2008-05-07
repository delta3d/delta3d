
# FIXME: For OS X, need flag for Framework or dylib

SET(LIB_NAME     dtCore)


IF (WIN32)
   SET(PlatformSources deltawin_win32.cpp uniqueid_win32.cpp)
ELSE (WIN32) 
   IF (APPLE)
      SET(PlatformSources deltawin_carbon.cpp uniqueid_macosx.cpp)
   ELSE (APPLE)
      SET(PlatformSources deltawin_x11.cpp uniqueid_linux.cpp)      
   ENDIF (APPLE)
ENDIF (WIN32)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/inc/${LIB_NAME})
SET(SOURCE_PATH ${DELTA3D_SOURCE_DIR}/src/${LIB_NAME})
file(GLOB LIB_PUBLIC_HEADERS "${HEADER_PATH}/*.h")
file(GLOB LIB_SOURCES "*.cpp")
LIST(REMOVE_ITEM LIB_SOURCES 
   ${SOURCE_PATH}/precomp.cpp
   ${SOURCE_PATH}/deltawin_win32.cpp 
   ${SOURCE_PATH}/uniqueid_win32.cpp 
   ${SOURCE_PATH}/deltawin_carbon.cpp 
   ${SOURCE_PATH}/uniqueid_macosx.cpp 
   ${SOURCE_PATH}/deltawin_x11.cpp 
   ${SOURCE_PATH}/uniqueid_linux.cpp)

ADD_LIBRARY(${LIB_NAME} SHARED
    ${LIB_PUBLIC_HEADERS} 
    ${LIB_SOURCES}
    ${PlatformSources}
)

LINK_WITH_VARIABLES(${LIB_NAME}
											  DTUTIL_LIBRARY
											  OSG_LIBRARY
											  OSGTEXT_LIBRARY
                                   OSGSIM_LIBRARY
											  OSGDB_LIBRARY
											  OSGGA_LIBRARY
											  OSGVIEWER_LIBRARY
											  OSGPARTICLE_LIBRARY
											  OSGUTIL_LIBRARY
											  OPENTHREADS_LIBRARY
											  ODE_LIBRARY
											  XERCES_LIBRARY
											  UUID_LIBRARY)

TARGET_LINK_LIBRARIES( ${LIB_NAME}			
					  ${OPENGL_LIBRARY} 
					  ${MATH_LIBRARY}
					  ${CARBON_LIBRARY}
					  )

if (X11_FOUND)
   FIND_LIBRARY(DTCORE_XXF86VM_LIB Xxf86vm)
   TARGET_LINK_LIBRARIES( ${LIB_NAME}
               ${X11_LIBRARIES} ${DTCORE_XXF86VM_LIB})
endif (X11_FOUND)
                      
IF (WIN32)
TARGET_LINK_LIBRARIES(${LIB_NAME} Rpcrt4)
ENDIF(WIN32)

SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL DT_CORE_LIBRARY)


INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)
