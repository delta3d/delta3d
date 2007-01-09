/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation.
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
#include <dtUtil/fileutils.h>
#include <dtUtil/exception.h>

#include <dtCore/refptr.h>
#include <dtCore/scene.h>
#include <dtCore/system.h>
#include <dtCore/globals.h>
#include <dtCore/camera.h>
#include <dtCore/deltawin.h>

#include <cppunit/extensions/HelperMacros.h>

#if defined (WIN32) || defined (_WIN32) || defined (__WIN32__)
   #include <Windows.h>
   #define SLEEP(milliseconds) Sleep((milliseconds))
#else
   #include <unistd.h>
   #define SLEEP(milliseconds) usleep(((milliseconds) * 1000))
#endif

class CameraTests : public CPPUNIT_NS::TestFixture
{
      CPPUNIT_TEST_SUITE(CameraTests);
   
         CPPUNIT_TEST(TestSaveScreenShot);
   
      CPPUNIT_TEST_SUITE_END();
   
   public:
      void setUp()
      {
         try
         {
            dtCore::SetDataFilePathList(dtCore::GetDeltaDataPathList());
      
            mScene = new dtCore::Scene;
            mWin = new dtCore::DeltaWin();
            mWin->SetPosition(0, 0, 50, 50);
            mCamera = new dtCore::Camera;
            mCamera->SetWindow(mWin.get());
            mCamera->SetScene(mScene.get());
            dtCore::System::GetInstance().Config();

            dtCore::System::GetInstance().SetShutdownOnWindowClose(false);
            dtCore::System::GetInstance().Start();
             
            dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
            std::string currentDir = fileUtils.CurrentDirectory();
            std::string projectDir("dtCore");
            if (currentDir.substr(currentDir.size() - projectDir.size()) != projectDir)
               fileUtils.PushDirectory(projectDir);
            
         }
         catch (const dtUtil::Exception& e)
         {
            CPPUNIT_FAIL((std::string("Error: ") + e.What()).c_str());
         }
         catch (const std::exception& ex)
         {
            CPPUNIT_FAIL(std::string("Error: ") + ex.what());
         }
      }
      
      void tearDown()
      {
         dtCore::System::GetInstance().Stop();
         mScene = NULL;
         mCamera->SetScene(NULL);
         mCamera->SetWindow(NULL);
         mCamera = NULL;
         mWin = NULL;
      
         dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
         
         if (fileUtils.DirExists(SCREEN_SHOT_DIR))
            fileUtils.DirDelete(SCREEN_SHOT_DIR, true);
         
         std::string currentDir = fileUtils.CurrentDirectory();
         std::string projectDir("dtCore");
         if (currentDir.substr(currentDir.size() - projectDir.size()) == projectDir)
            fileUtils.PopDirectory();
      }
      
      void TestSaveScreenShot()
      {
         dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
         fileUtils.MakeDirectory(SCREEN_SHOT_DIR);
         const std::string prefix = SCREEN_SHOT_DIR + "/Mojo";
         const std::string result = mCamera->TakeScreenShot(prefix);
         
         CPPUNIT_ASSERT_MESSAGE("The resulting string should be longer.", result.size() > prefix.size());
         
         CPPUNIT_ASSERT_MESSAGE("The result should begin with the prefix.", 
            prefix == result.substr(0,prefix.size()));
         
         CPPUNIT_ASSERT_MESSAGE(result + " should not exist yet.", !fileUtils.FileExists(result));
         
         dtCore::System::GetInstance().Step();      
         CPPUNIT_ASSERT_MESSAGE(result + " should exist.", fileUtils.FileExists(result));
   
         fileUtils.DirDelete(SCREEN_SHOT_DIR, true);

         dtCore::System::GetInstance().Step();      
         CPPUNIT_ASSERT_MESSAGE("A screenshot should not have been written again.", !fileUtils.FileExists(result));
      }
   
   private:
      static const std::string SCREEN_SHOT_DIR;
      
      dtCore::RefPtr<dtCore::Scene> mScene;
      dtCore::RefPtr<dtCore::Camera> mCamera;
      dtCore::RefPtr<dtCore::DeltaWin> mWin;
};


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(CameraTests);

const std::string CameraTests::SCREEN_SHOT_DIR("TestScreenShot");
