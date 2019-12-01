/**
 * @author ECS, Inc. ( Joseph Del Rocco )
 * @date 2006/07/07 - 11:33
 *
 * @file job_jnlp.cpp
 * @version 1.0
 */

// local
#include <job_jnlp.h>
#include <package_profile.h>
#include <package_utils.h>
// ansi
#include <iostream>



//======================================
// PUBLIC FUNCTIONS
//======================================

JobJNLP::JobJNLP() : JobXML()
{
}

JobJNLP::~JobJNLP()
{ 
}

void JobJNLP::Execute( PackageProfile *profile )
{
   XMLNode *root = CreateDocument( "jnlp" );
   XMLNode *curr = root;

   // locals
   std::string url    = profile->GetURL();
   std::string app    = profile->GetApplicationName();
   std::string vendor = profile->GetApplicationVendor();
   std::string launch = profile->GetJavaLaunchProgram();
   std::string config = profile->GetJavaLaunchConfig();
   std::string mclass = profile->GetJavaLaunchMainClass();

   // jnlp filename is application name with no spaces
   //std::string jnlpName = app;
   //removeSpaces( jnlpName );
   //SetFileExtension( jnlpName, "jnlp" );

   // jnlp filename is hardcoded right now ( specified in header )
   std::string jnlpName = JNLP_FILENAME;
   SetFileExtension( jnlpName, "jnlp" );

   // root element
   SetAttribute( root, "spec", "1.0+" );
   SetAttribute( root, "codebase", url );
   SetAttribute( root, "href", jnlpName );

   // information element
   curr = AddElement( root, "information" );
   AddElement( curr, "title", app );
   AddElement( curr, "vendor", vendor );
   AddElement( curr, "offline-allowed" );

   // security element
   curr = AddElement( root, "security" );
   AddElement( curr, "all-permissions" );

   // resource elements
   {
      XMLNode                 *resource;
      std::vector<std::string> files;
      std::vector<Options>     options;

      // launching app
      resource = AddElement( root, "resources" );
      curr = AddElement( resource, "j2se" );
      SetAttribute( curr, "version", "1.6.0" );
      SetAttribute( curr, "java-vm-args", "-esa -Xnoclassgc" );
      curr = AddElement( resource, "jar" );
      SetAttribute( curr, "href", launch );

      // data
      profile->GetJARFiles( PackageProfile::JAR_DATA, files, options );
      for ( unsigned int i=0; i<files.size(); i++ )
      {
         resource = AddElement( root, "resources" );
         curr = AddElement( resource, "j2se" );
         SetAttribute( curr, "version", "1.6.0" );
         SetAttribute( curr, "java-vm-args", "-esa -Xnoclassgc" );
         curr = AddElement( resource, "jar" );
         SetFileExtension( files[i], "jar" );
         SetAttribute( curr, "href", files[i] );
      }

      // libs
      profile->GetJARFiles( PackageProfile::JAR_LIBRARY, files, options );
      for ( unsigned int i=0; i<files.size(); i++ )
      {
         resource = AddElement( root, "resources" );
         std::string platform = options[i].Get( "platform" );

         //determine target platform for this lib
         PackageProfile::TargetPlatform enumPlatform;
         if ( ToUpperCase(platform) == "WINDOWS" )
            enumPlatform = PackageProfile::TP_WINDOWS;
         else if ( ToUpperCase(platform) == "LINUX" )
            enumPlatform = PackageProfile::TP_LINUX;

         if ( profile->IncludePlatform(enumPlatform) )
         {
            platform[0] = toupper( platform[0] );
            SetAttribute( resource, "os", platform );
            curr = AddElement( resource, "nativelib" );
            SetFileExtension( files[i], "jar" );
            SetAttribute( curr, "href", files[i] );
         }
      }
   }

   // application element   
   curr = AddElement( root, "application-desc" );
   SetAttribute( curr, "main-class", mclass );
   AddElement( curr, "argument", "-c" );
   AddElement( curr, "argument", RelativeToAbsolutePath( config, url ) );

   // save out the file
   std::string outputDir    = profile->GetOutputDirectory();
   std::string jnlpFilename = RelativeToAbsolutePath( jnlpName, outputDir );
   SaveToFile( jnlpFilename );

   // cleanup
   CloseDocument();

   // confirmation
   std::cout << "JNLP file created." << std::endl;
}


//======================================
// PUBLIC FUNCTIONS
//======================================

void JobJNLP::removeSpaces( std::string &str )
{
   for ( unsigned int i=0; i<str.length(); i++ )
   {
      if ( str[i] == ' ' )
      {
         str = str.erase( i, 1 );
         i--;
      }
   }
}
