
SET(APP_NAME WebPackager)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/utilities/LMS/${APP_NAME}/inc)
SET(SOURCE_PATH ${DELTA3D_SOURCE_DIR}/utilities/LMS/${APP_NAME}/src)

SET(PROG_HEADERS ${HEADER_PATH}/job_base.h
                 ${HEADER_PATH}/job_deployment.h
                 ${HEADER_PATH}/job_html.h
                 ${HEADER_PATH}/job_jar.h
                 ${HEADER_PATH}/job_jardata.h
                 ${HEADER_PATH}/job_jarlib.h
                 ${HEADER_PATH}/job_jnlp.h
                 ${HEADER_PATH}/job_launchconfig.h
                 ${HEADER_PATH}/job_launchpage.h
                 ${HEADER_PATH}/job_manifest.h
                 ${HEADER_PATH}/job_sgml.h
                 ${HEADER_PATH}/job_xml.h
                 ${HEADER_PATH}/package_builder.h
                 ${HEADER_PATH}/package_profile.h
                 ${HEADER_PATH}/package_utils.h  )
                 
SET(PROG_SOURCES ${SOURCE_PATH}/job_base.cpp
                 ${SOURCE_PATH}/job_deployment.cpp
                 ${SOURCE_PATH}/job_html.cpp
                 ${SOURCE_PATH}/job_jar.cpp
                 ${SOURCE_PATH}/job_jardata.cpp
                 ${SOURCE_PATH}/job_jarlib.cpp
                 ${SOURCE_PATH}/job_jnlp.cpp
                 ${SOURCE_PATH}/job_launchconfig.cpp
                 ${SOURCE_PATH}/job_launchpage.cpp
                 ${SOURCE_PATH}/job_manifest.cpp
                 ${SOURCE_PATH}/job_sgml.cpp
                 ${SOURCE_PATH}/job_xml.cpp
                 ${SOURCE_PATH}/main.cpp
                 ${SOURCE_PATH}/package_builder.cpp
                 ${SOURCE_PATH}/package_profile.cpp
                 ${SOURCE_PATH}/package_utils.cpp )

INCLUDE_DIRECTORIES(${HEADER_PATH})

ADD_EXECUTABLE(${APP_NAME}
    ${PROG_HEADERS}
    ${PROG_SOURCES}
)

TARGET_LINK_LIBRARIES( ${APP_NAME}
                       ${DTUTIL_LIBRARY}
                     )           

IF (MSVC)
  SET_TARGET_PROPERTIES(${APP_NAME} PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")
ENDIF (MSVC)