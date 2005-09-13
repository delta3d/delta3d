#include "dtABC/application.h"
#include "dtCore/system.h"
#include "dtUtil/log.h"
#include "dtUtil/xerceswriter.h"
#include "dtUtil/xerceserrorhandler.h"
#include "dtUtil/stringutils.h"

#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMElement.hpp>

#include <osgDB/FileUtils>

#include <cassert>

using namespace dtABC;
XERCES_CPP_NAMESPACE_USE

IMPLEMENT_MANAGEMENT_LAYER(Application)

Application::Application(const std::string& configFilename) :  BaseABC("Application")
{
   RegisterInstance(this);

   std::string foundPath = osgDB::findDataFile(configFilename);
   if( foundPath.empty() )
   {
      LOG_WARNING("Application: Can't find config file, " + configFilename + ", using defaults instead.")
      GenerateDefaultConfigFile();
      CreateInstances(); //create default window, camera, etc.
   }

   else
   {
      // parse the file
      if( !ParseConfigFile( foundPath ) )
      {
         LOG_WARNING("Application: Error loading config file, using defaults instead.");
         CreateInstances(); //create default window, camera, etc.
      }
   }
}



/** destructor */
Application::~Application(void)
{
   #if !defined(_WIN32) && !defined(WIN32) && !defined(__WIN32__)
   //mWindow->ChangeScreenResolution( mOriginalRes );
   #endif
   
   DeregisterInstance(this);   
}



/** This method kicks off the forever process which will start triggering the
  * PreFrame(), Frame(), and PostFrame() methods.
  */
void  Application::Run( void )
{
   dtCore::System::Instance()->Run();
}


/** protected methods */
/** virtual*/
void  Application::PreFrame( const double deltaFrameTime )
{
   // NOP
}

/** virtual*/
void  Application::Frame( const double deltaFrameTime )
{
   // NOP
}

/** virtual*/
void  Application::PostFrame( const double deltaFrameTime )
{
   // NOP
}



/** Private methods */
/** Create basic instances and set up system hooks */
void  Application::CreateInstances(const std::string& name, int x, int y, int width, int height, bool cursor, bool fullScreen )
{
   //create the instances and hook-up the default
   //connections.  The instance attributes may be
   //overwritten using a config file
   BaseABC::CreateInstances();

   mWindow = new dtCore::DeltaWin( name, x, y, width, height, cursor, fullScreen );
   
   assert( mWindow.get() );

   mCamera->SetWindow( mWindow.get() );

   mKeyboard = mWindow->GetKeyboard();
   assert( mKeyboard.get() );

   mKeyboard->AddKeyboardListener( this );

   mMouse = mWindow->GetMouse();
   assert( mMouse.get() );

   mMouse->AddMouseListener( this );
}


/** Read an existing data file and setup the internal class
  * members with attributes from the data file.
  *
  * \todo does this need to initialize xerces xml utils?
  */
bool Application::ParseConfigFile(const std::string& file)
{
   LOG_INFO("Parsing configuration file, " + file)

   bool retval(false);

   try  // to initialize the xml tools
   {
      XMLPlatformUtils::Initialize();
   }
   catch(const XMLException& e)
   {
      char* message = XMLString::transcode( e.getMessage() );
      std::string msg(message);
      LOG_ERROR("An exception occurred during XMLPlatformUtils::Initialize() with message: " + msg);
      XMLString::release( &message );
      return false;
   }
   catch(...)
   {
      LOG_ERROR("An exception occurred during XMLPlatformUtils::Initialize()");
      return false;
   }

   SAX2XMLReader* parser;
   try  // to create a reader
   {
      parser = XMLReaderFactory::createXMLReader();
   }
   catch(...)
   {
      LOG_ERROR("Could not create a Xerces SAX2XMLReader")
      return false;
   }

   // set the error handler
   dtUtil::XercesErrorHandler errors;
   parser->setErrorHandler( &errors );

   // set the content handler
   AppXMLContentHandler handler(this);
   parser->setContentHandler( &handler );

   try  // to parse
   {
      parser->parse( file.c_str() );
      retval = true;
   }
   catch(const XMLException& e)
   {
      char* message = XMLString::transcode( e.getMessage() );
      std::string msg(message);
      LOG_ERROR("An exception occurred while parsing file, " + file + ", with message: " + msg)
      XMLString::release( &message );

      delete parser;
      return false;
   }
   catch(...)
   {
      LOG_ERROR("An exception occurred while parsing file, " + file)

      delete parser;
      return false;
   }

   try
   {
      XMLPlatformUtils::Terminate();
   }
   catch(const XMLException& e)
   {
      char* message = XMLString::transcode( e.getMessage() );
      std::string msg(message);
      LOG_ERROR("An exception occurred during XMLPlatformUtils::Terminate() with message: " + msg);
      XMLString::release( &message );

      delete parser;
      return false;
   }
   catch(...)
   {
      LOG_ERROR("An exception occurred during XMLPlatformUtils::Terminate()");

      delete parser;
      return false;
   }

   return retval;
}


