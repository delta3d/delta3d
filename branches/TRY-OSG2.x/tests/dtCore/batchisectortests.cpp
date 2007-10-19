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
#include <prefix/dtgameprefix-src.h>
#include <cppunit/extensions/HelperMacros.h>

#include <dtCore/camera.h>
#include <dtCore/view.h>
#include <dtCore/deltawin.h>
#include <dtCore/infiniteterrain.h>
#include <dtCore/batchisector.h>
#include <dtCore/scene.h>
#include <dtCore/system.h>
#include <dtCore/exceptionenum.h>
#include <dtABC/application.h>

#include <dtUtil/exception.h>

#include <osg/io_utils>
//#include <dtABC/application.h>

class BatchISectorTests : public CPPUNIT_NS::TestFixture 
{
   CPPUNIT_TEST_SUITE(BatchISectorTests);

      CPPUNIT_TEST(TestIntersection);

   CPPUNIT_TEST_SUITE_END();

   public:

      void setUp()
      {
         mBatchIsector = new dtCore::BatchIsector();
         
         mApp = new dtABC::Application;
         mScene = mApp->GetScene();
         mCamera = mApp->GetCamera();
         mWin = mApp->GetWindow();
         mWin->SetPosition(0, 0, 50, 50);
         
         mApp->Config();
         
         dtCore::System::GetInstance().Config();

         dtCore::System::GetInstance().SetShutdownOnWindowClose(false);
         dtCore::System::GetInstance().Start();

         mBatchIsector->EnableAndGetISector(0);
      }
      
      void tearDown()
      {
         mBatchIsector = NULL;
         mScene = NULL;
         mCamera->SetWindow(NULL);
         mCamera = NULL;
         mWin = NULL;
         dtCore::System::GetInstance().Stop();
      }

     
      void TestIntersection()
      {
         dtCore::BatchIsector::SingleISector& iSector = mBatchIsector->EnableAndGetISector(0);

         osg::Vec3 point1(0.0f, 0.0f, 1000.0f);
         osg::Vec3 point2(0.0f, 0.0f, -1000.0f);
         iSector.SetSectorAsLineSegment(point1, point2);
         iSector.SetToCheckForClosestDrawable(true);

         dtCore::RefPtr<dtCore::InfiniteTerrain> terrain = new dtCore::InfiniteTerrain();
         terrain->SetBuildDistance(1500.f);
         terrain->SetSegmentDivisions(64);
         mBatchIsector->SetScene(mScene.get());
         mScene->AddDrawable(terrain.get());

         dtCore::System::GetInstance().Step();

         float height = terrain->GetHeight(0.0f, 0.0f, false);
         osg::Vec3 expectedNormal;
         terrain->GetNormal(0.0f, 0.0f, expectedNormal, true);
         
               
         CPPUNIT_ASSERT(mBatchIsector->Update(osg::Vec3(0,0,0)));
         CPPUNIT_ASSERT(iSector.GetNumberOfHits() > 0);
         CheckIsectorValues(height, expectedNormal);
         CPPUNIT_ASSERT(terrain.get() == iSector.GetClosestDrawable());

         mBatchIsector->Reset();
         CPPUNIT_ASSERT_EQUAL(0, iSector.GetNumberOfHits());
         CPPUNIT_ASSERT(NULL == iSector.GetClosestDrawable());
      
            
         CPPUNIT_ASSERT(!mBatchIsector->Update(osg::Vec3(0,0,0)));
                
         mBatchIsector->SetQueryRoot(terrain.get());
         
         mBatchIsector->Reset();
         iSector.ToggleIsOn(true);
         CPPUNIT_ASSERT(mBatchIsector->Update(osg::Vec3(0,0,0), true));
         CPPUNIT_ASSERT(iSector.GetNumberOfHits() > 0);
         CheckIsectorValues(height, expectedNormal);
         CPPUNIT_ASSERT(iSector.GetClosestDrawable() == terrain.get());


         //0.0, 0.0, 0.0 should be within the LOD
         mBatchIsector->Reset();
         iSector.ToggleIsOn(true);
         CPPUNIT_ASSERT(mBatchIsector->Update(osg::Vec3(3.0f, 11.5f, 9.4f), true));
         CPPUNIT_ASSERT(iSector.GetNumberOfHits() > 0);
         CheckIsectorValues(height, expectedNormal);
         CPPUNIT_ASSERT(iSector.GetClosestDrawable() == terrain.get());


         //0.0, 0.0, 0.0 should NOT be within the LOD
//         mBatchIsector->Reset();
//         iSector.ToggleIsOn(true);
//         printf("iSector.GetNumberOfHits() %d\n", iSector.GetNumberOfHits());
//         CPPUNIT_ASSERT(!mBatchIsector->Update(osg::Vec3(500000.0f, 938380.5f, 9.4f), false));
//         CPPUNIT_ASSERT(iSector.GetNumberOfHits() == 0);
//         CPPUNIT_ASSERT(iSector.GetClosestDrawable() == NULL);

         try 
         {
            mBatchIsector->EnableAndGetISector(33);
         }
         catch (const dtUtil::Exception& ex)
         {
            //  correct
            CPPUNIT_ASSERT_MESSAGE(ex.ToString().c_str(), ex.TypeEnum() == dtCore::ExceptionEnum::INVALID_PARAMETER);
         }

         try 
         {
            mBatchIsector->EnableAndGetISector(-1);
         }
         catch (const dtUtil::Exception& ex)
         {
            //  correct
            CPPUNIT_ASSERT_MESSAGE(ex.ToString().c_str(), ex.TypeEnum() == dtCore::ExceptionEnum::INVALID_PARAMETER);
         }
      }


   private:
      dtCore::RefPtr<dtCore::BatchIsector>   mBatchIsector;
      dtCore::RefPtr<dtCore::Scene>          mScene;
      dtCore::RefPtr<dtCore::Camera>         mCamera;
      dtCore::RefPtr<dtCore::DeltaWin>       mWin;
      dtCore::RefPtr<dtABC::Application>     mApp;
      
      void CheckIsectorValues(const float height, const osg::Vec3& expectedNormal) const
      {
         osg::Vec3 hitPoint;
         osg::Vec3 normal;

         dtCore::BatchIsector::SingleISector& iSector = mBatchIsector->EnableAndGetISector(0);

         iSector.GetHitPoint(hitPoint, 0);
         iSector.GetHitPointNormal(normal, 0);
   
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

CPPUNIT_TEST_SUITE_REGISTRATION(BatchISectorTests);
