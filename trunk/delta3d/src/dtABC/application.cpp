#include <dtABC/application.h>
#include <dtCore/generickeyboardlistener.h>

#include <dtCore/stats.h>
#include <dtCore/system.h>
#include <dtCore/camera.h>
#include <dtCore/scene.h>
#include <dtUtil/log.h>
#include <dtUtil/xerceswriter.h>
#include <dtUtil/xerceserrorhandler.h>
#include <dtUtil/xercesutils.h>
#include <dtUtil/xercesparser.h>
#include <dtCore/mouse.h>

#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMElement.hpp>

#include <osgDB/FileUtils>

#include <cassert>

using namespace dtABC;
XERCES_CPP_NAMESPACE_USE

IMPLEMENT_MANAGEMENT_LAYER(Application)

Application::Application(const std::string& configFilename) : BaseABC("Application"),
   mKeyboardListener(new dtCore::GenericKeyboardListener())
{
   RegisterInstance(this);

   mKeyboardListener->SetPressedCallback(dtCore::GenericKeyboardListener::CallbackType(this,&Application::KeyPressed));

   CreateInstances(); //create default window, camera, etc.

   if( !configFilename.empty() )
   {
      std::string foundPath = osgDB::findDataFile(configFilename);
      if( foundPath.empty() )
      {
         LOG_WARNING("Application: Can't find config file, " + configFilename + ", using defaults instead.")
      }
      else if( !ParseConfigFile( foundPath ) )
      {
         LOG_WARNING("Application: Error loading config file, using defaults instead.");
      }
   }
}

Application::~Application(void)
{  
   DeregisterInstance(this);   
}

void Application::Run( void )
{
   dtCore::System::Instance()->Run();
}

void Application::PreFrame( const double deltaFrameTime )
{
}

void Application::Frame( const double deltaFrameTime )
{
}

void Application::PostFrame( const double deltaFrameTime )
{
}

bool Application::KeyPressed(const dtCore::Keyboard* keyboard, Producer::KeyboardKey key, Producer::KeyCharacter character)
{
   switch (key)
   {
   case Producer::Key_Escape:
      {
         Quit();
         return true;
      } break;
   default:
      {
         return false;
      } break;
   }

   return false;
}


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

   mMouse = mWindow->GetMouse();
   assert( mMouse.get() );

   mKeyboard->AddKeyboardListener( mKeyboardListener.get() );
}

bool Application::ParseConfigFile(const std::string& file)
{
   AppXMLContentHandler handler(this);
   dtUtil::XercesParser parser;
   return parser.Parse(file, handler, "application.xsd");
}

