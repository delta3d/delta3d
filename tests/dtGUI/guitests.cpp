/* -*-c++-*-
 * allTests - This source file (.h & .cpp) - Using 'The MIT License'
 * Copyright 2013,  David Guthrie
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
 * David Guthrie
 */

#include <prefix/unittestprefix.h>
//#include <prefix/dtgameprefix.h>
#include <cppunit/extensions/HelperMacros.h>

#include <dtCore/keyboard.h>
#include <dtCore/deltawin.h>
#include <dtGUI/gui.h>
#include <CEGUIWindow.h>

#include <dtABC/application.h>

namespace dtTest
{
   /// unit tests for dtGUI::GUI
   class GUITests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE( GUITests );
      CPPUNIT_TEST( TestCreateMultiple );
      CPPUNIT_TEST_SUITE_END();

   public:
      void setUp() {}

      void tearDown() {}

      void TestCreateMultiple()
      {
         // start an application with GUI support
         dtCore::RefPtr<dtABC::Application> app( new dtABC::Application );
         app->GetWindow()->SetPosition(0, 0, 50, 50);
         app->Config();

         dtCore::RefPtr<dtCore::Keyboard> kb = app->GetKeyboard();

         CPPUNIT_ASSERT(CEGUI::System::getSingletonPtr() == NULL);
         // create 2 guis to make sure it doesn't crash.
         dtCore::RefPtr<dtGUI::GUI> gui = new dtGUI::GUI(app->GetCamera(), kb, app->GetMouse());

         CPPUNIT_ASSERT(CEGUI::System::getSingletonPtr() != NULL);
         CPPUNIT_ASSERT(CEGUI::System::getSingletonPtr()->getRenderer() != NULL);

         dtCore::RefPtr<dtGUI::GUI> gui2 = new dtGUI::GUI(app->GetCamera(), kb, app->GetMouse());

         CPPUNIT_ASSERT(CEGUI::System::getSingletonPtr() != NULL);
         CPPUNIT_ASSERT(CEGUI::System::getSingletonPtr()->getRenderer() != NULL);

         gui = NULL;

         CPPUNIT_ASSERT_MESSAGE("Deleting 1 GUI but leaving another around should not cause the system to be deleted.", CEGUI::System::getSingletonPtr() != NULL);
         CPPUNIT_ASSERT_MESSAGE("Deleting 1 GUI but leaving another around should not cause the renderer.", CEGUI::System::getSingletonPtr()->getRenderer() != NULL);

         gui2 = NULL;

         CPPUNIT_ASSERT(CEGUI::System::getSingletonPtr() == NULL);
      }

   private:
   };
   // Registers the fixture into the 'registry'
   CPPUNIT_TEST_SUITE_REGISTRATION( GUITests );

}

