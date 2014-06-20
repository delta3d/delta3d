# Locate pal, the physics abstraction layer
# This module defines
# PAL_LIBRARY
# PAL_FOUND, if false, do not try to link to pal 
# PAL_INCLUDE_DIR, where to find the headers
#
# $PAL_DIR is an environment variable that would
# correspond to the ./configure --prefix=$PAL_DIR
#
# Created by David Guthrie. 

FUNCTION(CONFIGURE_SYSTEM_AOL)
        IF(CMAKE_C_COMPILER_ID STREQUAL "")
                # Unset for MSVC. Don't know why.
                IF(MSVC)
                        SET(CMAKE_C_COMPILER_ID "MSVC")
                ENDIF(MSVC)
        ENDIF()

        IF(CMAKE_SYSTEM_PROCESSOR STREQUAL "unknown")
                # Unset for CYGWIN. Don't know why. Just try with uname in
                # case it's unset for other OSs where uname -m works, too.
                EXECUTE_PROCESS(COMMAND "uname" "-m"
                                RESULT_VARIABLE UNAME_RESULT
                                ERROR_VARIABLE UNAME_ERR
                                OUTPUT_VARIABLE UNAME_OUT
                                OUTPUT_STRIP_TRAILING_WHITESPACE)
                IF(UNAME_RESULT EQUAL 0)
                        SET(CMAKE_SYSTEM_PROCESSOR ${UNAME_OUT})
                ENDIF()

        ENDIF()

        #uname -p says i386, but we know better when its x86 apple (since we don't support actual 32-bit OS X)
        IF(APPLE AND "${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "i386")
            SET(CMAKE_SYSTEM_PROCESSOR "x86_64")
        ENDIF()

        STRING(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_NAME}-${CMAKE_C_COMPILER_ID}"
                TEMP_SYSTEM_AOL)
        SET(SYSTEM_AOL ${TEMP_SYSTEM_AOL}
                CACHE INTERNAL "Detected Architecture-Operating System-Linker types (e.g., i686-cygwin-gnu, x86-windows-msvc), as CMAKE_SYSTEM_PROCESSOR-CMAKE_SYSTEM_NAME-CMAKE_C_COMPILER_ID."
                )
ENDFUNCTION(CONFIGURE_SYSTEM_AOL)

FIND_PATH(PAL_INCLUDE_DIR pal/Config.h
    PATH_SUFFIXES include inc inc/pal include/pal
    HINTS
    ${PAL_DIR}
    $ENV{PAL_DIR}
    $ENV{PAL_DIR}
    ${DELTA3D_EXT_DIR}
    $ENV{DELTA_ROOT}/ext
    $ENV{DELTA_ROOT}
    PATHS
    ~/Library/Frameworks
    /Library/Frameworks
    /usr/local
)
MARK_AS_ADVANCED(PAL_INCLUDE_DIR)

CONFIGURE_SYSTEM_AOL()

MACRO(FIND_PAL_LIBRARY MYLIBRARY MYLIBRARYNAME)


FIND_LIBRARY(${MYLIBRARY} 
    NAMES ${MYLIBRARYNAME}
    PATH_SUFFIXES lib lib/${SYSTEM_AOL} bin bin/${SYSTEM_AOL}
    HINTS
      ${PAL_DIR}
      $ENV{PAL_DIR}
      ${DELTA3D_EXT_DIR}
      $ENV{DELTA_ROOT}/ext
      $ENV{DELTA_ROOT}
    PATHS
    /usr/local
)
MARK_AS_ADVANCED(${MYLIBRARY})
ENDMACRO(FIND_PAL_LIBRARY MYLIBRARY MYLIBRARYNAME)

if (MSVC)
   SET(PAL_LIST libpal)
   SET(PAL_LIST_DEBUG libpald)
else (MSVC)
   SET(PAL_LIST pal)
   SET(PAL_LIST_DEBUG pald)
ENDIF (MSVC)

FIND_PAL_LIBRARY(PAL_LIBRARY "${PAL_LIST}")
FIND_PAL_LIBRARY(PAL_LIBRARY_DEBUG "${PAL_LIST_DEBUG}")


SET(PAL_FOUND "NO")
IF(PAL_LIBRARY AND PAL_INCLUDE_DIR)
    SET(PAL_FOUND "YES")
ENDIF(PAL_LIBRARY AND PAL_INCLUDE_DIR)


