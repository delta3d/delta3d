/**
 * @author ECS, Inc. ( Joseph Del Rocco )
 * @date 2006/07/10 - 10:22
 *
 * @file package_profile.cpp
 * @version 1.0 
 */

// local
#include <package_profile.h>
#include <package_utils.h>
// xerces
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/util/XMLException.hpp>
#include <xercesc/util/XMLString.hpp>
// ansi
#include <iostream>

#include <dtUtil/stringutils.h>

//======================================
// STARTUP FUNCTIONS
//======================================

PackageProfile::PackageProfile() : mParser(NULL), mDocument(NULL), mOutput(""),
   mFlags(0)
{
}

PackageProfile::~PackageProfile()
{
}

bool PackageProfile::LoadPackageProfile(const std::string &filename)
{
   //return value
   bool retVal = true;

   // handle for multiple loads
   if ( mParser != NULL ) unload();

   // create parser
   static const XMLCh gLS[] = { chLatin_L, chLatin_S, chNull };
   DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(gLS);

#if XERCES_VERSION_MAJOR >= 3
   mParser = impl->createLSParser(DOMImplementationLS::MODE_SYNCHRONOUS, NULL);
   // initialize parser
   mParser->getDomConfig()->setParameter(XMLUni::fgDOMNamespaces, false);
   mParser->getDomConfig()->setParameter(XMLUni::fgXercesSchema, false);
   mParser->getDomConfig()->setParameter(XMLUni::fgXercesSchemaFullChecking, false);
   mParser->getDomConfig()->setParameter(XMLUni::fgDOMValidate, false);
   mParser->getDomConfig()->setParameter(XMLUni::fgDOMDatatypeNormalization, false);
#else
   mParser = ((DOMImplementationLS*)impl)->createDOMBuilder(
      DOMImplementationLS::MODE_SYNCHRONOUS, 0 );
   // initialize parser
   mParser->setFeature( XMLUni::fgDOMNamespaces, false );
   mParser->setFeature( XMLUni::fgXercesSchema, false );
   mParser->setFeature( XMLUni::fgXercesSchemaFullChecking, false );
   mParser->setFeature( XMLUni::fgDOMValidation, false );
   mParser->setFeature( XMLUni::fgDOMDatatypeNormalization, false );
#endif


   std::string resolvedFilename = ResolveEnvironmentVariables(filename);

   // use parser to load xml file into memory, handle exceptions accordingly
   try
   {
      mDocument = mParser->parseURI( resolvedFilename.c_str() );

      if ( mDocument == NULL )
      {
         std::cout << "[Error] loading file: '" << resolvedFilename << "'" << std::endl;
         retVal = false;
      }
   }
   catch ( const XMLException &exp )
   {
      char *message = XMLString::transcode( exp.getMessage() );
      std::cout << "[Error] Parsing: '" << resolvedFilename << "'\n  "
           << message << std::endl;
      XMLString::release( &message );
      retVal = false;
   }
   catch ( const DOMException &exp )
   {
      const unsigned int maxChars = 2047;
      XMLCh errText[maxChars + 1];
      std::cout << "[Error] Parsing: '" << resolvedFilename << "'";
      if (DOMImplementation::loadDOMExceptionMsg(exp.code, errText, maxChars))
      {
         char *message = XMLString::transcode( errText );
         std::cout << "\n  (" << exp.code << ") " << errText << std::endl;
         XMLString::release( &message );
      }
      retVal = false;
   }
   catch (...)
   {
      std::cout << "[Error] Unexpected exception parsing: '"
                << resolvedFilename << "'\n";
      retVal = false;
   }

   return retVal;
}

void PackageProfile::ReleasePackageProfile()
{
   unload();
}


//======================================
// COMMON ACCESSORS
//======================================

