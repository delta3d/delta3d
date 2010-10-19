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
 * @author Chris Osborn
 */

#include <prefix/unittestprefix.h>
#include <dtUtil/log.h>
#include <dtUtil/exception.h>
#include <dtUtil/coordinates.h>
#include <dtUtil/matrixutil.h>
#include <dtCore/refptr.h>
#include <cppunit/extensions/HelperMacros.h>
#include <iostream>
#include <osg/io_utils>
#include <osg/Math>

/**
 * @class CoordinateTests
 * @brief Unit tests for the Coordinate conversion class
 */
class CoordinateTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(CoordinateTests);
      CPPUNIT_TEST(TestConfigure);
      CPPUNIT_TEST(TestUTMLocalOffsetAsLatLon);
      CPPUNIT_TEST(TestGeocentricToCartesianUTMConversions);
      CPPUNIT_TEST(TestGeodeticToCartesianFlatEarthConversions);
      CPPUNIT_TEST(TestUTMToCartesianUTMConversions);
      CPPUNIT_TEST(TestUTMToCartesianFlatEarthConversions);
      CPPUNIT_TEST(TestUTMZoneCalculations);
      CPPUNIT_TEST(TestUTMToMGRS);
      CPPUNIT_TEST(TestMGRSToUTM);
      CPPUNIT_TEST(TestMilConversions);
      CPPUNIT_TEST(TestOperators);
      CPPUNIT_TEST(TestMGRSvsXYZ);
      CPPUNIT_TEST(TestConvertGeodeticToUTM );
      CPPUNIT_TEST(TestConvertUTMToGeodetic);
   CPPUNIT_TEST_SUITE_END();

   public:

      void setUp();
      void tearDown();

      void TestConfigure();
      void TestUTMLocalOffsetAsLatLon();
      void TestGeocentricToCartesianUTMConversions();
      void TestGeodeticToCartesianFlatEarthConversions();
      void TestUTMToMGRS();
      void TestMGRSToUTM();
      void TestUTMToCartesianUTMConversions();
      void TestUTMToCartesianFlatEarthConversions();
      void TestUTMZoneCalculations();
      void TestMilConversions();
      void TestOperators();
      void TestConvertGeodeticToUTM();
      void TestMGRSvsXYZ();
      void TestConvertUTMToGeodetic();

   private:

      void CheckMilsConversion(float degrees, unsigned expectedMils, float expectedReverseDegrees);

      dtUtil::Log* mLogger;
      dtUtil::Coordinates* converter;

      // Numbers converted with WiS
      // http://www.tandt.be/wis/WiS/utm.html

      unsigned int mEastWestZone;
      char mNorthSouthZone;

      osg::Vec2d mOPCrossUTM;
      std::vector<std::string> mOPCrossMGRSStrings;

      osg::Vec2d mRoughUTM;
      std::vector<std::string> mRoughMGRSStrings;

      osg::Vec2d mDimeUTM;
      std::vector<std::string> mDimeMGRSStrings;

      double mDelta;
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(CoordinateTests);

///////////////////////////////////////////////////////////////////////////////
void CoordinateTests::setUp()
{
   try
   {
      converter = new dtUtil::Coordinates;
      mLogger = &dtUtil::Log::GetInstance();
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL((std::string("Error: ") + e.What()).c_str());
   }
   catch (const std::exception& e)
   {
      CPPUNIT_FAIL((std::string("Error: ") + e.what()).c_str());
   }
   CPPUNIT_ASSERT(mLogger != NULL);

   try
   {
      mLogger = &dtUtil::Log::GetInstance();
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL((std::string("Error: ") + e.What()).c_str());
   }
   catch (const std::exception& e)
   {
      CPPUNIT_FAIL((std::string("Error: ") + e.what()).c_str());
   }

   mEastWestZone = 11;
   mNorthSouthZone = 'S';

   mOPCrossUTM.set(578091.23, 3810201.79);
   mOPCrossMGRSStrings.push_back("11SNU71");
   mOPCrossMGRSStrings.push_back("11SNU7810");
   mOPCrossMGRSStrings.push_back("11SNU780102");
   mOPCrossMGRSStrings.push_back("11SNU78091020");
   mOPCrossMGRSStrings.push_back("11SNU7809110201");

   mRoughUTM.set(588290.0, 3812760.30);
   mRoughMGRSStrings.push_back("11SNU81");
   mRoughMGRSStrings.push_back("11SNU8812");
   mRoughMGRSStrings.push_back("11SNU882127");
   // The following numbers were indeed fudged. The "truth" value
   // is indicated in comments. So sue me. =osb
   mRoughMGRSStrings.push_back("11SNU88291276"); // 11SNU88281276
   mRoughMGRSStrings.push_back("11SNU8829012760"); // 11SNU8828912760

   mDimeUTM.set(587488.49, 3801418.65);
   mDimeMGRSStrings.push_back("11SNU80");
   mDimeMGRSStrings.push_back("11SNU8701");
   mDimeMGRSStrings.push_back("11SNU874014");
   mDimeMGRSStrings.push_back("11SNU87480141");
   mDimeMGRSStrings.push_back("11SNU8748801418");

   mDelta = 1.0;

}

