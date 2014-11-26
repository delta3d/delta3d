# Where is this and what do we need it for?
#INCLUDE(ListHandle)

  MACRO(READ_GCC_VERSION)
     if (CMAKE_COMPILER_IS_GNUCC)
        execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion
                        OUTPUT_VARIABLE GCC_VERSION)
        string(REGEX MATCHALL "[0-9]+" GCC_VERSION_COMPONENTS ${GCC_VERSION})
        list(GET GCC_VERSION_COMPONENTS 0 GCC_MAJOR)
        list(GET GCC_VERSION_COMPONENTS 1 GCC_MINOR)
     endif()
  ENDMACRO(READ_GCC_VERSION)


macro(MacroEmptyExternalProject proj dependencies)
 
    ExternalProject_Add(${proj}
      DOWNLOAD_COMMAND ""
      CONFIGURE_COMMAND ""
      BUILD_COMMAND ""
      INSTALL_COMMAND ""
      DEPENDS
        ${dependencies}
      )
 
endmacro()

  MACRO(FILTER_OUT FILTERS INPUTS OUTPUT)
       # Mimicks Gnu Make's $(filter-out) which removes elements 
       # from a list that match the pattern.
       # Arguments:
       #  FILTERS - list of patterns that need to be removed
       #  INPUTS  - list of inputs that will be worked on
       #  OUTPUT  - the filtered list to be returned
       # 
       # Example: 
       #  SET(MYLIST this that and the other)
       #  SET(FILTS this that)
       #
       #  FILTER_OUT("${FILTS}" "${MYLIST}" OUT)
       #  MESSAGE("OUTPUT = ${OUT}")
       #
       # The output - 
       #   OUTPUT = and;the;other
       #
       SET(FOUT "")
       FOREACH(INP ${INPUTS})
           SET(FILTERED 0)
           FOREACH(FILT ${FILTERS})
               IF(${FILTERED} EQUAL 0)
                   IF("${FILT}" STREQUAL "${INP}")
                       SET(FILTERED 1)
                   ENDIF("${FILT}" STREQUAL "${INP}")
               ENDIF(${FILTERED} EQUAL 0)
           ENDFOREACH(FILT ${FILTERS})
           IF(${FILTERED} EQUAL 0)
               SET(FOUT ${FOUT} ${INP})
           ENDIF(${FILTERED} EQUAL 0)
       ENDFOREACH(INP ${INPUTS})
       SET(${OUTPUT} ${FOUT})
   ENDMACRO(FILTER_OUT FILTERS INPUTS OUTPUT)


   MACRO(GET_HEADERS_EXTENSIONLESS DIR GLOB_PATTERN OUTPUT)
            FILE(GLOB TMP "${DIR}/${GLOB_PATTERN}" )
      #FOREACH(F ${TMP})
            #    MESSAGE(STATUS "header-->${F}<--")
      #ENDFOREACH(F ${TMP})
            FILTER_OUT("${DIR}/CVS" "${TMP}" TMP)
            FILTER_OUT("${DIR}/cvs" "${TMP}" ${OUTPUT})
            FILTER_OUT("${DIR}/.svn" "${TMP}" ${OUTPUT})
   ENDMACRO(GET_HEADERS_EXTENSIONLESS DIR GLOB_PATTERN OUTPUT)
   
MACRO(ADD_DIRS_TO_ENV_VAR _VARNAME )
 FOREACH(_ADD_PATH ${ARGN}) 
    FILE(TO_NATIVE_PATH ${_ADD_PATH} _ADD_NATIVE)
    #SET(_CURR_ENV_PATH $ENV{PATH})
    #LIST(SET _CURR_ENV_PATH ${_ADD_PATH})
    #SET(ENV{PATH} ${_CURR_ENV_PATH})${_FILE}
    IF(WIN32)
        SET(ENV{${_VARNAME}} "$ENV{${_VARNAME}};${_ADD_NATIVE}")
    ELSE(WIN32)
        SET(ENV{${_VARNAME}} "$ENV{${_VARNAME}}:${_ADD_NATIVE}")
    ENDIF(WIN32)
    #MESSAGE(" env ${_VARNAME} --->$ENV{${_VARNAME}}<---")
 ENDFOREACH(_ADD_PATH)
ENDMACRO(ADD_DIRS_TO_ENV_VAR _VARNAME )

