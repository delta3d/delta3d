/* -*-c++-*-
* allTests - This source file (.h & .cpp) - Using 'The MIT License'
* Copyright (C) 2005-2008, Alion Science and Technology Corporation
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

extern dtABC::Application& GetGlobalApplication();

class BatchISectorTests : public CPPUNIT_NS::TestFixture 
{
   CPPUNIT_TEST_SUITE(BatchISectorTests);

      CPPUNIT_TEST(TestIntersection);

   CPPUNIT_TEST_SUITE_END();

   public:

      void setUp()
      {
         mBatchIsector = new dtCore::BatchIsector();
         
         mApp = &GetGlobalApplication();
         mScene = mApp->GetScene();
         mCamera = mApp->GetCamera();
         mWin = mApp->GetWindow();
         
         dtCore::System::GetInstance().Config();

         dtCore::System::GetInstance().SetShutdownOnWindowClose(false);
         dtCore::System::GetInstance().Start();

         mBatchIsector->EnableAndGetISector(0);
      }
      
      void tearDown()
      {
         mBatchIsector = NULL;
         mScene = NULL;
         mCamera = NULL;
         mWin = NULL;
         mApp = NULL;
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
         terrain->SetBuildDistance(1500.0f);
         terrain->SetSegmentDivisions(64);
         mBatchIsector->SetScene(mScene.get());
         mScene->AddChild(terrain.get());

         dtCore::System::GetInstance().Step();

         float height = terrain->GetHeight(0.0f, 0.0f, false);
         osg::Vec3 expectedNormal;
         terrain->GetNormal(0.0f, 0.0f, expectedNormal, true);
         
               
         CPPUNIT_ASSERT(mBatchIsector->Update(osg::Vec3(0,0,0)));
         CPPUNIT_ASSERT(iSector.GetNumberOfHits() > 0);
         CheckIsectorValues(height, expectedNormal);
         CPPUNIT_ASSERT(terrain.get() == iSector.GetClosestDrawable());

         mBatchIsector->Reset();
         CPPUNIT_ASSERT_EQUAL(0U, iSector.GetNumberOfHits());
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


         CPPUNIT_ASSERT_THROW_MESSAGE("BatchIsector should have thrown exception on EnableAndGetISector(33)",
                                      mBatchIsector->EnableAndGetISector(33), dtCore::InvalidParameterException);

         CPPUNIT_ASSERT_THROW_MESSAGE("BatchIsector should have thrown exception on EnableAndGetISector(-1)",
                                       mBatchIsector->EnableAndGetISector(-1), dtCore::InvalidParameterException);
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