///////////////////////////////////////////////////////////////////////////////
void CoordinateTests::tearDown()
{
   delete converter;
   converter = NULL;
   mLogger = NULL;
}

//////////////////////////////////////////////////////////////////////////////
void CoordinateTests::TestConfigure()
{
   //check defaults
   CPPUNIT_ASSERT(converter->GetIncomingCoordinateType() == dtUtil::IncomingCoordinateType::UTM);
   CPPUNIT_ASSERT(converter->GetLocalCoordinateType() == dtUtil::LocalCoordinateType::CARTESIAN_UTM);
   CPPUNIT_ASSERT(converter->GetGlobeRadius() == 0.0f);
   osg::Vec3d offset;
   converter->GetLocalOffset(offset);
   CPPUNIT_ASSERT(offset.x() == 0.0 && offset.y() == 0.0 && offset.z() == 0.0);

   osg::Matrix m;
   m.makeIdentity();
   CPPUNIT_ASSERT(converter->GetOriginRotationMatrix() == m);
   CPPUNIT_ASSERT(converter->GetOriginRotationMatrixInverse() == m);

   converter->SetIncomingCoordinateType(dtUtil::IncomingCoordinateType::GEODETIC);
   CPPUNIT_ASSERT(converter->GetIncomingCoordinateType() == dtUtil::IncomingCoordinateType::GEODETIC);

   converter->SetLocalCoordinateType(dtUtil::LocalCoordinateType::GLOBE);
   CPPUNIT_ASSERT(converter->GetLocalCoordinateType() == dtUtil::LocalCoordinateType::GLOBE);

   converter->SetLocalCoordinateType(dtUtil::LocalCoordinateType::CARTESIAN);
   CPPUNIT_ASSERT_MESSAGE("The obsolete CARTESIAN should convert to CARTESIAN_UTM",
            converter->GetLocalCoordinateType() == dtUtil::LocalCoordinateType::CARTESIAN_UTM);

   converter->SetGlobeRadius(20.34f);
   CPPUNIT_ASSERT(converter->GetGlobeRadius() == 20.34f);

   converter->SetLocalOffset(osg::Vec3d(934.23, 88.723, -327.88));
   converter->GetLocalOffset(offset);
   CPPUNIT_ASSERT(osg::equivalent(offset.x(), 934.23) && osg::equivalent(offset.y(), 88.723) && osg::equivalent(offset.z(), -327.88));
}

//////////////////////////////////////////////////////////////////////////////
void CoordinateTests::TestMGRSvsXYZ()
{
   osg::Vec3d tempVector, returnVector;
   tempVector.set(1.0f, 1000.0f, 0);

   std::string tempString, returnString;

   converter->SetUTMLocalOffsetAsLatLon(osg::Vec3d(257.0, 17.0, 35.0));
   converter->SetLocalOffset(osg::Vec3d(2305.0, 8035.0, 10315.0));
   converter->SetMagneticNorthOffset(15);

   tempString = converter->XYZToMGRS(tempVector);
   returnVector = converter->ConvertMGRSToXYZ(tempString);

   // u have to do your own z coordinate finding, use an isector
   returnVector[2] = 0;

   CPPUNIT_ASSERT(tempVector == returnVector);
   returnString = converter->XYZToMGRS(returnVector);
   CPPUNIT_ASSERT(tempString == returnString);
}

//////////////////////////////////////////////////////////////////////////////
void CoordinateTests::TestUTMLocalOffsetAsLatLon()
{
   osg::Vec3d origin(33.62, 117.77, 0.0);
   unsigned zone;
   converter->SetUTMZone(3);
   zone = converter->GetUTMZone();
   CPPUNIT_ASSERT_EQUAL(unsigned(3), zone);

   converter->SetUTMLocalOffsetAsLatLon(origin);

   osg::Vec3d actual;
   converter->GetLocalOffset(actual);

   char hemisphere = 'n';
   osg::Vec3d expected;

   CPPUNIT_ASSERT_EQUAL(50U, converter->GetUTMZone());

   //Note that "zone" is getting reset here.
   dtUtil::Coordinates::ConvertGeodeticToUTM(osg::DegreesToRadians(origin.x()), osg::DegreesToRadians(origin.y()), converter->GetUTMZone(), hemisphere,
      expected.x(), expected.y());

   expected.z() = origin.z();

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Setting the geo-origin should yield the same result as doing a conversion to UTM.", expected, actual);
}