#---------------------------------------------------
# MACRO CORRECT_PATH VAR PATH 
# corrects slashes in PATH to be cmake conformous ( / ) 
# and puts result in VAR 
#---------------------------------------------------

MACRO(CORRECT_PATH VAR PATH)
    SET(${VAR} ${PATH})
IF(WIN32)    
    STRING(REGEX REPLACE "/" "\\\\" ${VAR} "${PATH}")
ENDIF(WIN32)    
ENDMACRO(CORRECT_PATH)

MACRO(TARGET_LOCATIONS_SET_FILE FILE)
 SET(ACCUM_FILE_TARGETS ${FILE})
 FILE(WRITE ${ACCUM_FILE_TARGETS} "")
ENDMACRO(TARGET_LOCATIONS_SET_FILE FILE)

MACRO(TARGET_LOCATIONS_ACCUM TARGET_NAME)
 IF(ACCUM_FILE_TARGETS)
  IF(EXISTS ${ACCUM_FILE_TARGETS})
    GET_TARGET_PROPERTY(_FILE_LOCATION ${TARGET_NAME} LOCATION)
    FILE(APPEND ${ACCUM_FILE_TARGETS} "${_FILE_LOCATION};")
    #SET(_TARGETS_LIST ${_TARGETS_LIST} "${_FILE_LOCATION}" CACHE INTERNAL "lista dll")
    #MESSAGE("adding target -->${TARGET_NAME}<-- file -->${_FILE_LOCATION}<-- to list -->${_TARGETS_LIST}<--")
    #SET(ACCUM_FILE_TARGETS ${ACCUM_FILE_TARGETS} ${_FILE_LOCATION})
  ENDIF(EXISTS ${ACCUM_FILE_TARGETS})
 ENDIF(ACCUM_FILE_TARGETS)
ENDMACRO(TARGET_LOCATIONS_ACCUM TARGET_NAME)

MACRO(TARGET_LOCATIONS_GET_LIST _VAR)
 IF(ACCUM_FILE_TARGETS)
  IF(EXISTS ${ACCUM_FILE_TARGETS})
      FILE(READ ${ACCUM_FILE_TARGETS} ${_VAR})    
  ENDIF(EXISTS ${ACCUM_FILE_TARGETS})
 ENDIF(ACCUM_FILE_TARGETS)
ENDMACRO(TARGET_LOCATIONS_GET_LIST _VAR)


MACRO(FIND_DEPENDENCY DEPNAME INCLUDEFILE LIBRARY SEARCHPATHLIST)

MESSAGE(STATUS "searching ${DEPNAME} -->${INCLUDEFILE}<-->${LIBRARY}<-->${SEARCHPATHLIST}<--")

SET(MY_PATH_INCLUDE )
SET(MY_PATH_LIB )
SET(MY_PATH_BIN )

FOREACH( MYPATH ${SEARCHPATHLIST} )
    SET(MY_PATH_INCLUDE ${MY_PATH_INCLUDE} ${MYPATH}/include)
    SET(MY_PATH_LIB ${MY_PATH_LIB} ${MYPATH}/lib)
    SET(MY_PATH_BIN ${MY_PATH_BIN} ${MYPATH}/bin)
ENDFOREACH( MYPATH ${SEARCHPATHLIST} )

SET(MYLIBRARY "${LIBRARY}")
SEPARATE_ARGUMENTS(MYLIBRARY)

#MESSAGE( " include paths: -->${MY_PATH_INCLUDE}<--")

#MESSAGE( " ${DEPNAME}_INCLUDE_DIR --> ${${DEPNAME}_INCLUDE_DIR}<--")
    
FIND_PATH("${DEPNAME}_INCLUDE_DIR" ${INCLUDEFILE}
  ${MY_PATH_INCLUDE}
)
MARK_AS_ADVANCED("${DEPNAME}_INCLUDE_DIR")
#MESSAGE( " ${DEPNAME}_INCLUDE_DIR --> ${${DEPNAME}_INCLUDE_DIR}<--")

