/*
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
#include <cppunit/extensions/HelperMacros.h>
#include <dtCore/isector.h>
#include <dtCore/camera.h>
#include <dtCore/system.h>
#include <dtCore/particlesystem.h>
#include <dtCore/infiniteterrain.h>


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
         dtCore::System::Instance()->Start();
      }
      
      void tearDown()
      {
         mIsector = NULL;
         dtCore::System::Instance()->Stop();
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
         CPPUNIT_ASSERT(mIsector->GetLength() == (mIsector->GetDirection().length()));
         
      }

      void TestIntersection()
      {
         osg::Vec3 point1(0.0f, 0.0f, 1000.0f);
         osg::Vec3 point2(0.0f, 0.0f, -1000.0f);
         mIsector->SetStartPosition(point1);
         mIsector->SetEndPosition(point2);
         
         dtCore::RefPtr<dtCore::Scene> testScene = new dtCore::Scene();
         dtCore::RefPtr<dtCore::InfiniteTerrain> terrain = new dtCore::InfiniteTerrain();
         mIsector->SetScene(testScene.get());  
         
         testScene->AddDrawable(terrain.get());
      
         dtCore::RefPtr<dtCore::DeltaWin> win = new dtCore::DeltaWin("", 0, 0, 50, 50, true, false);
         dtCore::RefPtr<dtCore::Camera> camera = new dtCore::Camera("defaultCam");
         camera->SetWindow(win.get());
         camera->SetScene(testScene.get());

         dtCore::System::Instance()->Step();

         float height = terrain->GetHeight(0.0f, 0.0f, false);
         
         osg::Vec3 hitPoint;
         
         CPPUNIT_ASSERT(mIsector->Update());
         CPPUNIT_ASSERT(mIsector->GetNumberOfHits() > 0);
         mIsector->GetHitPoint(hitPoint, 0);
         CPPUNIT_ASSERT(osg::equivalent(hitPoint.z(), height, 1e-4f));         
         CPPUNIT_ASSERT(mIsector->GetClosestDeltaDrawable() == terrain.get());
         mIsector->Reset();
         CPPUNIT_ASSERT(mIsector->GetNumberOfHits() == 0);
         CPPUNIT_ASSERT(mIsector->GetClosestDeltaDrawable() == NULL);
         
         CPPUNIT_ASSERT(mIsector->Update());
         CPPUNIT_ASSERT(mIsector->GetNumberOfHits() > 0);
         mIsector->GetHitPoint(hitPoint, 0);
         CPPUNIT_ASSERT(osg::equivalent(hitPoint.z(), height, 1e-4f));         
         CPPUNIT_ASSERT(mIsector->GetClosestDeltaDrawable() == terrain.get());
                
         mIsector->SetGeometry(terrain.get());

         mIsector->Reset();
         CPPUNIT_ASSERT(mIsector->Update());
         CPPUNIT_ASSERT(mIsector->GetNumberOfHits() > 0);
         mIsector->GetHitPoint(hitPoint, 0);
         CPPUNIT_ASSERT(osg::equivalent(hitPoint.z(), height, 1e-4f));         
         CPPUNIT_ASSERT(mIsector->GetClosestDeltaDrawable() == terrain.get());

         mIsector->SetUseEyePoint(true);

         //0.0, 0.0, 0.0 should be within the LOD
         mIsector->SetEyePoint(osg::Vec3(3.0f, 11.5f, 9.4f));
         mIsector->Reset();
         CPPUNIT_ASSERT(mIsector->Update());
         CPPUNIT_ASSERT(mIsector->GetNumberOfHits() > 0);
         mIsector->GetHitPoint(hitPoint, 0);
         CPPUNIT_ASSERT(osg::equivalent(hitPoint.z(), height, 1e-4f));         
         CPPUNIT_ASSERT(mIsector->GetClosestDeltaDrawable() == terrain.get());

         //0.0, 0.0, 0.0 should NOT be within the LOD
         mIsector->SetEyePoint(osg::Vec3(50000.0f, 93838.5f, 9.4f));
         mIsector->Reset();
         CPPUNIT_ASSERT(!mIsector->Update());
         CPPUNIT_ASSERT(mIsector->GetNumberOfHits() == 0);
         CPPUNIT_ASSERT(mIsector->GetClosestDeltaDrawable() == NULL);
      }


   private:
      dtCore::RefPtr<dtCore::Isector> mIsector;
};

CPPUNIT_TEST_SUITE_REGISTRATION(IsectorTests);