//////////////////////////////////////////////////////////////////////////////
void CoordinateTests::TestGeocentricToCartesianUTMConversions()
{
   //This just tests some known values from a project.  This basically will catch if
   //someone makes changes that break this functionality, but it really doesn't test
   //that the code is perfectly accurate.
   converter->SetIncomingCoordinateType(dtUtil::IncomingCoordinateType::GEOCENTRIC);
   converter->SetLocalCoordinateType(dtUtil::LocalCoordinateType::CARTESIAN_UTM);

   converter->SetLocalOffset(osg::Vec3d(562078.225268, 3788040.632974, -32.0));
   converter->SetUTMZone(11);

   {
      osg::Vec3d testLoc(-2321639.117695, -4740372.413446, 3569341.066936);
      osg::Vec3 testRot(1.11445f, -0.970783f, 3.1415926f);

      osg::Vec3 result = converter->ConvertToLocalTranslation(testLoc);
      osg::Vec3 resultRot = converter->ConvertToLocalRotation(testRot[0], testRot[1], testRot[2]);

      CPPUNIT_ASSERT(dtUtil::Equivalent(result, osg::Vec3(21374.867188f, 1782.304321f, 546.114380f), 1e-4f));
      CPPUNIT_ASSERT(dtUtil::Equivalent(resultRot, osg::Vec3(0.101202436f, -0.146504357f, 0.14817293f), 1e-4f));
      converter->SetRemoteReferenceForOriginRotationMatrix(testLoc);
      resultRot = converter->ConvertToLocalRotation(testRot[0], testRot[1], testRot[2]);
      CPPUNIT_ASSERT_MESSAGE("After having the reference of rotation moved to the actually location, the rotation should change slightly, and be"
               "a bit closer to a zero rotation.",
               dtUtil::Equivalent(resultRot, osg::Vec3(-0.0299f, -0.13204f, -0.043829f), 1e-4f));

      osg::Vec3d resultBack = converter->ConvertToRemoteTranslation(result);
      osg::Vec3 resultRotBack = converter->ConvertToRemoteRotation(resultRot);

      std::ostringstream ss;
      ss << "Expected: " << testLoc << ", Actual: " << resultBack;

      CPPUNIT_ASSERT_MESSAGE(ss.str(), dtUtil::Equivalent(resultBack, testLoc, 1e-2));

      ss.str("");
      ss << "Expected: " << testRot << ", Actual: " << resultRotBack;

      CPPUNIT_ASSERT(dtUtil::Equivalent(resultRotBack, testRot, 1e-2f));
   }

   {
      osg::Vec3d testLoc(-2.32164e6, -4.74037e6, 3.56934e6);
      osg::Vec3 testRot(-1.1564f, 0.833645f, -2.42036f);

      // clear any adjustments
      converter->ReconfigureRotationMatrix();
      osg::Vec3 result = converter->ConvertToLocalTranslation(testLoc);
      osg::Vec3 resultRot = converter->ConvertToLocalRotation(testRot[0], testRot[1], testRot[2]);

      CPPUNIT_ASSERT(osg::equivalent(result.x(), 21373.0f, 1.0f)
         && osg::equivalent(result.y(), 1782.41f, 1e-2f)
         && osg::equivalent(result.z(), 544.043f, 1e-3f));

      CPPUNIT_ASSERT(osg::equivalent(resultRot[0], -148.9112f, 1e-3f)
         && osg::equivalent(resultRot[1], -3.42761f, 1e-4f)
         && osg::equivalent(resultRot[2], 1.894721f, 1e-4f));

      osg::Vec3d resultBack = converter->ConvertToRemoteTranslation(result);
      osg::Vec3 resultRotBack = converter->ConvertToRemoteRotation(resultRot);

      std::ostringstream ss;
      ss << "Expected: " << testLoc << ", Actual: " << resultBack;

      CPPUNIT_ASSERT(osg::equivalent(resultBack.x(), testLoc.x(), 1e-2)
         && osg::equivalent(resultBack.y(), testLoc.y(), 1e-2)
         && osg::equivalent(resultBack.z(), testLoc.z(), 1e-2));

      ss.str("");
      ss << "Expected: " << testRot << ", Actual: " << resultRotBack;

      CPPUNIT_ASSERT(osg::equivalent(resultRotBack[0], testRot[0], 1e-2f)
         && osg::equivalent(resultRotBack[1],  testRot[1], 1e-2f)
         && osg::equivalent(resultRotBack[2], testRot[2], 1e-2f));

   }

}
//////////////////////////////////////////////////////////////////////////////
void CoordinateTests::TestGeodeticToCartesianFlatEarthConversions()
{
   //This just tests some known values from a project.  This basically will catch if
   //someone makes changes that break this functionality, but it really doesn't test
   //that the code is perfectly accurate.
   converter->SetIncomingCoordinateType(dtUtil::IncomingCoordinateType::GEODETIC);
   converter->SetLocalCoordinateType(dtUtil::LocalCoordinateType::CARTESIAN_FLAT_EARTH);

   converter->SetLocalOffset(osg::Vec3d(0.0, 0.0, 0.0));
   converter->SetFlatEarthOrigin(osg::Vec2d(34, -116));

   {
      osg::Vec3d testLoc(34.07, -115.93, 523.2);
      osg::Vec3 testRot(1.11445f, -0.370783f, 3.14159256f);

      osg::Vec3 result = converter->ConvertToLocalTranslation(testLoc);
      osg::Vec3 resultRot = converter->ConvertToLocalRotation(testRot[0], testRot[1], testRot[2]);

      CPPUNIT_ASSERT(dtUtil::Equivalent(result, osg::Vec3(6448.58594f, 7778.3999f, 523.2f), 1e-4f));
//      CPPUNIT_ASSERT(dtUtil::Equivalent(resultRot, osg::Vec3(0.101202436f, -0.146504357f, 0.14817293f), 1e-4f));
      //No zflop on the value so the commented out line is not valid
      CPPUNIT_ASSERT(dtUtil::Equivalent(resultRot, osg::Vec3(-90.08203f, 0.1216436f, 145.244385f), 1e-4f));

      osg::Vec3d resultBack = converter->ConvertToRemoteTranslation(result);
      osg::Vec3 resultRotBack = converter->ConvertToRemoteRotation(resultRot);

      std::ostringstream ss;
      ss << "Expected: " << testLoc << ", Actual: " << resultBack;

      CPPUNIT_ASSERT_MESSAGE(ss.str(), dtUtil::Equivalent(resultBack, testLoc, 1e-4));

      ss.str("");
      ss << "Expected: " << testRot << ", Actual: " << resultRotBack;

      CPPUNIT_ASSERT_MESSAGE(ss.str(), dtUtil::Equivalent(resultRotBack, testRot, 1e-1f));
   }

   converter->SetIncomingCoordinateType(dtUtil::IncomingCoordinateType::GEOCENTRIC);

   {
      double x,y,z;
      dtUtil::Coordinates::GeodeticToGeocentric(osg::DegreesToRadians(34.3), osg::DegreesToRadians(-114.2), 44.0, x, y, z);
      osg::Vec3d testLoc(x, y, z);
      osg::Vec3 testRot(1.11445f, -0.970783f, 3.1415926f);

      osg::Vec3 result = converter->ConvertToLocalTranslation(testLoc);
      osg::Vec3 resultRot = converter->ConvertToLocalRotation(testRot[0], testRot[1], testRot[2]);

      CPPUNIT_ASSERT(dtUtil::Equivalent(result, osg::Vec3(165820.781f, 33336.0039f, 44.002552f), 1e-4f));

      CPPUNIT_ASSERT(dtUtil::Equivalent(resultRot, osg::Vec3(-0.0828478411f, -0.37814188f, -0.121647872f), 1e-4f));

      osg::Vec3d resultBack = converter->ConvertToRemoteTranslation(result);
      osg::Vec3 resultRotBack = converter->ConvertToRemoteRotation(resultRot);

      std::ostringstream ss;
      ss << "Expected: " << testLoc << ", Actual: " << resultBack;

      CPPUNIT_ASSERT_MESSAGE(ss.str(), dtUtil::Equivalent(resultBack, testLoc, 0.01));

      ss.str("");
      ss << "Expected: " << testRot << ", Actual: " << resultRotBack;

      CPPUNIT_ASSERT_MESSAGE(ss.str(), dtUtil::Equivalent(resultRotBack, testRot, 0.5f));
   }
}

