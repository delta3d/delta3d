/**
 * @author ECS, Inc. ( Joseph Del Rocco )
 * @date 2006/07/07 - 10:54
 *
 * @file main.cpp
 * @version 1.0
 * @brief The main entry point file for the packager application.  Starts up the
 * PackageBuilder which contains the functionality for loading the package
 * profile and all the batch jobs.
 */

// local
#include "package_builder.h"
// ansi
#include <iostream>


//======================================
// CONSTANTS, MACROS, DEFINITIONS
//======================================

// prototypes
static void usage();


//======================================
// FUNCTIONS
//======================================

int main( int argc, const char **args )
{   
   // check number of arguments
   if(argc != 3)
   {
      usage();
      return 1;
   }
   
   // run builder
   PackageBuilder packager;
   packager.Initialize();
   packager.Build( std::string(args[1]), std::string(args[2]) );
   packager.Shutdown();

   return 0;
}

static void usage()
{
   std::cout << "\n"
   "Usage:\n"
   "    D3DWebPackager <package profile> <output file>\n\n"   
   "This program is designed to wrap a Delta3D simulation up into a web-deployable\n"
   "package such that is suitable for use by a SCORM-conforming LMS or a stand-\n"
   "alone web server.\n\n"
   "Options:\n"
   "    The first parameter is the location of the profile XML file you wish to use.\n"
   "    Please see Sample.xml for an example.\n"
   "    The second parameter is the output directory you wish to use."
   "This directory will be relative to the current working directory.\n"
   << std::endl;
}
