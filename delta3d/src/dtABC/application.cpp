#include <dtABC/application.h>
#include <dtABC/applicationkeyboardlistener.h>
#include <dtABC/applicationmouselistener.h>

#include <dtCore/stats.h>
#include <dtCore/system.h>
#include <dtUtil/log.h>
#include <dtUtil/xerceswriter.h>
#include <dtUtil/xerceserrorhandler.h>
#include <dtUtil/xercesutils.h>
#include <dtUtil/xercesparser.h>

#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMElement.hpp>

#include <osgDB/FileUtils>

#include <cassert>

using namespace dtABC;
XERCES_CPP_NAMESPACE_USE

IMPLEMENT_MANAGEMENT_LAYER(Application)

Application::Application(const std::string& configFilename) : BaseABC("Application")
{
   RegisterInstance(this);

   if( !configFilename.empty() )
   {
      std::string foundPath = osgDB::findDataFile(configFilename);
      if( foundPath.empty() )
      {
         LOG_WARNING("Application: Can't find config file, " + configFilename + ", using defaults instead.")
         CreateInstances(); //create default window, camera, etc.
      }
      else if( !ParseConfigFile( foundPath ) )
      {
         LOG_WARNING("Application: Error loading config file, using defaults instead.");
         CreateInstances(); //create default window, camera, etc.
      }
   }
   else
   {
      CreateInstances(); //create default window, camera, etc.
   }
}

/** destructor */
Application::~Application(void)
{  
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

   mKeyboard->AddKeyboardListener( this->GetApplicationKeyboardListener() );

   mMouse = mWindow->GetMouse();
   assert( mMouse.get() );

   mMouse->AddMouseListener( this->GetApplicationMouseListener() );
}


/** Read an existing data file and setup the internal class
  * members with attributes from the data file.
  */
bool Application::ParseConfigFile(const std::string& file)
{
   AppXMLContentHandler handler(this);
   dtUtil::XercesParser parser;
   return parser.Parse(file, handler, "application.xsd");
}


/** This method writes out all the default attributes from the internal Application
  * members and writes them out to a .xml file ("config.xml").
  */
std::string dtABC::Application::GenerateDefaultConfigFile()
{
   std::string filename("config.xml");
   std::string existingfile = osgDB::findDataFile( filename );

   if( !existingfile.empty() )
   {
      LOG_WARNING("Can't generate new configuration file: file already exists: " + existingfile);
      return existingfile;
   }

   // write out a default config file
   dtCore::RefPtr<dtUtil::XercesWriter> writer = new dtUtil::XercesWriter();

   writer->CreateDocument( "Application" );
   XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc = writer->GetDocument();
   DOMElement* app = doc->getDocumentElement();

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

   XMLCh* SCENE = XMLString::transcode("Scene");
   DOMElement* scene = doc->createElement(SCENE);
   XMLString::release( &SCENE );
   XMLCh* DEFAULTSCENE = XMLString::transcode("defaultScene");
   scene->setAttribute( NAME , DEFAULTSCENE );

   app->appendChild( scene );

   XMLCh* CAMERA = XMLString::transcode("Camera");
   DOMElement* camera = doc->createElement(CAMERA);
   XMLString::release( &CAMERA );

   XMLCh* DEFAULTCAM = XMLString::transcode("defaultCam");
   camera->setAttribute( NAME , DEFAULTCAM );
   XMLString::release( &DEFAULTCAM );

   XMLCh* WINDOWINSTANCE = XMLString::transcode("WindowInstance");
   XMLCh* SCENEINSTANCE = XMLString::transcode("SceneInstance");
   camera->setAttribute( WINDOWINSTANCE , DEFAULTWIN );
   camera->setAttribute( SCENEINSTANCE , DEFAULTSCENE );
   XMLString::release( &DEFAULTSCENE );
   XMLString::release( &WINDOWINSTANCE );
   XMLString::release( &SCENEINSTANCE );
   XMLString::release( &DEFAULTSCENE );

   app->appendChild( camera );

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

   return osgDB::findDataFile(filename);  // from #include <osgDB/FileNameUtils>
}

void Application::AppXMLContentHandler::startElement(const XMLCh* const uri,
                                                     const XMLCh* const localname,
                                                     const XMLCh* const qname,
                                                     const XERCES_CPP_NAMESPACE_QUALIFIER Attributes& attrs)
{
   char* elementname = XMLString::transcode( localname );
   std::string ename( elementname );
   XMLString::release( &elementname );

   if( ename == "Window" )
   {
      // set up some defaults
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

   if( ename == "Scene" )
   {
      dtCore::Scene* scene = mApp->GetScene();

      dtUtil::AttributeSearch sceneattrs;
      sceneattrs.GetSearchKeys().push_back("Name");
      dtUtil::AttributeSearch::ResultMap results = sceneattrs( attrs );

      dtUtil::AttributeSearch::ResultMap::iterator iter = results.find("Name");
      if( iter != results.end() )
      {
         scene->SetName( (*iter).second );
      }
   }

   if( ename == "Camera" )
   {
      // push some keys
      dtUtil::AttributeSearch camattrs;
      camattrs.GetSearchKeys().push_back("Name");
      camattrs.GetSearchKeys().push_back("WindowInstance");
      camattrs.GetSearchKeys().push_back("SceneInstance");

      // do the attribute search, catch the results
      dtUtil::AttributeSearch::ResultMap results = camattrs( attrs );

      dtUtil::AttributeSearch::ResultMap::iterator iter = results.find("Name");
      if( iter != results.end() )
      {
         mApp->GetCamera()->SetName( (*iter).second );
      }

      iter = results.find("WindowInstance");
      if( iter != results.end() )
      {
         if(dtCore::DeltaWin* win = dtCore::DeltaWin::GetInstance( (*iter).second) )
         {
            mApp->GetCamera()->SetWindow( win );
         }
         else
         {
            LOG_WARNING("Application:Can't find instance of DeltaWin, " + (*iter).second )
         }
      }

      iter = results.find("SceneInstance");
      if( iter != results.end() )
      {
         if(dtCore::Scene* scene = dtCore::Scene::GetInstance( (*iter).second) )
         {
            mApp->GetCamera()->SetScene( scene );
         }
         else
         {
            LOG_WARNING("Application:Can't find instance of Scene, " + (*iter).second )
         }
      }
   }
}