void PackageProfile::SetOutput(const std::string &output)
{
   // check if output should be SCORM compliant
   const XMLNode *n = getRoot();
   n = n->getFirstChild();
   for ( ; n != NULL; n = n->getNextSibling() )
   {
      // ignore comments, etc.
      if ( n->getNodeType() == XMLNode::ELEMENT_NODE )
      {
         char *pname = XMLString::transcode( n->getNodeName() );
         std::string name = pname;
         XMLString::release( &pname );

         // is this the SCORM element?
         if ( name.compare( "SCORM" ) == 0 )
         {
            setFlag( FLAG_SCORM_COMPLIANT );
            break;
         }
      }
   }     

   // check supported platforms
   n = getElement( getRoot(), "Deployment" );
   n = n->getFirstChild();
   for ( ; n != NULL; n = n->getNextSibling() )
   {
      // ignore comments, etc.
      if ( n->getNodeType() == XMLNode::ELEMENT_NODE )
      {
         char *pname = XMLString::transcode( n->getNodeName() );
         std::string name = pname;
         XMLString::release( &pname );

         // is this a Platform element?
         if ( name.compare( "Platform" ) == 0 )
         {
            if ( ToUpperCase(getElementContent(n)).compare( "WINDOWS") == 0 )
            {
               setFlag ( FLAG_PLATFORM_WINDOWS );
            }

            if ( ToUpperCase(getElementContent(n)).compare( "LINUX" ) == 0 )
            {
               setFlag( FLAG_PLATFORM_LINUX );
            }
         }
      }
   }

   // store output string
   mOutput = output;
   CleanupFileString( mOutput );

   // convert if relative path
   if ( !IsAbsolutePath( mOutput ) )
   {      
      mOutput = RelativeToAbsolutePath( mOutput );
   }
}

std::string PackageProfile::GetOutputDirectory() const
{   
   return GetFilePath( mOutput );
}

bool PackageProfile::IsSCORMCompliant() const
{
   return getFlag( FLAG_SCORM_COMPLIANT );
}

bool PackageProfile::IncludePlatform( TargetPlatform x ) const
{
   if ( x == TP_WINDOWS )    return getFlag( FLAG_PLATFORM_WINDOWS );
   else if ( x == TP_LINUX ) return getFlag( FLAG_PLATFORM_LINUX );
   else                   return false;
}

std::string PackageProfile::GetURL()
{
   // find node
   const XMLNode *n = NULL;
   n = getElement( getRoot(), "Deployment" );
   n = getElement( n, "URL" );
   
   // retrieve content
   std::string url = getElementContent( n );
   CleanupFileString( url );
   return url;
}

std::string PackageProfile::GetKeystoreFile()
{
   // find node
   const XMLNode *n = NULL;
   n = getElement( getRoot(), "Keystore" );
   n = getElement( n, "File");

   // retrieve content
   std::string file = ResolveEnvironmentVariables(getElementContent( n ));
   CleanupFileString( file );
   return file;
}

std::string PackageProfile::GetKeystoreKey()
{
   // find node
   const XMLNode *n = NULL;
   n = getElement( getRoot(), "Keystore" );
   n = getElement( n, "Key");

   // retrieve content
   std::string file = ResolveEnvironmentVariables(getElementContent( n ));
   CleanupFileString( file );
   return file;
}

std::string PackageProfile::GetKeystorePassword()
{
   return mPassword;
}

void PackageProfile::SetKeystorePassword(const std::string &password)
{
   mPassword = password;
}

std::string PackageProfile::GetApplicationName()
{
   // find node
   const XMLNode *n = NULL;
   n = getElement( getRoot(), "Application" );
   n = getElement( n, "ApplicationName" );

   // retrieve content
   return getElementContent( n );
}

std::string PackageProfile::GetApplicationVendor()
{
   // find node
   const XMLNode *n = NULL;
   n = getElement( getRoot(), "Application" );
   n = getElement( n, "ApplicationVendor" );

   // retrieve content
   return getElementContent( n );
}

std::string PackageProfile::GetApplicationLaunchParams()
{
   // find node
   const XMLNode *n = NULL;
   n = getElement( getRoot(), "Application" );
   n = getElement( n, "ApplicationData" );
   n = getElement( n, "GeneralLaunchParams" );

   // retrieve content
   return getElementContent( n );
}

const XMLNode *PackageProfile::GetChunk(const std::string &elementName)
{
   if ( elementName.length() == 0 ) return NULL;
   return getElement( getRoot(), elementName );
}


//======================================
// JOB UTILITY FUNCTIONS
//======================================

