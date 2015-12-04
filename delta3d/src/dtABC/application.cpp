
/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2004-2009 MOVES Institute
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Pfblic License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * John K. Grant
 * Erik Johnson
 */

#include <dtABC/application.h>

#include <osgViewer/CompositeViewer>

#include <dtABC/applicationconfigwriter.h>
#include <dtABC/applicationconfighandler.h>
#include <dtABC/applicationconfigdata.h>           // for return type, member
#include <dtCore/stats.h>

#include <dtCore/system.h>
#include <dtCore/view.h>
#include <dtCore/databasepager.h>
#include <dtCore/camera.h>
#include <dtCore/scene.h>
#include <dtCore/deltawin.h>
#include <dtCore/singletonmanager.h>
#include <dtUtil/log.h>
#include <dtUtil/stringutils.h>
#include <dtUtil/xercesparser.h>
#include <dtUtil/datapathutils.h>
#include <dtUtil/librarysharingmanager.h>
#include <dtUtil/mathdefines.h>
#include <dtUtil/threadpool.h>
#include <dtCore/mouse.h>

#include <cassert>

#include <dtUtil/xercesutils.h>
#include <xercesc/util/XMLString.hpp>

#include <osgViewer/Viewer>
#include <osgViewer/Viewer>
#include <osg/io_utils>
#include <osg/Version>
#include <osg/DisplaySettings>
#include <osgDB/Registry>

using namespace dtABC;
XERCES_CPP_NAMESPACE_USE

IMPLEMENT_MANAGEMENT_LAYER(Application)

const std::string Application::SIM_FRAME_RATE("System.SimFrameRate");
const std::string Application::MAX_TIME_BETWEEN_DRAWS("System.MaxTimeBetweenDraws");
const std::string Application::USE_FIXED_TIME_STEP("System.UseFixedTimeStep");
const std::string Application::NUM_WORKER_THREADS("ThreadPool.NumWorkerThreads");


/// A utility to apply the parsed data to the Application instance
class AppXMLApplicator
{
public:
   /// the method to apply the data
   /// @param data The data to be applied
   /// @param app The application to apply the data to
   /// @return true, if all went well.
   bool operator ()(const ApplicationConfigData& data, Application* app);
};

///////////////////////////////////////////////////////////////////////////////
Application::Application(const std::string& configFilename, dtCore::DeltaWin* win)
   : BaseClass("Application")
   , mFirstFrame(true)
   , mThreadPoolInitialized(false)
   , mKeyboardListener(new dtCore::GenericKeyboardListener())
   , mMouseListener(new dtCore::GenericMouseListener())
{
   RegisterInstance(this);

   mKeyboardListener->SetPressedCallback (dtCore::GenericKeyboardListener::CallbackType     (this, &Application::KeyPressed));
   mKeyboardListener->SetReleasedCallback(dtCore::GenericKeyboardListener::CallbackType     (this, &Application::KeyReleased));
   mMouseListener->SetPressedCallback    (dtCore::GenericMouseListener::ButtonCallbackType  (this, &Application::MouseButtonPressed));
   mMouseListener->SetReleasedCallback   (dtCore::GenericMouseListener::ButtonCallbackType  (this, &Application::MouseButtonReleased));
   mMouseListener->SetClickedCallback    (dtCore::GenericMouseListener::ClickCallbackType   (this, &Application::MouseButtonDoubleClicked));
   mMouseListener->SetMovedCallback      (dtCore::GenericMouseListener::MovementCallbackType(this, &Application::MouseMoved));
   mMouseListener->SetDraggedCallback    (dtCore::GenericMouseListener::MovementCallbackType(this, &Application::MouseDragged));
   mMouseListener->SetScrolledCallback   (dtCore::GenericMouseListener::WheelCallbackType   (this, &Application::MouseScrolled));

   mWindow = win;

   if (!dtUtil::GetDeltaDataPathList().empty() &&  dtUtil::GetDataFilePathList().find(dtUtil::GetDeltaDataPathList(), 0) == std::string::npos)
   {
      dtUtil::SetDataFilePathList(dtUtil::GetDataFilePathList() + ":" + dtUtil::GetDeltaDataPathList());
   }

   ApplicationConfigHandler handler;
   if (ParseConfigFile(configFilename, handler))
   {
      //create instances using values from the parsed config file
      CreateInstances(handler.mConfigData);

      //apply the config data to the created instances
      ApplyConfigData(handler);
   }
   else
   {
      //create instances using the default values
      CreateInstances(GetDefaultConfigData());
   }

   dtUtil::Log::SetLogTimeProvider(this);

   GetCamera()->SetupBackwardCompatibleStateset();
}

