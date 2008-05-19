/* -*-c++-*- 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2004-2005 MOVES Institute 
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free 
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
 */

#include <dtABC/application.h>

#include <osgViewer/CompositeViewer>

#include <dtABC/applicationconfigwriter.h>
#include <dtABC/applicationconfighandler.h>
#include <dtABC/applicationconfigdata.h>           // for return type, member
#include <dtCore/generickeyboardlistener.h>
#include <dtCore/stats.h>

#include <dtCore/system.h>
#include <dtCore/view.h>
#include <dtCore/camera.h>
#include <dtCore/scene.h>
#include <dtCore/globals.h>
#include <dtCore/deltawin.h>
#include <dtUtil/log.h>
#include <dtUtil/stringutils.h>
#include <dtUtil/xercesparser.h>
#include <dtUtil/librarysharingmanager.h>
#include <dtCore/mouse.h>

#include <cassert>

#include <dtUtil/xercesutils.h>
#include <xercesc/util/XMLString.hpp>

#include <osgViewer/Viewer>
#include <osg/io_utils>

using namespace dtABC;
XERCES_CPP_NAMESPACE_USE

IMPLEMENT_MANAGEMENT_LAYER(Application)

const std::string Application::SIM_FRAME_RATE("System.SimFrameRate");
const std::string Application::MAX_TIME_BETWEEN_DRAWS("System.MaxTimeBetweenDraws");
const std::string Application::USE_FIXED_TIME_STEP("System.UseFixedTimeStep");

const std::string Application::DATABASE_PAGER_PRECOMPILE_OBJECTS("System.DatabasePager.PrecompileObjects");
const std::string Application::DATABASE_PAGER_MAX_OBJECTS_TO_COMPILE_PER_FRAME("System.DatabasePager.MaxObjectsToCompilePerFrame");
const std::string Application::DATABASE_PAGER_MIN_TIME_FOR_OBJECT_COMPILE("System.DatabasePager.MinObjectCompileTime");
const std::string Application::DATABASE_PAGER_TARGET_FRAMERATE("System.DatabasePager.TargetFrameRate");
const std::string Application::DATABASE_PAGER_DRAWABLE_POLICY("System.DatabasePager.DrawablePolicy");
const std::string Application::DATABASE_PAGER_THREAD_PRIORITY("System.DatabasePager.ThreadPriority");
const std::string Application::DATABASE_PAGER_EXPIRY_DELAY("System.DatabasePager.ExpiryDelay");

