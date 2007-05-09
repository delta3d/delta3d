/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2007, Alion Science and Technology, BMH Operation.
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
 * David Guthrie
 */
#include <prefix/dtgameprefix-src.h>
#include <cppunit/extensions/HelperMacros.h>

#include <string>
#include <dtCore/refptr.h>
#include <dtHLAGM/ddmcamerageographiccalculator.h>
#include <dtHLAGM/ddmgeographicregiondata.h>
#include <dtHLAGM/ddmutil.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtCore/camera.h>
#include <dtCore/transform.h>
#include <dtUtil/coordinates.h>

#include <osg/Vec3>

class DDMCameraCalculatorGeographicTests : public CPPUNIT_NS::TestFixture 
{
   CPPUNIT_TEST_SUITE(DDMCameraCalculatorGeographicTests);
   
      CPPUNIT_TEST(TestMoveCamera);

   CPPUNIT_TEST_SUITE_END();

   public:
      void setUp()
      {
         mCamCalcGeo = new dtHLAGM::DDMCameraGeographicCalculator;
      }
      
      void tearDown()
      {
         mCamCalcGeo = NULL;
      }
      
      void TestMoveCamera()
      {
         dtCore::RefPtr<dtCore::Camera> cam = new dtCore::Camera("test");
         dtCore::Transform xform;
         cam->GetTransform(xform, dtCore::Transformable::REL_CS);
         xform.SetTranslation(osg::Vec3(-500.0f, 500.4f, 7.4f));
         cam->SetTransform(xform, dtCore::Transformable::REL_CS);
         mCamCalcGeo->SetCamera(cam.get());
         
         dtCore::RefPtr<dtHLAGM::DDMRegionData> data = mCamCalcGeo->CreateRegionData();
         CPPUNIT_ASSERT(mCamCalcGeo->UpdateRegionData(*data));
         
         CPPUNIT_ASSERT_EQUAL(3U, data->GetNumberOfExtents());

         const dtHLAGM::DDMRegionData::DimensionValues* dv = data->GetDimensionValue(0);
         CPPUNIT_ASSERT_EQUAL(mCamCalcGeo->GetFirstDimensionName(), dv->mName);
         CPPUNIT_ASSERT_EQUAL(dtHLAGM::DDMUtil::MapEnumerated(0, 0, 99), dv->mMin);
         CPPUNIT_ASSERT_EQUAL(dtHLAGM::DDMUtil::MapEnumerated(99, 0, 99), dv->mMax);
                 
         dv = data->GetDimensionValue(1);
         CPPUNIT_ASSERT_EQUAL(mCamCalcGeo->GetSecondDimensionName(), dv->mName);         
         unsigned long latMin = dv->mMin;
         unsigned long latMax = dv->mMax;
         
         dv = data->GetDimensionValue(2);
         CPPUNIT_ASSERT_EQUAL(mCamCalcGeo->GetThirdDimensionName(), dv->mName);
         unsigned long lonMin = dv->mMin;
         unsigned long lonMax = dv->mMax;

         cam->GetTransform(xform, dtCore::Transformable::REL_CS);
         // The x and y values are in very different ranges to test that they don't get mixed up
         // in the calculator
         xform.SetTranslation(osg::Vec3(500.0f, -500.0f, 7.4f));
         cam->SetTransform(xform, dtCore::Transformable::REL_CS);

         CPPUNIT_ASSERT(mCamCalcGeo->UpdateRegionData(*data));
         
         CPPUNIT_ASSERT_EQUAL(3U, data->GetNumberOfExtents());

         dv = data->GetDimensionValue(0);
         CPPUNIT_ASSERT_EQUAL(dtHLAGM::DDMUtil::MapEnumerated(0, 0, 99), dv->mMin);
         CPPUNIT_ASSERT_EQUAL(dtHLAGM::DDMUtil::MapEnumerated(99, 0, 99), dv->mMax);
                 
         dv = data->GetDimensionValue(1);
         CPPUNIT_ASSERT(dv->mMin < latMin);
         CPPUNIT_ASSERT(dv->mMax < latMax);
         
         dv = data->GetDimensionValue(2);
         CPPUNIT_ASSERT(dv->mMin > lonMin);
         CPPUNIT_ASSERT(dv->mMax > lonMax);         
      }
            
   private:
      
      dtCore::RefPtr<dtHLAGM::DDMCameraGeographicCalculator> mCamCalcGeo; 
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(DDMCameraCalculatorGeographicTests);
