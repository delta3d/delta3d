#include <CEGUI/CEGUISystem.h>
#include <CEGUI/CEGUIWindowManager.h>
#include <CEGUI/CEGUISchemeManager.h>
#include <CEGUI/CEGUIImagesetManager.h>
#include <CEGUI/CEGUILogger.h>
#include <CEGUI/CEGUIExceptions.h>
#include <CEGUI/CEGUIWindow.h>
#include <CEGUI/CEGUITexture.h>

#include "guimanager.h"
#include <dtUtil/log.h>
#include <dtGUI/ceuidrawable.h>

#include <osgDB/FileUtils>

using namespace dtGUI;

GUIManager::GUIManager(): BaseClass("GUIManager"),
   mDrawable(0)
{
}

GUIManager::~GUIManager()
{
}

dtGUI::CEUIDrawable* GUIManager::GetGUIDrawable()
{
   return mDrawable.get();
}

void GUIManager::InitializeGUI(dtCore::DeltaWin* dwin, dtGUI::BaseScriptModule* sm)
{
   // this code could start the system, but that code is in the drawable
   mDrawable = new dtGUI::CEUIDrawable(dwin,sm);
}

CEGUI::Scheme* GUIManager::LoadScheme(const std::string& file)
{
   CEGUI::Scheme* scheme(0);

   std::string schemefile( osgDB::findDataFile(file) );
   if( schemefile.empty() )
   {
      LOG_WARNING("Could not find: " + file )
      return scheme;
   }

   if( !mDrawable.valid() )
   {
      LOG_WARNING("GUI Scene not setup already when attempting to load: " + file )
      return scheme;
   }

   try
   {
      CEGUI::SchemeManager* sm = CEGUI::SchemeManager::getSingletonPtr();
      scheme = sm->loadScheme( schemefile );  ///< populate the window factories
   }
   catch(...)
   {
      LOG_ALWAYS("Everything is wrong with scheme loading.")
   }

   return scheme;
}

CEGUI::Window* GUIManager::LoadLayout(const std::string& file)
{
   CEGUI::Window* layoutwindow(0);

   ///\todo the ResourceProvider should able to provide this functionality, if it does, use it.
   /**\todo Also, ResourceProvider should derive and extend the CEGUI::ResourceProvider,
     * but dtCore should offer a tool like this too, to buffer the API from osgDB.
     */
   std::string layout( osgDB::findDataFile(file) );

   if( layout.empty() )
   {
      LOG_WARNING("Could not find: " + file )
      return layoutwindow;
   }

   if( CEGUI::System::getSingletonPtr()==0 )
   {
      LOG_WARNING("GUI System not setup already when attempting to load: " + file )
      return layoutwindow;
   }

   ///\todo perform 'find' with the full file path
   LoadedLayoutMap::iterator iter = mLoadedLayouts.find(file);

   if( iter != mLoadedLayouts.end() )
   {
      layoutwindow = (*iter).second;
   }
   else
   {
      try
      {
         CEGUI::WindowManager* wm = CEGUI::WindowManager::getSingletonPtr();
         layoutwindow = wm->loadWindowLayout( file );  // populate the system with windows

         ///\todo push the full path as the key, so that different local
         mLoadedLayouts.insert( LoadedLayoutMap::value_type(file,layoutwindow) );
      }
      catch(CEGUI::Exception& e)
      {
         LOG_WARNING("An exception occurred while attempting to load the layout, " +file+ ", with the exception message: " + std::string(e.getMessage().c_str()))
         return 0;
      }
   }

   return layoutwindow;
}

CEGUI::Imageset* GUIManager::CreateImageset(const std::string& name, void* buffer, int width, int height)
{
   CEGUI::Texture* cetex = CEGUI::System::getSingletonPtr()->getRenderer()->createTexture();
   cetex->loadFromMemory( buffer, width, height );

   CEGUI::ImagesetManager* mgr = CEGUI::ImagesetManager::getSingletonPtr();
   CEGUI::Imageset* imgset = mgr->createImageset(name,cetex);

   return imgset;
}

//static
CEGUI::Window* GUIManager::CreateGUIWindow(const std::string& wtype, const std::string& wname)
{
   CEGUI::WindowManager* wmgr = CEGUI::WindowManager::getSingletonPtr();
   CEGUI::Window* w = 0;

   try
   {
      w = wmgr->createWindow(wtype,wname);
   }
   catch (CEGUI::Exception &e)
   {
      std::string err;
      err += std::string("Can't create CEGUI Window of type ");
      err += wtype;
      err += std::string(".");
      err += std::string(e.getMessage().c_str());

      LOG_ERROR(err);
   }
   return w;
}

void GUIManager::SetLayout(CEGUI::Window* w)
{
   CEGUI::System::getSingleton().setGUISheet( w );
}

void GUIManager::SetGUILogLevel(enum CEGUI::LoggingLevel level)
{
   CEGUI::Logger::getSingletonPtr()->setLoggingLevel(level);
}

CEGUI::Window* GUIManager::GetWidgetByName(const std::string &name)
{
   CEGUI::WindowManager *wm = CEGUI::WindowManager::getSingletonPtr();
   assert(wm!=0);

   CEGUI::Window *win = NULL;

   if( wm->isWindowPresent(name) )
   {
      win = static_cast< CEGUI::Window* >( wm->getWindow(name) );
   }
   return win;
}

CEGUI::Event::Connection GUIManager::SubscribeEvent(CEGUI::Window *win, 
                                               const CEGUI::String &eventName,
                                               CEGUI::Event::Subscriber subscriber)
{
   if(win==0)
   {
      LOG_WARNING("No Window supplied to subscribe to Event '" + std::string(eventName.c_str()) + std::string("'"));

      return NULL;
   }

   return(win->subscribeEvent(eventName, subscriber));
}

CEGUI::Event::Connection GUIManager::SubscribeEvent(const std::string &winName, 
                                                    const CEGUI::String &eventName,
                                                    CEGUI::Event::Subscriber subscriber)
{
   CEGUI::WindowManager *wm = CEGUI::WindowManager::getSingletonPtr();

   if( !wm->isWindowPresent(winName) )
   {
      LOG_WARNING("Can't find Window '" + std::string(winName) +
         std::string("' to subscribe to Event '") + std::string(eventName.c_str()) + std::string("'"));
      return NULL;
   }

   CEGUI::Window *win = wm->getWindow(winName);
   return(SubscribeEvent( win, eventName, subscriber));
}

void GUIManager::ProducerToNormalized(float& mx, float& my)
{
   /** changes from producer coordinates to normalized CE coordinates */
   // --- horizontal math
   // ph = 1.0 - -1.0 = 2.0    // total horizontal distance in producer
   // nh = 1.0 -  0.0 = 1.0    // total horizontal distance in normalized units

   // --- vertical math
   // pv = 1.0 - -1.0 =  2.0   // total vertical distance in producer
   // nv = 1.0 -  0.0 =  1.0   // total vertical distance in ce

   // nx = cx + px * (nh/2 / ph/2) = 0.5 + px *(0.5/1.0) = 0.5 + px*0.5
   // ny = cy + py * (nv/2 / pv/2) = 0.5 + py *(0.5/1.0) = 0.5 + py*0.5
   // where cx is the "center" x value in normalized coordinates
   // and   cy is the "center" y value in normalized coordinates
   float nx(0.5 + mx*0.5);
   float ny(0.5 + my*0.5);
   mx = nx;
   my = ny;
}

void GUIManager::ShutdownGUI()
{
   if (mDrawable != NULL)
      mDrawable->ShutdownGUI();
}

