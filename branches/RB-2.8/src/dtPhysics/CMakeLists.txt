SET(LIB_NAME   dtPhysics)

SET(HEADER_PATH ${CMAKE_SOURCE_DIR}/include/${LIB_NAME})
SET(SOURCE_PATH ${CMAKE_SOURCE_DIR}/source/${LIB_NAME})

INCLUDE_DIRECTORIES(${PAL_INCLUDE_DIR})

file(GLOB LIB_PUBLIC_HEADERS 
   "${HEADER_PATH}/*.h"
   )

set(LIB_SOURCES 
action.cpp
bodywrapper.cpp
charactercontroller.cpp
charactermotionmodel.cpp
collisioncontact.cpp
convexhull.cpp
debugdrawable.cpp
geometry.cpp
mechanicstype.cpp
palphysicsworld.cpp
physicsactcomp.cpp
physicsactorregistry.cpp
physicscomponent.cpp
physicsinterface.cpp
physicsmaterialactor.cpp
physicsmaterials.cpp
physicsobject.cpp
physicsreaderwriter.cpp
primitivetype.cpp
raycast.cpp
trianglerecorder.cpp
)

ADD_LIBRARY(${LIB_NAME} SHARED
    ${LIB_PUBLIC_HEADERS}
    ${LIB_SOURCES}
)

ADD_DEPENDENCIES(${LIB_NAME} PAL)

LINK_WITH_VARIABLES(${LIB_NAME}
                                   OSG_LIBRARY
                                   OSGGA_LIBRARY
                                   OSGVIEWER_LIBRARY
                                   OSGUTIL_LIBRARY
                                   OSGFX_LIBRARY
                                   OSGSIM_LIBRARY
                                   OSGPARTICLE_LIBRARY
                                   OSGEPHEMERIS_LIBRARY
                                   OSGTEXT_LIBRARY
                                   XERCES_LIBRARY
                                   OSGDB_LIBRARY
                                   OPENTHREADS_LIBRARY
                                   PAL_LIBRARY
                                   )

TARGET_LINK_LIBRARIES(${LIB_NAME} 
    ${DTUTIL_LIBRARY}
    ${DTCORE_LIBRARY}
    ${DTABC_LIBRARY}
    ${DTGAME_LIBRARY}
    ${OPENGL_LIBRARY}
    ${AGL_LIBRARY}
)


INCLUDE(ModuleInstall OPTIONAL)

DELTA3D_ADD_LIBRARY_PROPERTIES(${LIB_NAME} DT_PHYSICS_LIBRARY ${LIB_PUBLIC_HEADERS})