std::string PackageProfile::GetDeploymentDirectory()
{
   // find node
   const XMLNode *n = NULL;
   n = getElement( getRoot(), "Deployment" );
   n = getElement( n, "InputDirectory" );

   // retrieve content
   std::string dir = GetElementAttribute( n, "location" );
   dir = ResolveEnvironmentVariables(dir);
   CleanupFileString( dir );
   return dir;
}

std::string PackageProfile::GetJavaApplet()
{
   // find node
   const XMLNode *n = NULL;
   n = getElement( getRoot(), "Deployment" );
   n = getElement( n, "JavaMessagingApplet" );

   // retrieve content
   return getElementContent( n );
}

std::string PackageProfile::GetJavaLaunchProgram()
{
   // find node
   const XMLNode *n = NULL;
   n = getElement( getRoot(), "Deployment" );
   n = getElement( n, "JavaLaunch" );
   
   // retrieve content
   return getElementContent( n );
}

std::string PackageProfile::GetJavaLaunchConfig()
{
   // find node
   const XMLNode *n = NULL;
   n = getElement( getRoot(), "Deployment" );
   n = getElement( n, "JavaLaunchConfig" );

   // retrieve content
   return getElementContent( n );
}

std::string PackageProfile::GetJavaLaunchMainClass()
{
   // find node
   const XMLNode *n = NULL;
   n = getElement( getRoot(), "Deployment" );
   n = getElement( n, "JavaLaunch" );

   // retrieve attribute
   return GetElementAttribute( n, "main-class" );
}

std::string PackageProfile::GetLaunchPage()
{
   // find node
   const XMLNode *n = NULL;
   n = getElement( getRoot(), "Deployment" );
   n = getElement( n, "InputDirectory" );

   // look for a certain child element
   n = n->getFirstChild();
   for ( ; n != NULL; n = n->getNextSibling() )
   {
      // ignore comments, etc.
      if ( n->getNodeType() == XMLNode::ELEMENT_NODE && n->hasAttributes() )
      {
         // launch page is denoted by an attribute
         std::string type = GetElementAttribute( n, "type" );
         if ( type.length() != 0 && type.compare( "LaunchPage" ) == 0 )
         {
            return getElementContent( n );
         }
      }
   }

   // not found, so return an empty string
   return std::string();
}

void PackageProfile::GetDeploymentInputDirectoryElements( std::vector<const XMLNode*> &elements)
{
   //make sure we start with an empty vector
   elements.clear();

   //find deployment node
   const XMLNode *depNode = NULL;
   depNode = getElement( getRoot(), "Deployment" );

   const XMLNode *n = depNode->getFirstChild();
   for ( ; n != NULL; n = n->getNextSibling() )
   {
      // ignore comments, etc.
      if ( n->getNodeType() == XMLNode::ELEMENT_NODE )
      {
         char *pname = XMLString::transcode( n->getNodeName() );
         std::string nodeName = pname;
         XMLString::release( &pname );

         if ( nodeName.compare( "InputDirectory" ) == 0)
         {
            elements.push_back(n);
         }
      }
   }
}

void PackageProfile::GetDeploymentContents( const XMLNode *inputElement,
                                            std::vector<std::string> &files,
                                            std::vector<Options> &options )
{
   // make sure inputElement node is valid
   if ( inputElement == NULL ) return;

   // make sure inputElement is an actual "InputDirectory" element
   char *pname = XMLString::transcode( inputElement->getNodeName() );
   std::string nodeName = pname;
   XMLString::release( &pname );
   if ( nodeName.compare( "InputDirectory" ) != 0) return;

   // make sure we start with empty vectors
   files.clear();
   options.clear();

   // these child elements represent filenames
   const XMLNode *n = inputElement->getFirstChild();
   for ( ; n != NULL; n = n->getNextSibling() )
   {
      // ignore comments, etc.
      if ( n->getNodeType() == XMLNode::ELEMENT_NODE )
      {
         Options opts;

         // attributes represent options per filename
         if ( n->hasAttributes() )
         {
            DOMNamedNodeMap *attributes = n->getAttributes();
            for( unsigned int i=0; i<attributes->getLength(); i++ )
            {
               DOMAttr *attr = (DOMAttr*)attributes->item( i );
               char *key = XMLString::transcode( attr->getName() );
               char *val = XMLString::transcode( attr->getValue() );
               opts.Set( std::string(key), std::string(val) );
               XMLString::release( &key );
               XMLString::release( &val );
            }
         }

         // store them in passed in vectors
         std::string content = getElementContent(n);

         //check to see if the content is ("*"), or ends with ("/*"), a unix-style wild-card;
         //if so, then change this to the windows-style "*.*"; NOTE: the deployment
         //job does not actually pass-in "*.*" to a directory utility as is, but rather
         //intercepts this literal and cycles through all files in the directory
         //one by one.
         if (content == "*")
         {
            content = "*.*";
         }
         else if (content.substr( (content.length() - 2), 2) == "/*")
         {
            content = content.substr(0, content.length() -1) + "*.*";
         }
         
         files.push_back( content );
         options.push_back( opts );
      }
   }
}

