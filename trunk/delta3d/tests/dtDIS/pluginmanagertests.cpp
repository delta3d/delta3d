#include <cppunit/extensions/HelperMacros.h>

#include <dtDIS/PluginManager.h>         // for testing
#include <dtCore/base.h>                 // for base class

namespace dtTest
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

using namespace dtTest;
CPPUNIT_TEST_SUITE_REGISTRATION( PluginManagerTests );

void PluginManagerTests::TestLoadUnload()
{
   dtDIS::PluginManager plug_mgr;

   std::string plug_name("plugins/Plugin_EntityStatePdu.dll");

   // test observability
   dtCore::RefPtr<dtTest::LibRegObserver> lrObs = new dtTest::LibRegObserver();
   plug_mgr.GetLoadedSignal().connect_slot( lrObs.get() , &dtTest::LibRegObserver::OnLoaded );
   plug_mgr.GetUnloadedSignal().connect_slot( lrObs.get() , &dtTest::LibRegObserver::OnUnloaded );

   CPPUNIT_ASSERT( plug_mgr.LoadPlugin( plug_name ) );
   CPPUNIT_ASSERT( lrObs->mLoadedHit );

   CPPUNIT_ASSERT( plug_mgr.UnloadPlugin( plug_name ) );
   CPPUNIT_ASSERT( lrObs->mUnloadedHit );

   // try it again to use the UnloadAllPlugins function
   lrObs->mLoadedHit = true;
   lrObs->mUnloadedHit = true;
   CPPUNIT_ASSERT( plug_mgr.LoadPlugin( plug_name ) );
   CPPUNIT_ASSERT( lrObs->mLoadedHit );

   plug_mgr.UnloadAllPlugins();
   CPPUNIT_ASSERT( lrObs->mUnloadedHit );

   CPPUNIT_ASSERT_EQUAL( plug_mgr.GetRegistry().size() , size_t(0) );
}

