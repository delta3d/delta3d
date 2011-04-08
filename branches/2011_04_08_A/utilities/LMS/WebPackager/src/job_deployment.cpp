/**
 * @author ECS, Inc. ( Joseph Del Rocco )
 * @date 2006/07/13 - 9:40
 *
 * @file job_deployment.h
 * @version 1.0
 */

// local
#include <job_deployment.h>
#include <package_profile.h>
#include <package_utils.h>
// delta3d
#include <dtUtil/fileutils.h>
#include <dtUtil/exception.h>
// ansi
#include <iostream>



//======================================
// PUBLIC FUNCTIONS
//======================================

JobDeployment::JobDeployment() : JobBase()
{
}

JobDeployment::~JobDeployment()
{ 
}

void JobDeployment::Execute( PackageProfile *profile )
{
   // make sure the section we are interested in, exists
   const XMLNode *chunk = profile->GetChunk( "Deployment" );
   if ( chunk == NULL )
   {
      std::cout << "[Error] 'Deployment' section is missing in project profile!" << std::endl;
      return;
   }

   // needed vars
   dtUtil::FileUtils &fileUtils = dtUtil::FileUtils::GetInstance();

   // cache working directory
   const std::string &cwd = fileUtils.CurrentDirectory();

   // output
   std::cout << "Deploying specified files." << std::endl;

   // check if output directory exists, if not then create it
   std::string outputDir = profile->GetOutputDirectory();
   if ( !fileUtils.DirExists( outputDir ) )
   {
      std::cout << "[Warning] Output directory does not exist, creating it now." << std::endl;
      MakeDirectoryEX( outputDir );
   }

   // get all deployment input directory elements
   std::vector<const XMLNode*> inputElements;
   profile->GetDeploymentInputDirectoryElements( inputElements );

   //loop through each input element and copy files from it to output directory
   for ( unsigned int i=0; i<inputElements.size(); i++)
   {
      //retrieve input directory string, resolving all environment variables
      const XMLNode *inputElement = inputElements[i];
      std::string inputDir = ResolveEnvironmentVariables(profile->GetElementAttribute(inputElement, "location"));

      //change to directory we are copying files from
      if ( inputDir.length() == 0 || !fileUtils.DirExists( inputDir ) )
      {
         std::cout << "[Error] Input directory '" << inputDir << "' does not exist." << std::endl;
         continue;
      }
      fileUtils.ChangeDirectory( inputDir );

      // get all the files we need to deploy
      std::vector<std::string>   fileNames;
      std::vector<Options>       fileOptions;
      profile->GetDeploymentContents(inputElement, fileNames, fileOptions);

      // traverse list of files
      for ( unsigned int i=0; i<fileNames.size(); i++ )
      {
         // gather file information
         std::string subpath  = GetFilePath( fileNames[i] );
         std::string file     = GetFileName( fileNames[i] );
         std::string fileName = GetFileNameNoExt( fileNames[i] );
         std::string fileExt  = GetFileExtension( fileNames[i] );
         std::string srcDir   = inputDir;
         std::string destDir  = outputDir;

         // file is located in a relative subdirectory
         if(!subpath.empty())
         {
            srcDir  = RelativeToAbsolutePath( subpath, inputDir );
            destDir = RelativeToAbsolutePath( subpath, outputDir );

            // make sure this relative input directory actually exists
            if ( !fileUtils.DirExists( srcDir ) )
            {
               std::cout << "[Warning] Directory '" << srcDir << "' does not exist." << std::endl;
               continue;
            }

            // create new destination directory if necessary
            MakeDirectoryEX( destDir );
         }

         // copy everything!
         if ( file == "*.*" )
         {
            std::string whatToIgnore = fileOptions[i].Get( std::string("ignore") );
            copyAllFiles( srcDir, destDir, whatToIgnore );
         }

         // copy all files of a certain extension
         else if ( fileName == "*" )
         {
            dtUtil::DirectoryContents files = fileUtils.DirGetFiles( srcDir );

            for( dtUtil::DirectoryContents::const_iterator itr = files.begin(); itr!=files.end(); ++itr )
            {            
               if ( GetFileExtension( itr->c_str() ) == fileExt )
               {
                  try
                  {
                     std::string absFile = RelativeToAbsolutePath( itr->c_str(), srcDir );
                     fileUtils.FileCopy( absFile, destDir, true );
                  }
                  catch ( dtUtil::Exception &e ) { e.Print(); }
               }
            }
         }

         // copy a single file
         else
         {
            try
            {
               std::string absFile = RelativeToAbsolutePath( file, srcDir );
               fileUtils.FileCopy( absFile, destDir, true );
            }
            catch ( dtUtil::Exception &e ) { e.Print(); }
         }

         // sign any JAR files
         if ( fileExt == "jar" || fileExt == "JAR" )
         {
            if ( fileOptions[i].Get( "sign" ) == "true" )
            {
               std::string absFile = RelativeToAbsolutePath( file, destDir );
               std::string cmdString;

               // used for signing JAR files
               std::string keystore = profile->GetKeystoreFile();
               std::string key = profile->GetKeystoreKey();
               std::string password = profile->GetKeystorePassword();

               if ( !IsAbsolutePath( keystore ) )
               {      
                  keystore = RelativeToAbsolutePath( keystore, cwd );
               }
               
               // sign the JAR file
               // example: jarsigner -J"-Xmx256m" -keystore C:/SomeDirectory/netc.keystore bleh.jar myKey
               std::cout << "Signing " << fileNames[i] << std::endl;
               cmdString = "jarsigner -storepass ";
               cmdString += password;
               cmdString += " -J\"-Xmx256m\" -keystore ";
               cmdString += "\"" + keystore + "\"";
               cmdString += " ";
               cmdString += "\"" + absFile + "\"";
               cmdString += " " + key;
               system( cmdString.c_str() );
            }
         }
      }
   }

   // restore working directory
   fileUtils.ChangeDirectory( cwd );

   // confirmation
   std::cout << "Deployment files copied." << std::endl;
}


//======================================
// PRIVATE FUNCTIONS
//======================================

void JobDeployment::copyAllFiles(const std::string &srcDir, 
                                 const std::string &destDir, 
                                 const std::string &ignore)
{
   dtUtil::FileUtils &fileUtils = dtUtil::FileUtils::GetInstance();

   // traverse files in source directory
   dtUtil::DirectoryContents files = fileUtils.DirGetFiles( srcDir );
   for( dtUtil::DirectoryContents::const_iterator itr = files.begin(); itr!=files.end(); ++itr )
   {
      std::string child  = *itr;
      std::string newSrc = RelativeToAbsolutePath( child, srcDir );
      dtUtil::FileInfo fileInfo = fileUtils.GetFileInfo( newSrc );

      // ignore certain files or directories
      if ( child == "." || child == ".." || child == ignore )
      {
         continue;
      }

      // handle directories
      else if ( fileInfo.fileType == dtUtil::DIRECTORY )
      {
         // create new destination directory if necessary
         std::string newDestDir = RelativeToAbsolutePath( child, destDir );
         if ( !fileUtils.DirExists( newDestDir ) )
         {
            MakeDirectoryEX( newDestDir );
         }

         // copy all files in this sub-directory
         copyAllFiles( newSrc, newDestDir, ignore );
      }

      // handle files
      else if ( fileInfo.fileType == dtUtil::REGULAR_FILE )
      {
         try
         {
            fileUtils.FileCopy( newSrc, destDir, true );
         }
         catch ( dtUtil::Exception &e ) { e.Print(); }
      }
   }
}