void PackageProfile::GetLibraries( LibraryType libType,
                                   std::vector<std::string> &files,
                                   std::vector<Options> &options,
                                   TargetPlatform targetPlatform)
{
   // find start node
   const XMLNode *n = NULL;
   n = getElement( getRoot(), "Application" );
   n = getElement( n, "ApplicationLibraries" );

   // make sure we start with empty vectors
   files.clear();
   options.clear();

   // we are looking for libraries of a certain type
   std::string libTypeName;
   if ( libType == LIB_DEPENDENCY )    libTypeName = "Library";
   else if ( libType == LIB_LAUNCHER ) libTypeName = "LauncherLibrary";
   else if ( libType == LIB_GAME )     libTypeName = "GameLibrary";

   // look through all child jar file elements
   n = n->getFirstChild();
   for ( ; n != NULL; n = n->getNextSibling() )
   {
      // ignore comments, etc.
      if ( n->getNodeType() == XMLNode::ELEMENT_NODE )
      {
         char *pname = XMLString::transcode( n->getNodeName() );
         std::string nodeName = pname;
         XMLString::release( &pname );

         // we are only interested in jar files
         if ( nodeName.compare( "JarFile" ) == 0 )
         {
            std::string platform;
            TargetPlatform enumPlatform;

            // find the target platform if specified
            if ( n->hasAttributes() )
            {
               DOMNamedNodeMap *attributes = n->getAttributes();
               XMLCh *attrName = XMLString::transcode( "platform" );
               DOMAttr *attr = (DOMAttr*)attributes->getNamedItem( attrName );
               XMLString::release( &attrName );
               if ( attr )
               {
                  char *val = XMLString::transcode( attr->getValue() );
                  platform = val;
                  XMLString::release( &val );

                  if ( ToUpperCase(platform) == "WINDOWS" )
                     enumPlatform = TP_WINDOWS;
                  else if ( ToUpperCase(platform) == "LINUX" )
                     enumPlatform = TP_LINUX;
               }
            }
            
            // look for all libraries listed in the jar file
            const XMLNode *n2 = getElement( n, "InputDirectory" );
            n2 = n2->getFirstChild();
            for ( ; n2 != NULL; n2 = n2->getNextSibling() )
            {
               // ignore comments, etc.
               if ( n2->getNodeType() == XMLNode::ELEMENT_NODE )
               {
                  pname = XMLString::transcode( n2->getNodeName() );
                  nodeName = pname;
                  XMLString::release( &pname );                  

                  // found one!
                  if ( nodeName.compare( libTypeName ) == 0 )
                  {
                     //make sure it's for the requested platform
                     if ( enumPlatform == targetPlatform )
                     {
                        Options opts;
                        opts.Set( "platform", platform );

                        // attributes represent options per filename
                        DOMNamedNodeMap *attributes = n2->getAttributes();
                        for( unsigned int i=0; i<attributes->getLength(); i++ )
                        {
                           DOMAttr *attr = (DOMAttr*)attributes->item( i );
                           char *key = XMLString::transcode( attr->getName() );
                           char *val = XMLString::transcode( attr->getValue() );
                           opts.Set( std::string(key), std::string(val) );
                           XMLString::release( &key );
                           XMLString::release( &val );
                        }

                        files.push_back( getElementContent( n2 ) );
                        options.push_back( opts );
                     }

                  }
               }
            }
         }
      }
   }
}

