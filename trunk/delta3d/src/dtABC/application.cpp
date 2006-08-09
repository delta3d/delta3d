#include <dtABC/application.h>
#include <dtABC/applicationconfigwriter.h>
#include <dtABC/applicationconfighandler.h>
#include <dtCore/generickeyboardlistener.h>

#include <dtCore/stats.h>
#include <dtCore/system.h>
#include <dtCore/camera.h>
#include <dtCore/scene.h>
#include <dtUtil/log.h>
#include <dtUtil/stringutils.h>
#include <dtUtil/xercesparser.h>
#include <dtCore/mouse.h>

#include <osgDB/FileUtils>

#include <cassert>

#include <dtUtil/xercesutils.h>
#include <xercesc/util/XMLString.hpp>

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
   dtCore::System::GetInstance().Run();
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


void Application::CreateInstances(const std::string& name, int x, int y, int width, int height, bool cursor, bool fullScreen )
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
   ApplicationConfigHandler handler;
   dtUtil::XercesParser parser;
   ///\todo log the result
   bool parsed_well = parser.Parse(file, handler, "application.xsd");

   AppXMLApplicator applicator;
   ///\todo log the result
   bool applied_well = applicator(handler.mConfigData, this);

   return( applied_well || parsed_well );
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
   ApplicationConfigWriter writer;
   writer( filename, GetDefaultConfigData());

   // return the resource path to the new file
   return osgDB::findDataFile( filename );
}

ApplicationConfigData Application::GetDefaultConfigData()
{
   ApplicationConfigData data;

   data.WINDOW_X = 100;
   data.WINDOW_Y = 100;

   data.SHOW_CURSOR = true;
   data.FULL_SCREEN = false;
   data.CHANGE_RESOLUTION = false;

   data.CAMERA_NAME = "defaultCam";
   data.SCENE_NAME = "defaultScene";
   data.WINDOW_NAME = "defaultWin";

   data.SCENE_INSTANCE = "defaultScene";
   data.WINDOW_INSTANCE = "defaultWin";

   data.RESOLUTION.width = 640;
   data.RESOLUTION.height = 480;
   data.RESOLUTION.bitDepth = 24;
   data.RESOLUTION.refresh = 60;

   dtUtil::Log& logger = dtUtil::Log::GetInstance();
   data.LOG_LEVELS.insert(make_pair(logger.GetName(), logger.GetLogLevelString(dtUtil::Log::LOG_WARNING)));

   return data;
}

// --- applicator's implementation --- //
bool Application::AppXMLApplicator::operator ()(const ApplicationConfigData& data, dtABC::Application* app)
{
   // apply the window settings
   dtCore::DeltaWin* dwin = app->GetWindow();

   for (std::map<std::string, std::string>::const_iterator i = data.LOG_LEVELS.begin();
      i != data.LOG_LEVELS.end(); ++i)
   {
      dtUtil::Log& logger = dtUtil::Log::GetInstance(i->first);
      
      logger.SetLogLevel(logger.GetLogLevelForString(i->second));
   }

   // John's unwittingly caught a confusing aspect of the Applications's config file here.
   // Historically, the Window's "name" attribute is used for the WindowTitle, while other
   // elements, such as Screen, use the "name" attribute for the Base name. John had followed
   // convention and just called SetName. Perhaps we need to add a new paramter called
   // "title". However, this would break the expectation of users with previously written
   // configuration files. Maybe we could do an automatically update whenver a config file
   // without the "title" attribute is passed to Application. See Case 722 -osb
   dwin->SetWindowTitle( data.WINDOW_NAME );
   dwin->SetName( data.WINDOW_NAME ); // Perhaps a different parameter is needed for this?

   dwin->SetPosition( data.WINDOW_X, data.WINDOW_Y, data.RESOLUTION.width, data.RESOLUTION.height );
   dwin->ShowCursor( data.SHOW_CURSOR );
   dwin->SetFullScreenMode( data.FULL_SCREEN );

   // change the resolution if needed and valid
   if( data.CHANGE_RESOLUTION )
   {
      if( dwin->IsValidResolution( data.RESOLUTION ) )
      {
         dwin->ChangeScreenResolution( data.RESOLUTION );
      }
   }

   // set up the scene
   dtCore::Scene* scene = app->GetScene();
   ///\todo should this only override when the string is not empty?
   scene->SetName( data.SCENE_NAME );

   // set up the camera
   dtCore::Camera* camera = app->GetCamera();
   ///\todo should this only override when the string is not empty?
   camera->SetName( data.CAMERA_NAME );

   // connect the camera, scene, and window
   // since they might not be the same as the app's instances, we will use the instance management layer
   dtCore::DeltaWin* dinst = dtCore::DeltaWin::GetInstance( data.WINDOW_INSTANCE );
   if( dinst != NULL )
   {
      camera->SetWindow( dinst );
   }
   else
   {
      LOG_WARNING("Application:Can't find instance of DeltaWin, " + data.SCENE_INSTANCE )
   }

   dtCore::Scene* sinst = dtCore::Scene::GetInstance( data.SCENE_INSTANCE );
   if( sinst != NULL )
   {
      camera->SetScene( sinst );
   }
   else
   {
      LOG_WARNING("Application:Can't find instance of Scene, " + data.SCENE_INSTANCE )
   }

   ///\todo Determine a way to know if something went wrong,
   /// maybe when instances were not found.
   return true;
}