FIND_LIBRARY("${DEPNAME}_LIBRARY" 
    NAMES ${MYLIBRARY}
  PATHS ${MY_PATH_LIB}
)
IF(${DEPNAME}_LIBRARY)
    GET_FILENAME_COMPONENT(MYLIBNAME ${${DEPNAME}_LIBRARY} NAME_WE)
    GET_FILENAME_COMPONENT(MYBINPATH ${${DEPNAME}_LIBRARY} PATH)
    GET_FILENAME_COMPONENT(MYBINPATH ${MYBINPATH} PATH)
    SET(MYBINPATH "${MYBINPATH}/bin")
    IF(EXISTS ${MYBINPATH})
        SET(MYFOUND 0)
        FOREACH(MYPATH ${MY_ACCUM_BINARY_DEP})
            IF(MYPATH MATCHES ${MYBINPATH})
                SET(MYFOUND 1)
                #MESSAGE("found -->${MYPATH}<-->${MYBINPATH}<--")
            ENDIF(MYPATH MATCHES ${MYBINPATH})
        ENDFOREACH(MYPATH )
        IF(MYFOUND EQUAL 0)
            SET(MY_ACCUM_BINARY_DEP ${MY_ACCUM_BINARY_DEP} ${MYBINPATH})
        ENDIF(MYFOUND EQUAL 0)
    ENDIF(EXISTS ${MYBINPATH})
    #MESSAGE("${DEPNAME}_BINDEP searching -->${MYLIBNAME}${CMAKE_SHARED_MODULE_SUFFIX}<--in-->${MY_PATH_BIN}<--")
#    FIND_FILE("${DEPNAME}_BINDEP" 
#        ${MYLIBNAME}${CMAKE_SHARED_MODULE_SUFFIX}
#      PATHS ${MY_PATH_BIN}
#    )
#    FIND_LIBRARY("${DEPNAME}_BINDEP" 
#        NAMES ${MYLIBRARY}
#      PATHS ${MY_PATH_BIN}
#    )
ENDIF(${DEPNAME}_LIBRARY)
MARK_AS_ADVANCED("${DEPNAME}_LIBRARY")
#MESSAGE( " ${DEPNAME}_LIBRARY --> ${${DEPNAME}_LIBRARY}<--")
IF(${DEPNAME}_INCLUDE_DIR)
  IF(${DEPNAME}_LIBRARY)
    SET( ${DEPNAME}_FOUND "YES" )
    SET( ${DEPNAME}_LIBRARIES ${${DEPNAME}_LIBRARY} )
  ENDIF(${DEPNAME}_LIBRARY)
ENDIF(${DEPNAME}_INCLUDE_DIR)
ENDMACRO(FIND_DEPENDENCY DEPNAME INCLUDEFILE LIBRARY SEARCHPATHLIST)

#SET(MACRO_MESSAGE_DEBUG TRUE)
MACRO(MACRO_MESSAGE MYTEXT)
    IF(MACRO_MESSAGE_DEBUG)
        MESSAGE("in file -->${CMAKE_CURRENT_LIST_FILE}<-- line -->${CMAKE_CURRENT_LIST_LINE}<-- message  ${MYTEXT}")
    ELSE(MACRO_MESSAGE_DEBUG)
        MESSAGE(STATUS "in file -->${CMAKE_CURRENT_LIST_FILE}<-- line -->${CMAKE_CURRENT_LIST_LINE}<-- message  ${MYTEXT}")
    ENDIF(MACRO_MESSAGE_DEBUG)
ENDMACRO(MACRO_MESSAGE MYTEXT)

MACRO(CREATE_LINK_LINES_FOR_TARGETS OUTVAR)
    FOREACH(varname ${ARGN})
        IF(${varname}_RELEASE)
           IF(${varname}_DEBUG)
               Set(${OUTVAR} ${${OUTVAR}} optimized "${${varname}_RELEASE}" debug "${${varname}_DEBUG}")
           ELSE(${varname}_DEBUG)
               Set(${OUTVAR} ${${OUTVAR}} optimized "${${varname}_RELEASE}" debug "${${varname}_RELEASE}" )
           ENDIF(${varname}_DEBUG)
        ELSE(${varname}_RELEASE)
           IF(${varname}_DEBUG)
               Set(${OUTVAR} ${${OUTVAR}} optimized "${${varname}}" debug "${${varname}_DEBUG}")
           ELSE(${varname}_DEBUG)
               Set(${OUTVAR} ${${OUTVAR}} optimized "${${varname}}" debug "${${varname}}" )
           ENDIF(${varname}_DEBUG)
        ENDIF(${varname}_RELEASE)
    ENDFOREACH(varname)
ENDMACRO(CREATE_LINK_LINES_FOR_TARGETS)