void PackageProfile::GetEntryPointLibrary( LibraryType libType,
                                           std::string &lib,
                                           Options &options,
                                           TargetPlatform targetPlatform)
{
   std::vector<std::string> libraries;
   std::vector<Options>     libopts;

   // query for list of appropriate libs
   GetLibraries( libType, libraries, libopts, targetPlatform );

   // look for the entry point lib
   for ( unsigned int i=0; i<libraries.size(); i++ )
   {
      std::string val = libopts[i].Get( "entrypoint" );
      if ( val.length() != 0 && dtUtil::ToType<bool>(val) )
      {
         lib = libraries[i];
         options = libopts[i];
         return;
      }
   }

   // no good, no entry library specified
   std::cout << "[WARNING] No entry point library found for requested library type." << std::endl;
}

void PackageProfile::GetJARFiles( JARFileType jarType,
                                  std::vector<std::string> &files,
                                  std::vector<Options> &options )
{
   // find appropriate start node
   const XMLNode *n = NULL;
   if ( jarType == JAR_DATA )
      n = getElement( getRoot(), "ApplicationData" );
   else if ( jarType == JAR_LIBRARY )
      n = getElement( getRoot(), "ApplicationLibraries" );
   else
      return;

   // make sure we start with empty vectors
   files.clear();
   options.clear();

   // look for all child jar file elements
   n = n->getFirstChild();
   for ( ; n != NULL; n = n->getNextSibling() )
   {
      // ignore comments, etc.
      if ( n->getNodeType() == XMLNode::ELEMENT_NODE )
      {
         char *pname = XMLString::transcode( n->getNodeName() );
         std::string nodeName = pname;
         XMLString::release( &pname );

         // we are only interested in jar files
         if ( nodeName.compare( "JarFile" ) == 0 )
         {
            Options opts;
            std::string jarName;

            // attributes represent options per jar file
            if ( n->hasAttributes() )
            {
               DOMNamedNodeMap *attributes = n->getAttributes();
               for( unsigned int i=0; i<attributes->getLength(); i++ )
               {
                  DOMAttr *attr = (DOMAttr*)attributes->item( i );
                  char *key = XMLString::transcode( attr->getName() );
                  char *val = XMLString::transcode( attr->getValue() );
                  
                  // this is the name of the jar file
                  if ( std::string(key).compare( "name" ) == 0 )
                  {
                     jarName = val;
                  }

                  // store attribute as an option
                  opts.Set( std::string(key), std::string(val) );

                  XMLString::release( &key );
                  XMLString::release( &val );
               }
            }

            // store them in passed in vectors
            files.push_back( jarName );
            options.push_back( opts );
         }                           
      }
   }
}