/** This method writes out all the default attributes from the internal Application
  * members and writes them out to a .xml file ("config.xml").
  *
  * \todo clean out the commented code.
  */
std::string dtABC::Application::GenerateDefaultConfigFile()
{
   //TiXmlDocument *xmlDoc = new TiXmlDocument( filename.c_str() );
   //if( xmlDoc->LoadFile( "config.xml" ) )
   //{
   //   ///\todo Look at this - both of these lines are questionable.
   //   LOG_WARNING("Application found existing config.xml, delete file before generating a new one.");
   //   return std::string("");    // commented this line so that the above statement was true.
   //}
   //TiXmlDocument xml( "config.xml" );
   //TiXmlDeclaration dec( "1.0", "iso-8859-1", "no" );
   //xml.InsertEndChild( dec );

   std::string filename("config.xml");
   std::string existingfile = osgDB::findDataFile( filename );

   if( !existingfile.empty() )
   {
      LOG_WARNING("Application found existing configuration file, "+ existingfile + ", delete that file if generating a new one is desired.");
      return existingfile;
   }

   ///\todo find out if we need to initialize the xerces utils before allocating a XercesWriter
   // write out a default config file
   dtCore::RefPtr<dtUtil::XercesWriter> writer = new dtUtil::XercesWriter();

   //TiXmlElement app( "Application" );
   writer->CreateDocument( "Application" );
   XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc = writer->GetDocument();
   DOMElement* app = doc->getDocumentElement();

   //TiXmlElement win( "Window" );
   //win.SetAttribute( "Name", "defaultWin" );
   //win.SetAttribute( "X", 100 );
   //win.SetAttribute( "Y", 100 );
   //win.SetAttribute( "Width", 640 );
   //win.SetAttribute( "Height", 480 );
   //win.SetAttribute( "PixelDepth", 24 ); 
   //win.SetAttribute( "RefreshRate", 60 ); 
   //win.SetAttribute( "ShowCursor", 1 );
   //win.SetAttribute( "FullScreen", 0 );
   //win.SetAttribute( "ChangeDisplayResolution", 0 );
   //app.InsertEndChild(win);
   XMLCh* NAME = XMLString::transcode("Name");
   XMLCh* DEFAULTWIN = XMLString::transcode("defaultWin");

   XMLCh* X = XMLString::transcode("X");
   XMLCh* Y = XMLString::transcode("Y");
   XMLCh* WIDTH = XMLString::transcode("Width");
   XMLCh* HEIGHT = XMLString::transcode("Height");

   XMLCh* PIXELDEPTH = XMLString::transcode("PixelDepth");
   XMLCh* REFRESHRATE = XMLString::transcode("RefreshRate");
   XMLCh* SHOWCURSOR = XMLString::transcode("ShowCursor");
   XMLCh* FULLSCREEN = XMLString::transcode("FullScreen");
   XMLCh* CHANGEDISPLAYRESOLUTION = XMLString::transcode("ChangeDisplayResolution");

   XMLCh* ONEHUNDRED = XMLString::transcode("100");
   XMLCh* SIXFORTY = XMLString::transcode("640");
   XMLCh* FOUREIGHTY = XMLString::transcode("480");
   XMLCh* SIXTY = XMLString::transcode("60");
   XMLCh* TWENTYFOUR = XMLString::transcode("24");
   XMLCh* ONE = XMLString::transcode("1");
   XMLCh* ZERO = XMLString::transcode("0");

   XMLCh* WINDO = XMLString::transcode("Window");
   DOMElement* windo = doc->createElement(WINDO);
   XMLString::release( &WINDO );
   windo->setAttribute( NAME, DEFAULTWIN );
   windo->setAttribute( X , ONEHUNDRED );
   windo->setAttribute( Y , ONEHUNDRED );
   windo->setAttribute( WIDTH , SIXFORTY );
   windo->setAttribute( HEIGHT , FOUREIGHTY );
   windo->setAttribute( PIXELDEPTH , TWENTYFOUR );
   windo->setAttribute( REFRESHRATE, SIXTY );
   windo->setAttribute( SHOWCURSOR, ONE );
   windo->setAttribute( FULLSCREEN , ZERO );
   windo->setAttribute( CHANGEDISPLAYRESOLUTION, ZERO );

   app->appendChild( windo );

   //TiXmlElement scene( "Scene" );
   //scene.SetAttribute( "Name", "defaultScene" );
   //app.InsertEndChild( scene );
   XMLCh* SCENE = XMLString::transcode("Scene");
   DOMElement* scene = doc->createElement(SCENE);
   XMLString::release( &SCENE );

   app->appendChild( scene );

   //TiXmlElement cam( "Camera" );
   //cam.SetAttribute( "Name", "defaultCam" );
   //cam.SetAttribute( "WindowInstance", "defaultWin" );
   //cam.SetAttribute( "SceneInstance", "defaultScene" );
   //app.InsertEndChild( cam );
   XMLCh* CAMERA = XMLString::transcode("Camera");
   DOMElement* camera = doc->createElement(CAMERA);
   XMLString::release( &CAMERA );

   XMLCh* DEFAULTCAM = XMLString::transcode("defaultCam");
   camera->setAttribute( NAME , DEFAULTCAM );
   XMLString::release( &DEFAULTCAM );

   XMLCh* WINDOWINSTANCE = XMLString::transcode("WindowInstance");
   XMLCh* SCENEINSTANCE = XMLString::transcode("SceneInstance");
   XMLCh* DEFAULTSCENE = XMLString::transcode("DefaultScene");
   camera->setAttribute( WINDOWINSTANCE , DEFAULTWIN );
   camera->setAttribute( SCENEINSTANCE , DEFAULTSCENE );
   XMLString::release( &WINDOWINSTANCE );
   XMLString::release( &SCENEINSTANCE );
   XMLString::release( &DEFAULTSCENE );

   app->appendChild( camera );

   //xml.InsertEndChild( app );

   //xml.SaveFile();
   writer->WriteFile( filename );

   XMLString::release( &NAME );
   XMLString::release( &DEFAULTWIN );

   XMLString::release( &X );
   XMLString::release( &Y );
   XMLString::release( &WIDTH );
   XMLString::release( &HEIGHT );

   XMLString::release( &PIXELDEPTH );
   XMLString::release( &REFRESHRATE );
   XMLString::release( &SHOWCURSOR );
   XMLString::release( &FULLSCREEN );
   XMLString::release( &CHANGEDISPLAYRESOLUTION );

   XMLString::release( &ONEHUNDRED );
   XMLString::release( &SIXFORTY );
   XMLString::release( &FOUREIGHTY );
   XMLString::release( &SIXTY );
   XMLString::release( &TWENTYFOUR );
   XMLString::release( &ONE );
   XMLString::release( &ZERO );

   // Termination is not needed.
   //try
   //{
   //   XMLPlatformUtils::Terminate();
   //}
   //catch(const XMLException& e)
   //{
   //   char* message = XMLString::transcode( e.getMessage() );
   //   std::string msg(message);
   //   LOG_ERROR("An exception occurred during XMLPlatformUtils::Terminate() with message: " + msg);
   //   XMLString::release( &message );
   //   return filename;
   //}
   //catch(...)
   //{
   //   LOG_ERROR("An exception occurred during XMLPlatformUtils::Terminate()");
   //   return filename;
   //}

   return osgDB::findDataFile(filename);  // from #include <osgDB/FileNameUtils>
}

