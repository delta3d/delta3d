
SET(LIB_NAME     dtDirector)


SET(HEADER_PATH ${CMAKE_SOURCE_DIR}/inc/${LIB_NAME})
SET(SOURCE_PATH ${CMAKE_SOURCE_DIR}/src/${LIB_NAME})

SET(LIB_PUBLIC_HEADERS ${HEADER_PATH}/actionnode.h
    ${HEADER_PATH}/arrayvaluenode.h
    ${HEADER_PATH}/colors.h
    ${HEADER_PATH}/director.h
    ${HEADER_PATH}/directorbinary.h
    ${HEADER_PATH}/directorgraph.h
    ${HEADER_PATH}/directornotifier.h
    ${HEADER_PATH}/directortypefactory.h
    ${HEADER_PATH}/directorxml.h
    ${HEADER_PATH}/directorxmlhandler.h
    ${HEADER_PATH}/eventnode.h
    ${HEADER_PATH}/export.h
    ${HEADER_PATH}/groupnode.h
    ${HEADER_PATH}/inputlink.h
    ${HEADER_PATH}/latentactionnode.h
    ${HEADER_PATH}/messagegmcomponent.h
    ${HEADER_PATH}/mutatornode.h
    ${HEADER_PATH}/node.h
    ${HEADER_PATH}/nodemanager.h
    ${HEADER_PATH}/nodepluginregistry.h
    ${HEADER_PATH}/nodetype.h
    ${HEADER_PATH}/outputlink.h
    ${HEADER_PATH}/valuelink.h
    ${HEADER_PATH}/valuenode.h
)

SET (LIB_SOURCES ${SOURCE_PATH}/actionnode.cpp
     ${SOURCE_PATH}/arrayvaluenode.cpp
     ${SOURCE_PATH}/colors.cpp
     ${SOURCE_PATH}/director.cpp
     ${SOURCE_PATH}/directorbinary.cpp
     ${SOURCE_PATH}/directorgraph.cpp
     ${SOURCE_PATH}/directornotifier.cpp
     ${SOURCE_PATH}/directortypefactory.cpp
     ${SOURCE_PATH}/directorxml.cpp
     ${SOURCE_PATH}/directorxmlhandler.cpp
     ${SOURCE_PATH}/eventnode.cpp
     ${SOURCE_PATH}/groupnode.cpp
     ${SOURCE_PATH}/inputlink.cpp
     ${SOURCE_PATH}/latentactionnode.cpp
     ${SOURCE_PATH}/messagegmcomponent.cpp
     ${SOURCE_PATH}/mutatornode.cpp
     ${SOURCE_PATH}/node.cpp
     ${SOURCE_PATH}/nodemanager.cpp
     ${SOURCE_PATH}/nodepluginregistry.cpp
     ${SOURCE_PATH}/nodetype.cpp
     ${SOURCE_PATH}/outputlink.cpp
     ${SOURCE_PATH}/valuelink.cpp
     ${SOURCE_PATH}/valuenode.cpp
    )

SET(LIB_DEPS
  ${DTUTIL_LIBRARY}
  ${DTCORE_LIBRARY}
  ${DTGAME_LIBRARY}
)

DELTA3D_ADD_LIBRARY(${LIB_NAME} DT_DIRECTOR_LIBRARY)