void PackageProfile::GetJARFileContents( JARFileType jarType,
                                         std::string jarFilename,
                                         std::string &inputDir,
                                         std::vector<std::string> &files,
                                         std::vector<Options> &options )
{
   // find appropriate start node
   const XMLNode *n = NULL;
   if ( jarType == JAR_DATA )
      n = getElement( getRoot(), "ApplicationData" );
   else if ( jarType == JAR_LIBRARY )
      n = getElement( getRoot(), "ApplicationLibraries" );
   else
      return;

   // make sure we start with empty vectors
   files.clear();
   options.clear();

   // look for all child jar file elements
   n = n->getFirstChild();
   for ( ; n != NULL; n = n->getNextSibling() )
   {
      // ignore comments, etc.
      if ( n->getNodeType() == XMLNode::ELEMENT_NODE )
      {
         char *pname = XMLString::transcode( n->getNodeName() );
         std::string nodeName = pname;
         XMLString::release( &pname );

         // we are only interested in jar files
         if ( nodeName.compare( "JarFile" ) == 0 )
         {
            std::string jarName = GetElementAttribute( n, "name" );

            // this is the name of the jar file we want
            if ( jarName.compare( jarFilename ) == 0 )
            {               
               // move one element lower
               n = getElement( n, "InputDirectory" );
               if ( n == NULL ) return;

               // retrieve input directory
               inputDir = ResolveEnvironmentVariables(GetElementAttribute( n, "location" ));
               
               // sift through all child elements
               n = n->getFirstChild();
               for ( ; n != NULL; n = n->getNextSibling() )
               {
                  // ignore comments, etc.
                  if ( n->getNodeType() != XMLNode::ELEMENT_NODE )
                     continue;
                  
                  Options opts;
                  std::string contentFile = getElementContent( n );

                  // attributes represent options per element
                  if ( n->hasAttributes() )
                  {
                     DOMNamedNodeMap *attributes = n->getAttributes();
                     for( unsigned int i=0; i<attributes->getLength(); i++ )
                     {
                        DOMAttr *attr = (DOMAttr*)attributes->item( i );
                        char *key = XMLString::transcode( attr->getName() );
                        char *val = XMLString::transcode( attr->getValue() );
                        opts.Set( std::string(key), std::string(val) );
                        XMLString::release( &key );
                        XMLString::release( &val );
                     }
                  }

                  // store them in passed in vectors
                  files.push_back( contentFile );
                  options.push_back( opts );
               }

               // we found the jar file we were interested in and retrieved its
               // contents, so we can exit out now
               return;
            }
         }                           
      }
   }   
}

std::string PackageProfile::GetJarFilePlatform( JARFileType jarType, std::string jarFilename)
{
   // find appropriate start node
   const XMLNode *n = NULL;
   if ( jarType == JAR_DATA )
      n = getElement( getRoot(), "ApplicationData" );
   else if ( jarType == JAR_LIBRARY )
      n = getElement( getRoot(), "ApplicationLibraries" );
   else
      return "";

   // look for all child jar file elements
   n = n->getFirstChild();
   for ( ; n != NULL; n = n->getNextSibling() )
   {
            // ignore comments, etc.
      if ( n->getNodeType() == XMLNode::ELEMENT_NODE )
      {
         char *pname = XMLString::transcode( n->getNodeName() );
         std::string nodeName = pname;
         XMLString::release( &pname );

         // we are only interested in jar files
         if ( nodeName.compare( "JarFile" ) == 0 )
         {
            std::string jarName = GetElementAttribute( n, "name" );

            // this is the name of the jar file we want
            if ( jarName.compare( jarFilename ) == 0 )
            { 
               return GetElementAttribute( n, "platform" );
            }
         }
      }
   }

   //could not find jar file
   return "";
}

void PackageProfile::GetSCORMContents( std::vector<const XMLNode*> &elements )
{
   // find start node
   const XMLNode *n = NULL;
   n = getElement( getRoot(), "SCORM" );
   if ( n == NULL ) return;

   // make sure we start with an empty vector
   elements.clear();   
   
   // find all child elements
   n = n->getFirstChild();
   for ( ; n != NULL; n = n->getNextSibling() )
   {
      // ignore comments, etc.
      if ( n->getNodeType() == XMLNode::ELEMENT_NODE )
      {
         elements.push_back( n );
      }
   }
}

unsigned int PackageProfile::GetSCORM_SCO_Count()
{
   // find start node
   const XMLNode *n = NULL;
   n = getElement( getRoot(), "SCORM" );
   n = getElement( n, "assessment" );

   // number of SCOs
   unsigned int count = 0;

   // find all SCO child elements
   n = n->getFirstChild();
   for ( ; n != NULL; n = n->getNextSibling() )
   {
      // ignore comments, etc.
      if ( n->getNodeType() == XMLNode::ELEMENT_NODE )
      {
         char *pname = XMLString::transcode( n->getNodeName() );
         std::string nodeName = pname;
         XMLString::release( &pname );

         // found one
         if ( nodeName.compare( "sco" ) == 0 )
         {
            count++;
         }         
      }
   }

   return count;
}

