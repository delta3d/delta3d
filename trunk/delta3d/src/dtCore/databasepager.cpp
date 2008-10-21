#include <dtCore/databasepager.h>
#include <dtCore/deltadrawable.h>
#include <dtUtil/configproperties.h>

using namespace dtCore;

const std::string DatabasePager::DATABASE_PAGER_PRECOMPILE_OBJECTS("System.DatabasePager.PrecompileObjects");
const std::string DatabasePager::DATABASE_PAGER_MAX_OBJECTS_TO_COMPILE_PER_FRAME("System.DatabasePager.MaxObjectsToCompilePerFrame");
const std::string DatabasePager::DATABASE_PAGER_MIN_TIME_FOR_OBJECT_COMPILE("System.DatabasePager.MinObjectCompileTime");
const std::string DatabasePager::DATABASE_PAGER_TARGET_FRAMERATE("System.DatabasePager.TargetFrameRate");
const std::string DatabasePager::DATABASE_PAGER_DRAWABLE_POLICY("System.DatabasePager.DrawablePolicy");
const std::string DatabasePager::DATABASE_PAGER_THREAD_PRIORITY("System.DatabasePager.ThreadPriority");
const std::string DatabasePager::DATABASE_PAGER_EXPIRY_DELAY("System.DatabasePager.ExpiryDelay");



dtCore::DatabasePager::DatabasePager():
mDatabasePager(osgDB::DatabasePager::create()),
mConfigProperties(NULL)
{
}

//////////////////////////////////////////////////////////////////////////
dtCore::DatabasePager::DatabasePager( osgDB::DatabasePager &pager ):
mDatabasePager(&pager)
{

}

//////////////////////////////////////////////////////////////////////////
dtCore::DatabasePager::~DatabasePager()
{

}

//////////////////////////////////////////////////////////////////////////
void dtCore::DatabasePager::RegisterDrawable( dtCore::DeltaDrawable &drawable)
{
   mDatabasePager->registerPagedLODs( drawable.GetOSGNode() );
}



