#include "application.h"
#include "system.h"

#include "osgDB/FileUtils"

using namespace dtABC;
using namespace dtCore;



IMPLEMENT_MANAGEMENT_LAYER(Application)



/** public methods */
/** constructor */
Application::Application(std::string configFilename)
   :  BaseABC("Application")
{
   RegisterInstance(this);

   //mOriginalRes.width = 0;
   //mOriginalRes.height = 0;
   //mOriginalRes.bitDepth = 0;
   //mOriginalRes.refresh = 0;
   
   //if config file passed in
   if ( configFilename != "" )
   {
      //  parse config file
      std::string foundPath = osgDB::findDataFile(configFilename);
        
      TiXmlDocument *xmlDoc = new TiXmlDocument(foundPath.c_str());
      if(foundPath.empty())
      {
         Notify(WARN, "Application: can't find config file %s, using defaults instead.", configFilename.c_str());
         CreateInstances(); //create default window, camera, etc.
      }
      else
      {
         xmlDoc->LoadFile();
         TiXmlElement *root = xmlDoc->RootElement();
         if (root != NULL)  ParseConfigFile(root);
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
   #if !defined(_WIN32) && !defined(WIN32) && !defined(__WIN32__)
   //mWindow->ChangeScreenResolution( mOriginalRes );
   #endif
   
   DeregisterInstance(this);
   Notify(DEBUG_INFO, "Destroying application ref:%d", referenceCount());
}



/** This method kicks off the forever process which will start triggering the
  * PreFrame(), Frame(), and PostFrame() methods.
  */
void  Application::Run( void )
{
   System::GetSystem()->Run();
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
void  Application::CreateInstances( std::string name, int x, int y, int width, int height, bool cursor, bool fullScreen )
{
   //create the instances and hook-up the default
   //connections.  The instance attributes may be
   //overwritten using a config file
   BaseABC::CreateInstances();

   mWindow = new DeltaWin( name, x, y, width, height, cursor, fullScreen );
   
   assert( mWindow.get() );

   //mWindow->GetRenderSurface()->realize();
   //mOriginalRes = mWindow->GetCurrentResolution();

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
  */
void  Application::ParseConfigFile( TiXmlElement* rootNode )
{
   TiXmlElement* win = rootNode->FirstChildElement( "Window" );
   if (win != NULL)
   {
      std::string name( "defaultWin" );
      if( const char* nameChar = win->Attribute( "Name" ) )
         name = nameChar;

      int width( 640 );
      if( const char* widthChar = win->Attribute( "Width" ) )
         width = atoi( widthChar );

      int height( 480 );
      if( const char* heightChar = win->Attribute( "Height" ) )
         height = atoi( heightChar );

      int winX( 100 );
      if( const char* winXChar = win->Attribute( "X" ) )
         winX = atoi( winXChar );

      int winY( 100 );
      if( const char* winYChar = win->Attribute( "Y" ) )
         winY = atoi( winYChar );

      bool showCursor( true );
      if( const char* showCursorChar = win->Attribute( "ShowCursor" ) )
         showCursor = atoi( showCursorChar );

      bool fullScreen( false );
      if( const char* fullScreenChar = win->Attribute( "FullScreen" ) )
         fullScreen = atoi( fullScreenChar );
         
      int pixelDepth( 24 );
      if( const char* pixelChar = win->Attribute( "PixelDepth" ) )
         pixelDepth = atoi( pixelChar );

      int refreshRate( 60 );
      if( const char* refreshChar    = win->Attribute("RefreshRate") )
         refreshRate = atoi( refreshChar );

      bool changeRes( false );
      if( const char* changeResChar  = win->Attribute("ChangeDisplayResolution") )
         changeRes = atoi( changeResChar );

      if( changeRes )
         DeltaWin::ChangeScreenResolution( width, height, pixelDepth, refreshRate );

      CreateInstances( name, winX, winY, width, height, showCursor, fullScreen );

   }

   TiXmlElement*  scene = rootNode->FirstChildElement("Scene");
   if (scene != NULL)
   {
      std::string name = scene->Attribute("Name");

      mScene->SetName(name);
   }
   
   TiXmlElement* cam = rootNode->FirstChildElement("Camera");
   if (cam != NULL)
   {
      std::string name = cam->Attribute("Name");
      std::string winInst = cam->Attribute("WindowInstance");
      std::string sceneInst = cam->Attribute("SceneInstance");

      mCamera->SetName(name);
      DeltaWin* win = DeltaWin::GetInstance(winInst);
      if (win != NULL)
         mCamera->SetWindow(win);
      else
         Notify(WARN, "Application:Can't find instance of DeltaWin '%s'", winInst.c_str() );

      Scene* scene = Scene::GetInstance(sceneInst);
      if (scene != NULL)
         mCamera->SetScene(scene);
      else
         Notify(WARN, "Application:Can't find instance of Scene '%s'", sceneInst.c_str() );
   }
}



/** This method grabs all the required attributes from the internal members
  * and writes them out to a .xml file ("config.xml").
  * @todo probably should check to see if the filename already exists so it 
  *       doesn't overwrite it.
  */
void dtABC::Application::GenerateDefaultConfigFile()
{
   TiXmlDocument *xmlDoc = new TiXmlDocument( "config.xml" );
   if (xmlDoc->LoadFile( "config.xml" ))
   {
      Notify(WARN, "Application found existing config.xml, delete file before generating a new one.");
      return;
   }

   TiXmlDocument xml( "config.xml" );
   TiXmlDeclaration dec( "1.0", "iso-8859-1", "no" );
   xml.InsertEndChild( dec );

   TiXmlElement app( "Application" );

   TiXmlElement win( "Window" );
   win.SetAttribute( "Name", "defaultWin" );
   win.SetAttribute( "X", 100 );
   win.SetAttribute( "Y", 100 );
   win.SetAttribute( "Width", 640 );
   win.SetAttribute( "Height", 480 );
   win.SetAttribute( "PixelDepth", 24 ); 
   win.SetAttribute( "RefreshRate", 60 ); 
   win.SetAttribute( "ShowCursor", 1 );
   win.SetAttribute( "FullScreen", 0 );
   win.SetAttribute( "ChangeDisplayResolution", 0 );
   app.InsertEndChild(win);

   TiXmlElement scene( "Scene" );
   scene.SetAttribute( "Name", "defaultScene" );
   app.InsertEndChild( scene );

   TiXmlElement cam( "Camera" );
   cam.SetAttribute( "Name", "defaultCam" );
   cam.SetAttribute( "WindowInstance", "defaultWin" );
   cam.SetAttribute( "SceneInstance", "defaultScene" );
   app.InsertEndChild( cam );

   xml.InsertEndChild( app );

   xml.SaveFile();
}