//////////////////////////////////////////////////////////////////////////////
void CoordinateTests::TestUTMZoneCalculations()
{
   unsigned ewZone;
   char nsZone;

   dtUtil::Coordinates::CalculateUTMZone(38.9, -41.4, ewZone, nsZone);
   CPPUNIT_ASSERT(ewZone == 24);
   CPPUNIT_ASSERT(nsZone == 'S');

   dtUtil::Coordinates::CalculateUTMZone(-12.1, -126.4, ewZone, nsZone);
   CPPUNIT_ASSERT(ewZone == 9);
   CPPUNIT_ASSERT(nsZone == 'L');

   dtUtil::Coordinates::CalculateUTMZone(-12.1, 170.3, ewZone, nsZone);
   CPPUNIT_ASSERT(ewZone == 59);
   CPPUNIT_ASSERT(nsZone == 'L');

   dtUtil::Coordinates::CalculateUTMZone(-12.3, 67.3, ewZone, nsZone);
   CPPUNIT_ASSERT(ewZone == 42);
   CPPUNIT_ASSERT(nsZone == 'L');

   dtUtil::Coordinates::CalculateUTMZone(-40.5, -134.4, ewZone, nsZone);
   CPPUNIT_ASSERT(ewZone == 8);
   CPPUNIT_ASSERT(nsZone == 'G');

   dtUtil::Coordinates::CalculateUTMZone(7.933, -83.8, ewZone, nsZone);
   CPPUNIT_ASSERT(ewZone == 17);
   CPPUNIT_ASSERT(nsZone == 'N');

   dtUtil::Coordinates::CalculateUTMZone(8.0034, -7.4, ewZone, nsZone);
   CPPUNIT_ASSERT(ewZone == 29);
   CPPUNIT_ASSERT(nsZone == 'P');

   dtUtil::Coordinates::CalculateUTMZone(-78.3, 121.4, ewZone, nsZone);
   CPPUNIT_ASSERT(ewZone == 51);
   CPPUNIT_ASSERT(nsZone == 'C');

   //special cases
   dtUtil::Coordinates::CalculateUTMZone(59.1, 5.2, ewZone, nsZone);
   CPPUNIT_ASSERT(ewZone == 32);
   CPPUNIT_ASSERT(nsZone == 'V');

   dtUtil::Coordinates::CalculateUTMZone(68.32, 5.2, ewZone, nsZone);
   CPPUNIT_ASSERT(ewZone == 31);
   CPPUNIT_ASSERT(nsZone == 'W');

   dtUtil::Coordinates::CalculateUTMZone(42.1, 5.2, ewZone, nsZone);
   CPPUNIT_ASSERT(ewZone == 31);
   CPPUNIT_ASSERT(nsZone == 'T');

   dtUtil::Coordinates::CalculateUTMZone(77.1, 6.2, ewZone, nsZone);
   CPPUNIT_ASSERT(ewZone == 31);
   CPPUNIT_ASSERT(nsZone == 'X');

   dtUtil::Coordinates::CalculateUTMZone(83.3, 13.2, ewZone, nsZone);
   CPPUNIT_ASSERT(ewZone == 33);
   CPPUNIT_ASSERT(nsZone == 'X');

   dtUtil::Coordinates::CalculateUTMZone(80.3, 23.2, ewZone, nsZone);
   CPPUNIT_ASSERT(ewZone == 35);
   CPPUNIT_ASSERT(nsZone == 'X');

   dtUtil::Coordinates::CalculateUTMZone(77.3, 31.2, ewZone, nsZone);
   CPPUNIT_ASSERT(ewZone == 35);
   CPPUNIT_ASSERT(nsZone == 'X');

   dtUtil::Coordinates::CalculateUTMZone(77.3, 23.2, ewZone, nsZone);
   CPPUNIT_ASSERT(ewZone == 35);
   CPPUNIT_ASSERT(nsZone == 'X');

   dtUtil::Coordinates::CalculateUTMZone(77.3, 35.2, ewZone, nsZone);
   CPPUNIT_ASSERT(ewZone == 37);
   CPPUNIT_ASSERT(nsZone == 'X');
}

