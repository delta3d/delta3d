
SET(LIB_NAME     testAnim)

file(GLOB TARGET_SRC "*.cpp" "*.txt")
file(GLOB TARGET_H "*.h")

INCLUDE_DIRECTORIES( ${CAL3D_INCLUDE_DIR} )#for dtAnim

BUILD_LIB_EXAMPLE(${LIB_NAME} TEST_ANIM_LIBRARY
                      ${DTUTIL_LIBRARY}
                      ${DTCORE_LIBRARY}
                      ${DTABC_LIBRARY}
                      ${DTANIM_LIBRARY}
                      ${DTAI_LIBRARY}
                      ${DTGAME_LIBRARY}
                      ${DTACTORS_LIBRARY}
                      )