/////////////////////////////////////////////
#include <dtUtil/stringutils.h>
static void ReadDBPagerConfig(osgDB::DatabasePager& pager, dtUtil::ConfigProperties* config)
{
   if (config == NULL)
   {
      return;
   }

   std::string value;

   value = config->GetConfigPropertyValue(DatabasePager::DATABASE_PAGER_PRECOMPILE_OBJECTS);
   if (!value.empty())
   {
      bool precompile = dtUtil::ToType<bool>(value);
      pager.setDoPreCompile(precompile);
   }

   value = config->GetConfigPropertyValue(DatabasePager::DATABASE_PAGER_MAX_OBJECTS_TO_COMPILE_PER_FRAME);
   if (!value.empty())
   {
      unsigned int maxNum = dtUtil::ToType<unsigned int>(value);
      //Can't be less than 1.  That doesn't make sense.
      maxNum = std::max(maxNum, 1U);
      pager.setMaximumNumOfObjectsToCompilePerFrame(maxNum);
   }

   value = config->GetConfigPropertyValue(DatabasePager::DATABASE_PAGER_MIN_TIME_FOR_OBJECT_COMPILE);
   if (!value.empty())
   {
      float minTime = dtUtil::ToType<float>(value);
      pager.setMinimumTimeAvailableForGLCompileAndDeletePerFrame(minTime);
   }

   value = config->GetConfigPropertyValue(DatabasePager::DATABASE_PAGER_TARGET_FRAMERATE);
   if (!value.empty())
   {
      double target = dtUtil::ToType<double>(value);
      pager.setTargetFrameRate(target);
   }
//   else if (dtCore::System::GetInstance().GetUsesFixedTimeStep())
//   {
//      pager.setTargetFrameRate(dtCore::System::GetInstance().GetFrameRate());
//   }

   value = config->GetConfigPropertyValue(DatabasePager::DATABASE_PAGER_DRAWABLE_POLICY);
   if (!value.empty())
   {
      if (value == "DoNotModify")
      {
          pager.setDrawablePolicy(osgDB::DatabasePager::DO_NOT_MODIFY_DRAWABLE_SETTINGS);
      }
      else if (value == "DisplayList" || value == "DisplayLists" || value == "DL")
      {
          pager.setDrawablePolicy(osgDB::DatabasePager::USE_DISPLAY_LISTS);
      }
      else if (value == "VBO")
      {
          pager.setDrawablePolicy(osgDB::DatabasePager::USE_VERTEX_BUFFER_OBJECTS);
      }
      else if (value == "VertexArrays" || value == "VertexArray"||  value == "VA")
      {
          pager.setDrawablePolicy(osgDB::DatabasePager::USE_VERTEX_ARRAYS);
      }
   }

   value = config->GetConfigPropertyValue(DatabasePager::DATABASE_PAGER_THREAD_PRIORITY);
   if (!value.empty())
   {
      if (value == "DEFAULT")
      {
          pager.setSchedulePriority(OpenThreads::Thread::THREAD_PRIORITY_DEFAULT);
      }
      else if (value == "MIN")
      {
         pager.setSchedulePriority(OpenThreads::Thread::THREAD_PRIORITY_MIN);
      }
      else if (value == "LOW")
      {
         pager.setSchedulePriority(OpenThreads::Thread::THREAD_PRIORITY_LOW);
      }
      else if (value == "NOMINAL")
      {
         pager.setSchedulePriority(OpenThreads::Thread::THREAD_PRIORITY_NOMINAL);
      }
      else if (value == "HIGH")
      {
         pager.setSchedulePriority(OpenThreads::Thread::THREAD_PRIORITY_HIGH);
      } 
      else if (value == "MAX")
      {
         pager.setSchedulePriority(OpenThreads::Thread::THREAD_PRIORITY_MAX);
      }
   }

   value = config->GetConfigPropertyValue(DatabasePager::DATABASE_PAGER_EXPIRY_DELAY);
   if (!value.empty())
   {
      double delay = dtUtil::ToType<double>(value);
      pager.setExpiryDelay(delay);
   }
}

//////////////////////////////////////////////////////////////////////////
void dtCore::DatabasePager::SetConfiguration( dtUtil::ConfigProperties* config )
{
   ReadDBPagerConfig( *mDatabasePager, config );
   mConfigProperties = config;
}

//////////////////////////////////////////////////////////////////////////
dtUtil::ConfigProperties* dtCore::DatabasePager::GetConfiguration()
{
   return mConfigProperties;
}


//////////////////////////////////////////////////////////////////////////
const dtUtil::ConfigProperties* dtCore::DatabasePager::GetConfiguration() const
{
   return mConfigProperties;
}

//////////////////////////////////////////////////////////////////////////
void dtCore::DatabasePager::SetTargetFrameRate( double targetFR )
{
   mDatabasePager->setTargetFrameRate(targetFR);
}

//////////////////////////////////////////////////////////////////////////
double dtCore::DatabasePager::GetTargetFrameRate() const
{
   return mDatabasePager->getTargetFrameRate();
}

//////////////////////////////////////////////////////////////////////////
void dtCore::DatabasePager::SignalBeginFrame(const osg::FrameStamp* framestamp) const
{
   mDatabasePager->signalBeginFrame(framestamp);
}

//////////////////////////////////////////////////////////////////////////
void dtCore::DatabasePager::SignalEndFrame() const
{
   mDatabasePager->signalEndFrame();
}

//////////////////////////////////////////////////////////////////////////
void dtCore::DatabasePager::UpdateSceneGraph(double currentFrameTime) const
{
   mDatabasePager->updateSceneGraph(currentFrameTime);
}

//////////////////////////////////////////////////////////////////////////
void dtCore::DatabasePager::CompileGLObjects(osg::State& state, double& availableTime) const
{
   mDatabasePager->compileGLObjects(state, availableTime);
}
