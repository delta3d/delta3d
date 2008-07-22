SET(TARGET_SRC testpython.cpp readme.txt)

INCLUDE_DIRECTORIES( ${PYTHON_INCLUDE_PATH} )

BUILD_EXE_EXAMPLE( testPython 
                  ${DTSCRIPT_LIBRARY}
                  ${DTABC_LIBRARY}
                 )
                                        
ADD_DEPENDENCIES( testPython
                   PyDtUtil
                   PyDtCore
                )