void PackageProfile::GetSCORM_SCO_Options( unsigned int scoIdx, Options &options )
{
   // get the SCO we are interested in
   const XMLNode *sco = findSCORMSCO( scoIdx );
   
   // attributes represent options per SCO
   if ( sco->hasAttributes() )
   {
      DOMNamedNodeMap *attributes = sco->getAttributes();
      for( unsigned int i=0; i<attributes->getLength(); i++ )
      {
         DOMAttr *attr = (DOMAttr*)attributes->item( i );
         char *key = XMLString::transcode( attr->getName() );
         char *val = XMLString::transcode( attr->getValue() );
         options.Set( std::string(key), std::string(val) );
         XMLString::release( &key );
         XMLString::release( &val );
      }
   }
}

void PackageProfile::GetSCORM_SCO_LaunchPage( unsigned int scoIdx, std::string &launchPage )
{
   // get the SCO we are interested in
   const XMLNode *sco = findSCORMSCO( scoIdx );

   // traverse child elements looking for launch page
   XMLNode *n = sco->getFirstChild();
   for ( ; n != NULL; n = n->getNextSibling() )
   {
      // ignore comments, etc.
      if ( n->getNodeType() == XMLNode::ELEMENT_NODE )
      {
         char *pname = XMLString::transcode( n->getNodeName() );
         std::string nodeName = pname;
         XMLString::release( &pname );

         // found it
         if ( nodeName.compare( "scolaunchpage" ) == 0 )
         {
            launchPage = getElementContent( n );
            return;
         }
      }
   }
}

void PackageProfile::GetSCORM_SCO_LaunchParams( unsigned int scoIdx, std::string &launchParams )
{
   // get the SCO we are interested in
   const XMLNode *sco = findSCORMSCO( scoIdx );

   // traverse child elements looking for launch parameters
   XMLNode *n = sco->getFirstChild();
   for ( ; n != NULL; n = n->getNextSibling() )
   {
      // ignore comments, etc.
      if ( n->getNodeType() == XMLNode::ELEMENT_NODE )
      {
         char *pname = XMLString::transcode( n->getNodeName() );
         std::string nodeName = pname;
         XMLString::release( &pname );

         // found it
         if ( nodeName.compare( "scolaunchparams" ) == 0 )
         {
            launchParams = getElementContent( n );
            return;
         }
      }
   }
}

void PackageProfile::GetSCORM_SCO_Objectives( unsigned int scoIdx, std::vector<std::string> &objectives, std::vector<Options> &options )
{
   // get the SCO we are interested in
   const XMLNode *sco = findSCORMSCO( scoIdx );
   const XMLNode *n   = getElement( sco, "objectives" );
  
   // start with empty vectors
   objectives.clear();
   options.clear();

   // recursively search for all objectives
   findSCORMSCOObjectives( n, objectives, options );
}


//======================================
// PRIVATE FUNCTIONS
//======================================

void PackageProfile::unload()
{
   // dump the xml file loaded into memory
   if ( mParser )
   {
      mParser->resetDocumentPool();
      mParser->release();
      mParser   = NULL;
      mDocument = NULL;
   }
}

void PackageProfile::setFlag( PackageProfileFlag flag, bool on )
{
   if ( on ) mFlags |= flag;
   else      mFlags &= ~flag;
}

bool PackageProfile::getFlag( PackageProfileFlag flag ) const
{
   return ( ( mFlags & flag ) != 0 );
}

const XMLNode *PackageProfile::getRoot() const
{
   return (XMLNode*)mDocument->getDocumentElement();
}

const XMLNode *PackageProfile::getElement( const XMLNode *parent, std::string name )
{
   // error check
   if ( name.length() == 0 ) return NULL;

   // do search to find the element
   const XMLNode *element = getElementBFS( parent, name );
   
   // handle missing element
   if ( element == NULL )
   {
      std::cout << "[Warning] Request for missing element: '" << name << "'." << std::endl;
   }
   
   return element;
}

