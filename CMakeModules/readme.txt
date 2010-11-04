The FinddtX.cmake files are helpful to developers who are creating applications 
using the Delta3D libraries and wish to utilize CMake's ability to create
the project files.

Each FinddtX.cmake file can be used to include that particular library's
dependency to an application that uses that library.  In some cases,
additional dependencies will be searched for (the Delta3D External Dependencies).
If these external dependencies cannot be found, then the Delta3D library will
be considered NOT_FOUND as well.

The FinddtX.cmake files will attempt to find the debug and release versions of
the Delta3D libraries.  If no debug version is found, then cmake will generate a 
statement and proceed.

Each FinddtX.cmake file will define the following:
DTX_LIBRARY : The full path to the release configuration of this library
DTX_LIBRARY_DEBUG : The full path to the debug configuration of this library.
                    If none found, then the release version will be used.
DTX_INCLUDE_DIRECTORIES : List of full paths which contain the header files
                          for this library (and external depencencies in some 
                          cases).  This can be used in CMakeList.txt files.
DTX_LIBRARIES : List of full paths to the library files for this library.
                This defines both optimized and debug libraries, as well as
                external dependencies for this library, if appropriate.
                This can be used in CMakeList.txt files. 


***Example***
To use these files in an external application, add something like
the following to a MakeLists.txt file:

  PROJECT(MyApp)
  
  FIND_PACKAGE(dtCore REQUIRED)
  
  INCLUDE_DIRECTORIES(${DTCORE_INCLUDE_DIRECTORIES})
  
  ADD_EXECUTABLE(MyApp main.cpp)
  
  TARGET_LINK_LIBRARIES(MyApp ${DTCORE_LIBRARIES})


Note, ${DTCORE_LIBRARIES} will contain dtCore library definitions as well as it's
external dependency libraries.