///////////////////////////////////////////////////////////////////////////////
void Application::Config()
{
   BaseClass::Config();

   //Temporary insurance policy.  If an application is expecting an openGL
   //context outside of the draw, setting the following environment variable will
   //keep a context valid throughout the whole frame.  This is a bit of a crutch
   //for applications upgrading to OSG 2.6.0 that are crashing due to openGL
   //context issues.  Users should not rely on this.


#if OSG_VERSION_GREATER_THAN(2,6,0)
   char* deltaReleaseContext = getenv("DELTA_RELEASE_CONTEXT");
   if (deltaReleaseContext)
   {
      GetCompositeViewer()->setReleaseContextAtEndOfFrameHint(false);
   }
#endif
   ReadSystemProperties();
}

///////////////////////////////////////////////////////////////////////////////
void Application::ReadSystemProperties()
{
   std::string value;

   value = GetConfigPropertyValue(SIM_FRAME_RATE);

   if (!value.empty())
   {
      double simFrameRate = dtUtil::ToDouble(value);
      dtCore::System::GetInstance().SetFrameRate(simFrameRate);
   }

   value.clear();

   value = GetConfigPropertyValue(MAX_TIME_BETWEEN_DRAWS);
   if (!value.empty())
   {
      double timeBetween = dtUtil::ToDouble(value);
      dtCore::System::GetInstance().SetMaxTimeBetweenDraws(timeBetween);
   }

   value.clear();

   value = GetConfigPropertyValue(USE_FIXED_TIME_STEP);
   if (!value.empty())
   {
      bool useFixed = dtUtil::ToType<bool>(value);
      dtCore::System::GetInstance().SetUseFixedTimeStep(useFixed);
   }

   if (GetView()->GetDatabasePager())
   {
      GetView()->GetDatabasePager()->SetConfiguration(this);
   }

   if (!dtUtil::ThreadPool::IsInitialized())
   {
      value = GetConfigPropertyValue(NUM_WORKER_THREADS);
      if (value.empty())
      {
         dtUtil::ThreadPool::Init();
         mThreadPoolInitialized = true;
      }
      else if (value != "OFF" && value != "off")
      {
         int intVal = dtUtil::ToType<int>(value);
         dtUtil::ThreadPool::Init(intVal);
         mThreadPoolInitialized = true;
      }
   }

}

///////////////////////////////////////////////////////////////////////////////
Application::~Application()
{
   if (mThreadPoolInitialized) { dtUtil::ThreadPool::Shutdown(); }

   //osgDB::Registry::instance()->clearArchiveCache();
   //osgDB::Registry::instance()->clearObjectCache();
   //osgDB::Registry::instance()->closeAllLibraries();
   mCompositeViewer = NULL;
   delete mStats;
   dtCore::SingletonManager::Destroy(); //destroy any singletons that need to go away
   DeregisterInstance(this);
}

///////////////////////////////////////////////////////////////////////////////
void Application::Run()
{
   dtCore::System::GetInstance().Run();
}

///////////////////////////////////////////////////////////////////////////////
void Application::EventTraversal(const double deltaSimTime)
{
   //We advance the OSG clock in the EventTraversal message because it is our first message
   //also we call frame() on the composite viewer our first frame since it has extra code
   //to realize the window and we would need our own derivation to replicate it.

   if(!mFirstFrame || !mCompositeViewer->done())
   {
      mCompositeViewer->eventTraversal();
   }

   // The clock time is in GMT, so I have to change the DateTime to GMT, then assign the time,
   // then switch it back to local.
   mCurrentFrameTime.SetGMTOffset(0, false);
   mCurrentFrameTime.SetTime(dtCore::System::GetInstance().GetRealClockTime() / 1000000);
   mCurrentFrameTime.AdjustTimeZone(dtUtil::DateTime::GetLocalGMTOffset(true));
}

