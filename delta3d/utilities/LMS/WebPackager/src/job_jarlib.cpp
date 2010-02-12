/**
 * @author ECS, Inc. ( Joseph Del Rocco )
 * @date 2006/07/07 - 11:33
 *
 * @file job_jarlib.cpp
 * @version 1.0
 */

// local
#include <job_jarlib.h>
#include <package_profile.h>
#include <package_utils.h>
// delta3d
#include <dtUtil/fileutils.h>
#include <dtUtil/exception.h>
// ansi
#include <iostream>


//======================================
// CONSTANTS, MACROS, DEFINITIONS
//======================================

//======================================
// PUBLIC FUNCTIONS
//======================================

JobJARLibrary::JobJARLibrary() : JobJAR()
{
}

JobJARLibrary::~JobJARLibrary()
{ 
}

void JobJARLibrary::Execute( PackageProfile *profile )
{
   // make sure the section we are interested in, exists
   const XMLNode *chunk = profile->GetChunk( "ApplicationLibraries" );
   if ( chunk == NULL )
   {
      std::cout << "[Warning] 'ApplicationLibraries' section is missing. No libraries will be deployed." << std::endl;
      return;
   }

   // locals   
   std::vector<std::string>   jarFiles;
   std::vector<std::string>   jarContents;   
   std::vector<Options>       options; // not used right now
   dtUtil::FileUtils         &fileUtils = dtUtil::FileUtils::GetInstance();

   // cache working directory
   std::string cwd = fileUtils.CurrentDirectory();
   CleanupFileString( cwd );

   // used for signing JAR files
   std::string keystore = profile->GetKeystoreFile();
   std::string key = profile->GetKeystoreKey();
   std::string password = profile->GetKeystorePassword();            
   if ( !IsAbsolutePath( keystore ) )
   {      
      keystore = RelativeToAbsolutePath( keystore, cwd );
   }

   // get the list of library jar files
   profile->GetJARFiles( PackageProfile::JAR_LIBRARY, jarFiles, options );

   // do some jaring for each jar file in the library section
   for ( unsigned int i=0; i<jarFiles.size(); i++ )
   {      
      std::string inputDir;
      std::string jarCmdString;

      // retrieve the input directory for this jar file and its contents
      profile->GetJARFileContents( PackageProfile::JAR_LIBRARY, jarFiles[i], inputDir, jarContents, options );

      // check to see if this jar file should be included based upon desired platform support
      std::string platform = profile->GetJarFilePlatform( PackageProfile::JAR_LIBRARY, jarFiles[i]);
      if ( ToUpperCase(platform).compare( "WINDOWS" ) == 0 )
      {
         if ( profile->IncludePlatform(PackageProfile::TP_WINDOWS) != true )
         {
            //not including windows support; skip this jar file
            continue;
         }
      }
      else if ( ToUpperCase(platform).compare( "LINUX" ) == 0 )
      {
 	if ( profile->IncludePlatform( PackageProfile::TP_LINUX ) 
	     != true )
          {
             //not including windows support; skip this jar file
             continue;
          }
      }

      // change to input directory for this jar file
      CleanupFileString( inputDir );
      if ( inputDir.length() == 0 || !fileUtils.DirExists( inputDir ) )
      {
         std::cout << "[Error] Input directory '" << inputDir << "' does not exist. Cannot JAR files." << std::endl;
         continue;
      }
      fileUtils.ChangeDirectory( inputDir );

      // make sure jar filename has appropriate extension
      SetFileExtension( jarFiles[i], "jar" );

      // generate a full list of files that need to be jared
      std::string listOfFiles;
      for ( unsigned int j=0; j<jarContents.size(); j++ )
      {
         listOfFiles += jarContents[j];
         listOfFiles += " ";
      }

      // JAR up files
      // jar cvf bleh.jar file1.dll file2.dll file3.dll
      std::cout << "Creating " << jarFiles[i] << std::endl;
      jarCmdString = "jar cf ";
      jarCmdString += jarFiles[i];
      jarCmdString += " ";
      jarCmdString += listOfFiles;
      system( jarCmdString.c_str() );

      // sign the JAR file
      // jarsigner -J"-Xmx256m" -keystore C:/devel/projects/delta3d-ECS/LMS/WebPackager/netc.keystore bleh.jar lmsKey
      std::cout << "Signing " << jarFiles[i] << std::endl;
      jarCmdString = "jarsigner -storepass ";
      jarCmdString += password;
      jarCmdString += " -J\"-Xmx256m\" -keystore ";
      jarCmdString += "\"" + keystore + "\"";
      jarCmdString += " ";
      jarCmdString += "\"" + jarFiles[i] + "\"";
      jarCmdString += " " + key;
      system( jarCmdString.c_str() );

      // move the resulting JAR file to the output directory
      try
      {
         std::string absFile = RelativeToAbsolutePath( jarFiles[i], inputDir );
         std::string outputDir = profile->GetOutputDirectory();
         fileUtils.FileMove( absFile, outputDir, true );
      }
      catch ( dtUtil::Exception &e ) { e.Print(); }
   }

   // restore working directory
   fileUtils.ChangeDirectory( cwd );

   // confirmation
   std::cout << "Application libraries JARed and deployed." << std::endl;
}
