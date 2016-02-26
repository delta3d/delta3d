include(UtilityMacros)

IF(APPLE)
   OPTION(BUILD_FOR_IOS OFF)

   #SET(CMAKE_SHARED_LINKER_FLAGS "-Wl,-single_module -compatibility_version 4 -current_version 4 -F/Library/Frameworks" )
   #SET(CMAKE_MODULE_LINKER_FLAGS "-F/Library/Frameworks" )
   SET(CMAKE_INSTALL_NAME_DIR "@executable_path/../lib" CACHE STRING "install name dir for compiled frameworks")
   SET(CMAKE_C_FLAGS "-pipe -Wnewline-eof")

   SET(OUTPUT_FRAMEWORK_DIR ${PROJECT_BINARY_DIR}/Frameworks)
   MAKE_DIRECTORY(${OUTPUT_FRAMEWORK_DIR})
   SET(CMAKE_FRAMEWORK_OUTPUT_DIRECTORY ${OUTPUT_FRAMEWORK_DIR} CACHE PATH "Framework Build Directory")

   if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pipe -Wall -Wextra -Wnewline-eof")

      if (NOT CMAKE_OSX_ARCHITECTURES)
         set(CMAKE_OSX_ARCHITECTURES "i386;x86_64" CACHE STRING "Build architectures for OSX" FORCE)
      endif()

      if (NOT CMAKE_OSX_SYSROOT)
         set(CMAKE_OSX_SYSROOT "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.8.sdk" CACHE STRING "Build SDK root" FORCE )
      endif()


      SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -stdlib=libc++" )
      SET(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -stdlib=libc++" )
   else()
      #TODO update for new g++ now installed with xcode.
      READ_GCC_VERSION()
      SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wnewline-eof -pipe")

      if (NOT CMAKE_OSX_ARCHITECTURES)
         set(CMAKE_OSX_ARCHITECTURES "i386" CACHE STRING "Build architectures for OSX" FORCE)
      endif()

   endif()

   if (NOT CMAKE_OSX_DEPLOYMENT_TARGET)
      # 10.8 is probably a good choice since it's pretty old.
      set(CMAKE_OSX_DEPLOYMENT_TARGET "10.8" CACHE STRING "Build SDK root" FORCE)
   endif()

   FIND_LIBRARY(APPSERVICES_LIBRARY ApplicationServices)
   MARK_AS_ADVANCED(APPSERVICES_LIBRARY)

   FIND_LIBRARY(IOKIT_LIBRARY IOKit)
   MARK_AS_ADVANCED(IOKIT_LIBRARY)

   FIND_LIBRARY(COREFOUNDATION_LIBRARY CoreFoundation)
   MARK_AS_ADVANCED(COREFOUNDATION_LIBRARY)

   FIND_LIBRARY(COCOA_LIBRARY Cocoa)
   MARK_AS_ADVANCED(COCOA_LIBRARY)

   #SET(CMAKE_EXE_LINKER_FLAGS "-F/Library/Frameworks" )
   SET(CMAKE_SHARED_LINKER_FLAGS "-Wl,-single_module -compatibility_version 4 -current_version 4" )
   #SET(CMAKE_MODULE_LINKER_FLAGS "-F/Library/Frameworks" )
   INCLUDE_DIRECTORIES(${CMAKE_OSX_SYSROOT}/usr/include/malloc)

   OPTION(DELTA3D_COMPILE_FRAMEWORKS "compile frameworks instead of dylibs" ON)
   SET(DELTA3D_FRAMEWORK_INSTALL_NAME_DIR "@executable_path/../Frameworks" CACHE STRING "install name dir for compiled frameworks")
   SET(DELTA3D_DYLIB_INSTALL_NAME_DIR "@executable_path/../lib" CACHE STRING "install name dir for compiled dylibs and bundles")
   SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE CACHE STRING "Build with install names and rpaths set.")
   
elseif(UNIX)
   READ_GCC_VERSION()
   FIND_PACKAGE(X11)
   FIND_LIBRARY(XXF86VM_LIBRARY Xxf86vm)
   MARK_AS_ADVANCED(XXF86VM_LIBRARY)

   SET(X11_LIBRARIES
     ${X11_LIBRARIES}
     ${XXF86VM_LIBRARY})
   # Some Unicies need explicit linkage to the Math library or the build fails.
   FIND_LIBRARY(MATH_LIBRARY m)
   MARK_AS_ADVANCED(MATH_LIBRARY)

   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-non-virtual-dtor -Wreturn-type")
endif()

# Common global definitions

# Platform specific definitions