///////////////////////////////////////////////////////////////////////////////
void Application::Frame(const double deltaSimTime)
{
   
   LOGN_DEBUG("application.cpp" ,"Frame Start");

   if(!mCompositeViewer->done())
   {
      bool singleThreaded = mCompositeViewer->getThreadingModel() == osgViewer::ViewerBase::SingleThreaded;


      //NOTE: The OSG frame() advances the clock and does three traversals, event, update, and render.
      //We are moving the event traversal to be its own message so we can reliably accept input during the
      //typical Delta3D update of PreFrame().  The only exception to this is that we need
      if(mFirstFrame)
      {
         LOGN_DEBUG("application.cpp" ,"First Frame");

#ifndef MULTITHREAD_FIX_HACK_BREAKS_CEGUI
         dtCore::ObserverPtr<osgViewer::GraphicsWindow> gw;
         if (GetWindow() != NULL)
         {
            gw = GetWindow()->GetOsgViewerGraphicsWindow();
         }

         if (!singleThreaded && gw.valid() && gw->isRealized())
         {
            gw->releaseContext();
         }
#endif

         if (singleThreaded) { GetCompositeViewer()->setReleaseContextAtEndOfFrameHint(false); }
         mCompositeViewer->setUpThreading();
         mCompositeViewer->frame(dtCore::System::GetInstance().GetSimTimeSinceStartup());
         mFirstFrame = false;
      }

      // NOTE: The new version OSG (2.2) relies on absolute frame time
      // to update drawables; especially particle systems.
      // The time delta will be ignored here and the absolute simulation
      // time passed to the OSG scene updater.
      
      LOGN_DEBUG("application.cpp", "Advance Composite Viewer");
      
      LOGN_DEBUG("ApplicationTime", "Total time simulated: " + dtUtil::ToString(dtCore::System::GetInstance().GetSimTimeSinceStartup()));

      mCompositeViewer->advance(dtCore::System::GetInstance().GetSimTimeSinceStartup());
   
      LOGN_DEBUG("application.cpp" ,"Update Traversals");

      mCompositeViewer->updateTraversal();
      
      LOGN_DEBUG("application.cpp" ,"Rendering Traversals");

      mCompositeViewer->renderingTraversals();

   }
   
   LOGN_DEBUG("application.cpp" ,"Frame End");

}


///////////////////////////////////////////////////////////////////////////////
void Application::PreFrame(const double deltaSimTime)
{
   LOGN_DEBUG("application.cpp" ,"PreFrame");

}

///////////////////////////////////////////////////////////////////////////////
void Application::PostFrame(const double deltaSimTime)
{
   LOGN_DEBUG("application.cpp" ,"Post Frame");

   while (!mViewsToDelete.empty())
   {
      LOGN_DEBUG("application.cpp" ,"Deleting views.");

      RemoveViewImpl(*mViewsToDelete.back());
      mViewsToDelete.pop_back();
   }
}

///////////////////////////////////////////////////////////////////////////////
void Application::Pause(const double deltaRealTime)
{
   LOGN_DEBUG("application.cpp" ,"Pause.");

}