std::string dtABC::Application::GenerateDefaultConfigFile(const std::string& filename)
{
   std::string existingfile = osgDB::findDataFile( filename );

   if( !existingfile.empty() )
   {
      LOG_WARNING("Can't generate new configuration file: file already exists: " + existingfile);
      return existingfile;
   }

   // write out a new file
   AppConfigWriter writer;
   writer( filename );

   // return the resource path to the new file
   return osgDB::findDataFile( filename );
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
      std::string name("DefaultApp"),
                  xstring("100"), ystring("100"),
                  wstring("640"), hstring("480"),
                  showstring("1"), fullstring("0"),
                  changeres("0"), bitdepth("24"), refresh("60");

      // search for specific named attributes, append the list of searched strings
      dtUtil::AttributeSearch windowattrs;

      // perform the search
      typedef dtUtil::AttributeSearch::ResultMap RMap;
      RMap results = windowattrs( attrs );

      // if the named attributes were found, use them, else use defaults
      RMap::iterator iter = results.find("Name");
      if( iter != results.end() )
         name = iter->second;

      iter = results.find("X");
      if( iter != results.end() )
         xstring = iter->second;

      iter = results.find("Y");
      if( iter != results.end() )
         ystring = iter->second;

      iter = results.find("Width");
      if( iter != results.end() )
         wstring = iter->second;

      iter = results.find("Height");
      if( iter != results.end() )
         hstring = iter->second;

      iter = results.find("ShowCursor");
      if( iter != results.end() )
         showstring = iter->second;

      iter = results.find("FullScreen");
      if( iter != results.end() )
         fullstring = iter->second;

      iter = results.find("ChangeDisplayResolution");
      if( iter != results.end() )
         changeres = iter->second;

      iter = results.find("PixelDepth");
      if( iter != results.end() )
         bitdepth = iter->second;

      iter = results.find("RefreshRate");
      if( iter != results.end() )
         refresh = iter->second;

      int winX = atoi(xstring.c_str());
      int winY = atoi(ystring.c_str());
      int width = atoi(wstring.c_str());
      int height = atoi(hstring.c_str());
      bool showCursor = atoi(showstring.c_str());
      bool fullScreen = atoi(fullstring.c_str());
      bool resolution_changed = atoi(changeres.c_str());
      int depth = atoi(bitdepth.c_str());
      int rate = atoi(refresh.c_str());

      dtCore::DeltaWin* mWindow = mApp->GetWindow();
      mWindow->SetName(name);
      mWindow->SetPosition(winX,winY,width,height);
      mWindow->ShowCursor(showCursor);
      mWindow->SetFullScreenMode(fullScreen);

      if( resolution_changed )
      {
         dtCore::DeltaWin::Resolution res;
         res.width = width;
         res.height = height;
         res.bitDepth = depth;
         res.refresh = rate;
         if( mWindow->IsValidResolution(res) )
         {
            mWindow->ChangeScreenResolution(res);
         }
      }
   }

   if( ename == "Scene" )
   {
      dtCore::Scene* scene = mApp->GetScene();

      dtUtil::AttributeSearch sceneattrs;
      dtUtil::AttributeSearch::ResultMap results = sceneattrs( attrs );

      dtUtil::AttributeSearch::ResultMap::iterator iter = results.find("Name");
      if( iter != results.end() )
      {
         scene->SetName( iter->second );
      }
   }

   if( ename == "Camera" )
   {
      // push some keys
      dtUtil::AttributeSearch camattrs;

      // do the attribute search, catch the results
      dtUtil::AttributeSearch::ResultMap results = camattrs( attrs );

      dtUtil::AttributeSearch::ResultMap::iterator iter = results.find("Name");
      if( iter != results.end() )
      {
         mApp->GetCamera()->SetName( iter->second );
      }

      iter = results.find("WindowInstance");
      if( iter != results.end() )
      {
         if(dtCore::DeltaWin* win = dtCore::DeltaWin::GetInstance( iter->second) )
         {
            mApp->GetCamera()->SetWindow( win );
         }
         else
         {
            LOG_WARNING("Application:Can't find instance of DeltaWin, " + iter->second )
         }
      }

      iter = results.find("SceneInstance");
      if( iter != results.end() )
      {
         if(dtCore::Scene* scene = dtCore::Scene::GetInstance( iter->second) )
         {
            mApp->GetCamera()->SetScene( scene );
         }
         else
         {
            LOG_WARNING("Application:Can't find instance of Scene, " + iter->second )
         }
      }
   }
}


// --- App config code's implementation --- //
void Application::AppConfigWriter::operator ()(const std::string& filename)
{
   // initialize the xerces xml system.
   dtCore::RefPtr<dtUtil::XercesWriter> writer = new dtUtil::XercesWriter();

   // create instances of the models, using the xerces system,
   // so it needs to be initilized with the writer first.
   DefaultModel def;
   SchemaModel sch;

   // specify the name of the top node.
   writer->CreateDocument( "Application" );
   XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc = writer->GetDocument();
   DOMElement* app = doc->getDocumentElement();

   DOMElement* windo = doc->createElement(sch.WINDOW);
   windo->setAttribute( sch.NAME, def.WINDOW_NAME );
   windo->setAttribute( sch.X , def.WINDOW_X );
   windo->setAttribute( sch.Y , def.WINDOW_Y );
   windo->setAttribute( sch.WIDTH , def.WINDOW_WIDTH );
   windo->setAttribute( sch.HEIGHT , def.WINDOW_HEIGHT );
   windo->setAttribute( sch.PIXELDEPTH , def.PIXEL_DEPTH );
   windo->setAttribute( sch.REFRESHRATE, def.REFRESH );
   windo->setAttribute( sch.SHOWCURSOR, def.SHOW_CURSOR );
   windo->setAttribute( sch.FULLSCREEN , def.FULL_SCREEN );
   windo->setAttribute( sch.CHANGEDISPLAYRESOLUTION, def.CHANGE_RESOLUTION );
   app->appendChild( windo );

   DOMElement* scene = doc->createElement(sch.SCENE);
   scene->setAttribute( sch.NAME , def.SCENE_NAME );
   app->appendChild( scene );

   DOMElement* camera = doc->createElement(sch.CAMERA);
   camera->setAttribute( sch.NAME , def.CAMERA_NAME );
   camera->setAttribute( sch.WINDOW , def.WINDOW_NAME );
   camera->setAttribute( sch.SCENE , def.SCENE_NAME );
   app->appendChild( camera );

   writer->WriteFile( filename );
}