MACRO(LINK_WITH_VARIABLES TRGTNAME)
    FOREACH(varname ${ARGN})
      IF (WIN32)
        IF(${varname}_RELEASE)
           IF(${varname}_DEBUG)
               TARGET_LINK_LIBRARIES(${TRGTNAME} optimized "${${varname}_RELEASE}" debug "${${varname}_DEBUG}")
           ELSE(${varname}_DEBUG)
               TARGET_LINK_LIBRARIES(${TRGTNAME} optimized "${${varname}_RELEASE}" debug "${${varname}_RELEASE}" )
           ENDIF(${varname}_DEBUG)
        ELSE(${varname}_RELEASE)
           IF(${varname}_DEBUG)
               TARGET_LINK_LIBRARIES(${TRGTNAME} optimized "${${varname}}" debug "${${varname}_DEBUG}")
           ELSE(${varname}_DEBUG)
               TARGET_LINK_LIBRARIES(${TRGTNAME} optimized "${${varname}}" debug "${${varname}}" )
           ENDIF(${varname}_DEBUG)
        ENDIF(${varname}_RELEASE)
      ELSE (WIN32)
        IF(${varname}_RELEASE)
           TARGET_LINK_LIBRARIES(${TRGTNAME} "${${varname}_RELEASE}")
        ELSE(${varname}_RELEASE)
           TARGET_LINK_LIBRARIES(${TRGTNAME} "${${varname}}")
        ENDIF(${varname}_RELEASE)
      ENDIF (WIN32)
    ENDFOREACH(varname)
ENDMACRO(LINK_WITH_VARIABLES TRGTNAME)

# Try to ascertain the version...
MACRO(FIND_OSG_VERSION)
    IF(OSG_INCLUDE_DIR)

        SET(_osg_Version_file "${OSG_INCLUDE_DIR}/osg/Version")
        IF("${OSG_INCLUDE_DIR}" MATCHES "\\.framework$" AND NOT EXISTS "${_osg_Version_file}")
            SET(_osg_Version_file "${OSG_INCLUDE_DIR}/Headers/Version")
        ENDIF()

        IF(EXISTS "${_osg_Version_file}")
          FILE(READ "${_osg_Version_file}" _osg_Version_contents)
        ELSE()
          SET(_osg_Version_contents "unknown")
        ENDIF()

        STRING(REGEX MATCH ".*#define OSG_VERSION_MAJOR[ \t]+[0-9]+.*"
            _osg_old_defines "${_osg_Version_contents}")
        STRING(REGEX MATCH ".*#define OPENSCENEGRAPH_MAJOR_VERSION[ \t]+[0-9]+.*"
            _osg_new_defines "${_osg_Version_contents}")
        IF(_osg_old_defines)
            STRING(REGEX REPLACE ".*#define OSG_VERSION_MAJOR[ \t]+([0-9]+).*"
                "\\1" _osg_VERSION_MAJOR ${_osg_Version_contents})
            STRING(REGEX REPLACE ".*#define OSG_VERSION_MINOR[ \t]+([0-9]+).*"
                "\\1" _osg_VERSION_MINOR ${_osg_Version_contents})
            STRING(REGEX REPLACE ".*#define OSG_VERSION_PATCH[ \t]+([0-9]+).*"
                "\\1" _osg_VERSION_PATCH ${_osg_Version_contents})
        ELSEIF(_osg_new_defines)
            STRING(REGEX REPLACE ".*#define OPENSCENEGRAPH_MAJOR_VERSION[ \t]+([0-9]+).*"
                "\\1" _osg_VERSION_MAJOR ${_osg_Version_contents})
            STRING(REGEX REPLACE ".*#define OPENSCENEGRAPH_MINOR_VERSION[ \t]+([0-9]+).*"
                "\\1" _osg_VERSION_MINOR ${_osg_Version_contents})
            STRING(REGEX REPLACE ".*#define OPENSCENEGRAPH_PATCH_VERSION[ \t]+([0-9]+).*"
                "\\1" _osg_VERSION_PATCH ${_osg_Version_contents})
        ELSE()
            MESSAGE("[ FindOpenSceneGraph.cmake:${CMAKE_CURRENT_LIST_LINE} ] "
                "Failed to parse version number, please report this as a bug")
        ENDIF()

        SET(OSG_VERSION_MAJOR ${_osg_VERSION_MAJOR})
        SET(OSG_VERSION_MINOR ${_osg_VERSION_MINOR})
        SET(OSG_VERSION_PATCH ${_osg_VERSION_PATCH})

        SET(OPENSCENEGRAPH_VERSION "${_osg_VERSION_MAJOR}.${_osg_VERSION_MINOR}.${_osg_VERSION_PATCH}"
                                    CACHE INTERNAL "The version of OSG which was detected")
    ENDIF()
