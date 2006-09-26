/**
 * @author ECS, Inc. ( Joseph Del Rocco )
 * @date 2006/07/07 - 11:33
 *
 * @file job_launchconfig.cpp
 * @version 1.0
 */

// local
#include "job_launchconfig.h"
#include "package_profile.h"
#include "package_utils.h"
// ansi
#include <iostream>
#include <cctype> //for toupper()


//======================================
// CONSTANTS, MACROS, DEFINITIONS
//======================================

// forward refs
class PackageProfile;


//======================================
// PUBLIC FUNCTIONS
//======================================

JobLaunchConfig::JobLaunchConfig() : JobXML()
{
}

JobLaunchConfig::~JobLaunchConfig()
{ 
}

void JobLaunchConfig::Execute( PackageProfile *profile )
{
   XMLNode *root = CreateDocument( "javaLaunch" );
   XMLNode *curr = root;

   // locals
   std::string app    = profile->GetApplicationName();
   std::string config = profile->GetJavaLaunchConfig();      
   
   // used for searching
   std::vector<std::string> files;
   std::vector<Options>     options;

   // app information
   AddElement( root, "applicationName", app );
   curr = AddElement( root, "loggingLevel" );
   SetContent( curr, "WARNING" );
   //AddComment( curr, " Debug logging level. " );
   //AddComment( curr, " Options: ALL, CONFIG, FINE, FINER, FINEST, INFO, OFF, SEVERE, WARNING " );

   // data information   
   AddElement( root, "dataDirectory", "data" );
   curr = AddElement( root, "dataOverwrite" );
   SetContent( curr, "NEWER" );
   //AddComment( curr, " Should data files be overwritten on client when extracting? " );
   //AddComment( curr, " Options: ALL, NONE, NEWER " );
   curr = AddElement( root, "dataExtractFiles" );
   profile->GetJARFiles( PackageProfile::JAR_DATA, files, options );
   for ( unsigned int i=0; i<files.size(); i++ )
   {
      AddElement( curr, "dataExtractFile", files[i] );
   }

   // lib information
   AddElement( root, "libraryDirectory", "bin" );

   std::string libname;
   Options libopts;

   // get windows launcher library
   if ( profile->IncludePlatform(PackageProfile::TargetPlatform::WINDOWS) )
   {
      profile->GetEntryPointLibrary( PackageProfile::LIB_LAUNCHER, libname, libopts, PackageProfile::TargetPlatform::WINDOWS );
      curr = AddElement( root, "launcherLibrary", libname );
      SetAttribute( curr, "platform", libopts.Get( "platform" ) );
   }

   // get linux launcher library
   if ( profile->IncludePlatform(PackageProfile::TargetPlatform::LINUX) )
   {
      profile->GetEntryPointLibrary( PackageProfile::LIB_LAUNCHER, libname, libopts, PackageProfile::TargetPlatform::LINUX );
      curr = AddElement( root, "launcherLibrary", libname );
      SetAttribute( curr, "platform", libopts.Get( "platform" ) );
   }

   // get windows game library
   if ( profile->IncludePlatform(PackageProfile::TargetPlatform::WINDOWS) )
   {
      profile->GetEntryPointLibrary( PackageProfile::LIB_GAME, libname, libopts, PackageProfile::TargetPlatform::WINDOWS );

      if ( libopts.Get( "removeDebugChar" ).compare( "true" ) == 0 )
         removeDebugCharFromFilename( libname );

      removeLibAndExtension( libname );
      curr = AddElement( root, "gameLibrary", libname );
      SetAttribute( curr, "platform", libopts.Get( "platform" ) );
   }

   // get linux game library
   if ( profile->IncludePlatform(PackageProfile::TargetPlatform::LINUX) )
   {
      profile->GetEntryPointLibrary( PackageProfile::LIB_GAME, libname, libopts, PackageProfile::TargetPlatform::LINUX );

      if ( libopts.Get( "removeDebugChar" ).compare( "true" ) == 0 )
         removeDebugCharFromFilename( libname );

      removeLibAndExtension( libname );
      curr = AddElement( root, "gameLibrary", libname );
      SetAttribute( curr, "platform", libopts.Get( "platform" ) );
   }

   // general launch parameters
   std::string launchParams = profile->GetApplicationLaunchParams();
   AddElement( root, "generalLaunchParameters", launchParams );

   // dependency libraries
   XMLNode *libs = AddElement( root, "codeLibraries" );

   // windows dependency libraries
   if ( profile->IncludePlatform(PackageProfile::TargetPlatform::WINDOWS) )
   {
      profile->GetLibraries( PackageProfile::LIB_DEPENDENCY, files, options, PackageProfile::TargetPlatform::WINDOWS );
      for ( unsigned int i=0; i<files.size(); i++ )
      {
         curr = AddElement( libs, "library", files[i] );
         SetAttribute( curr, "platform", options[i].Get( "platform" ) );
      }
   }

   // linux dependency libraries
   if ( profile->IncludePlatform(PackageProfile::TargetPlatform::LINUX) )
   {
      profile->GetLibraries( PackageProfile::LIB_DEPENDENCY, files, options, PackageProfile::TargetPlatform::LINUX );
      for ( unsigned int i=0; i<files.size(); i++ )
      {
         curr = AddElement( libs, "library", files[i] );
         SetAttribute( curr, "platform", options[i].Get( "platform" ) );
      }
   }

   // the windows game library also needs to be tacked on to the resource libraries
   if ( profile->IncludePlatform(PackageProfile::TargetPlatform::WINDOWS) )
   {
      profile->GetEntryPointLibrary( PackageProfile::LIB_GAME, libname, libopts, PackageProfile::TargetPlatform::WINDOWS );
      curr = AddElement( libs, "library", libname );
      SetAttribute( curr, "platform", libopts.Get( "platform" ) );
   }

   // the linux game library also needs to be tacked on to the resource libraries
   if ( profile->IncludePlatform(PackageProfile::TargetPlatform::LINUX) )
   {
      profile->GetEntryPointLibrary( PackageProfile::LIB_GAME, libname, libopts, PackageProfile::TargetPlatform::LINUX );
      curr = AddElement( libs, "library", libname );
      SetAttribute( curr, "platform", libopts.Get( "platform" ) );
   }

   // SCORM specifications
   if ( profile->IsSCORMCompliant() )
   {
      std::vector<const XMLNode*> scormStuff;
      profile->GetSCORMContents( scormStuff );

      // add SCORM specifications to config file
      for ( unsigned int i=0; i<scormStuff.size(); i++ )
      {
         AddElement( root, scormStuff[i] );
      }
   }

   // save out the file
   std::string outputDir = profile->GetOutputDirectory();
   std::string filename  = RelativeToAbsolutePath( config, outputDir );
   SaveToFile( filename );

   // cleanup
   CloseDocument();

   // confirmation
   std::cout << "Config file created." << std::endl;
}