///////////////////////////////////////////////////////////////////////////////
bool Application::KeyPressed(const dtCore::Keyboard* keyboard, int kc)
{
   switch (kc)
   {
   case osgGA::GUIEventAdapter::KEY_Escape:
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

///////////////////////////////////////////////////////////////////////////////
bool Application::KeyReleased(const dtCore::Keyboard* keyboard, int kc)
{
   return false;
}

//////////////////////////////////////////////////////////////////////////
bool Application::MouseButtonPressed(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button)
{
   return false;
}

//////////////////////////////////////////////////////////////////////////
bool Application::MouseButtonReleased(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button)
{
   return false;
}

//////////////////////////////////////////////////////////////////////////
bool Application::MouseButtonDoubleClicked(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button, int clickCount)
{
   return false;
}

//////////////////////////////////////////////////////////////////////////
bool Application::MouseMoved(const dtCore::Mouse* mouse, float x, float y)
{
   return false;
}

//////////////////////////////////////////////////////////////////////////
bool Application::MouseDragged(const dtCore::Mouse* mouse, float x, float y)
{
   return false;
}

//////////////////////////////////////////////////////////////////////////
bool Application::MouseScrolled(const dtCore::Mouse* mouse, int delta)
{
   return false;
}

///////////////////////////////////////////////////////////////////////////////
void Application::CreateInstances(const ApplicationConfigData& data)
{
   //create the instances and hook-up the default
   //connections.  The instance attributes may be
   //overwritten using a config file
   BaseClass::CreateInstances();

   if (mWindow == NULL)
   {
      dtCore::DeltaWin::DeltaWinTraits traits;
      traits.name = data.WINDOW_NAME;
      traits.x = data.WINDOW_X;
      traits.y = data.WINDOW_Y;
      traits.width = data.RESOLUTION.width;
      traits.height = data.RESOLUTION.height;
      traits.showCursor = data.SHOW_CURSOR;
      traits.fullScreen = data.FULL_SCREEN;
      traits.realizeUponCreate = data.REALIZE_UPON_CREATE;
      traits.vsync = data.VSYNC;

      // pre DeltaWin creation anti-aliasing
      osg::DisplaySettings::instance()->setNumMultiSamples(data.MULTI_SAMPLE);

      mWindow = new dtCore::DeltaWin(traits);
   }

   GetCamera()->SetWindow(mWindow.get());

   mCompositeViewer = new osgViewer::CompositeViewer;

   // Here we can set the OSG threading model.
   // So as not to break interoperability with Qt we must for now set it here, after
   // creating the composite viewer but prior to adding any views.
   // As an alternative to redesigning the API we have for now exposed an environment
   // variable "DELTA_THREADING" that provides the application programmer or end-user
   // the means to specify the threading model.
   // This is only a temporary, working solution.
   {
      std::string deltaThreadingEnvVar = dtUtil::GetEnvironment("DELTA_THREADING");

      // I had to do the length thing because dtUtil::GetEnvironment oddly returns a non empty string
      // when the value is not set.
      if (deltaThreadingEnvVar.length() < 3)
      {
         std::map<std::string, std::string>::const_iterator i = data.mProperties.find("System.OSGThreadingModel");
         if (i !=  data.mProperties.end())
         {
            deltaThreadingEnvVar = i->second;
         }
      }

      if (deltaThreadingEnvVar == "SingleThreaded")
      {
         mCompositeViewer->setThreadingModel(osgViewer::CompositeViewer::SingleThreaded);
      }
      else if (deltaThreadingEnvVar == "CullDrawThreadPerContext")
      {
         mCompositeViewer->setThreadingModel(osgViewer::CompositeViewer::CullDrawThreadPerContext);
      }
      else if (deltaThreadingEnvVar == "DrawThreadPerContext")
      {
         mCompositeViewer->setThreadingModel(osgViewer::CompositeViewer::DrawThreadPerContext);
      }
      else if (deltaThreadingEnvVar == "CullThreadPerCameraDrawThreadPerContext")
      {
         mCompositeViewer->setThreadingModel(osgViewer::CompositeViewer::CullThreadPerCameraDrawThreadPerContext);
      }
      else
      {
         mCompositeViewer->setUpThreading();
      }
   }

   mCompositeViewer->addView(mViewList.front()->GetOsgViewerView());

   // Disable OSG's default behavior of quitting when the Escape key is pressed.
   // Not disabling this causes Delta3D and OSG to get into a bad state when the
   // Escape key is pressed.
   GetCompositeViewer()->setKeyEventSetsDone(0);

   GetKeyboard()->AddKeyboardListener(mKeyboardListener.get());
   GetMouse()->AddMouseListener(mMouseListener.get());

   mStats = new dtCore::StatsHandler(*mCompositeViewer);
}

///////////////////////////////////////////////////////////////////////////////
DT_DISABLE_WARNING_START_CLANG("-Wreturn-stack-address")
const std::string& Application::GetConfigPropertyValue(
         const std::string& name, const std::string& defaultValue) const
{
   AppConfigPropertyMap::const_iterator i = mConfigProperties.find(name);
   if (i == mConfigProperties.end())
   {
      return defaultValue;
   }
   else
   {
      return i->second;
   }
}
DT_DISABLE_WARNING_END

///////////////////////////////////////////////////////////////////////////////
void Application::GetConfigPropertiesWithPrefix(const std::string& prefix, std::vector<std::pair<std::string,std::string> >& resultOut, bool removePrefix) const
{
   size_t prefLen = prefix.length();
   AppConfigPropertyMap::const_iterator i,iend;
   i = mConfigProperties.begin();
   iend = mConfigProperties.end();
   for (;i != iend; ++i)
   {
      const std::string& key = i->first;
      if (key.length() >= prefLen && key.substr(0, prefLen) == prefix)
      {
         if (removePrefix)
         {
            resultOut.push_back(std::make_pair(key.substr(prefLen), i->second));
         }
         else
         {
            resultOut.push_back(std::make_pair(key, i->second));
         }
      }
   }
}

///////////////////////////////////////////////////////////////////////////////
void Application::SetConfigPropertyValue(const std::string& name, const std::string& value)
{
   mConfigProperties[name] = value;
}

///////////////////////////////////////////////////////////////////////////////
void Application::RemoveConfigPropertyValue(const std::string& name)
{
   mConfigProperties.erase(name);
}

////////////////////////////////////////////////////////////////////////////////
bool dtABC::Application::IsConfigPropertyDefined(const std::string& name) const
{
   return mConfigProperties.find(name) != mConfigProperties.end();
}

///////////////////////////////////////////////////////////////////////////////
bool Application::ParseConfigFile(const std::string& file, ApplicationConfigHandler& handler) const
{
   if (file.empty())
   {
      return false;
   }

   const std::string foundPath = dtUtil::FindFileInPathList(file);
   if (foundPath.empty())
   {
      LOG_WARNING("Application: Can't find config file, " + file + ", using defaults instead.");
      return false;
   }

   dtUtil::XercesParser parser;

   bool parsed_well = parser.Parse(foundPath, handler, "application.xsd");
   if (!parsed_well)
   {
      LOG_ERROR("The Application config file, " + foundPath + ", wasn't parsed correctly.");
      return false;
   }

   return true;
}

///////////////////////////////////////////////////////////////////////////////
std::string dtABC::Application::GenerateDefaultConfigFile(const std::string& filename)
{
   std::string existingfile = dtUtil::FindFileInPathList(filename);

   if (!existingfile.empty())
   {
      LOG_WARNING("Can't generate new configuration file: file already exists: " + existingfile);
      return existingfile;
   }

   // write out a new file
   ApplicationConfigWriter writer;
   writer(filename, GetDefaultConfigData());

   // return the resource path to the new file
   return dtUtil::FindFileInPathList(filename);
}


///////////////////////////////////////////////////////////////////////////////
void dtABC::Application::SetNextStatisticsType()
{
   mStats->SelectNextType();
}

///////////////////////////////////////////////////////////////////////////////
const dtUtil::DateTime& dtABC::Application::GetDateTime()
{
	return mCurrentFrameTime;
}

///////////////////////////////////////////////////////////////////////////////
unsigned dtABC::Application::GetFrameNumber()
{
	return GetCompositeViewer()->getFrameStamp()->getFrameNumber();
}

///////////////////////////////////////////////////////////////////////////////
osg::Referenced* dtABC::Application::AsReferenced()
{
	return this;
}

///////////////////////////////////////////////////////////////////////////////
ApplicationConfigData Application::GetDefaultConfigData()
{
   ApplicationConfigData data;
   const dtCore::DeltaWin::DeltaWinTraits winTraits; //use the default DeltaWin values

   data.WINDOW_X = 100;
   data.WINDOW_Y = 100;

   data.SHOW_CURSOR = winTraits.showCursor;
   data.FULL_SCREEN = winTraits.fullScreen;
   data.REALIZE_UPON_CREATE = winTraits.realizeUponCreate;
   data.CHANGE_RESOLUTION = false;

   data.CAMERA_NAME = "defaultCam";
   data.SCENE_NAME = "defaultScene";
   data.WINDOW_NAME = "defaultWin";
   data.VIEW_NAME = "defaultView";

   data.SCENE_INSTANCE = "defaultScene";
   data.WINDOW_INSTANCE = "defaultWin";
   data.CAMERA_INSTANCE = "defaultCam";

   data.RESOLUTION.width = winTraits.DEFAULT_WIDTH;
   data.RESOLUTION.height = winTraits.DEFAULT_HEIGHT;
   data.RESOLUTION.bitDepth = 24;
   data.RESOLUTION.refresh = 60;

   //Set to default in the constructor of the data.
   //data.VSYNC
   //data.MULTI_SAMPLE

   dtUtil::Log& logger = dtUtil::Log::GetInstance();
   data.LOG_LEVELS.insert(make_pair(logger.GetName(), logger.GetLogLevelString(dtUtil::Log::LOG_WARNING)));

   return data;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// --- applicator's implementation --- //
bool AppXMLApplicator::operator ()(const ApplicationConfigData& data, dtABC::Application* app)
{
   // set up the View
   if (!data.VIEW_NAME.empty())
   {
      dtCore::View* view = app->GetView();
      if (view != NULL)
      {
         view->SetName(data.VIEW_NAME);
      }
   }

   // set up the scene
   if (!data.SCENE_NAME.empty())
   {
      dtCore::Scene* scene = app->GetScene();
      if (scene != NULL)
      {
         scene->SetName(data.SCENE_NAME);
      }
   }

   // set up the camera
   if (!data.CAMERA_NAME.empty())
   {
      dtCore::Camera* camera = app->GetCamera();
      if (camera != NULL)
      {
         camera->SetName(data.CAMERA_NAME);
      }
   }

   // apply the window settings
   dtCore::DeltaWin* dwin = app->GetWindow();

   //set the default log level for all future Log instances
   dtUtil::Log::SetDefaultLogLevel(dtUtil::Log::GetLogLevelForString(data.GLOBAL_LOG_LEVEL));

   //Also set the level for any existing Log instances
   dtUtil::Log::SetAllLogLevels(dtUtil::Log::GetLogLevelForString(data.GLOBAL_LOG_LEVEL));

   //Set the log level for any specifically defined Log instances, overwriting the default level
   for (std::map<std::string, std::string>::const_iterator i = data.LOG_LEVELS.begin();
      i != data.LOG_LEVELS.end(); ++i)
   {
      dtUtil::Log& logger = dtUtil::Log::GetInstance(i->first);

      logger.SetLogLevel(logger.GetLogLevelForString(i->second));
   }


   dtUtil::LibrarySharingManager& lsm = dtUtil::LibrarySharingManager::GetInstance();
   for (std::vector<std::string>::const_iterator i = data.LIBRARY_PATHS.begin();
      i != data.LIBRARY_PATHS.end(); ++i)
   {
      lsm.AddToSearchPath(*i);
   }

   for (std::map<std::string, std::string>::const_iterator i = data.mProperties.begin();
      i != data.mProperties.end(); ++i)
   {
      app->SetConfigPropertyValue(i->first, i->second);
   }

   dwin->SetName(data.WINDOW_NAME); // Perhaps a different parameter is needed for this?

   //The following are redundant calls.  The values have already been set
   //via the constructor of DeltaWin
   //dwin->SetWindowTitle(data.WINDOW_NAME);
   //dwin->SetPosition(data.WINDOW_X, data.WINDOW_Y, data.RESOLUTION.width, data.RESOLUTION.height);
   //dwin->SetShowCursor(data.SHOW_CURSOR);
   //dwin->SetFullScreenMode(data.FULL_SCREEN);

   // change the resolution if needed and valid
   if (data.CHANGE_RESOLUTION && data.FULL_SCREEN)
   {
      if (dwin->IsValidResolution(data.RESOLUTION))
      {
         dwin->ChangeScreenResolution(data.RESOLUTION);
      }
      else
      {
         LOG_WARNING("Cannot change screen resolution: invalid resolution specified in config file.");
      }
   }

   bool valid = true; //optimistic

   // connect the camera, scene, and window
   // since they might not be the same as the app's instances, we will use the instance management layer
   dtCore::DeltaWin* win = dtCore::DeltaWin::GetInstance(data.WINDOW_INSTANCE);
   dtCore::Camera* camera = dtCore::Camera::GetInstance(data.CAMERA_INSTANCE);
   dtCore::View* view = dtCore::View::GetInstance(data.VIEW_NAME);
   dtCore::Scene* sinst = dtCore::Scene::GetInstance(data.SCENE_INSTANCE);

   if ((win != NULL) && (camera != NULL))
   {
      camera->SetWindow(win);
      if ((data.VIEWPORT_W > 0) && (data.VIEWPORT_H > 0))
      {
         camera->GetOSGCamera()->setViewport(data.VIEWPORT_X, data.VIEWPORT_Y,
                                             data.VIEWPORT_W, data.VIEWPORT_H);
      }
   }
   else
   {
      LOG_WARNING("Application:Can't find instance of DeltaWin '" + data.WINDOW_INSTANCE +
                  "' and/or instance of Camera '" + data.CAMERA_INSTANCE + "'");
      valid = false;
   }

   if ((camera != NULL) && (view != NULL))
   {
      view->SetCamera(camera);
   }
   else
   {
       LOG_WARNING("Application:Can't find instance of Camera, " + data.CAMERA_INSTANCE );
       valid = false;
   }

   if ((sinst != NULL) && (view != NULL))
   {
      view->SetScene(sinst);
   }
   else
   {
      LOG_WARNING("Application:Can't find instance of Scene, " + data.SCENE_INSTANCE );
      valid = false;
   }

   return valid;
}
////////////////////////////////////////////////////////
void Application::AddView(dtCore::View& view)
{
   if (mCompositeViewer.valid() == false)
   {
      mCompositeViewer = new osgViewer::CompositeViewer;
   }

   mCompositeViewer->addView(view.GetOsgViewerView());
   mViewList.push_back(&view);

   //By adding our internal listeners to added Views, all input from all
   //Views will be routed to the Application.
   if (view.GetKeyboard() != NULL)
   {
      view.GetKeyboard()->AddKeyboardListener(mKeyboardListener.get());
   }

   if (view.GetMouse() != NULL)
   {
      view.GetMouse()->AddMouseListener(mMouseListener.get());
   }
}

////////////////////////////////////////////////////////
bool Application::ContainsView(dtCore::View &view)
{
   ViewList::iterator it = std::find(mViewList.begin(), mViewList.end(), &view);
   return it != mViewList.end();
}

////////////////////////////////////////////////////////
void Application::RemoveView(dtCore::View &view, bool immediately)
{
   if (immediately)
   {
      RemoveViewImpl(view);
   }
   else
   {
      mViewsToDelete.push_back(&view);
   }
}
////////////////////////////////////////////////////////////////////////////////
void Application::RemoveViewImpl(dtCore::View& view)
{
   ViewList::iterator it = std::find(mViewList.begin(), mViewList.end(), &view);
   if (it != mViewList.end())
   {
      if (view.GetKeyboard() != NULL)
      {
         view.GetKeyboard()->RemoveKeyboardListener(mKeyboardListener.get());
      }

      if (view.GetMouse() != NULL)
      {
         view.GetMouse()->RemoveMouseListener(mMouseListener.get());
      }

      mCompositeViewer->removeView(view.GetOsgViewerView());
      mViewList.erase(it);
   }
}

////////////////////////////////////////////////////////////////////////////////
bool Application::ApplyConfigData(const ApplicationConfigHandler &handler)
{
   AppXMLApplicator applicator;
   bool applied_well = applicator(handler.mConfigData, this);

   if (!applied_well)
   {
      LOG_ERROR("The Application config file data wasn't applied correctly.");
   }

   return applied_well;
}

///////////////////////////////////////////////////////////////////////////////
void Application::StartThreading()
{
   LOGN_DEBUG("application.cpp", "Starting threading.");
   mCompositeViewer->startThreading();
}

///////////////////////////////////////////////////////////////////////////////
void Application::StopThreading()
{
   LOGN_DEBUG("application.cpp", "Stopping threading.");
   mCompositeViewer->stopThreading();
}

///////////////////////////////////////////////////////////////////////////////
bool Application::GetThreadsActive() const
{
   return mCompositeViewer->areThreadsRunning();
}

