#include <cppunit/extensions/HelperMacros.h>

#include <dtDIS/pluginmanager.h>         // for testing
#include <dtCore/base.h>                 // for base class

namespace dtDIS
{
   /// a way to spin some cycles while I am waiting for an actual design.
   class PluginManagerTests : public CPPUNIT_NS::TestFixture
   {
   public:
      void setup();
      void teardown(); 

      void TestLoadUnload();

      CPPUNIT_TEST_SUITE( PluginManagerTests );
         CPPUNIT_TEST( TestLoadUnload );
      CPPUNIT_TEST_SUITE_END();
   };

   /// a class that does bookkeeping for unit tests
   class LibRegObserver : public dtCore::Base
   {
   public:
      LibRegObserver()
      {
         mLoadedHit = false;
         mUnloadedHit = false;
      }

      void OnLoaded(const std::string& path, dtDIS::PluginManager::RegistryEntry& entry)
      {
         mLoadedHit = true;
         mLoadedPath = path;
         mLoadedEntry = entry;
      }

      void OnUnloaded(const std::string& path, dtDIS::PluginManager::RegistryEntry& entry)
      {
         mUnloadedHit = true;
         mUnloadedPath = path;
         mUnloadedEntry = entry;
      }

   protected:
      ~LibRegObserver()
      {
      }

   public:
      bool mLoadedHit, mUnloadedHit;

      std::string mLoadedPath, mUnloadedPath;
      dtDIS::PluginManager::RegistryEntry mLoadedEntry, mUnloadedEntry;
   };
}

using namespace dtDIS;
//CPPUNIT_TEST_SUITE_REGISTRATION( PluginManagerTests );

void PluginManagerTests::TestLoadUnload()
{
   dtDIS::PluginManager plugMgr;

   std::string pluginBaseName("Plugin_EntityStatePdu");
   
   std::string pluginPlatformDependentName(dtUtil::LibrarySharingManager::GetPlatformSpecificLibraryName(pluginBaseName));
   
   std::string plugName("plugins/" + pluginPlatformDependentName);

   // test observability
   dtCore::RefPtr<dtDIS::LibRegObserver> lrObs = new dtDIS::LibRegObserver();
   plugMgr.GetLoadedSignal().connect_slot( lrObs.get() , &dtDIS::LibRegObserver::OnLoaded );
   plugMgr.GetUnloadedSignal().connect_slot( lrObs.get() , &dtDIS::LibRegObserver::OnUnloaded );

   CPPUNIT_ASSERT( plugMgr.LoadPlugin( plugName ) );
   CPPUNIT_ASSERT( lrObs->mLoadedHit );

   CPPUNIT_ASSERT( plugMgr.UnloadPlugin( plugName ) );
   CPPUNIT_ASSERT( lrObs->mUnloadedHit );

   // try it again to use the UnloadAllPlugins function
   lrObs->mLoadedHit = true;
   lrObs->mUnloadedHit = true;
   CPPUNIT_ASSERT( plugMgr.LoadPlugin( plugName ) );
   CPPUNIT_ASSERT( lrObs->mLoadedHit );

   plugMgr.UnloadAllPlugins();
   CPPUNIT_ASSERT( lrObs->mUnloadedHit );

   CPPUNIT_ASSERT_EQUAL( plugMgr.GetRegistry().size() , size_t(0) );
}

