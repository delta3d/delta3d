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

#include <dtDIS/mastercomponent.h>       // for testing
#include <dtDIS/sharedstate.h>

#include <dtCore/scene.h>                // for using the GM
#include <dtGame/gamemanager.h>          // for using the GM

#include <cstddef>                       // for size_t definition

#include <dtABC/application.h>
extern dtABC::Application& GetGlobalApplication();

namespace dtDIS
{
   /// a way to spin some cycles while I am waiting for an actual design.
   class MasterComponentTests : public CPPUNIT_NS::TestFixture
   {
   public:
      void setup();
      void teardown(); 

      void Test();

      CPPUNIT_TEST_SUITE( MasterComponentTests );
         CPPUNIT_TEST( Test );
      CPPUNIT_TEST_SUITE_END();
   };
}

using namespace dtDIS;
CPPUNIT_TEST_SUITE_REGISTRATION( MasterComponentTests );

void MasterComponentTests::Test()
{
   //dtUtil::Log::GetInstance().SetLogLevel( dtUtil::Log::LOG_DEBUG );

   // get a GM going
   dtCore::RefPtr<dtCore::Scene> tscene = new dtCore::Scene();
   dtCore::RefPtr<dtGame::GameManager> gm = new dtGame::GameManager( *GetGlobalApplication().GetScene() );

   // add the component for testing
   ///\todo supply real file handles
   dtDIS::SharedState config;
   dtDIS::ConnectionData cd;
   
   config.SetConnectionData(cd);

   dtCore::RefPtr<dtDIS::MasterComponent> mc = new dtDIS::MasterComponent( &config );
   // needs to load at least one plugin for the following unit test to pass
   gm->AddComponent( *mc , dtGame::GameManager::ComponentPriority::NORMAL );
   //////CPPUNIT_ASSERT( mc->GetPluginManager().GetRegistry().size() > 0 );

   // remove the component for testing
   gm->RemoveComponent( *mc );
   //////CPPUNIT_ASSERT_EQUAL( mc->GetPluginManager().GetRegistry().size() , size_t(0) );
}