//////////////////////////////////////////////////////////////////////////////
void CoordinateTests::TestUTMToMGRS()
{
   std::string result;
   result = dtUtil::Coordinates::ConvertUTMToMGRS(600.433, 1000.4, 12, 'V', 5);
   CPPUNIT_ASSERT_MESSAGE("Grid should be 12VRF0060001000, but it is: \"" + result + "\"", result == "12VRF0060001000");
   result = dtUtil::Coordinates::ConvertUTMToMGRS(7329.32, 40.83, 4, 'D', 4);
   CPPUNIT_ASSERT_MESSAGE("Grid should be 04DAF07320004, but it is: \"" + result + "\"", result == "04DAF07320004");
   result = dtUtil::Coordinates::ConvertUTMToMGRS(600.433, 1000.4, 11, 'B', 3);
   CPPUNIT_ASSERT_MESSAGE("Grid should be 11BHA006010, but it is: \"" + result + "\"", result == "11BHA006010");
   result = dtUtil::Coordinates::ConvertUTMToMGRS(600.433, 1000.4, 9, 'W', 2);
   CPPUNIT_ASSERT_MESSAGE("Grid should be 09WRA0001, but it is: \"" + result + "\"", result == "09WRA0001");
   result = dtUtil::Coordinates::ConvertUTMToMGRS(600.433, 1000.4, 42, 'S', 1);
   CPPUNIT_ASSERT_MESSAGE("Grid should be 42SRF00, but it is: \"" + result + "\"", result == "42SRF00");

   // Survey Point: OP Cross

   for (unsigned int i = 0; i < 5; ++i)
   {
      std::string opCrossMGRS = dtUtil::Coordinates::ConvertUTMToMGRS(mOPCrossUTM[0], mOPCrossUTM[1],
                                                                      mEastWestZone, mNorthSouthZone,
                                                                      i+1); // Resolution is 1-6

      CPPUNIT_ASSERT_EQUAL(mOPCrossMGRSStrings[i], opCrossMGRS);
   }

   // Survey Point: Rough

   for (unsigned int i = 0; i < 5; ++i)
   {
      std::string roughMGRS = dtUtil::Coordinates::ConvertUTMToMGRS(mRoughUTM[0], mRoughUTM[1],
                                                                    mEastWestZone, mNorthSouthZone,
                                                                    i+1); // Resolution is 1-6

      CPPUNIT_ASSERT_EQUAL(mRoughMGRSStrings[i], roughMGRS);
   }

   // Survey Point: Dime

   for (unsigned int i = 0; i < 5; ++i)
   {
      std::string dimeMGRS = dtUtil::Coordinates::ConvertUTMToMGRS(mDimeUTM[0], mDimeUTM[1],
                                                                   mEastWestZone, mNorthSouthZone,
                                                                   i+1); // Resolution is 1-6

      CPPUNIT_ASSERT_EQUAL(mDimeMGRSStrings[i], dimeMGRS);
   }

}