ENDMACRO(FIND_OSG_VERSION)


macro(ADD_DIRECTORY_TO_BUNDLE SOURCE_FILES_VAR DIR_SOURCE BUNDLE_TARGET)
  file(GLOB_RECURSE dataFiles RELATIVE ${DIR_SOURCE} ${DIR_SOURCE}/*)
  foreach(data_file ${dataFiles})
     #message("file: " ${DIR_SOURCE}/${data_file})
     get_filename_component(dirname ${data_file} PATH)
     LIST(APPEND ${SOURCE_FILES_VAR} ${DIR_SOURCE}/${data_file})
     SET_SOURCE_FILES_PROPERTIES(
        ${DIR_SOURCE}/${data_file}
        PROPERTIES
        MACOSX_PACKAGE_LOCATION ${BUNDLE_TARGET}/${dirname}
     )
  endforeach(data_file)
endmacro(ADD_DIRECTORY_TO_BUNDLE)


MACRO(DELTA3D_ADD_LIBRARY_PROPERTIES LIB_NAME EXPORT_SYMBOL LIBRARY_TYPE)
   if (MSVC_IDE AND LIBRARY_TYPE STREQUAL SHARED)
      SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL ${EXPORT_SYMBOL})
   endif ()
ENDMACRO()


MACRO(DELTA3D_ADD_LIBRARY LIB_NAME EXPORT_MACRO)


   if (NOT BUILD_STATIC)
      SET(LIBRARY_TYPE ${ARGV2})

      #MESSAGE(${LIB_NAME}"${ARGV2}")
      if (NOT LIBRARY_TYPE OR "${LIBRARY_TYPE}" STREQUAL "FRAMEWORK")
         SET(LIBRARY_TYPE SHARED)
         SET(DELTA3D_ALLOW_FRAMEWORK ON)
      else()
         SET(DELTA3D_ALLOW_FRAMEWORK OFF)
      endif()
   else()
      SET(LIBRARY_TYPE STATIC)
   endif()

   ADD_LIBRARY(${LIB_NAME} ${LIBRARY_TYPE}
       ${LIB_PUBLIC_HEADERS}
       ${LIB_SOURCES}
   )

   LINK_WITH_VARIABLES(${LIB_NAME}
                       ${LIB_EXTERNAL_DEPS})

   TARGET_LINK_LIBRARIES(${LIB_NAME}
                         ${LIB_DEPS}
                         )

   DELTA3D_ADD_LIBRARY_PROPERTIES(${LIB_NAME} ${EXPORT_MACRO} ${LIB_PUBLIC_HEADERS} ${LIBRARY_TYPE})

   INCLUDE(ModuleInstall OPTIONAL)

ENDMACRO()

MACRO(DELTA3D_ADD_PROGRAM APP_NAME)
   SET(APP_TYPE ${ARGV1})
   SET(EXPORT_MACRO ${ARGV2})


   if (APPLE)
      if (NOT PROG_QT_CONF_FILE)
         set(PROG_QT_CONF_FILE "${CMAKE_SOURCE_DIR}/CMakeModules/OSX/qt.conf")
      endif()

      if (NOT PROG_INFO_PLIST)
         set(PROG_INFO_PLIST "OSX/delta3dAppBundle.plist.in")
      endif()
    
      SET(apple_bundle_sources "${APP_NAME}.icns" ${PROG_QT_CONF_FILE})
      SET_SOURCE_FILES_PROPERTIES(
          ${apple_bundle_sources}
          PROPERTIES
          MACOSX_PACKAGE_LOCATION Resources
      )

      # add delta data to the bundle
      ADD_DIRECTORY_TO_BUNDLE(apple_bundle_sources ${CMAKE_SOURCE_DIR}/data Resources/deltaData)

      ADD_EXECUTABLE(${APP_NAME} ${APP_TYPE}
         ${PROG_HEADERS}
         ${PROG_SOURCES}
         ${MOC_SOURCES}
         ${RCC_SOURCES}
         ${UI_SOURCES}
         ${PROG_BUNDLE_SOURCES}
         ${apple_bundle_sources}
      )
      SET_TARGET_PROPERTIES(${APP_NAME} PROPERTIES
          MACOSX_BUNDLE_INFO_PLIST ${PROG_INFO_PLIST}
          MACOSX_BUNDLE_ICON_FILE ${APP_NAME}
          MACOSX_BUNDLE_INFO_STRING "${APP_NAME}"
          MACOSX_BUNDLE_GUI_IDENTIFIER "${APP_NAME}"
          MACOSX_BUNDLE_LONG_VERSION_STRING "${delta3d_VERSION_MAJOR}.${delta3d_VERSION_MINOR}.${delta3d_VERSION_PATCH}"
          MACOSX_BUNDLE_BUNDLE_NAME "${APP_NAME}"
          MACOSX_BUNDLE_SHORT_VERSION_STRING "${delta3d_VERSION_MAJOR}.${delta3d_VERSION_MINOR}"
          MACOSX_BUNDLE_BUNDLE_VERSION  1
          MACOSX_BUNDLE_COPYRIGHT "2014 CaperHoldings LLC.")
   else ()
      ADD_EXECUTABLE(${APP_NAME} ${APP_TYPE}
         ${PROG_HEADERS}
         ${PROG_SOURCES}
         ${MOC_SOURCES}
         ${RCC_SOURCES}
         ${UI_SOURCES}
      )
   endif ()


   LINK_WITH_VARIABLES(${APP_NAME}
                       ${APP_EXTERNAL_DEPS}
                      )

   TARGET_LINK_LIBRARIES(${APP_NAME}
                         ${APP_DEPS}
                        )

   if (EXPORT_MACRO)
      SET_TARGET_PROPERTIES(${APP_NAME}
                           PROPERTIES DEFINE_SYMBOL ${EXPORT_MACRO} 
      )
      # export symbols so plugins get access
      SET_TARGET_PROPERTIES(${APP_NAME} PROPERTIES ENABLE_EXPORTS true)
   endif()
   
   INCLUDE(ProgramInstall OPTIONAL)
   
ENDMACRO()


MACRO (SETUP_PLUGIN_WITH_OUTPUT_DIRS LIB_NAME EXPORT_MACRO SUBFOLDER)
   #put the binary into a "STAGE plugins" folder
   if (WIN32)
      # This design only makes sense on windows because there is no bin dir for libraries.
      SET(OUTPUT_BINDIR ${PROJECT_BINARY_DIR}/bin/${SUBFOLDER})
      SET(OUTPUT_LIBDIR ${PROJECT_BINARY_DIR}/lib)
   elseif (APPLE)
      SET(OUTPUT_LIBDIR ${PROJECT_BINARY_DIR}/bin/STAGE.app/Contents/PlugIns/${SUBFOLDER})
   else()
      SET(OUTPUT_LIBDIR ${PROJECT_BINARY_DIR}/lib/${SUBFOLDER})
   endif()

   SET (CMAKE_ARCHIVE_OUTPUT_DIRECTORY  ${OUTPUT_LIBDIR})
   SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${OUTPUT_BINDIR})
   IF(WIN32)
     SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${OUTPUT_BINDIR})
   ELSE(WIN32)
     SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_LIBDIR}) 
   ENDIF(WIN32)

   # For each configuration (Debug, Release, MinSizeRel... and/or anything the user chooses) 
   FOREACH(CONF ${CMAKE_CONFIGURATION_TYPES})
      # Go uppercase (DEBUG, RELEASE...)
      STRING(TOUPPER "${CONF}" CONF)
        
      #Handle the debug differently by putting the output into a debug subfolder
      IF (${CONF} STREQUAL "DEBUG")
        SET(DEBUG_FOLDER "Debug")
      ELSE (${CONF} STREQUAL "DEBUG")
        SET(DEBUG_FOLDER "")
      ENDIF (${CONF} STREQUAL "DEBUG")

      SET("CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONF}" "${OUTPUT_LIBDIR}")
      SET("CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONF}" "${OUTPUT_BINDIR}/${DEBUG_FOLDER}")
      IF(WIN32)
        SET("CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONF}" "${OUTPUT_BINDIR}/${DEBUG_FOLDER}")
      ELSE()
        SET("CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONF}" "${OUTPUT_LIBDIR}")
      ENDIF()
   ENDFOREACH()


   ADD_LIBRARY(${LIB_NAME} MODULE
       ${LIB_PUBLIC_HEADERS}
       ${LIB_SOURCES}
   )

   LINK_WITH_VARIABLES(${LIB_NAME}
                       ${LIB_EXTERNAL_DEPS})

   TARGET_LINK_LIBRARIES(${LIB_NAME}
                         ${LIB_DEPS}
                         )

   SET_TARGET_PROPERTIES(${LIB_NAME}
                        PROPERTIES DEFINE_SYMBOL ${EXPORT_MACRO})


   SET(INSTALL_INCDIR include)
   SET(INSTALL_BINDIR bin)
   IF(WIN32)
       SET(INSTALL_LIBDIR bin/${SUBFOLDER})
       SET(INSTALL_ARCHIVEDIR lib/${SUBFOLDER})
   ELSE(WIN32)
       SET(INSTALL_LIBDIR lib${LIB_POSTFIX}/${SUBFOLDER})
       SET(INSTALL_ARCHIVEDIR lib${LIB_POSTFIX}/${SUBFOLDER})
       SET(INSTALL_FRAMEWORKDIR Frameworks)
   ENDIF(WIN32)

   SET(HEADERS_GROUP "Header Files")

   SOURCE_GROUP(
       ${HEADERS_GROUP}
       FILES ${LIB_PUBLIC_HEADERS}
   )

   IF(MSVC AND OSG_MSVC_VERSIONED_DLL)
      HANDLE_MSVC_DLL()
   ENDIF(MSVC AND OSG_MSVC_VERSIONED_DLL)

   if (APPLE)
      set(CMAKE_INSTALL_RPATH "${DELTA3D_FRAMEWORK_INSTALL_NAME_DIR}")
      
      if (DELTA3D_DYLIB_INSTALL_NAME_DIR)
         SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES
            INSTALL_NAME_DIR "${DELTA3D_DYLIB_INSTALL_NAME_DIR}"
         )
      else()
         set(CMAKE_INSTALL_RPATH "${INSTALL_INCDIR}/${LIB_NAME}")
         SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES
            INSTALL_NAME_DIR "${INSTALL_INCDIR}/${LIB_NAME}"
         )
      endif()
   endif()
   
   INSTALL (
      FILES        ${LIB_PUBLIC_HEADERS}
      DESTINATION  ${INSTALL_INCDIR}/${LIB_NAME}
      COMPONENT delta3d-dev
   )
   if (MSVC)
      INSTALL(
        TARGETS ${LIB_NAME}
        CONFIGURATIONS Release RelWithDebInfo MinSizeRel
        RUNTIME DESTINATION ${INSTALL_BINDIR} COMPONENT delta3d
        LIBRARY DESTINATION ${INSTALL_LIBDIR} COMPONENT delta3d
        FRAMEWORK DESTINATION ${INSTALL_FRAMEWORKDIR} COMPONENT delta3d
        ARCHIVE DESTINATION ${INSTALL_ARCHIVEDIR} COMPONENT delta3d-dev
      )
      INSTALL(
        TARGETS ${LIB_NAME}
        CONFIGURATIONS Debug
        RUNTIME DESTINATION ${INSTALL_BINDIR}/Debug COMPONENT delta3d
        LIBRARY DESTINATION ${INSTALL_LIBDIR}/Debug COMPONENT delta3d
        FRAMEWORK DESTINATION ${INSTALL_FRAMEWORKDIR} COMPONENT delta3d
        ARCHIVE DESTINATION ${INSTALL_ARCHIVEDIR} COMPONENT delta3d-dev
      )
   else()
      INSTALL(
        TARGETS ${LIB_NAME}
        RUNTIME DESTINATION ${INSTALL_BINDIR} COMPONENT delta3d
        LIBRARY DESTINATION ${INSTALL_LIBDIR} COMPONENT delta3d
        FRAMEWORK DESTINATION ${INSTALL_FRAMEWORKDIR} COMPONENT delta3d
        ARCHIVE DESTINATION ${INSTALL_ARCHIVEDIR} COMPONENT delta3d-dev
      )
   endif()
   
   
   
   #make sure that the debug build is the only one that gets built in the debug folder.
   IF (MSVC)
     SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES DEBUG_OUTPUT_NAME ${LIB_NAME})
   ENDIF (MSVC)

ENDMACRO()

