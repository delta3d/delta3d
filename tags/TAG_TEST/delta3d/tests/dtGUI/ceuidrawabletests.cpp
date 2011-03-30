/* -*-c++-*-
* allTests - This source file (.h & .cpp) - Using 'The MIT License'
* Copyright (C) 2006-2008, MOVES Institute
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
*
* @author John K. Grant
*/
#include <prefix/unittestprefix.h>
//#include <prefix/dtgameprefix.h>
#include <cppunit/extensions/HelperMacros.h>

#include <dtGUI/ceuidrawable.h>
#include <dtCore/keyboard.h>
#include <dtCore/deltawin.h>
#include <CEGUIInputEvent.h>
#include <dtGUI/ceguikeyboardlistener.h>
#include <CEGUIWindow.h>

#include "guiapp.h"
#include "guimanager.h"

namespace dtTest
{
   /// unit tests for dtGUI::CEUIDrawable
   class CEUIDrawableTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE( CEUIDrawableTests );
//      CPPUNIT_TEST( TestKeyMap );
      CPPUNIT_TEST( TestInput );
      CPPUNIT_TEST_SUITE_END();

      public:
         void setUp() {}

         void tearDown() {}

//         void TestKeyMap();
         void TestInput();

      private:
   };

   class KeyScanObserver
   {
   public:
      KeyScanObserver(CEGUI::Key::Scan key):
         mKey(key),
         mHit(false)
      {
      }

      bool Handle(const CEGUI::EventArgs& ea)
      {
         mHit = true;
         const CEGUI::KeyEventArgs& kea = static_cast<const CEGUI::KeyEventArgs&>( ea );
         if ( kea.scancode == mKey )
         {
            return true;
         }

         return false;
      }

      bool GetHit() const { return mHit; }
      void ResetHit() { mHit = false; }

   private:
      CEGUI::Key::Scan mKey;
      bool mHit;
   };
}

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( dtTest::CEUIDrawableTests );

using namespace dtTest;

//TODO ledocc
//void CEUIDrawableTests::TestKeyMap()
//{
//   CPPUNIT_ASSERT_EQUAL( CEGUI::Key::A , dtGUI::CEGUIKeyboardListener::KeyboardKeyToKeyScan(dtCore::Keyboard::KeyCharacterToKeyboardKey(Producer::KeyChar_a)) );
//}

void CEUIDrawableTests::TestInput()
{
   // start an application with GUI support
   dtCore::RefPtr<dtTest::GUIApp> app( new dtTest::GUIApp() );
   app->GetWindow()->SetPosition(0, 0, 50, 50);
   app->Config();

   dtCore::RefPtr<dtCore::Keyboard> kb = app->GetKeyboard();
   CPPUNIT_ASSERT_EQUAL( 2, static_cast<int>(kb->GetListeners().size()) );  // better be application & gui listeners

   // CHECK THE ORDER OF THE LISTENERS!  GUI SHOULD BE FIRST
   dtGUI::GUIManager* gmgr = app->GetGUIManager();
   CPPUNIT_ASSERT(static_cast<const dtCore::KeyboardListener*>(gmgr->GetGUIDrawable()->GetKeyboardListener())
                       == kb->GetListeners().front().get() );

   KeyScanObserver obs(CEGUI::Key::N);
   obs.ResetHit();
   CPPUNIT_ASSERT( !obs.GetHit() );  // better not be hit

   CEGUI::Window* sheet = dtGUI::GUIManager::CreateGUIWindow( "DefaultWindow" , "my_window" );
   CPPUNIT_ASSERT( sheet );  // better be valid
   sheet->subscribeEvent( CEGUI::Window::EventKeyDown, CEGUI::Event::Subscriber(&KeyScanObserver::Handle,&obs) );
   dtGUI::GUIManager::SetLayout( sheet );

   // test to see if the application's pressed callback is connected
   CPPUNIT_ASSERT( !kb->KeyDown('M') );  // better NOT handle it
   //CPPUNIT_ASSERT( obs.GetHit() );  // better be hit
   //CPPUNIT_ASSERT( kb->KeyPress('n') );  // better handle it

   //obs.ResetHit();
   //CPPUNIT_ASSERT( !obs.GetHit() );  // better not be hit
   //CPPUNIT_ASSERT( !obs.GetHit() );  // better not be hit

   //// test to see if the applicaiton's released callback is connected
   //CPPUNIT_ASSERT( !kb->KeyRelease('M') );  // better NOT handle it
   //CPPUNIT_ASSERT( !obs.GetHit() );  // better be hit
   //CPPUNIT_ASSERT( obs.GetHit() );  // better be hit
   //CPPUNIT_ASSERT( kb->KeyRelease('n') );  // better handle it
}