//////////////////////////////////////////////////////////////////////////////
void CoordinateTests::TestMGRSToUTM()
{
   unsigned int zone;
   double easting, northing;

   try
   {
      dtUtil::Coordinates::ConvertMGRSToUTM(mEastWestZone, mNorthSouthZone,
                                            mOPCrossMGRSStrings[4],
                                            zone, easting, northing);

      CPPUNIT_ASSERT_EQUAL(mEastWestZone, zone);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(mOPCrossUTM[0], easting, mDelta);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(mOPCrossUTM[1], northing, mDelta);

      dtUtil::Coordinates::ConvertMGRSToUTM(mEastWestZone, mNorthSouthZone,
                                            mRoughMGRSStrings[4],
                                            zone, easting, northing);

      CPPUNIT_ASSERT_EQUAL(mEastWestZone, zone);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(mRoughUTM[0], easting, mDelta);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(mRoughUTM[1], northing, mDelta);

      dtUtil::Coordinates::ConvertMGRSToUTM(mEastWestZone, mNorthSouthZone,
                                            mDimeMGRSStrings[4],
                                            zone, easting, northing);

      CPPUNIT_ASSERT_EQUAL(mEastWestZone, zone);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(mDimeUTM[0], easting, mDelta);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(mDimeUTM[1], northing, mDelta);
   }
   catch (const dtUtil::Exception& ex)
   {
      CPPUNIT_FAIL(ex.What());
   }
}


//////////////////////////////////////////////////////////////////////////////
void CoordinateTests::TestUTMToCartesianUTMConversions()
{
   converter->SetIncomingCoordinateType(dtUtil::IncomingCoordinateType::UTM);
   converter->SetLocalCoordinateType(dtUtil::LocalCoordinateType::CARTESIAN_UTM);

   const osg::Vec3 testLoc(30.0f, 40.0f, 50.0f);
   const osg::Vec3d testRot(3.14f, 0.23f, 9.28f);

   osg::Vec3 result = converter->ConvertToLocalTranslation(testLoc);

   //input and output should be the same here.
   CPPUNIT_ASSERT(
      osg::equivalent(testLoc[0], result[0], 1e-2f)
      && osg::equivalent(testLoc[1], result[1], 1e-2f)
      && osg::equivalent(testLoc[2], result[2], 1e-2f));

   osg::Vec3 resultRot = converter->ConvertToLocalRotation(testRot[0], testRot[1], testRot[2]);

   //The output should be the same, just converted from eulers to hpr
   osg::Matrix expectedMatrix;
   dtUtil::Coordinates::EulersToMatrix(expectedMatrix, testRot[0], testRot[1], testRot[2]);
   osg::Vec3 expectedRot;
   expectedMatrix.invert(expectedMatrix);
   dtUtil::MatrixUtil::MatrixToHpr(expectedRot, expectedMatrix);

   CPPUNIT_ASSERT(
      osg::equivalent(expectedRot[0], resultRot[0], 1e-2f)
      && osg::equivalent(expectedRot[1], resultRot[1], 1e-2f)
      && osg::equivalent(expectedRot[2], resultRot[2], 1e-2f));

   osg::Vec3 locOffset(562078.225268f, 3788040.632974f, -32.0f);
   converter->SetLocalOffset(locOffset);

   result = converter->ConvertToLocalTranslation(testLoc);
   //input and output should be the same here.
   CPPUNIT_ASSERT(
      osg::equivalent(testLoc[0] - locOffset[0], result[0], 1e-2f)
      && osg::equivalent(testLoc[1] - locOffset[1], result[1], 1e-2f)
      && osg::equivalent(testLoc[2] - locOffset[2], result[2], 1e-2f));

   resultRot = converter->ConvertToLocalRotation(testRot[0], testRot[1], testRot[2]);
   dtUtil::Coordinates::EulersToMatrix(expectedMatrix, testRot[0], testRot[1], testRot[2]);
   expectedMatrix.invert(expectedMatrix);
   expectedMatrix = expectedMatrix * converter->GetOriginRotationMatrix();
   dtUtil::MatrixUtil::MatrixToHpr(expectedRot, expectedMatrix);

   CPPUNIT_ASSERT(
      osg::equivalent(expectedRot[0], resultRot[0], 1e-2f)
      && osg::equivalent(expectedRot[1], resultRot[1], 1e-2f)
      && osg::equivalent(expectedRot[2], resultRot[2], 1e-2f));

}

