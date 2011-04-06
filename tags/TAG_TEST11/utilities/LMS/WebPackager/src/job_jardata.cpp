/**
 * @author ECS, Inc. ( Joseph Del Rocco )
 * @date 2006/07/07 - 11:33
 *
 * @file job_jardata.cpp
 * @version 1.0
 */

// local
#include <job_jardata.h>
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

JobJARData::JobJARData() : JobJAR()
{
}

JobJARData::~JobJARData()
{ 
}

void JobJARData::Execute( PackageProfile *profile )
{
   // make sure the section we are interested in, exists
   const XMLNode *chunk = profile->GetChunk( "ApplicationData" );
   if ( chunk == NULL )
   {
      std::cout << "[Warning] 'ApplicationData' section is missing. No data will be deployed." << std::endl;
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

   // get the list of data jar files
   profile->GetJARFiles( PackageProfile::JAR_DATA, jarFiles, options );

   // do some jaring for each jar file in the data section
   for ( unsigned int i=0; i<jarFiles.size(); i++ )
   {
      std::string inputDir;
      std::string jarCmdString;

      // retrieve the input directory for this jar file and its contents
      profile->GetJARFileContents( PackageProfile::JAR_DATA, jarFiles[i], inputDir, jarContents, options );

      // change to input directory for this jar file
      CleanupFileString( inputDir );
      if ( inputDir.empty() || !fileUtils.DirExists( inputDir ) )
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
         // gather information about this file
         std::string subpath  = GetFilePath( jarContents[j] );
         std::string file     = GetFileName( jarContents[j] );
         std::string fileName = GetFileNameNoExt( jarContents[j] );
         std::string fileExt  = GetFileExtension( jarContents[j] );

         // append everything in this directory and below
         if ( file == "*.*" )
         {
            std::string whatToIgnore = options[j].Get( "ignore" );
            appendAllFiles( inputDir, subpath, listOfFiles, whatToIgnore );
         }

         // append all files in this directory with the same extension
         else if ( fileName == "*" )
         {
            std::string srcDir = RelativeToAbsolutePath( subpath, inputDir );
            dtUtil::DirectoryContents files = fileUtils.DirGetFiles( srcDir );
            for( dtUtil::DirectoryContents::const_iterator itr = files.begin(); itr!=files.end(); ++itr )
            {
               if ( GetFileExtension( itr->c_str() ) == fileExt )
               {
                  std::string absFile = RelativeToAbsolutePath( itr->c_str(), subpath );
                  listOfFiles += absFile;
                  listOfFiles += " ";
               }
            }
         }

         // append only this file
         else
         {
            listOfFiles += jarContents[j];
            listOfFiles += " ";
         }
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
   std::cout << "Application data JARed and deployed." << std::endl;
}


//======================================
// PRIVATE FUNCTIONS
//======================================

void JobJARData::appendAllFiles(const std::string &inputDir, 
                                const std::string &subpath,
                                std::string &listOfFiles, 
                                const std::string &ignore )
{
   dtUtil::FileUtils &fileUtils = dtUtil::FileUtils::GetInstance();

   // traverse files in this directory
   std::string srcDir = RelativeToAbsolutePath( subpath, inputDir );
   
   dtUtil::DirectoryContents files;
   try
   {
      files = fileUtils.DirGetFiles( srcDir );
   }
   catch ( dtUtil::Exception &)
   { 
      std::cout << "Input Directory '" << srcDir.c_str() << "' not found. Ignoring..." << std::endl; 
   }

   for( dtUtil::DirectoryContents::const_iterator itr = files.begin(); itr!=files.end(); ++itr )
   {
      std::string child = itr->c_str();
      std::string absFile = RelativeToAbsolutePath( child, srcDir );
      dtUtil::FileInfo fileInfo = fileUtils.GetFileInfo( absFile );

      // ignore certain files or directories
      if ( child == "." || child == ".." || child == ignore )
      {
         continue;
      }

      // handle directories
      else if ( fileInfo.fileType == dtUtil::DIRECTORY )
      {
         std::string newsubpath = RelativeToAbsolutePath( child, subpath );
         appendAllFiles( inputDir, newsubpath, listOfFiles, ignore );
      }

      // handle files
      else if ( fileInfo.fileType == dtUtil::REGULAR_FILE )
      {
         std::string file = RelativeToAbsolutePath( child, subpath );
         listOfFiles += file;
         listOfFiles += " ";
      }
   }
}




