IF(APPLE)
   OPTION(BUILD_FOR_IOS OFF)

   SET(CMAKE_SHARED_LINKER_FLAGS "-Wl,-single_module -compatibility_version 4 -current_version 4 -F/Library/Frameworks" )
   SET(CMAKE_MODULE_LINKER_FLAGS "-F/Library/Frameworks" )
   SET(CMAKE_INSTALL_NAME_DIR "@executable_path/../lib" CACHE STRING "install name dir for compiled frameworks")
   SET(CMAKE_C_FLAGS "-pipe -Wnewline-eof")

   if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pipe -stdlib=libc++ -std=c++11 -Wall -Wextra -Wnewline-eof")

      if (NOT CMAKE_OSX_ARCHITECTURES)
         set(CMAKE_OSX_ARCHITECTURES "i386;x86_64" CACHE STRING "Build architectures for OSX" FORCE)
      endif()

      if (NOT CMAKE_OSX_SYSROOT)
         set(CMAKE_OSX_SYSROOT "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.8.sdk" CACHE STRING "Build SDK root" FORCE )
      endif()

      if (NOT CMAKE_OSX_DEPLOYMENT_TARGET)
         # use 10.7 because it's pretty old now.  10.8 is probably a good choice now that Mavericks is out.
         set(CMAKE_OSX_DEPLOYMENT_TARGET "10.7" CACHE STRING "Build SDK root" FORCE)
      endif()

      SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -stdlib=libc++" )
      SET(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -stdlib=libc++" )
   else()
      READ_GCC_VERSION()
      SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wnewline-eof -pipe")

      if (NOT CMAKE_OSX_ARCHITECTURES)
         set(CMAKE_OSX_ARCHITECTURES "i386" CACHE STRING "Build architectures for OSX" FORCE)
      endif()

      if (NOT CMAKE_OSX_SYSROOT)
         set(CMAKE_OSX_SYSROOT "/Developer/SDKs/MacOSX10.6.sdk" CACHE STRING "Build SDK root" FORCE )
      endif()

      if (NOT CMAKE_OSX_DEPLOYMENT_TARGET)
         # use 10.6 because that's the oldest that has atomics
         set(CMAKE_OSX_DEPLOYMENT_TARGET "10.6" CACHE STRING "Build SDK root" FORCE)

         # this is an ugly hack to fix what seems to be a problem on OS X 10.8
         # when building with the 10.6 sdk.  Unfortunatly, this will break if
         # building as x86_64.  Need to find a better solution for this.  Maybe
         # they'll just fix the bug.  I have to build with the 10.6 sdk because of
         # cegui, which also forces the 32 bit problem :-/
         INCLUDE_DIRECTORIES(${CMAKE_OSX_SYSROOT}/usr/include/c++/4.2.1/i686-apple-darwin10)
      endif()


   endif()


   FIND_LIBRARY(APPSERVICES_LIBRARY ApplicationServices)
   FIND_LIBRARY(IOKIT_LIBRARY IOKit)
   FIND_LIBRARY(COREFOUNDATION_LIBRARY CoreFoundation)
   FIND_LIBRARY(COCOA_LIBRARY Cocoa)

   SET(CMAKE_EXE_LINKER_FLAGS "-F/Library/Frameworks" )
   SET(CMAKE_SHARED_LINKER_FLAGS "-Wl,-single_module -compatibility_version 4 -current_version 4 -F/Library/Frameworks" )
   SET(CMAKE_MODULE_LINKER_FLAGS "-F/Library/Frameworks" )
   INCLUDE_DIRECTORIES(${CMAKE_OSX_SYSROOT}/usr/include/malloc)
elseif(UNIX)
   READ_GCC_VERSION()
   FIND_PACKAGE(X11)
   FIND_LIBRARY(XXF86VM_LIBRARY Xxf86vm)
   SET(X11_LIBRARIES
     ${X11_LIBRARIES}
     ${XXF86VM_LIBRARY})
   # Some Unicies need explicit linkage to the Math library or the build fails.
   FIND_LIBRARY(MATH_LIBRARY m)

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
