# Locate dtRender
#

INCLUDE(delta3d_common)
  
#variable names of the individual Delta3D libraries.  Can be used in application cmakelist.txt files.
FIND_DELTA3D_LIBRARY(DTVOXEL_LIBRARY         dtVoxel)

SET(DTVOXEL_INCLUDE_DIRECTORIES ${DELTA3D_INCLUDE_DIR} ${TBB_INCLUDE_DIR})

set(SUB_PACKAGES TBB OpenEXR OpenVDB)

foreach(package ${SUB_PACKAGES})
   if (FIND_dtVoxel_REQUIRED)
      find_package(${package} REQUIRED)
   elseif(FIND_dtVoxel_QUIETLY)
      find_package(${package} QUIETLY)
   else()
      find_package(${package})
   endif()
endforeach()


CREATE_LINK_LINES_FOR_TARGETS(DTVOXEL_LIBRARIES
        DTVOXEL_LIBRARY
        )
        
list(APPEND DTVOXEL_LIBRARIES    ${TBB_LIBRARIES} ${OPENEXR_Half_LIBRARY} ${OPENVDB_LIBRARIES})

# handle the QUIETLY and REQUIRED arguments and set DELTA3D_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(dtVoxel DEFAULT_MSG DELTA3D_INCLUDE_DIR  DTVOXEL_LIBRARY )