void Application::AppXMLContentHandler::startElement(const XMLCh* const uri,
                                                     const XMLCh* const localname,
                                                     const XMLCh* const qname,
                                                     const Attributes& attrs)
{
   char* elementname = XMLString::transcode( localname );
   std::string ename( elementname );
   XMLString::release( &elementname );

   if( ename == "Window" )
   {
      // --- attributes --- //
      std::string name("DefaultApp"), xstring("100"), ystring("100"), wstring("640"), hstring("480"), showstring("1"), fullstring("0");

      // search for specific named attributes, append the list of searched strings
      dtUtil::AttributeSearch windowattrs;
      windowattrs.GetSearchKeys().push_back("Name");
      windowattrs.GetSearchKeys().push_back("X");
      windowattrs.GetSearchKeys().push_back("Y");
      windowattrs.GetSearchKeys().push_back("Width");
      windowattrs.GetSearchKeys().push_back("Height");
      windowattrs.GetSearchKeys().push_back("ShowCursor");
      windowattrs.GetSearchKeys().push_back("FullScreen");

      // perform the search
      typedef dtUtil::AttributeSearch::ResultMap RMap;
      RMap results = windowattrs( attrs );

      // if the named attributes were found, use them, else use defaults
      RMap::iterator iter = results.find("Name");
      if( iter != results.end() )
         name = (*iter).second;

      iter = results.find("X");
      if( iter != results.end() )
         xstring = (*iter).second;

      iter = results.find("Y");
      if( iter != results.end() )
         ystring = (*iter).second;

      iter = results.find("Width");
      if( iter != results.end() )
         wstring = (*iter).second;

      iter = results.find("Height");
      if( iter != results.end() )
         hstring = (*iter).second;

      iter = results.find("ShowCursor");
      if( iter != results.end() )
         showstring = (*iter).second;

      iter = results.find("FullScreen");
      if( iter != results.end() )
         fullstring = (*iter).second;

      int winX = atoi(xstring.c_str());
      int winY = atoi(ystring.c_str());
      int width = atoi(wstring.c_str());
      int height = atoi(hstring.c_str());
      bool showCursor = atoi(showstring.c_str());
      bool fullScreen = atoi(fullstring.c_str());

      mApp->CreateInstances( name, winX, winY, width, height, showCursor, fullScreen );
   }

   //TiXmlElement* win = rootNode->FirstChildElement( "Window" );
   //if (win != NULL)
   //{
   //   std::string name( "defaultWin" );
   //   if( const char* nameChar = win->Attribute( "Name" ) )
   //      name = nameChar;

   //   int width( 640 );
   //   if( const char* widthChar = win->Attribute( "Width" ) )
   //      width = atoi( widthChar );

   //   int height( 480 );
   //   if( const char* heightChar = win->Attribute( "Height" ) )
   //      height = atoi( heightChar );

   //   int winX( 100 );
   //   if( const char* winXChar = win->Attribute( "X" ) )
   //      winX = atoi( winXChar );

   //   int winY( 100 );
   //   if( const char* winYChar = win->Attribute( "Y" ) )
   //      winY = atoi( winYChar );

   //   bool showCursor( true );
   //   if( const char* showCursorChar = win->Attribute( "ShowCursor" ) )
   //      showCursor = atoi( showCursorChar );

   //   bool fullScreen( false );
   //   if( const char* fullScreenChar = win->Attribute( "FullScreen" ) )
   //      fullScreen = atoi( fullScreenChar );
   //      
   //   int pixelDepth( 24 );
   //   if( const char* pixelChar = win->Attribute( "PixelDepth" ) )
   //      pixelDepth = atoi( pixelChar );

   //   int refreshRate( 60 );
   //   if( const char* refreshChar    = win->Attribute("RefreshRate") )
   //      refreshRate = atoi( refreshChar );

   //   bool changeRes( false );
   //   if( const char* changeResChar  = win->Attribute("ChangeDisplayResolution") )
   //      changeRes = atoi( changeResChar );

   //   if( changeRes )
   //      DeltaWin::ChangeScreenResolution( width, height, pixelDepth, refreshRate );

   //   CreateInstances( name, winX, winY, width, height, showCursor, fullScreen );

   //}

   if( ename == "Scene" )
   {
      dtUtil::AttributeSearch sceneattrs;
      sceneattrs.GetSearchKeys().push_back("Name");

      dtUtil::AttributeSearch::ResultMap results = sceneattrs( attrs );

      dtUtil::AttributeSearch::ResultMap::iterator iter = results.find("Name");
      if( iter != results.end() )
      {
         mApp->GetScene()->SetName( (*iter).second );
      }
   }

   //TiXmlElement*  scene = rootNode->FirstChildElement("Scene");
   //if (scene != NULL)
   //{
   //   std::string name = scene->Attribute("Name");

   //   mScene->SetName(name);
   //}
   //

   if( ename == "Camera" )
   {
      dtUtil::AttributeSearch camattrs;
      camattrs.GetSearchKeys().push_back("Name");
      //camattrs.GetSearchKeys().push_back("WindowInstance");
      //camattrs.GetSearchKeys().push_back("SceneInstance");

      dtUtil::AttributeSearch::ResultMap results = camattrs( attrs );

      dtUtil::AttributeSearch::ResultMap::iterator iter = results.find("Name");
      if( iter != results.end() )
      {
         mApp->GetCamera()->SetName( (*iter).second );
      }
   }

   //TiXmlElement* cam = rootNode->FirstChildElement("Camera");
   //if (cam != NULL)
   //{
   //   std::string name = cam->Attribute("Name");
   //   std::string winInst = cam->Attribute("WindowInstance");
   //   std::string sceneInst = cam->Attribute("SceneInstance");

   //   mCamera->SetName(name);
   //   DeltaWin* win = DeltaWin::GetInstance(winInst);
   //   if (win != NULL)
   //      mCamera->SetWindow(win);
   //   else
   //      Log::GetInstance().LogMessage( Log::LOG_WARNING, __FILE__, "Application:Can't find instance of DeltaWin '%s'", winInst.c_str() );

   //   Scene* scene = Scene::GetInstance(sceneInst);
   //   if (scene != NULL)
   //      mCamera->SetScene(scene);
   //   else
   //      Log::GetInstance().LogMessage( Log::LOG_WARNING, __FILE__, "Application:Can't find instance of Scene '%s'", sceneInst.c_str() );
   //}
}
