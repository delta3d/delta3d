/* -*-c++-*-
* allTests - This source file (.h & .cpp) - Using 'The MIT License'
* Copyright (C) 2006-2008, Alion Science and Technology Corporation
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
* This software was developed by Alion Science and Technology Corporation under
* circumstances in which the U. S. Government may have rights in the software.
*
* @author David Guthrie
*/
#include <prefix/unittestprefix.h>
#include <cppunit/extensions/HelperMacros.h>

#include <dtCore/camera.h>
#include <dtCore/deltawin.h>
#include <dtCore/infiniteterrain.h>
#include <dtCore/isector.h>
#include <dtCore/particlesystem.h>
#include <dtCore/scene.h>
#include <dtCore/system.h>
#include <dtABC/application.h>

#include <osg/io_utils>

extern dtABC::Application& GetGlobalApplication();

class IsectorTests : public CPPUNIT_NS::TestFixture 
{
   CPPUNIT_TEST_SUITE(IsectorTests);

      CPPUNIT_TEST(TestProperties);
      CPPUNIT_TEST(TestEndPosition);
      CPPUNIT_TEST(TestIntersection);

   CPPUNIT_TEST_SUITE_END();

   public:

      void setUp()
      {
         mIsector = new dtCore::Isector();
         mApp = &GetGlobalApplication();
         mScene = mApp->GetScene();
         mCamera = mApp->GetCamera();
         mWin = mApp->GetWindow();
         
         dtCore::System::GetInstance().Config();
         
         dtCore::System::GetInstance().SetShutdownOnWindowClose(false);
         dtCore::System::GetInstance().Start();
         dtCore::System::GetInstance().Step();
      }
      
      void tearDown()
      {
         mIsector = NULL;
         mApp = NULL;
         mScene = NULL;
         mCamera = NULL;
         mWin = NULL;
         dtCore::System::GetInstance().Stop();
      }

      void TestProperties()
      {
         //call it to make sure it doesn't blow.
         mIsector->Reset();        

         osg::Vec3 point(2.93f, 1.1f, 3.4f);
         const dtCore::Isector* constIsector = mIsector.get();         

         CPPUNIT_ASSERT(mIsector->GetEyePoint() == osg::Vec3(0.0f, 0.0f, 0.0f));
         mIsector->SetEyePoint(point);
         CPPUNIT_ASSERT(mIsector->GetEyePoint() == point);
         
         CPPUNIT_ASSERT(!mIsector->GetUseEyePoint()); 
         mIsector->SetUseEyePoint(true);
         CPPUNIT_ASSERT(mIsector->GetUseEyePoint()); 
         
         CPPUNIT_ASSERT(mIsector->GetStartPosition() == osg::Vec3(0.0f, 0.0f, 0.0f));
         mIsector->SetStartPosition(point);
         CPPUNIT_ASSERT(mIsector->GetStartPosition() == point);
         
         CPPUNIT_ASSERT(mIsector->GetDirection() == osg::Vec3(0.0f, 1.0f, 0.0f));
         mIsector->SetDirection(point);
         CPPUNIT_ASSERT(mIsector->GetDirection() == point);
         
         CPPUNIT_ASSERT(mIsector->GetLength() == 1000000.0f);
         mIsector->SetLength(382.0f);
         CPPUNIT_ASSERT(mIsector->GetLength() == 382.0f);
         
         CPPUNIT_ASSERT(mIsector->GetQueryRoot() == NULL);
         dtCore::RefPtr<dtCore::ParticleSystem> ps = new dtCore::ParticleSystem(); 
         mIsector->SetGeometry(ps.get());
         CPPUNIT_ASSERT(mIsector->GetQueryRoot() == ps.get());
         CPPUNIT_ASSERT(constIsector->GetQueryRoot() == ps.get());
         mIsector->ClearQueryRoot();
         CPPUNIT_ASSERT(mIsector->GetQueryRoot() == NULL);
         CPPUNIT_ASSERT(constIsector->GetQueryRoot() == NULL);
         
         CPPUNIT_ASSERT(mIsector->GetScene() == NULL);
         dtCore::RefPtr<dtCore::Scene> testScene = new dtCore::Scene();
         mIsector->SetScene(testScene.get());
         CPPUNIT_ASSERT(mIsector->GetScene() == testScene.get());
         CPPUNIT_ASSERT(constIsector->GetScene() == testScene.get());
         
         CPPUNIT_ASSERT(mIsector->GetClosestDeltaDrawable() == 0);
         CPPUNIT_ASSERT(mIsector->GetNumberOfHits() == 0);
         
         //call it to make sure it doesn't blow.
         mIsector->Reset();        
         
      }
      
