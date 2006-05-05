/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Delta3D
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
 * @author John K. Grant
 */
#include <cppunit/extensions/HelperMacros.h>

#include <dtGUI/ceuidrawable.h>
#include <dtCore/keyboard.h>
#include <CEGUI/CEGUIInputEvent.h>
#include <dtGUI/ceguikeyboardlistener.h>
#include <CEGUI/CEGUIWindow.h>

#include "guiapp.h"
#include "guimanager.h"

namespace dtTest
{
   /// unit tests for dtCore::Application
   class CEUIDrawableTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE( CEUIDrawableTests );
      CPPUNIT_TEST( TestKeyMap );
      CPPUNIT_TEST( TestInput );
      CPPUNIT_TEST_SUITE_END();

      public:
         void setUp() {}
         void tearDown() {}

         void TestKeyMap();
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
         if( kea.scancode == mKey )
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

void CEUIDrawableTests::TestKeyMap()
{
   CPPUNIT_ASSERT_EQUAL( CEGUI::Key::A , dtGUI::CEGUIKeyboardListener::KeyboardKeyToKeyScan(dtCore::Keyboard::KeyCharacterToKeyboardKey(Producer::KeyChar_a)) );
}

void CEUIDrawableTests::TestInput()
{
   // start an application with GUI support
   dtCore::RefPtr<dtTest::GUIApp> app( new dtTest::GUIApp() );
   app->Config();

   dtCore::RefPtr<dtCore::Keyboard> kb = app->GetKeyboard();
   CPPUNIT_ASSERT_EQUAL( 2, static_cast<int>(kb->GetListeners().size()) );  // better be application & gui listeners

   // CHECK THE ORDER OF THE LISTENERS!  GUI SHOULD BE FIRST
   dtGUI::GUIManager* gmgr = app->GetGUIManager();
   CPPUNIT_ASSERT_EQUAL(static_cast<const dtCore::KeyboardListener*>(gmgr->GetGUIDrawable()->GetKeyboardListener()),
                        kb->GetListeners().front().get() );

   KeyScanObserver obs(CEGUI::Key::N);
   obs.ResetHit();
   CPPUNIT_ASSERT( !obs.GetHit() );  // better not be hit

   CEGUI::Window* sheet = dtGUI::GUIManager::CreateGUIWindow( "DefaultWindow" , "my_window" );
   CPPUNIT_ASSERT( sheet );  // better be valid
   sheet->subscribeEvent( CEGUI::Window::EventKeyDown, CEGUI::Event::Subscriber(&KeyScanObserver::Handle,&obs) );
   dtGUI::GUIManager::SetLayout( sheet );

   // test to see if the applicaiton's pressed callback is connected
   CPPUNIT_ASSERT( !kb->KeyDown(Producer::KeyChar_M) );  // better NOT handle it
   //CPPUNIT_ASSERT( obs.GetHit() );  // better be hit
   //CPPUNIT_ASSERT( kb->KeyPress(Producer::KeyChar_n) );  // better handle it

   //obs.ResetHit();
   //CPPUNIT_ASSERT( !obs.GetHit() );  // better not be hit
   //CPPUNIT_ASSERT( !obs.GetHit() );  // better not be hit

   //// test to see if the applicaiton's released callback is connected
   //CPPUNIT_ASSERT( !kb->KeyRelease(Producer::KeyChar_M) );  // better NOT handle it
   //CPPUNIT_ASSERT( !obs.GetHit() );  // better be hit
   //CPPUNIT_ASSERT( obs.GetHit() );  // better be hit
   //CPPUNIT_ASSERT( kb->KeyRelease(Producer::KeyChar_n) );  // better handle it
}

