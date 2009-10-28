# This defines the follow variables
# DELTA3D_ROOT : The root of the Delta3D directory
# DELTA3D_INCLUDE_DIR : The Delta3D include directory
# DELTA3D_EXT_DIR : The directory that contains the Delta3D external dependency
#
# This folder contains some functions which helps find the indidivual parts of Delta3D
# and is typically included by other .cmake files.

#where to find the root Delta3D folder
FIND_PATH(DELTA3D_ROOT src
          PATHS
          $ENV{DELTA_ROOT}
          DOC "The root folder of Delta3D"
          )

#where to find the Delta3D include dir
FIND_PATH(DELTA3D_INCLUDE_DIR dtCore/dt.h
          PATHS
         ${DELTA3D_ROOT}/inc
         $ENV{DELTA_ROOT}/inc
         /usr/local/include
         /usr/freeware/include     
         DOC "The Delta3D include folder. Should contain 'dtCore', 'dtUtil', 'dtABC',..."
)

#where to find the Delta3D "ext" folder
FIND_PATH(DELTA3D_EXT_DIR inc/al.h
          PATHS
         ${DELTA3D_ROOT}/ext
         $ENV{DELTA_ROOT}/ext
         DOC "The root of the Delta3D external dependency folder"
)

#where to find the Delta3D lib dir
SET(DELTA3D_LIB_SEARCH_PATH 
             ${DELTA3D_ROOT}/lib
             ${DELTA3D_ROOT}/build/lib
             $ENV{DELTA_LIB}
             $ENV{DELTA_ROOT}/lib
             $ENV{DELTA_ROOT}/build/lib
             /usr/local/lib
             /usr/lib
)

MACRO(FIND_DELTA3D_LIBRARY LIB_VAR LIB_NAME)
  FIND_LIBRARY(${LIB_VAR} NAMES ${LIB_NAME}
               PATHS
               ${DELTA3D_LIB_SEARCH_PATH}
              )
ENDMACRO(FIND_DELTA3D_LIBRARY LIB_VAR LIB_NAME)            



#
# DELTA_FIND_PATH
#
function(DELTA3D_FIND_PATH module header)
   string(TOUPPER ${module} module_uc)

   # Try the user's environment request before anything else.
   find_path(${module_uc}_INCLUDE_DIR ${header}
       HINTS
            $ENV{${module_uc}_DIR}
            $ENV{DELTA_ROOT}
       PATH_SUFFIXES include inc
       PATHS
            /sw # Fink
            /opt/local # DarwinPorts
            /opt/csw # Blastwave
            /opt
            /usr/freeware
            /usr
            /usr/include
            /usr/local/include
            ${DELTA3D_EXT_DIR} #defined in delta3d_common.cmake
   )
endfunction(DELTA3D_FIND_PATH module header)


#
# DELTA3D_FIND_LIBRARY
#
function(DELTA3D_FIND_LIBRARY module library)
   string(TOUPPER ${module} module_uc)
   find_library(${module_uc}_LIBRARY
       NAMES ${library}
       HINTS
            $ENV{${module_uc}_DIR}
            $ENV{DELTA_ROOT}
       PATH_SUFFIXES lib64 lib
       PATHS
            /sw # Fink
            /opt/local # DarwinPorts
            /opt/csw # Blastwave
            /opt
            /usr/freeware
            /usr/lib
            ${DELTA3D_EXT_DIR}/lib
            ${DELTA3D_EXT_DIR}/lib64
            $ENV{DELTA_ROOT}/ext/lib
            $ENV{DELTA_ROOT}/ext/lib64
   )

   #Modify each entry to tack on "d" and "_d" for the debug file name
   FOREACH(debug_lib ${library})
     LIST(APPEND debug_list ${debug_lib}d ${debug_lib}_d ${debug_lib}_debug)
   ENDFOREACH(debug_lib ${library})
    
   find_library(${module_uc}_LIBRARY_DEBUG
       NAMES ${debug_list}
       HINTS
            $ENV{${module_uc}_DIR}
            $ENV{DELTA_ROOT}
       PATH_SUFFIXES lib64 lib
       PATHS
            /sw # Fink
            /opt/local # DarwinPorts
            /opt/csw # Blastwave
            /opt
            /usr/freeware
            /usr/lib
            ${DELTA3D_EXT_DIR}/lib
            ${DELTA3D_EXT_DIR}/lib64
            $ENV{DELTA_ROOT}/ext/lib
            $ENV{DELTA_ROOT}/ext/lib64            
    )

   if(NOT ${module_uc}_LIBRARY_DEBUG)
      # They don't have a debug library
      set(${module_uc}_LIBRARY_DEBUG ${${module_uc}_LIBRARY} PARENT_SCOPE)
      set(${module_uc}_LIBRARIES ${${module_uc}_LIBRARY} PARENT_SCOPE)
      MESSAGE(STATUS "No debug library was found for ${module_uc}_LIBRARY_DEBUG")
   else()
      # They really have a FOO_LIBRARY_DEBUG
      set(${module_uc}_LIBRARIES 
          optimized ${${module_uc}_LIBRARY}
          debug ${${module_uc}_LIBRARY_DEBUG}
          PARENT_SCOPE
      )
   endif()
endfunction(DELTA3D_FIND_LIBRARY module library)
