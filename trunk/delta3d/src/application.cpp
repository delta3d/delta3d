#include "application.h"
#include "system.h"



using namespace dtABC;
using namespace dtCore;



IMPLEMENT_MANAGEMENT_LAYER(Application)



/** public methods */
/** constructor */
Application::Application(std::string configFilename)
   :  BaseABC("Application"),
      mHorz(0),
      mVert(0),
      mDepth(0)
{
   RegisterInstance(this);
   CreateInstances();

   //if config file passed in
   if (configFilename!="")
   {
      //  parse config file
      TiXmlDocument *xmlDoc = new TiXmlDocument(configFilename.c_str());
      if (!xmlDoc->LoadFile(configFilename.c_str()))
      {
         Notify(WARN, "Application: can't find config file %s", configFilename.c_str());
         Notify(WARN, "             Generating default one.");
         GenerateConfigFile();      
      }
      else
      {
         TiXmlElement *root = xmlDoc->RootElement();
         if (root != NULL)  ParseConfigFile(root);
      }
   }
   else
      GenerateConfigFile();
}



/** destructor */
Application::~Application(void)
{
   #if !defined(_WIN32) && !defined(WIN32) && !defined(__WIN32__)
   if( mHorz != 0 && mVert != 0 && mDepth != 0)
      //SwitchToOriginalResolution();
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
void  Application::CreateInstances( void )
{
   //create the instances and hook-up the default
   //connections.  The instance attributes may be
   //overwritten using a config file
   BaseABC::CreateInstances();

   mWindow = new DeltaWin("defaultWin");
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
  */
void  Application::ParseConfigFile( TiXmlElement* rootNode )
{
   TiXmlElement* win = rootNode->FirstChildElement( "Window" );
   if (win != NULL)
   {
       const char* nameChar       = win->Attribute("Name");
      const char* widthChar      = win->Attribute("Width");
      const char* heightChar     = win->Attribute("Height");
      const char* winXChar       = win->Attribute("X");
      const char* winYChar       = win->Attribute("Y");
      const char* showCursorChar = win->Attribute("ShowCursor");
      const char* fullScreenChar = win->Attribute("FullScreen");
      const char* pixelChar      = win->Attribute("PixelDepth");
      const char* refreshChar    = win->Attribute("RefreshRate");
      const char* changeResChar  = win->Attribute("ChangeDisplayResolution");

      if( nameChar )
      {
         std::string name = nameChar;
         mWindow->SetName(name);
         mWindow->SetWindowTitle(nameChar);
      }

      if( widthChar && heightChar && winXChar && winYChar )
      {
         int         width       = atoi( widthChar );
         int         height      = atoi( heightChar );
         int         winX        = atoi( winXChar );
         int         winY        = atoi( winYChar );

         mWindow->SetPosition(winX, winY, width, height);
      }
    
      if( showCursorChar )
      {
         bool        showCursor  = atoi( showCursorChar );
         mWindow->ShowCursor(showCursor);
      }

      if( fullScreenChar )
      {
         bool        fullScreen  = atoi( fullScreenChar );
         //mWindow->SetFullScreenMode(fullScreen);
         //mWindow->SetFullscreenFlag(fullScreen);
      }

      if( pixelChar && refreshChar && changeResChar )
      {
         int      pixelDepth  = atoi( pixelChar );
         int      refreshRate = atoi( refreshChar );
         bool     changeRes   = atoi( changeResChar );
         
         if( changeRes )
         {
            //mHorz = width;
            //mVert = height;
            //mDepth = pixelDepth;
            //SaveOriginalResolution( width, height, pixelDepth );
            //mWindow->ChangeScreenResolution( width, height, pixelDepth, 60 );
            //mWindow->SetChangeScreenResolutionFlag(width, height, pixelDepth, refreshRate);
         }
      }
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
void dtABC::Application::GenerateConfigFile()
{
   TiXmlDocument xml("config.xml");
   TiXmlDeclaration dec("1.0", "iso-8859-1", "no");
   xml.InsertEndChild(dec);

   TiXmlElement app("Application");

   TiXmlElement win("Window");
   win.SetAttribute("Name", mWindow->GetName().c_str());
   int x,y,w,h;
   mWindow->GetPosition(&x, &y, &w, &h);
   bool cursor, fullscreen;
   cursor = mWindow->GetShowCursor();
   fullscreen = mWindow->GetFullScreenMode();
   win.SetAttribute("Width", w);
   win.SetAttribute("Height", h);
   win.SetAttribute("X", x);
   win.SetAttribute("Y", y);
   win.SetAttribute("PixelDepth", 24); //default!
   win.SetAttribute("RefreshRate", 60); //default!
   win.SetAttribute("ShowCursor", cursor);
   win.SetAttribute("FullScreen", fullscreen);
   win.SetAttribute("ChangeDisplayResolution", 0);
   app.InsertEndChild(win);

   TiXmlElement scene("Scene");
   scene.SetAttribute("Name", mScene->GetName().c_str());
   app.InsertEndChild(scene);

   TiXmlElement cam("Camera");
   cam.SetAttribute("Name", mCamera->GetName().c_str());
   cam.SetAttribute("WindowInstance", mCamera->GetWindow()->GetName().c_str() );
   cam.SetAttribute("SceneInstance", mCamera->GetScene()->GetName().c_str());
   app.InsertEndChild(cam);

   xml.InsertEndChild(app);

   xml.SaveFile();
}


void  dtABC::Application::SaveOriginalResolution()
{
   Resolution r = mWindow->GetCurrentResolution();
   mHorz = r.width;
   mVert = r.height;
   mDepth = r.bitDepth;
}

void  dtABC::Application::SwitchToOriginalResolution()
{
   //mWindow->ChangeScreenResolution( mHorz, mVert, mDepth );
}
