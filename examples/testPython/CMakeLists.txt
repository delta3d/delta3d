SET(TARGET_SRC testpython.cpp readme.txt)

INCLUDE_DIRECTORIES( ${PYTHON_INCLUDE_PATH} )

BUILD_EXE_EXAMPLE( testPython 
                  ${DTSCRIPT_LIBRARY}
                  ${DTABC_LIBRARY}
                 )
                                        
ADD_DEPENDENCIES( testPython
                   PyDtUtil
                   PyDtCore
                   PyDtABC
                )

IF (MSVC)
  SET(CMAKE_EXE_LINKER_FLAGS "/NODEFAULTLIB:python25_d /NODEFAULTLIB:python26_d /NODEFAULTLIB:python27_d") #ignore the auto-link library Python adds in
ENDIF (MSVC)
