/* -*-c++-*-
* allTests - This source file (.h & .cpp) - Using 'The MIT License'
* Copyright (C) 2007-2008, Alion Science and Technology Corporation
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
* David Guthrie
*/
#include <prefix/unittestprefix.h>

#include <dtCore/camera.h>
#include <dtCore/refptr.h>
#include <dtCore/transform.h>

#include <dtCore/datatype.h>
#include <dtCore/floatactorproperty.h>

#include <dtHLAGM/ddmcameracalculatorgeographic.h>
#include <dtHLAGM/ddmgeographicregiondata.h>
#include <dtHLAGM/ddmutil.h>

#include <dtUtil/coordinates.h>

#include <cppunit/extensions/HelperMacros.h>

#include <osg/Vec3>

#include <string>

class DDMCameraCalculatorGeographicTests : public CPPUNIT_NS::TestFixture 
{
   CPPUNIT_TEST_SUITE(DDMCameraCalculatorGeographicTests);
   
      CPPUNIT_TEST(TestProperties);
      CPPUNIT_TEST(TestOtherAppSpace);
      CPPUNIT_TEST(TestMoveCamera);

   CPPUNIT_TEST_SUITE_END();

   public:
      void setUp()
      {
         mCamCalcGeo = new dtHLAGM::DDMCameraCalculatorGeographic;
      }
      
      void tearDown()
      {
         mCamCalcGeo = NULL;
      }
      
      void TestProperties()
      {
         TestFloatProp(dtHLAGM::DDMCameraCalculatorGeographic::PROP_X_SUBSCRIPTION_RANGE, 100.0f);
         CPPUNIT_ASSERT_DOUBLES_EQUAL(173.99f, mCamCalcGeo->GetXSubscriptionRange(), 0.01f);
         TestFloatProp(dtHLAGM::DDMCameraCalculatorGeographic::PROP_Y_SUBSCRIPTION_RANGE, 100.0f);
         CPPUNIT_ASSERT_DOUBLES_EQUAL(173.99f, mCamCalcGeo->GetYSubscriptionRange(), 0.01f);
         TestFloatProp(dtHLAGM::DDMCameraCalculatorGeographic::PROP_MIN_TIME_BETWEEN_UPDATES, 0.25f);
         CPPUNIT_ASSERT_DOUBLES_EQUAL(173.99f, mCamCalcGeo->GetMinTimeBetweenUpdates(), 0.01f);
      }
               
      void TestOtherAppSpace()
      {
         dtCore::RefPtr<dtCore::Camera> cam = new dtCore::Camera("test");
         dtCore::Transform xform;
         cam->GetTransform(xform, dtCore::Transformable::REL_CS);
         xform.SetTranslation(osg::Vec3(-500.0f, 500.4f, 7.4f));
         cam->SetTransform(xform, dtCore::Transformable::REL_CS);
         mCamCalcGeo->SetCamera(cam.get());

         mCamCalcGeo->SetCalculatorObjectKind(dtHLAGM::DDMCalculatorGeographic::DDMObjectKind::OBJECT_KIND_OTHER);
         mCamCalcGeo->SetDefaultRegionType(dtHLAGM::DDMCalculatorGeographic::RegionCalculationType::APP_SPACE_ONLY);
         
         std::vector<dtCore::RefPtr<dtHLAGM::DDMRegionData> > data;
         mCamCalcGeo->CreateSubscriptionRegionData(data);

         CPPUNIT_ASSERT_EQUAL(1U, unsigned(data.size()));
      }
      