Application::AppConfigWriter::SchemaModel::SchemaModel()
{
   WINDOW = XMLString::transcode( Application::ConfigSchemaModel::WINDOW.c_str() );
   NAME = XMLString::transcode( Application::ConfigSchemaModel::NAME.c_str() );
   SCENE = XMLString::transcode( Application::ConfigSchemaModel::SCENE.c_str() );
   CAMERA = XMLString::transcode( Application::ConfigSchemaModel::CAMERA.c_str() );

   X = XMLString::transcode( Application::ConfigSchemaModel::X.c_str() );
   Y = XMLString::transcode( Application::ConfigSchemaModel::Y.c_str() );
   WIDTH = XMLString::transcode( Application::ConfigSchemaModel::WIDTH.c_str() );
   HEIGHT = XMLString::transcode( Application::ConfigSchemaModel::HEIGHT.c_str() );

   PIXELDEPTH = XMLString::transcode( Application::ConfigSchemaModel::PIXELDEPTH.c_str() );
   REFRESHRATE = XMLString::transcode( Application::ConfigSchemaModel::REFRESHRATE.c_str() );
   SHOWCURSOR = XMLString::transcode( Application::ConfigSchemaModel::SHOWCURSOR.c_str() );
   FULLSCREEN = XMLString::transcode( Application::ConfigSchemaModel::FULLSCREEN.c_str() );
   CHANGEDISPLAYRESOLUTION = XMLString::transcode( Application::ConfigSchemaModel::CHANGEDISPLAYRESOLUTION.c_str() );

   WINDOWINSTANCE = XMLString::transcode( Application::ConfigSchemaModel::WINDOWINSTANCE.c_str() );
   SCENEINSTANCE = XMLString::transcode( Application::ConfigSchemaModel::SCENEINSTANCE.c_str() );
}

Application::AppConfigWriter::SchemaModel::~SchemaModel()
{
   XMLString::release( &WINDOW );
   XMLString::release( &NAME );
   XMLString::release( &SCENE );
   XMLString::release( &CAMERA );

   XMLString::release( &X );
   XMLString::release( &Y );
   XMLString::release( &WIDTH );
   XMLString::release( &HEIGHT );

   XMLString::release( &PIXELDEPTH );
   XMLString::release( &REFRESHRATE );
   XMLString::release( &SHOWCURSOR );
   XMLString::release( &FULLSCREEN );
   XMLString::release( &CHANGEDISPLAYRESOLUTION );

   XMLString::release( &WINDOWINSTANCE );
   XMLString::release( &SCENEINSTANCE );
}

Application::AppConfigWriter::DefaultModel::DefaultModel()
{
   WINDOW_X = XMLString::transcode("100");
   WINDOW_Y = XMLString::transcode("100");
   WINDOW_WIDTH = XMLString::transcode("640");
   WINDOW_HEIGHT = XMLString::transcode("480");
   REFRESH = XMLString::transcode("60");
   PIXEL_DEPTH = XMLString::transcode("24");
   SHOW_CURSOR = XMLString::transcode("1");
   FULL_SCREEN = XMLString::transcode("0");
   CHANGE_RESOLUTION = XMLString::transcode("0");

   SCENE_NAME = XMLString::transcode("defaultScene");
   WINDOW_NAME = XMLString::transcode("defaultWin");
   CAMERA_NAME = XMLString::transcode("defaultCam");
}

Application::AppConfigWriter::DefaultModel::~DefaultModel()
{
   XMLString::release( &WINDOW_NAME );
   XMLString::release( &WINDOW_X );
   XMLString::release( &WINDOW_Y );
   XMLString::release( &WINDOW_WIDTH );
   XMLString::release( &WINDOW_HEIGHT );

   XMLString::release( &REFRESH );
   XMLString::release( &PIXEL_DEPTH );
   XMLString::release( &SHOW_CURSOR );
   XMLString::release( &FULL_SCREEN );

   XMLString::release( &SCENE_NAME );
   XMLString::release( &CAMERA_NAME );
}

// --- config model implementation --- //
const std::string Application::ConfigSchemaModel::WINDOW="Window";
const std::string Application::ConfigSchemaModel::NAME="Name";
const std::string Application::ConfigSchemaModel::SCENE="Scene";
const std::string Application::ConfigSchemaModel::CAMERA="Camera";

const std::string Application::ConfigSchemaModel::X="X";
const std::string Application::ConfigSchemaModel::Y="Y";
const std::string Application::ConfigSchemaModel::WIDTH="Width";
const std::string Application::ConfigSchemaModel::HEIGHT="Height";

const std::string Application::ConfigSchemaModel::PIXELDEPTH="PixelDepth";
const std::string Application::ConfigSchemaModel::REFRESHRATE="RefreshRate";
const std::string Application::ConfigSchemaModel::SHOWCURSOR="ShowCursor";
const std::string Application::ConfigSchemaModel::FULLSCREEN="FullScreen";
const std::string Application::ConfigSchemaModel::CHANGEDISPLAYRESOLUTION="ChangeDisplayResolution";

const std::string Application::ConfigSchemaModel::WINDOWINSTANCE="WINDOWINSTANCE";
const std::string Application::ConfigSchemaModel::SCENEINSTANCE="SCENEINSTANCE";