const XMLNode *PackageProfile::getElementBFS( const XMLNode *parent, std::string name )
{
   // init
   mXmlSearchQueue.clear();
   mXmlSearchQueue.push_back( parent );

   // breadth first search
   while ( !mXmlSearchQueue.empty() )
   {
      // dequeue node
      const XMLNode *node = mXmlSearchQueue.front();
      mXmlSearchQueue.erase( mXmlSearchQueue.begin() );
      if ( node == NULL ) continue;

      // is this the element we want?
      char *nodeName = XMLString::transcode( node->getNodeName() );
      if ( strcmp( nodeName, name.c_str() ) == 0 )
      {
         XMLString::release( &nodeName );
         return node;
      }
      XMLString::release( &nodeName );

      // enqueue children of node
      XMLNode *itr = node->getFirstChild(); 
      for ( ; itr != NULL; itr = itr->getNextSibling() )
      {
         if ( itr->getNodeType() == XMLNode::ELEMENT_NODE )
            mXmlSearchQueue.push_back( itr );
      }
   }

   // element not found
   return NULL;
}

std::string PackageProfile::getElementContent( const XMLNode *element ) const
{
   // error check
   if ( element == NULL || element->getNodeType() != XMLNode::ELEMENT_NODE )
      return std::string();

   // retrieve element's text content and return
   char *value = XMLString::transcode( element->getTextContent() );
   if ( value == NULL ) return std::string();
   std::string content = value;
   XMLString::release( &value );
   return content;
}

std::string PackageProfile::GetElementAttribute( const XMLNode *element,
                                                 std::string attrName ) const
{
   // error check
   if ( element == NULL                                 ||
        element->getNodeType() != XMLNode::ELEMENT_NODE ||
        element->hasAttributes() == false               ||
        attrName.length() == 0                          )
      return std::string();

   // retrieve element's specified attribute
   DOMNamedNodeMap *attributes = element->getAttributes();
   DOMAttr *attr = (DOMAttr*)attributes->getNamedItem(XMLString::transcode(attrName.c_str()));

   // give it up!
   if ( attr != NULL )
   {
      char *value = XMLString::transcode( attr->getValue() );
      std::string ret = value;
      XMLString::release( &value );
      return ret;
   }
   else
   {
      return std::string();
   }
}

const XMLNode *PackageProfile::findSCORMSCO( unsigned int scoIdx )
{
   // find start node
   const XMLNode *n = NULL;
   n = getElement( getRoot(), "SCORM" );
   n = getElement( n, "assessment" );

   // index into array of sco elements
   unsigned int idx = 0;

   // traverse SCO child elements
   n = n->getFirstChild();
   for ( ; n != NULL; n = n->getNextSibling() )
   {
      // ignore comments, etc.
      if ( n->getNodeType() == XMLNode::ELEMENT_NODE )
      {
         char *pname = XMLString::transcode( n->getNodeName() );
         std::string nodeName = pname;
         XMLString::release( &pname );

         // found one
         if ( nodeName.compare( "sco" ) == 0 )
         {
            // this is the one we are looking for
            if ( idx == scoIdx )
            {
               return n;
            }

            idx++;
         }
      }
   }

   return NULL;
}

void PackageProfile::findSCORMSCOObjectives( const XMLNode *scoElement,
                                             std::vector<std::string> &objectives,
                                             std::vector<Options> &options )
{
   // traverse all children elements
   XMLNode *itr = scoElement->getFirstChild();
   for ( ; itr != NULL; itr = itr->getNextSibling() )
   {
      if ( itr->getNodeType() == XMLNode::ELEMENT_NODE )
      {
         char *elementName = XMLString::transcode( itr->getNodeName() );
         std::string name = elementName;
         XMLString::release( &elementName );

         // found an objective!
         if ( name.compare( "objective" ) == 0 )
         {
            Options opts;

            // attributes represent options per element
            if ( itr->hasAttributes() )
            {
               DOMNamedNodeMap *attributes = itr->getAttributes();
               for( unsigned int i=0; i<attributes->getLength(); i++ )
               {
                  DOMAttr *attr = (DOMAttr*)attributes->item( i );
                  char *key = XMLString::transcode( attr->getName() );
                  char *val = XMLString::transcode( attr->getValue() );
                  opts.Set( std::string(key), std::string(val) );
                  XMLString::release( &key );
                  XMLString::release( &val );
               }
            }

            // store the objective and its attributes
            objectives.push_back( name );
            options.push_back( opts );
         }

         if ( itr->hasChildNodes() )
         {
            findSCORMSCOObjectives( itr, objectives, options );
         }
      }
   }
}