      void TestMoveCamera()
      {
         dtCore::RefPtr<dtCore::Camera> cam = new dtCore::Camera("test");
         dtCore::Transform xform;
         cam->GetTransform(xform, dtCore::Transformable::REL_CS);
         xform.SetTranslation(osg::Vec3(-500.0f, 500.4f, 7.4f));
         cam->SetTransform(xform, dtCore::Transformable::REL_CS);
         mCamCalcGeo->SetCamera(cam.get());
         
         unsigned int latMin;
         unsigned int latMax;

         unsigned int lonMin;
         unsigned int lonMax;

         std::set<dtHLAGM::DDMCalculatorGeographic::DDMForce* > foundForces;
         
         std::vector<dtCore::RefPtr<dtHLAGM::DDMRegionData> > data;
         mCamCalcGeo->CreateSubscriptionRegionData(data);

         mCamCalcGeo->SetNeutralRegionType(dtHLAGM::DDMCalculatorGeographic::RegionCalculationType::GEOGRAPHIC_SPACE);
         mCamCalcGeo->SetFriendlyRegionType(dtHLAGM::DDMCalculatorGeographic::RegionCalculationType::GEOGRAPHIC_SPACE);
         mCamCalcGeo->SetEnemyRegionType(dtHLAGM::DDMCalculatorGeographic::RegionCalculationType::APP_SPACE_ONLY);
         
         mCamCalcGeo->SetCalculatorObjectKind(dtHLAGM::DDMCalculatorGeographic::DDMObjectKind::OBJECT_KIND_ENTITY);

         CPPUNIT_ASSERT_EQUAL(3U, unsigned(data.size()));

         for (unsigned i = 0; i < data.size(); ++i)
         {
            dtHLAGM::DDMRegionData& regionData = *data[i];
            CPPUNIT_ASSERT(mCamCalcGeo->UpdateRegionData(regionData));
            if (i == 2)
               CPPUNIT_ASSERT_EQUAL(1U, regionData.GetNumberOfExtents());
            else
               CPPUNIT_ASSERT_EQUAL(3U, regionData.GetNumberOfExtents());
            
            dtHLAGM::DDMGeographicRegionData* grd = dynamic_cast<dtHLAGM::DDMGeographicRegionData*>(&regionData);
            CPPUNIT_ASSERT(grd != NULL);
            
            CPPUNIT_ASSERT_MESSAGE("No forces should be duplicated in the the list of regions created.", 
                  foundForces.find(&grd->GetForce()) == foundForces.end());
            foundForces.insert(&grd->GetForce());
            
            const dtHLAGM::DDMRegionData::DimensionValues* dv;
            
            dv = regionData.GetDimensionValue(0);
            CPPUNIT_ASSERT_EQUAL(mCamCalcGeo->GetFirstDimensionName(), dv->mName);
            
            /// The two app space value, min and max, should always be the same and should match the
            /// ones set for the kind on the calculator and force on the region data.
            std::pair<dtHLAGM::DDMCalculatorGeographic::RegionCalculationType*, unsigned> valuePair;
            valuePair = mCamCalcGeo->GetAppSpaceValues(grd->GetForce(), mCamCalcGeo->GetCalculatorObjectKind());
            CPPUNIT_ASSERT_EQUAL(dtHLAGM::DDMUtil::MapEnumerated(valuePair.second, 0, 30), dv->mMin);
            CPPUNIT_ASSERT_EQUAL(dtHLAGM::DDMUtil::MapEnumerated(valuePair.second, 0, 30), dv->mMax);
                    
            /// we have 3 extents on geographic space regions
            if (regionData.GetNumberOfExtents() > 1)
            {
               /// This better be setup for geographic
               CPPUNIT_ASSERT(*valuePair.first == dtHLAGM::DDMCalculatorGeographic::RegionCalculationType::GEOGRAPHIC_SPACE);               
               dv = regionData.GetDimensionValue(1);
               CPPUNIT_ASSERT_EQUAL(mCamCalcGeo->GetSecondDimensionName(), dv->mName);         
               
               if (i == 0)
               {
                  latMin = dv->mMin;
                  latMax = dv->mMax;
               }
               else
               {
                  CPPUNIT_ASSERT_EQUAL(latMin, dv->mMin);
                  CPPUNIT_ASSERT_EQUAL(latMax, dv->mMax);
               }
               
               
               dv = regionData.GetDimensionValue(2);
               CPPUNIT_ASSERT_EQUAL(mCamCalcGeo->GetThirdDimensionName(), dv->mName);
               if (i == 0)
               {
                  lonMin = dv->mMin;
                  lonMax = dv->mMax;
               }
               else
               {
                  CPPUNIT_ASSERT_EQUAL(lonMin, dv->mMin);
                  CPPUNIT_ASSERT_EQUAL(lonMax, dv->mMax);
               }
            } 
            else
            {
               /// This better be setup for app space only since we only have 1 extent.
               CPPUNIT_ASSERT(*valuePair.first == dtHLAGM::DDMCalculatorGeographic::RegionCalculationType::APP_SPACE_ONLY);
            }
         }
         
         //All forces should be found in the set.
         CPPUNIT_ASSERT(foundForces.find(&dtHLAGM::DDMCalculatorGeographic::DDMForce::FORCE_FRIENDLY) != foundForces.end());
         CPPUNIT_ASSERT(foundForces.find(&dtHLAGM::DDMCalculatorGeographic::DDMForce::FORCE_ENEMY) != foundForces.end());
         CPPUNIT_ASSERT(foundForces.find(&dtHLAGM::DDMCalculatorGeographic::DDMForce::FORCE_NEUTRAL) != foundForces.end());


         cam->GetTransform(xform, dtCore::Transformable::REL_CS);
         // The x and y values are in very different ranges to test that they don't get mixed up
         // in the calculator
         xform.SetTranslation(osg::Vec3(500.0f, -500.0f, 7.4f));
         cam->SetTransform(xform, dtCore::Transformable::REL_CS);

         /// switching to app space requires cleaning up the dimensions.
         mCamCalcGeo->SetNeutralRegionType(dtHLAGM::DDMCalculatorGeographic::RegionCalculationType::APP_SPACE_ONLY);
         ///Switch two around to make sure it works.
         mCamCalcGeo->SetEnemyRegionType(dtHLAGM::DDMCalculatorGeographic::RegionCalculationType::GEOGRAPHIC_SPACE);

         for (unsigned i = 0; i < data.size(); ++i)
         {
            dtHLAGM::DDMRegionData& regionData = *data[i];
            CPPUNIT_ASSERT(mCamCalcGeo->UpdateRegionData(regionData));

            dtHLAGM::DDMGeographicRegionData* grd = dynamic_cast<dtHLAGM::DDMGeographicRegionData*>(&regionData);
            CPPUNIT_ASSERT(grd != NULL);

            // only the last one should be app space only
            if (i == 1)
               CPPUNIT_ASSERT_EQUAL(1U, regionData.GetNumberOfExtents());
            else
               CPPUNIT_ASSERT_EQUAL(3U, regionData.GetNumberOfExtents());

            
            const dtHLAGM::DDMRegionData::DimensionValues* dv;
            
            dv = regionData.GetDimensionValue(0);
            CPPUNIT_ASSERT_EQUAL(mCamCalcGeo->GetFirstDimensionName(), dv->mName);
            /// The two app space value, min and max, should always be the same and should match the
            /// ones set for the kind on the calculator and force on the region data.
            std::pair<dtHLAGM::DDMCalculatorGeographic::RegionCalculationType*, unsigned> valuePair;
            valuePair = mCamCalcGeo->GetAppSpaceValues(grd->GetForce(), mCamCalcGeo->GetCalculatorObjectKind());
            CPPUNIT_ASSERT_EQUAL(dtHLAGM::DDMUtil::MapEnumerated(valuePair.second, 0, 30), dv->mMin);
            CPPUNIT_ASSERT_EQUAL(dtHLAGM::DDMUtil::MapEnumerated(valuePair.second, 0, 30), dv->mMax);

            /// we have 3 extents on geographic space regions
            if (regionData.GetNumberOfExtents() > 1)
            {
               /// This better be setup for geographic
               CPPUNIT_ASSERT(*valuePair.first == dtHLAGM::DDMCalculatorGeographic::RegionCalculationType::GEOGRAPHIC_SPACE);               

               dv = regionData.GetDimensionValue(1);
               CPPUNIT_ASSERT(dv->mMin < latMin);
               CPPUNIT_ASSERT(dv->mMax < latMax);
               
               dv = regionData.GetDimensionValue(2);
               CPPUNIT_ASSERT(dv->mMin > lonMin);
               CPPUNIT_ASSERT(dv->mMax > lonMax);         
            }
            else
            {               
               /// This better be setup for app space only since we only have 1 extent.
               CPPUNIT_ASSERT(*valuePair.first == dtHLAGM::DDMCalculatorGeographic::RegionCalculationType::APP_SPACE_ONLY);
            }
         }
      }
            
   private:

      void TestFloatProp(const std::string& propName, float expectedDefault)
      {
         dtCore::FloatActorProperty* fap = NULL;
         mCamCalcGeo->GetProperty(propName, fap);
         CPPUNIT_ASSERT(fap != NULL);
         
         CPPUNIT_ASSERT_EQUAL(expectedDefault, fap->GetValue());
         fap->SetValue(173.99f);
         CPPUNIT_ASSERT_DOUBLES_EQUAL(173.99f, fap->GetValue(), 0.01f);
      }

      dtCore::RefPtr<dtHLAGM::DDMCameraCalculatorGeographic> mCamCalcGeo; 
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(DDMCameraCalculatorGeographicTests);