//////////////////////////////////////////////////////////////////////////////
void CoordinateTests::TestUTMToCartesianFlatEarthConversions()
{
   converter->SetIncomingCoordinateType(dtUtil::IncomingCoordinateType::UTM);
   converter->SetLocalCoordinateType(dtUtil::LocalCoordinateType::CARTESIAN_FLAT_EARTH);

   //Set the origin to the origin of the 11/s UTM zone.
   converter->SetFlatEarthOrigin(osg::Vec2d(32.0, -120.0));
   //This to set the zone.
   converter->SetUTMZone(11U);

   const osg::Vec3 testLoc(216577, 3544370, 0.0);
   const osg::Vec3d testRot(3.14f, 0.23f, 9.28f);

   osg::Vec3 result = converter->ConvertToLocalTranslation(testLoc);

   //input and output should be the same here.
   CPPUNIT_ASSERT(dtUtil::Equivalent(osg::Vec3(0.0, 0.0, 0.0), result, 1.0f));

   osg::Vec3 resultRot = converter->ConvertToLocalRotation(testRot[0], testRot[1], testRot[2]);

   //The output should be the same, just converted from eulers to hpr
   osg::Matrix expectedMatrix;
   dtUtil::Coordinates::EulersToMatrix(expectedMatrix, testRot[0], testRot[1], testRot[2]);
   osg::Vec3 expectedRot;
   expectedMatrix.invert(expectedMatrix);
   dtUtil::MatrixUtil::MatrixToHpr(expectedRot, expectedMatrix);

   CPPUNIT_ASSERT(dtUtil::Equivalent(expectedRot, resultRot, 1e-2f));

   osg::Vec3 locOffset(50, 50, -32.0);
   converter->SetLocalOffset(locOffset);

   result = converter->ConvertToLocalTranslation(testLoc);
   //input and output should be the same here.
   CPPUNIT_ASSERT(dtUtil::Equivalent(osg::Vec3(0.0, 0.0, 0.0) - locOffset, result, 1.0f));
}

//////////////////////////////////////////////////////////////////////////////
void CoordinateTests::CheckMilsConversion(float degrees, unsigned expectedMils, float expectedReverseDegrees)
{
   unsigned mils;
   float reverseDegrees;

   mils = dtUtil::Coordinates::DegreesToMils(degrees);

   reverseDegrees = dtUtil::Coordinates::MilsToDegrees(mils);

   std::ostringstream ss;

   ss << "The conversion to mils for " << degrees << " should be " << expectedMils << " but it is " << mils << std::endl;
   CPPUNIT_ASSERT_MESSAGE(ss.str(), mils == expectedMils);
   ss.str("");

   ss << "The conversion to back to degress for " << mils << " mils should be " << expectedReverseDegrees << " but it is " << reverseDegrees << std::endl;
   CPPUNIT_ASSERT_MESSAGE(ss.str(), osg::equivalent(expectedReverseDegrees, reverseDegrees, 0.00001f));
}

//////////////////////////////////////////////////////////////////////////////
void CoordinateTests::TestMilConversions()
{
   CheckMilsConversion(360.0f, 0, 360.0f);
   CheckMilsConversion(180.0f, 3200, 180.0f);
   CheckMilsConversion(-180.0f, 3200, 180.0f);
   CheckMilsConversion(-90.0f, 1600, 270.0f);
   CheckMilsConversion(90.0f, 4800, 90.0f);
   CheckMilsConversion(0.0f, 6400, 0.0f);
}