//======================================
// PRIVATE FUNCTIONS
//======================================

/**
 * This is a function to remove the debug character of a filename.  This
 * character is usually a 'd' or 'D' indicating a debug library build.
 * @param filename The name of the file to remove character from.
 */
void JobLaunchConfig::removeDebugCharFromFilename( std::string &filename )
{
   // remove the last character from the filename
   std::string name = GetFileNameNoExt( filename );
   std::string ext  = GetFileExtension( filename );
   filename = name.substr( 0, name.length()-1 );
   filename += ".";
   filename += ext;
}

/**
 * This is a function that will remove a 'lib' prefix from a filename string and will
 * also remove the '.dll' or '.so' extension. In other words, it will change strings
 * like 'libHello.so' or 'Hello.dll' to 'Hello'.
 */
void JobLaunchConfig::removeLibAndExtension( std::string &filename )
{
   //remove the 'lib' prefix from the filename if it exists
   if (ToUpperCase(filename.substr(0, 3)) == "LIB")
   {
      filename = filename.substr(3, filename.length() - 3);
   }

   //remove the '.dll' extension if it exists
   if (ToUpperCase(filename.substr(filename.length() - 4, 4)) == ".DLL")
   {
      filename = filename.substr(0, filename.length() - 4);
   }

   //remove the '.so' extension if it exists
   if (ToUpperCase(filename.substr(filename.length() - 3, 3)) == ".SO")
   {
      filename = filename.substr(0, filename.length() - 3);
   }
}