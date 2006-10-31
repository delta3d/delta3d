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
 * @author Erik Johnson 05/16/2006
 */

#include <cppunit/extensions/HelperMacros.h>
#include <dtCore/deltawin.h>
#include <dtUtil/exception.h>
#include <Producer/RenderSurface>
#include <osg/ref_ptr>

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
      //default constructor test
      dtCore::RefPtr<dtCore::DeltaWin> win = new dtCore::DeltaWin();
      CPPUNIT_ASSERT_MESSAGE("Default constructor didn't work", win.valid());

      //make sure these guys throw an exception
      CPPUNIT_ASSERT_THROW(win->SetKeyboard(0), dtUtil::Exception);
      CPPUNIT_ASSERT_THROW(win->SetMouse(0), dtUtil::Exception);

      const int x = 111;
      const int y = 222;
      const int w = 444;
      const int h = 555;
      win->SetPosition(x,y,w,h);

      int x2, y2, w2, h2;
      win->GetPosition(x2, y2, w2, h2);

      CPPUNIT_ASSERT_MESSAGE("Window GetPosition() didn't return the values from SetPosition()",
                              (x2==x || y2==y || w2==w || h2==h));

      const std::string title("hello there");
      win->SetWindowTitle(title);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("GetWindowTitle() didn't return the value from SetWindowTitle()",
                                    title,
                                    win->GetWindowTitle() );

   }
   {
      //overloaded constructor test
      osg::ref_ptr<Producer::RenderSurface> rs = 0; 
      
      //should throw exception due to NULL param
      CPPUNIT_ASSERT_THROW( new dtCore::DeltaWin("testWin", rs.get()),
                           dtUtil::Exception);

      rs = new Producer::RenderSurface();
      dtCore::RefPtr<dtCore::DeltaWin> win = new dtCore::DeltaWin("testWin", rs.get());
      CPPUNIT_ASSERT_MESSAGE("Overloaded constructor failed", win.valid() );
   }
}