///////////////////////////////////////////////////////////////////////////////
Application::Application(const std::string& configFilename, dtCore::DeltaWin *win) 
:  BaseClass("Application"),
   mKeyboardListener(new dtCore::GenericKeyboardListener())
{
   RegisterInstance(this);

   mKeyboardListener->SetPressedCallback(dtCore::GenericKeyboardListener::CallbackType(this,&Application::KeyPressed));
   mKeyboardListener->SetReleasedCallback(dtCore::GenericKeyboardListener::CallbackType(this,&Application::KeyReleased));

   mWindow = win;

   CreateInstances(); //create default Viewer View

   mStats = new dtCore::StatsHandler( *mCompositeViewer );

   if( !configFilename.empty() )
   {
      std::string foundPath = dtCore::FindFileInPathList(configFilename);
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

///////////////////////////////////////////////////////////////////////////////
void Application::Config()
{
   BaseClass::Config();
   ReadSystemProperties();
}

///////////////////////////////////////////////////////////////////////////////
void Application::ReadSystemProperties()
{
   std::string value;

   value = GetConfigPropertyValue(SIM_FRAME_RATE);

   if(!value.empty())
   {
      double simFrameRate = dtUtil::ToDouble(value);
      dtCore::System::GetInstance().SetFrameRate(simFrameRate);
   }
   
   value.clear();

   value = GetConfigPropertyValue(MAX_TIME_BETWEEN_DRAWS);
   if(!value.empty())
   {
      double timeBetween = dtUtil::ToDouble(value);
      dtCore::System::GetInstance().SetMaxTimeBetweenDraws(timeBetween);
   }

   value.clear();

   value = GetConfigPropertyValue(USE_FIXED_TIME_STEP);
   if(!value.empty())
   {
      bool useFixed = dtUtil::ToType<bool>(value);
      dtCore::System::GetInstance().SetUseFixedTimeStep(useFixed);
   }

   osgDB::DatabasePager* pager = GetView()->GetOsgViewerView()->getDatabasePager();

   value = GetConfigPropertyValue(DATABASE_PAGER_PRECOMPILE_OBJECTS);
   if (!value.empty())
   {
      bool precompile = dtUtil::ToType<bool>(value);
      pager->setDoPreCompile(precompile);
   }

   value = GetConfigPropertyValue(DATABASE_PAGER_MAX_OBJECTS_TO_COMPILE_PER_FRAME);
   if (!value.empty())
   {
      unsigned int maxNum = dtUtil::ToType<unsigned int>(value);
      //Can't be less than 1.  That doesn't make sense.
      maxNum = std::max(maxNum, 1U);
      pager->setMaximumNumOfObjectsToCompilePerFrame(maxNum);
   }

   value = GetConfigPropertyValue(DATABASE_PAGER_MIN_TIME_FOR_OBJECT_COMPILE);
   if (!value.empty())
   {
      float minTime = dtUtil::ToType<float>(value);
      pager->setMinimumTimeAvailableForGLCompileAndDeletePerFrame(minTime);
   }

   value = GetConfigPropertyValue(DATABASE_PAGER_TARGET_FRAMERATE);
   if (!value.empty())
   {
      double target = dtUtil::ToType<double>(value);
      pager->setTargetFrameRate(target);
   }
   else if (dtCore::System::GetInstance().GetUsesFixedTimeStep())
   {
      pager->setTargetFrameRate(dtCore::System::GetInstance().GetFrameRate());
   }

   value = GetConfigPropertyValue(DATABASE_PAGER_DRAWABLE_POLICY);
   if (!value.empty())
   {
      if (value == "DoNotModify")
      {
          pager->setDrawablePolicy(osgDB::DatabasePager::DO_NOT_MODIFY_DRAWABLE_SETTINGS);
      }
      else if (value == "DisplayList" || value == "DisplayLists" || value == "DL")
      {
          pager->setDrawablePolicy(osgDB::DatabasePager::USE_DISPLAY_LISTS);
      }
      else if (value == "VBO")
      {
          pager->setDrawablePolicy(osgDB::DatabasePager::USE_VERTEX_BUFFER_OBJECTS);
      }
      else if (value == "VertexArrays" || value == "VertexArray"||  value == "VA")
      {
          pager->setDrawablePolicy(osgDB::DatabasePager::USE_VERTEX_ARRAYS);
      }
   }

   value = GetConfigPropertyValue(DATABASE_PAGER_THREAD_PRIORITY);
   if (!value.empty())
   {
      if (value == "DEFAULT")
      {
          pager->setSchedulePriority(OpenThreads::Thread::THREAD_PRIORITY_DEFAULT);
      }
      else if (value == "MIN")
      {
         pager->setSchedulePriority(OpenThreads::Thread::THREAD_PRIORITY_MIN);
      }
      else if (value == "LOW")
      {
         pager->setSchedulePriority(OpenThreads::Thread::THREAD_PRIORITY_LOW);
      }
      else if (value == "NOMINAL")
      {
         pager->setSchedulePriority(OpenThreads::Thread::THREAD_PRIORITY_NOMINAL);
      }
      else if (value == "HIGH")
      {
         pager->setSchedulePriority(OpenThreads::Thread::THREAD_PRIORITY_HIGH);
      } 
      else if (value == "MAX")
      {
         pager->setSchedulePriority(OpenThreads::Thread::THREAD_PRIORITY_MAX);
      }
   }

   value = GetConfigPropertyValue(DATABASE_PAGER_EXPIRY_DELAY);
   if (!value.empty())
   {
      double delay = dtUtil::ToType<double>(value);
      pager->setExpiryDelay(delay);
   }
}

///////////////////////////////////////////////////////////////////////////////
Application::~Application()
{  
   mCompositeViewer = NULL;
   delete mStats;
   DeregisterInstance(this);
}

///////////////////////////////////////////////////////////////////////////////
void Application::Run()
{
   dtCore::System::GetInstance().Run();
}

///////////////////////////////////////////////////////////////////////////////
void Application::PreFrame( const double deltaSimTime )
{
}

///////////////////////////////////////////////////////////////////////////////
void Application::Frame( const double deltaSimTime )
{
   // NOTE: The new version OSG (2.2) relies on absolute frame time
   // to update drawables; especially particle systems.
   // The time delta will be ignored here and the absolute simulation
   // time passed to the OSG scene updater.
   mCompositeViewer->frame( dtCore::System::GetInstance().GetSimTimeSinceStartup() );
}

///////////////////////////////////////////////////////////////////////////////
void Application::PostFrame( const double deltaSimTime )
{
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
///////////////////////////////////////////////////////////////////////////////
void Application::CreateInstances(const std::string& name, int x, int y, int width, 
                                  int height, bool cursor, bool fullScreen )
{
   //create the instances and hook-up the default
   //connections.  The instance attributes may be
   //overwritten using a config file
   BaseClass::CreateInstances();
   
   if (mWindow == NULL)
   {
       mWindow = new dtCore::DeltaWin( name, x, y, width, height, cursor, fullScreen );
   }

   GetCamera()->SetWindow( mWindow.get() );

   mCompositeViewer = new osgViewer::CompositeViewer;
//   mCompositeViewer->setThreadingModel(osgViewer::CompositeViewer::SingleThreaded);
   mCompositeViewer->addView(mViewList.front()->GetOsgViewerView());
   
   GetKeyboard()->AddKeyboardListener( mKeyboardListener.get() );
}

///////////////////////////////////////////////////////////////////////////////
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

///////////////////////////////////////////////////////////////////////////////
void Application::SetConfigPropertyValue(const std::string& name, const std::string& value)
{
   if (!mConfigProperties.insert(std::make_pair(name, value)).second)
   {
      AppConfigPropertyMap::iterator i = mConfigProperties.find(name);
      /// "i" can't be the "end()" because the insert returned false, meaning it does have that key.
      i->second = value;
   }
}

///////////////////////////////////////////////////////////////////////////////
void Application::RemoveConfigPropertyValue(const std::string& name)
{
   mConfigProperties.erase(name);
}

///////////////////////////////////////////////////////////////////////////////
bool Application::ParseConfigFile(const std::string& file)
{
   ApplicationConfigHandler handler;
   dtUtil::XercesParser parser;

   bool parsed_well = parser.Parse(file, handler, "application.xsd");
   if (!parsed_well)
   {
      LOG_ERROR("The Application config file, " + file + ", wasn't parsed correctly.");
   } 

   AppXMLApplicator applicator;
   bool applied_well = applicator(handler.mConfigData, this);
   if (!applied_well)
   {
      LOG_ERROR("The Application config file data wasn't applied correctly.");
   }

   return( applied_well || parsed_well );
}

///////////////////////////////////////////////////////////////////////////////
std::string dtABC::Application::GenerateDefaultConfigFile(const std::string& filename)
{
   std::string existingfile = dtCore::FindFileInPathList( filename );

   if( !existingfile.empty() )
   {
      LOG_WARNING("Can't generate new configuration file: file already exists: " + existingfile);
      return existingfile;
   }

   // write out a new file
   ApplicationConfigWriter writer;
   writer( filename, GetDefaultConfigData());

   // return the resource path to the new file
   return dtCore::FindFileInPathList( filename );
}


///////////////////////////////////////////////////////////////////////////////
void dtABC::Application::SetNextStatisticsType()
{
   mStats->SelectNextType();
}


///////////////////////////////////////////////////////////////////////////////
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
   data.VIEW_NAME = "defaultView";
   
   data.SCENE_INSTANCE = "defaultScene";
   data.WINDOW_INSTANCE = "defaultWin";
   data.CAMERA_INSTANCE = "defaultCam";

   data.RESOLUTION.width = 640;
   data.RESOLUTION.height = 480;
   data.RESOLUTION.bitDepth = 24;
   data.RESOLUTION.refresh = 60;

   dtUtil::Log& logger = dtUtil::Log::GetInstance();
   data.LOG_LEVELS.insert(make_pair(logger.GetName(), logger.GetLogLevelString(dtUtil::Log::LOG_WARNING)));

   return data;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// --- applicator's implementation --- //
bool Application::AppXMLApplicator::operator ()(const ApplicationConfigData& data, dtABC::Application* app)
{
   // set up the View
   if (!data.VIEW_NAME.empty())
   {
      dtCore::View* view = app->GetView();
      if (view != NULL)
      {
         view->SetName( data.VIEW_NAME );
      }
   }

   // set up the scene
   if (!data.SCENE_NAME.empty())
   {
      dtCore::Scene* scene = app->GetScene();
      if (scene != NULL)
      {
         scene->SetName( data.SCENE_NAME );
      }
   }

   // set up the camera
   if (!data.CAMERA_NAME.empty())
   {
      dtCore::Camera* camera = app->GetCamera();
      if (camera != NULL)
      {
         camera->SetName( data.CAMERA_NAME );
      }
   }

   // apply the window settings
   dtCore::DeltaWin* dwin = app->GetWindow();

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
   
   bool valid = true; //optimistic
   
   // connect the camera, scene, and window
   // since they might not be the same as the app's instances, we will use the instance management layer
   dtCore::DeltaWin* dinst = dtCore::DeltaWin::GetInstance( data.WINDOW_INSTANCE );
   dtCore::Camera *camera = dtCore::Camera::GetInstance( data.CAMERA_INSTANCE );
   dtCore::View *view = dtCore::View::GetInstance( data.VIEW_NAME );
   dtCore::Scene* sinst = dtCore::Scene::GetInstance( data.SCENE_INSTANCE );

   if( (dinst != NULL) && (camera != NULL) )
   {
      camera->SetWindow( dinst );
   }
   else
   {
      LOG_WARNING("Application:Can't find instance of DeltaWin, " + data.SCENE_INSTANCE );
      valid = false;
   }

   if( (camera != NULL) && (view != NULL) )
   {
      view->SetCamera( camera );
   }
   else
   {
       LOG_WARNING("Application:Can't find instance of Camera, " + data.CAMERA_INSTANCE );
       valid = false;
   }
   
   if( (sinst != NULL) && (view != NULL) )
   {
      view->SetScene( sinst );
   }
   else
   {
      LOG_WARNING("Application:Can't find instance of Scene, " + data.SCENE_INSTANCE );
      valid = false;
   }

   return valid;
}
////////////////////////////////////////////////////////
void Application::AddView(dtCore::View &view)
{
   if (mCompositeViewer.get() == NULL)
   {
      mCompositeViewer = new osgViewer::CompositeViewer;
   }
   
   mCompositeViewer->addView(view.GetOsgViewerView());
   mViewList.push_back(&view);
}

////////////////////////////////////////////////////////
void Application::RemoveView(dtCore::View &view)
{
   ViewList::iterator it = std::find(mViewList.begin(), mViewList.end(), &view);
   if (it != mViewList.end())
   {
      mViewList.erase(it);
      mCompositeViewer->removeView(view.GetOsgViewerView());
   }
}