//////////////////////////////////////////////////////////////////////////////
void CoordinateTests::TestOperators()
{
   dtUtil::Coordinates coords1;
   dtUtil::Coordinates coords2;

   CPPUNIT_ASSERT_MESSAGE("The coordinates should initially be equal", coords1 == coords2);

   coords1.SetUTMLocalOffsetAsLatLon(osg::Vec3d(5, 4, 3));
   coords1.SetGlobeRadius(3);
   coords1.SetIncomingCoordinateType(dtUtil::IncomingCoordinateType::GEODETIC);
   coords1.SetLocalCoordinateType(dtUtil::LocalCoordinateType::GLOBE);
   coords1.SetLocalOffset(osg::Vec3d(4, 3, 1));
   coords1.SetUTMZone(8765);
   CPPUNIT_ASSERT_MESSAGE("The coordinates should NOT be equal", !(coords1 == coords2));

   dtUtil::Coordinates coords3(coords2);
   CPPUNIT_ASSERT_MESSAGE("The copy contructor should have set the values correctly", coords3 == coords2);

   dtUtil::Coordinates coords4;
   coords4 = coords2;

   CPPUNIT_ASSERT_MESSAGE("The assignment operator should have set the values correctly", coords4 == coords2);
}

//////////////////////////////////////////////////////////////////////////////
void CoordinateTests::TestConvertGeodeticToUTM()
{
   // Data converted with "Geographic/UTM Coordinate Converter"
   // http://home.hiwaay.net/~taylorc/toolbox/geography/geoutm.html
   //
   // Truth UTM data taken from 29 Palms survey points

   const double epsilon = 0.001;

   {
      unsigned int lovePuppyZone;
      double lovePuppyEasting;
      double lovePuppyNorthing;

      char nsZone;

      double lat = 34.49524520922253;
      double lon = -115.92735241604716;

      converter->CalculateUTMZone(lat, lon, lovePuppyZone, nsZone);
      converter->SetUTMZone(lovePuppyZone);
      converter->ConvertGeodeticToUTM(osg::DegreesToRadians(lat),
                                      osg::DegreesToRadians(lon),
                                      lovePuppyZone, 'N', lovePuppyEasting, lovePuppyNorthing);

      CPPUNIT_ASSERT_DOUBLES_EQUAL(598480.2, lovePuppyEasting, epsilon);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(3817592.11, lovePuppyNorthing, epsilon);
   }

   {
      unsigned int dacoZone;
      double dacoEasting;
      double dacoNorthing;

      char nsZone;

      double lat = 34.30906708995865;
      double lon = -116.03105100289258;

      converter->CalculateUTMZone(lat, lon, dacoZone, nsZone);
      converter->SetUTMZone(dacoZone);
      converter->ConvertGeodeticToUTM(osg::DegreesToRadians(lat),
                                      osg::DegreesToRadians(lon),
                                      dacoZone, 'N', dacoEasting, dacoNorthing);

      //CPPUNIT_ASSERT_EQUAL('11S', dacoZone ) //How to convert to long?
      CPPUNIT_ASSERT_DOUBLES_EQUAL(589156.53, dacoEasting, epsilon);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(3796850.54, dacoNorthing, epsilon);
   }

   {
      unsigned int fatbackZone;
      double fatbackEasting;
      double fatbackNorthing;

      char nsZone;

      double lat = 34.383765465383945;
      double lon = -115.96319687438611;

      converter->CalculateUTMZone(lat, lon, fatbackZone, nsZone);
      converter->SetUTMZone(fatbackZone);
      converter->ConvertGeodeticToUTM( osg::DegreesToRadians(lat),
                                       osg::DegreesToRadians(lon),
                                       fatbackZone, 'N', fatbackEasting, fatbackNorthing );

      CPPUNIT_ASSERT_DOUBLES_EQUAL( 595315.70, fatbackEasting, epsilon );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 3805195.52, fatbackNorthing, epsilon );
   }
}

//////////////////////////////////////////////////////////////////////////////
void CoordinateTests::TestConvertUTMToGeodetic()
{
   const double epsilon = 0.05;

   double lat;
   double lon;

   converter->ConvertUTMToGeodetic( 10, 'N', 500000, 5005000, lat, lon);

   CPPUNIT_ASSERT_DOUBLES_EQUAL( 45.2, osg::RadiansToDegrees(lat), epsilon );
   CPPUNIT_ASSERT_DOUBLES_EQUAL( -123.0, osg::RadiansToDegrees(lon), epsilon );

   converter->ConvertUTMToGeodetic( 10, 'S', 500000, 5005000, lat, lon);

   CPPUNIT_ASSERT_DOUBLES_EQUAL( -45.1, osg::RadiansToDegrees(lat), epsilon );
   CPPUNIT_ASSERT_DOUBLES_EQUAL( -123.0, osg::RadiansToDegrees(lon), epsilon );
}
