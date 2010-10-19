/* -*-c++-*-
* allTests - This source file (.h & .cpp) - Using 'The MIT License'
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/
#include <prefix/unittestprefix.h>
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