      void TestEndPosition()
      {
         osg::Vec3 point1(0.0f, 0.0f, 0.0f);
         osg::Vec3 point2(10.0f, 10.0f, 10.0f);
         mIsector->SetStartPosition(point1);
         mIsector->SetEndPosition(point2);

         CPPUNIT_ASSERT(mIsector->GetDirection() == (point2 - point1));
         CPPUNIT_ASSERT_DOUBLES_EQUAL( mIsector->GetLength(), mIsector->GetDirection().length(), 0.001f );
         
      }

      void TestIntersection()
      {
         osg::Vec3 point1(0.0f, 0.0f, 1000.0f);
         osg::Vec3 point2(0.0f, 0.0f, -1000.0f);
         mIsector->SetStartPosition(point1);
         mIsector->SetEndPosition(point2);
         
         dtCore::RefPtr<dtCore::InfiniteTerrain> terrain = new dtCore::InfiniteTerrain();
         terrain->SetBuildDistance(1500.0f);
         terrain->SetSegmentDivisions(64);
         mIsector->SetScene(mScene.get());
         mScene->AddChild(terrain.get());

         dtCore::System::GetInstance().Step();

         float height = terrain->GetHeight(0.0f, 0.0f, false);
         osg::Vec3 expectedNormal;
         terrain->GetNormal(0.0f, 0.0f, expectedNormal, true);
         
               
         CPPUNIT_ASSERT(mIsector->Update());
         CPPUNIT_ASSERT(mIsector->GetNumberOfHits() > 0);
         CheckIsectorValues(height, expectedNormal);
         CPPUNIT_ASSERT(terrain.get() == mIsector->GetClosestDeltaDrawable());

         mIsector->Reset();
         CPPUNIT_ASSERT_EQUAL(0, mIsector->GetNumberOfHits());
         CPPUNIT_ASSERT(NULL == mIsector->GetClosestDeltaDrawable());
      
            
         CPPUNIT_ASSERT(mIsector->Update());
         CPPUNIT_ASSERT(mIsector->GetNumberOfHits() > 0);
         CheckIsectorValues(height, expectedNormal);
         CPPUNIT_ASSERT(mIsector->GetClosestDeltaDrawable() == terrain.get());
                
         mIsector->SetGeometry(terrain.get());
         
         mIsector->Reset();
         CPPUNIT_ASSERT(mIsector->Update());
         CPPUNIT_ASSERT(mIsector->GetNumberOfHits() > 0);
         CheckIsectorValues(height, expectedNormal);
         CPPUNIT_ASSERT(mIsector->GetClosestDeltaDrawable() == terrain.get());

         mIsector->SetUseEyePoint(true);

         //0.0, 0.0, 0.0 should be within the LOD
         mIsector->SetEyePoint(osg::Vec3(3.0f, 11.5f, 9.4f));
         mIsector->Reset();
         CPPUNIT_ASSERT(mIsector->Update());
         CPPUNIT_ASSERT(mIsector->GetNumberOfHits() > 0);
         CheckIsectorValues(height, expectedNormal);
         CPPUNIT_ASSERT(mIsector->GetClosestDeltaDrawable() == terrain.get());


         //0.0, 0.0, 0.0 should NOT be within the LOD
//         mIsector->SetEyePoint(osg::Vec3(50000.0f, 93838.5f, 9.4f));
//         mIsector->Reset();
//         CPPUNIT_ASSERT(!mIsector->Update());
//         CPPUNIT_ASSERT(mIsector->GetNumberOfHits() == 0);
//         CPPUNIT_ASSERT(mIsector->GetClosestDeltaDrawable() == NULL);
      }


   private:
      dtCore::RefPtr<dtCore::Isector> mIsector;
      dtCore::RefPtr<dtCore::Scene> mScene;
      dtCore::RefPtr<dtCore::Camera> mCamera;
      dtCore::RefPtr<dtCore::DeltaWin> mWin;
      dtCore::RefPtr<dtABC::Application> mApp;
      
      void CheckIsectorValues(const float height, const osg::Vec3& expectedNormal) const
      {
         osg::Vec3 hitPoint;
         osg::Vec3 normal;

         mIsector->GetHitPoint(hitPoint, 0);
         mIsector->GetHitPointNormal(normal, 0);
   
         std::ostringstream ss;
         ss << "The height should be \"" << height << "\" but it is \"" << hitPoint.z() << "\".";

         CPPUNIT_ASSERT_MESSAGE(ss.str(), osg::equivalent(hitPoint.z(), height, 1e-4f));                  

         ss.str("");
         ss << "The normal should be \"" << expectedNormal << "\" but it is \"" << normal << "\".";
         
         CPPUNIT_ASSERT_MESSAGE(ss.str(),osg::equivalent(normal.x(), expectedNormal.x(), 1e-1f) &&
            osg::equivalent(normal.y(), expectedNormal.y(), 1e-1f) &&
            osg::equivalent(normal.z(), expectedNormal.z(), 1e-1f));         
      }

};

CPPUNIT_TEST_SUITE_REGISTRATION(IsectorTests);