IF(WIN32)
   ADD_DEFINITIONS(-D_SCL_SECURE_NO_WARNINGS)
   ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE)
   #needed for net plugin
   SET (DELTA3D_SOCKET_LIBS wsock32)
   ADD_DEFINITIONS(-DNOMINMAX)

   IF (WIN32)
	   OPTION(BUILD_WITH_MP "Enables the /MP multi-processor compiler option for Visual Studio 2005 and above" ON)
	   MARK_AS_ADVANCED(BUILD_WITH_MP)
	   IF(BUILD_WITH_MP)
	     SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
	   ENDIF(BUILD_WITH_MP)
	   SET(CMAKE_DEBUG_POSTFIX  "d") 
   ENDIF (WIN32)


   IF(MSVC)
      IF(${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} EQUAL 4 AND ${CMAKE_PATCH_VERSION} LESS 7)
         MESSAGE("Warning:  disabling versioned options 2.4.6 exibits inconsintencies in .pdb naming, at least under MSVC, suggested upgrading at least to 2.4.7")
         SET(DELTA3D_MSVC_VERSIONED_DLL OFF)
         SET(DELTA3D_MSVC_DEBUG_INCREMENTAL_LINK ON)
      ELSE(${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} EQUAL 4 AND ${CMAKE_PATCH_VERSION} LESS 7)
         OPTION(DELTA3D_MSVC_VERSIONED_DLL "Set to ON to build OpenSceneGraph with versioned dll names" ON)
         MARK_AS_ADVANCED(DELTA3D_MSVC_VERSIONED_DLL)
         OPTION(DELTA3D_MSVC_DEBUG_INCREMENTAL_LINK "Set to OFF to build OpenSceneGraph without incremental linking in debug (release is off by default)" ON)
         MARK_AS_ADVANCED(DELTA3D_MSVC_DEBUG_INCREMENTAL_LINK)
         IF(NOT DELTA3D_MSVC_DEBUG_INCREMENTAL_LINK)
            SET(CMAKE_MODULE_LINKER_FLAGS_DEBUG "/debug /INCREMENTAL:NO")
            SET(CMAKE_SHARED_LINKER_FLAGS_DEBUG "/debug /INCREMENTAL:NO")
            SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "/debug /INCREMENTAL:NO")
         ENDIF(NOT DELTA3D_MSVC_DEBUG_INCREMENTAL_LINK)
      ENDIF(${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} EQUAL 4 AND ${CMAKE_PATCH_VERSION} LESS 7)
   ENDIF(MSVC)
ENDIF(WIN32)


IF("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
   FILE(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin ${CMAKE_BINARY_DIR}/lib )
ENDIF("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")


SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
    
SET(OUTPUT_BINDIR ${PROJECT_BINARY_DIR}/bin)
MAKE_DIRECTORY(${OUTPUT_BINDIR})
SET(EXECUTABLE_OUTPUT_PATH ${OUTPUT_BINDIR})

SET(OUTPUT_LIBDIR ${PROJECT_BINARY_DIR}/lib)
MAKE_DIRECTORY(${OUTPUT_LIBDIR})
SET(LIBRARY_OUTPUT_PATH ${OUTPUT_LIBDIR})

SET (CMAKE_ARCHIVE_OUTPUT_DIRECTORY  ${OUTPUT_LIBDIR} CACHE PATH "build directory")
SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${OUTPUT_BINDIR} CACHE PATH "build directory")
IF(MSVC_IDE)
  SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${OUTPUT_BINDIR} CACHE PATH "build directory")
ELSE(MSVC_IDE)
  SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_LIBDIR} CACHE PATH "build directory")
ENDIF(MSVC_IDE)


# For each configuration (Debug, Release, MinSizeRel... and/or anything the
# user chooses)
FOREACH(CONF ${CMAKE_CONFIGURATION_TYPES})
  # Go uppercase (DEBUG, RELEASE...)
  STRING(TOUPPER "${CONF}" CONF)
  SET("CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONF}" "${OUTPUT_LIBDIR}")
  SET("CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONF}" "${OUTPUT_BINDIR}")
  IF(MSVC_IDE)
    SET("CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONF}" "${OUTPUT_BINDIR}")
  ELSE()
    SET("CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONF}" "${OUTPUT_LIBDIR}")
  ENDIF()
ENDFOREACH()


# Expose CMAKE_INCLUDE_PATH and CMAKE_LIBARY_PATH to the GUI so users
# may set these values without needing to manipulate the environment.
SET(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} CACHE STRING "You may add additional search paths here. Use ; to separate multiple paths.")
SET(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} CACHE STRING "You may add additional search paths here. Use ; to separate multiple paths.")
SET(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} CACHE STRING "You may add additional search paths here. Use ; to separate multiple paths.")
