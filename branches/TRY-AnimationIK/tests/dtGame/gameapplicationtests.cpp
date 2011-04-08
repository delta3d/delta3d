/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
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
 * @author David Guthrie
 */
#include <prefix/dtgameprefix-src.h>
#include <dtUtil/log.h>
#include <dtUtil/macros.h>
#include <dtCore/refptr.h>
#include <dtGame/gameapplication.h>

#include <cppunit/extensions/HelperMacros.h>

#ifdef DELTA_WIN32
   #include <Windows.h>
   #define SLEEP(milliseconds) Sleep((milliseconds))
#else
   #include <unistd.h>
   #define SLEEP(milliseconds) usleep(((milliseconds) * 1000))
#endif

class GameApplicationTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(GameApplicationTests);

      CPPUNIT_TEST(TestProperties);

   CPPUNIT_TEST_SUITE_END();

public:
   void setUp()
   {
      mGameApplication = new dtGame::GameApplication(0, NULL);
   }
   
   void tearDown()
   {
      mGameApplication = NULL;
   }

   void TestProperties()
   {
      std::string name = "client";
      
      CPPUNIT_ASSERT(mGameApplication->GetGameLibraryName().empty());
      mGameApplication->SetGameLibraryName(name);
      CPPUNIT_ASSERT(mGameApplication->GetGameLibraryName() == name);
   }

private:
   dtUtil::Log* mLogger;

   dtCore::RefPtr<dtGame::GameApplication> mGameApplication;

};