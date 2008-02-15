/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006 MOVES Institute
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
 * Erik Johnson 05/16/2006
 */
#include <prefix/dtgameprefix-src.h>
#include <cppunit/extensions/HelperMacros.h>
#include <dtABC/application.h>
#include <dtCore/deltawin.h>
#include <dtUtil/exception.h>
#include <osg/ref_ptr>
#include <osgViewer/GraphicsWindow>

extern dtABC::Application& GetGlobalApplication();

///used to test the DeltaWin functionality
class  DeltaWinTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(DeltaWinTests);
      CPPUNIT_TEST(TestWindow);
   CPPUNIT_TEST_SUITE_END();

public:

   void setUp();
   void tearDown();
   void TestWindow();
   osgViewer::GraphicsWindow * CreateGraphicsWindow();
};

CPPUNIT_TEST_SUITE_REGISTRATION(DeltaWinTests);

void DeltaWinTests::setUp()
{
}

void DeltaWinTests::tearDown()
{
}

void DeltaWinTests::TestWindow()
{
   {
      dtCore::RefPtr<dtCore::DeltaWin> win = GetGlobalApplication().GetWindow();

      //make sure these guys throw an exception
//      CPPUNIT_ASSERT_THROW(win->SetKeyboard(0), dtUtil::Exception);
//      CPPUNIT_ASSERT_THROW(win->SetMouse(0), dtUtil::Exception);

      const int x = 111;
      const int y = 222;
      const int w = 444;
      const int h = 555;
      win->SetPosition(x,y,w,h);

      int x2, y2, w2, h2;
      win->GetPosition(x2, y2, w2, h2);

      CPPUNIT_ASSERT_MESSAGE("Window GetPosition() didn't return the values from SetPosition()",
                              (x2==x || y2==y || w2==w || h2==h));

      win->SetPosition(0, 0, 50, 50);

      const std::string title("hello there");
      win->SetWindowTitle(title);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("GetWindowTitle() didn't return the value from SetWindowTitle()",
                                    title,
                                    win->GetWindowTitle() );

   }
   {
      //overloaded constructor test
      osg::ref_ptr<osgViewer::GraphicsWindow> gw = 0; 
      
      gw = CreateGraphicsWindow();
      
      dtCore::RefPtr<dtCore::DeltaWin> win = new dtCore::DeltaWin("testWin", *gw);
      CPPUNIT_ASSERT_MESSAGE("Overloaded constructor failed", win.valid() );
   }
}

osgViewer::GraphicsWindow * DeltaWinTests::CreateGraphicsWindow()
{
   osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;

   traits->readDISPLAY();
   if (traits->displayNum<0)
      traits->displayNum = 0;

   traits->windowName = "testWin";
   traits->screenNum = 0;
   traits->x = 0;
   traits->y = 0;
   traits->width = 640;
   traits->height = 480;
   traits->doubleBuffer = true;
   
   osg::GraphicsContext* gc = osg::GraphicsContext::createGraphicsContext(traits.get());
   
   osgViewer::GraphicsWindow* gw = dynamic_cast<osgViewer::GraphicsWindow*>(gc);
   if (gw)
   {
      gw->getEventQueue()->getCurrentEventState()->setWindowRectangle(0, 0, 640, 480 );
   }
   
   return (gw);
}

