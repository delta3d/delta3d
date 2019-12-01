#Build a Delta3D console app executable.
#Expects files to be compiled in TARGET_SRC and TARGET_H.
#Supply the target name as the first parameter.  Additional
#parameters will be passed to LINK_WITH_VARIABLES
include(UtilityMacros)

function(BUILD_EXE_EXAMPLE TGTNAME)
  
   if (APPLE)
      if (NOT PROG_QT_CONF_FILE)
         set(PROG_QT_CONF_FILE "${CMAKE_SOURCE_DIR}/CMakeModules/OSX/qt.conf")
      endif()
    
      SET(apple_bundle_sources "${CMAKE_SOURCE_DIR}/CMakeModules/OSX/Example.icns" ${PROG_QT_CONF_FILE})
 
      SET_SOURCE_FILES_PROPERTIES(
       ${apple_bundle_sources}
       PROPERTIES
       MACOSX_PACKAGE_LOCATION Resources
      )
      if (NOT PROG_CONFIG_FILE AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/config.xml)
         set(PROG_CONFIG_FILE config.xml)
      endif()
      
      if (PROG_CONFIG_FILE)
         LIST(APPEND apple_bundle_sources ${PROG_CONFIG_FILE})
         SET_SOURCE_FILES_PROPERTIES(
            ${PROG_CONFIG_FILE}
             PROPERTIES
             MACOSX_PACKAGE_LOCATION Resources
          )
         ADD_DIRECTORY_TO_BUNDLE(apple_bundle_sources ${CMAKE_SOURCE_DIR}/data Resources/deltaData)
      endif()
   
      ADD_EXECUTABLE(${TGTNAME} MACOSX_BUNDLE
          ${TARGET_SRC} ${TARGET_H}
          ${apple_bundle_sources}
      )
            
      SET_TARGET_PROPERTIES(${TGTNAME} PROPERTIES 
         MACOSX_BUNDLE_INFO_PLIST OSX/delta3dAppBundle.plist.in
         MACOSX_BUNDLE_ICON_FILE Example
         MACOSX_BUNDLE_INFO_STRING "delta3d Example"
         MACOSX_BUNDLE_GUI_IDENTIFIER "delta3d Example"
         MACOSX_BUNDLE_LONG_VERSION_STRING "${delta3d_VERSION_MAJOR}.${delta3d_VERSION_MINOR}.${delta3d_VERSION_PATCH}"
         MACOSX_BUNDLE_BUNDLE_NAME "${TGTNAME}"
         MACOSX_BUNDLE_SHORT_VERSION_STRING "${delta3d_VERSION_MAJOR}.${delta3d_VERSION_MINOR}"
         MACOSX_BUNDLE_BUNDLE_VERSION  1
         MACOSX_BUNDLE_COPYRIGHT "2014 CaperHoldings LLC.")
   else ()
      ADD_EXECUTABLE(${TGTNAME} ${TARGET_SRC} ${TARGET_H})
   endif ()
  
  FOREACH(varname ${ARGN})
      TARGET_LINK_LIBRARIES( ${TGTNAME} ${varname} )
  ENDFOREACH(varname)
    
  IF (MSVC)
    SET_TARGET_PROPERTIES(${TGTNAME} PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")
  ENDIF (MSVC)
  
  # needed for install
  SET(TGT_NAME ${TGTNAME})
  
  INCLUDE(ExampleInstall OPTIONAL)
  
endfunction(BUILD_EXE_EXAMPLE)

MACRO(BUILD_LIB_EXAMPLE TGTNAME ExportMacro)
   ADD_LIBRARY(${TGTNAME} SHARED ${TARGET_SRC} ${TARGET_H})
  
   FOREACH(varname ${ARGN})
      TARGET_LINK_LIBRARIES( ${TGTNAME} ${varname} )
   ENDFOREACH(varname)
    
   SET_TARGET_PROPERTIES(${TGTNAME}
                     PROPERTIES DEFINE_SYMBOL ${ExportMacro})

   IF (MSVC)
     SET_TARGET_PROPERTIES(${TGTNAME} PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")
   ENDIF (MSVC)
  
   # needed for install
   SET(TGT_NAME ${TGTNAME})

   if (NOT PROG_CONFIG_FILE AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/config.xml)
      set(PROG_CONFIG_FILE config.xml)
   endif()

   BUILD_GAME_START(${TGTNAME} ON)
   
   INCLUDE(ExampleInstall OPTIONAL)

ENDMACRO(BUILD_LIB_EXAMPLE)
