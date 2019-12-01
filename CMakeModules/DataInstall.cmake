
file(GLOB DATA_FILES ${CMAKE_SOURCE_DIR}/data/*.xsd ${CMAKE_SOURCE_DIR}/data/*.config)

file(GLOB DATA_FILES_FONTS ${CMAKE_SOURCE_DIR}/data/fonts/*.ttf ${CMAKE_SOURCE_DIR}/data/fonts/*.txt)

file(GLOB DATA_FILES_SHADERS ${CMAKE_SOURCE_DIR}/data/shaders/*.frag ${CMAKE_SOURCE_DIR}/data/shaders/*.vert)

file(GLOB DATA_FILES_FED ${CMAKE_SOURCE_DIR}/data/Federations/*.xml)

INSTALL (
   FILES        ${DATA_FILES}
   DESTINATION  data
   COMPONENT delta3d
)

INSTALL (
   FILES        ${DATA_FILES_FONTS}
   DESTINATION  data/fonts
   COMPONENT delta3d
)

INSTALL (
   FILES        ${DATA_FILES_SHADERS}
   DESTINATION  data/shaders
   COMPONENT delta3d
)

INSTALL (
   FILES        ${DATA_FILES_FED}
   DESTINATION  data/Federations
   COMPONENT delta3d
)

INSTALL (
   DIRECTORY    ${CMAKE_SOURCE_DIR}/demos/fireFighter/FireFighterProject
   DESTINATION  demos/FireFighter
   COMPONENT    delta3d-demos
   PATTERN .svn EXCLUDE
   PATTERN  *~  EXCLUDE
)

INSTALL (
   DIRECTORY    ${CMAKE_SOURCE_DIR}/examples/data
   DESTINATION  examples
   COMPONENT    delta3d-demos
   PATTERN .svn EXCLUDE
   PATTERN  *~  EXCLUDE
)

file(GLOB EXAMPLE_DIRS ${CMAKE_SOURCE_DIR}/examples/test*)
     
foreach(dir ${EXAMPLE_DIRS})
   #message("example: " ${dir})

   file(GLOB EX_FILES ${dir}/*.xml 
      ${dir}/readme.txt 
      ${dir}/*.py )

   if (EX_FILES)
      get_filename_component(strippedDir ${dir} NAME)
      
      INSTALL (
         FILES        ${EX_FILES}
         DESTINATION  examples/${strippedDir}
         COMPONENT    delta3d-examples
      )
   endif()
endforeach()

INSTALL (
   DIRECTORY    ${CMAKE_SOURCE_DIR}/examples/testAAR/AARProject
   DESTINATION  examples/testAAR
   COMPONENT    delta3d-examples
   PATTERN .svn EXCLUDE
   PATTERN  *~  EXCLUDE
)

file(GLOB CMAKE_MODULE_FILES "${CMAKE_SOURCE_DIR}/CMakeModules/*.cmake" "${CMAKE_SOURCE_DIR}/CMakeModules/*.cmake.in")


if("${CMAKE_SYSTEM}" MATCHES "Linux")
   INSTALL (
      FILES    ${CMAKE_MODULE_FILES}
      DESTINATION  share/cmake/Modules
      COMPONENT    delta3d
   )
else ()
   INSTALL (
      FILES    ${CMAKE_MODULE_FILES}
      DESTINATION  CMakeModules
      COMPONENT    delta3d
   )
endif()

if (BUILD_HLA)
   INSTALL (
      DIRECTORY    ${CMAKE_SOURCE_DIR}/RTIPlugins
      DESTINATION  .
      COMPONENT    delta3d-rtiplugin-src
   )
endif()
