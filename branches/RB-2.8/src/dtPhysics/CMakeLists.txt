SET(LIB_NAME   dtPhysics)

SET(HEADER_PATH ${CMAKE_SOURCE_DIR}/inc/${LIB_NAME})

INCLUDE_DIRECTORIES(${PAL_INCLUDE_DIR})

file(GLOB LIB_PUBLIC_HEADERS 
   "${HEADER_PATH}/*.h"
   )

set(LIB_SOURCES 
action.cpp
bodywrapper.cpp
buoyancyaction.cpp
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

SET(LIB_EXTERNAL_DEPS
  PAL_LIBRARY
)

SET(LIB_DEPS 
    ${DTUTIL_LIBRARY}
    ${DTCORE_LIBRARY}
    ${DTABC_LIBRARY}
    ${DTGAME_LIBRARY}
    ${OPENGL_LIBRARY}
)


DELTA3D_ADD_LIBRARY(${LIB_NAME} DT_PHYSICS_LIBRARY SHARED)
