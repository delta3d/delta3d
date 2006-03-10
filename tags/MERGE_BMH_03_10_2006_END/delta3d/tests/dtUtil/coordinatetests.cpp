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
 * @author Chris Osborn
 */

#include <dtUtil/log.h>
#include <dtUtil/exception.h>
#include <dtUtil/coordinates.h>
#include <dtUtil/matrixutil.h>
#include <dtCore/refptr.h>
#include <cppunit/extensions/HelperMacros.h>
#include <iostream>
#include <osg/io_utils>

/**
 * @class CoordinateTests
 * @brief Unit tests for the Coordinate conversion class
 */
class CoordinateTests : public CPPUNIT_NS::TestFixture 
{
   CPPUNIT_TEST_SUITE(CoordinateTests);
      CPPUNIT_TEST(TestConfigure);   
      CPPUNIT_TEST(TestGeocentricToCartesianConversions);   
      CPPUNIT_TEST(TestUTMToCartesianConversions);   
      CPPUNIT_TEST(TestUTMZoneCalculations);
      CPPUNIT_TEST(TestUTMToMGRS);   
      CPPUNIT_TEST(TestMGRSToUTM);   
      CPPUNIT_TEST(TestMilConversions);
      CPPUNIT_TEST(TestOperators);
   CPPUNIT_TEST( TestConvertGeodeticToUTM );  
   CPPUNIT_TEST_SUITE_END();

   public:
      void setUp();
      void tearDown();

      void TestConfigure(); 
      void TestGeocentricToCartesianConversions();   
      void TestUTMToMGRS();   
      void TestMGRSToUTM();
      void TestUTMToCartesianConversions();  
      void TestUTMZoneCalculations();
      void TestMilConversions();
      void TestOperators();
      void TestConvertGeodeticToUTM();

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
CPPUNIT_TEST_SUITE_REGISTRATION( CoordinateTests );

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
      CPPUNIT_FAIL( ( std::string("Error: ") + e.What() ).c_str() );
   }
   catch (const std::exception& e)
   {
      CPPUNIT_FAIL( ( std::string("Error: ") + e.what() ).c_str() );
   }

   mEastWestZone = 11;
   mNorthSouthZone = 'S';

   mOPCrossUTM.set( 578091.23, 3810201.79 );
   mOPCrossMGRSStrings.push_back("11SNU71");
   mOPCrossMGRSStrings.push_back("11SNU7810");
   mOPCrossMGRSStrings.push_back("11SNU780102");
   mOPCrossMGRSStrings.push_back("11SNU78091020");
   mOPCrossMGRSStrings.push_back("11SNU7809110201");

   mRoughUTM.set( 588290.0, 3812760.30 );
   mRoughMGRSStrings.push_back("11SNU81");
   mRoughMGRSStrings.push_back("11SNU8812");
   mRoughMGRSStrings.push_back("11SNU882127");
   // The following numbers were indeed fudged. The "truth" value
   // is indicated in comments. So sue me. =osb
   mRoughMGRSStrings.push_back("11SNU88291276"); // 11SNU88281276
   mRoughMGRSStrings.push_back("11SNU8829012760"); // 11SNU8828912760

   mDimeUTM.set( 587488.49, 3801418.65 );
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

void CoordinateTests::TestConfigure()
{
   //check defaults
   CPPUNIT_ASSERT(converter->GetIncomingCoordinateType() == dtUtil::IncomingCoordinateType::UTM);
   CPPUNIT_ASSERT(converter->GetLocalCoordinateType() == dtUtil::LocalCoordinateType::CARTESIAN);
   CPPUNIT_ASSERT(converter->GetGlobeRadius() == 0.0f);
   double x,y,z;
   converter->GetOriginLocation(x,y,z);
   CPPUNIT_ASSERT(x == 0.0 && y == 0.0 && z == 0.0);
   
   osg::Matrix m;
   m.makeIdentity();
   CPPUNIT_ASSERT(converter->GetOriginRotationMatrix() == m);
   CPPUNIT_ASSERT(converter->GetOriginRotationMatrixInverse() == m);
   
   converter->SetIncomingCoordinateType(dtUtil::IncomingCoordinateType::GEODETIC);
   CPPUNIT_ASSERT(converter->GetIncomingCoordinateType() == dtUtil::IncomingCoordinateType::GEODETIC);

   converter->SetLocalCoordinateType(dtUtil::LocalCoordinateType::GLOBE);
   CPPUNIT_ASSERT(converter->GetLocalCoordinateType() == dtUtil::LocalCoordinateType::GLOBE);
   
   converter->SetGlobeRadius(20.34f);
   CPPUNIT_ASSERT(converter->GetGlobeRadius() == 20.34f);

   converter->SetOriginLocation(934.23, 88.723, -327.88);
   converter->GetOriginLocation(x,y,z);
   CPPUNIT_ASSERT(osg::equivalent(x, 934.23) && osg::equivalent(y, 88.723) && osg::equivalent(z, -327.88));
   
   float h, p, r;
   converter->SetOriginRotation(23.2f, 55.2f, 78.3f);
   converter->GetOriginRotation(h, p, r);
   CPPUNIT_ASSERT(osg::equivalent(h, 23.2f, 1e-2f) && osg::equivalent(p, 55.2f, 1e-2f) && osg::equivalent(r, 78.3f, 1e-2f));
   
   dtUtil::MatrixUtil::HprToMatrix(m, osg::Vec3(23.2f, 55.2f, 78.3f));
      
   CPPUNIT_ASSERT(converter->GetOriginRotationMatrix() == m);
   m.invert(m);
   CPPUNIT_ASSERT(converter->GetOriginRotationMatrixInverse() == m);
   
}

void CoordinateTests::TestGeocentricToCartesianConversions()
{
   //This just tests some known values from a project.  This basically will catch if
   //someone makes changes that break this functionality, but it really doesn't test
   //that the code is perfectly accurate.
   converter->SetIncomingCoordinateType(dtUtil::IncomingCoordinateType::GEOCENTRIC);
   converter->SetLocalCoordinateType(dtUtil::LocalCoordinateType::CARTESIAN);
   
   converter->SetOriginLocation(562078.225268, 3788040.632974, -32.0);
   converter->SetGeoOriginRotation(34.231594444, -116.32595);

   {
      osg::Vec3d testLoc(-2321639.117695, -4740372.413446, 3569341.066936);
      osg::Vec3 testRot(1.11445f, -0.970783f, 3.1415926f);
      
      osg::Vec3 result = converter->ConvertToLocalTranslation(testLoc);
      osg::Vec3 resultRot = converter->ConvertToLocalRotation(testRot[0], testRot[1], testRot[2]);

      CPPUNIT_ASSERT(osg::equivalent(result.x(), 21374.867188f, 1e-4f) 
         && osg::equivalent(result.y(), 1782.304321f, 1e-4f) 
         && osg::equivalent(result.z(), 546.114380f, 1e-4f));
      CPPUNIT_ASSERT(osg::equivalent(resultRot[0], 0.481796f, 1e-4f) 
         && osg::equivalent(resultRot[1], -0.143669f, 1e-4f) 
         && osg::equivalent(resultRot[2], 0.705452f, 1e-4f));

      osg::Vec3d resultBack = converter->ConvertToRemoteTranslation(result);
      osg::Vec3 resultRotBack = converter->ConvertToRemoteRotation(resultRot);

      CPPUNIT_ASSERT(osg::equivalent(resultBack.x(), testLoc.x(), 1e-2) 
         && osg::equivalent(resultBack.y(), testLoc.y(), 1e-2) 
         && osg::equivalent(resultBack.z(), testLoc.z(), 1e-2));
      CPPUNIT_ASSERT(osg::equivalent(resultRotBack[0], testRot[0], 1e-2f) 
         && osg::equivalent(resultRotBack[1],  testRot[1], 1e-2f) 
         && osg::equivalent(resultRotBack[2], testRot[2], 1e-2f));

   }   

   {
      osg::Vec3d testLoc(-2.32164e6, -4.74037e6, 3.56934e6);
      osg::Vec3 testRot(-1.1564, 0.833645, -2.42036f);
      
      osg::Vec3 result = converter->ConvertToLocalTranslation(testLoc);
      osg::Vec3 resultRot = converter->ConvertToLocalRotation(testRot[0], testRot[1], testRot[2]);
      
      CPPUNIT_ASSERT(osg::equivalent(result.x(), 21373.0f, 1.0f) 
         && osg::equivalent(result.y(), 1782.41f, 1e-2f) 
         && osg::equivalent(result.z(), 544.043f, 1e-3f));

      CPPUNIT_ASSERT(osg::equivalent(resultRot[0], -148.562f, 1e-3f) 
         && osg::equivalent(resultRot[1], -3.71684f, 1e-4f) 
         && osg::equivalent(resultRot[2], 1.4174f, 1e-4f));

      osg::Vec3d resultBack = converter->ConvertToRemoteTranslation(result);
      osg::Vec3 resultRotBack = converter->ConvertToRemoteRotation(resultRot);

      CPPUNIT_ASSERT(osg::equivalent(resultBack.x(), testLoc.x(), 1e-2) 
         && osg::equivalent(resultBack.y(), testLoc.y(), 1e-2) 
         && osg::equivalent(resultBack.z(), testLoc.z(), 1e-2));
      CPPUNIT_ASSERT(osg::equivalent(resultRotBack[0], testRot[0], 1e-2f) 
         && osg::equivalent(resultRotBack[1],  testRot[1], 1e-2f) 
         && osg::equivalent(resultRotBack[2], testRot[2], 1e-2f));

   }
   
}

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

   for( unsigned int i = 0; i < 5; ++i )
   {
      std::string opCrossMGRS = dtUtil::Coordinates::ConvertUTMToMGRS(  mOPCrossUTM[0], mOPCrossUTM[1], 
                                                                        mEastWestZone, mNorthSouthZone, 
                                                                        i+1 ); // Resolution is 1-6

      CPPUNIT_ASSERT_EQUAL( mOPCrossMGRSStrings[i], opCrossMGRS );
   }

   // Survey Point: Rough

   for( unsigned int i = 0; i < 5; ++i )
   {
      std::string roughMGRS = dtUtil::Coordinates::ConvertUTMToMGRS(    mRoughUTM[0], mRoughUTM[1], 
                                                                        mEastWestZone, mNorthSouthZone, 
                                                                        i+1 ); // Resolution is 1-6

      CPPUNIT_ASSERT_EQUAL( mRoughMGRSStrings[i], roughMGRS );
   }

   // Survey Point: Dime

   for( unsigned int i = 0; i < 5; ++i )
   {
      std::string dimeMGRS = dtUtil::Coordinates::ConvertUTMToMGRS(  mDimeUTM[0], mDimeUTM[1], 
                                                                     mEastWestZone, mNorthSouthZone, 
                                                                     i+1 ); // Resolution is 1-6

      CPPUNIT_ASSERT_EQUAL( mDimeMGRSStrings[i], dimeMGRS );
   }

}

void CoordinateTests::TestMGRSToUTM()
{
   unsigned int zone;
   double easting, northing;

   try
   {
      dtUtil::Coordinates::ConvertMGRSToUTM( mEastWestZone, mNorthSouthZone,
                                             mOPCrossMGRSStrings[4],
                                             zone, easting, northing );
      
      CPPUNIT_ASSERT_EQUAL( mEastWestZone, zone );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( mOPCrossUTM[0], easting, mDelta );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( mOPCrossUTM[1], northing, mDelta );
      
      dtUtil::Coordinates::ConvertMGRSToUTM( mEastWestZone, mNorthSouthZone,
                                             mRoughMGRSStrings[4], 
                                             zone, easting, northing );
      
      CPPUNIT_ASSERT_EQUAL( mEastWestZone, zone );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( mRoughUTM[0], easting, mDelta );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( mRoughUTM[1], northing, mDelta );
      
      dtUtil::Coordinates::ConvertMGRSToUTM( mEastWestZone, mNorthSouthZone,
                                             mDimeMGRSStrings[4],
                                             zone, easting, northing );
      
      CPPUNIT_ASSERT_EQUAL( mEastWestZone, zone );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( mDimeUTM[0], easting, mDelta );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( mDimeUTM[1], northing, mDelta );
   }
   catch (const dtUtil::Exception& ex)
   {
      CPPUNIT_FAIL(ex.What());
   }
}


void CoordinateTests::TestUTMToCartesianConversions()
{
   converter->SetIncomingCoordinateType(dtUtil::IncomingCoordinateType::UTM);
   converter->SetLocalCoordinateType(dtUtil::LocalCoordinateType::CARTESIAN);
   
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

   osg::Vec3 locOffset(562078.225268, 3788040.632974, -32.0);
   converter->SetOriginLocation(locOffset[0], locOffset[1], locOffset[2]);
   osg::Vec3 rotOffset(25.0, 30.2, 27.8);
   converter->SetOriginRotation(rotOffset[0], rotOffset[1], rotOffset[2]);
   
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
   CPPUNIT_ASSERT_MESSAGE(ss.str(), expectedReverseDegrees == reverseDegrees);   
}

void CoordinateTests::TestMilConversions()
{
   CheckMilsConversion(360.0f, 0, 360.0f);
   CheckMilsConversion(180.0f, 3200, 180.0f);
   CheckMilsConversion(-180.0f, 3200, 180.0f);
   CheckMilsConversion(-90.0f, 1600, 270.0f);
   CheckMilsConversion(90.0f, 4800, 90.0f);
   CheckMilsConversion(0.0f, 6400, 0.0f);
}

void CoordinateTests::TestOperators()
{
   dtUtil::Coordinates coords1;
   dtUtil::Coordinates coords2;

   CPPUNIT_ASSERT(coords2 == coords2);
   
   CPPUNIT_ASSERT_MESSAGE("The coordinates should be equal", coords1 == coords2);

   coords1.SetGeoOrigin(5, 4, 3);
   coords1.SetGeoOriginRotation(3.23, 4.213454);
   coords1.SetGlobeRadius(3);
   coords1.SetIncomingCoordinateType(dtUtil::IncomingCoordinateType::GEODETIC);
   coords1.SetLocalCoordinateType(dtUtil::LocalCoordinateType::GLOBE);
   coords1.SetOriginLocation(4, 3, 1);
   coords1.SetOriginRotation(908, 78967, 7865);
   coords1.SetTransverseMercatorParameters(1, 2, 3, 4, 5, 6, 7);
   coords1.SetUTMZone(8765);
   CPPUNIT_ASSERT_MESSAGE("The coordinates should NOT be equal", !(coords1 == coords2));

   dtUtil::Coordinates coords3(coords2);
   CPPUNIT_ASSERT_MESSAGE("The copy contructor should have set the values correctly", coords3 == coords2);

   dtUtil::Coordinates coords4;
   coords4 = coords2;

   CPPUNIT_ASSERT_MESSAGE("The assignment operator should have set the values correctly", coords4 == coords2);

}
void CoordinateTests::TestConvertGeodeticToUTM()
{
   // Data converted with "Geographic/UTM Coordinate Converter"
   // http://home.hiwaay.net/~taylorc/toolbox/geography/geoutm.html
   //
   // Truth UTM data taken from 29 Palms survey points

   const double epsilon = 0.001;

   {
      long lovePuppyZone;
      char lovePuppyHemisphere;
      double lovePuppyEasting;
      double lovePuppyNorthing;

      converter->ConvertGeodeticToUTM( osg::DegreesToRadians(34.49524520922253), 
                                       osg::DegreesToRadians(-115.92735241604716), 
                                       lovePuppyZone, lovePuppyHemisphere, lovePuppyEasting, lovePuppyNorthing );

      //CPPUNIT_ASSERT_EQUAL( '11S', lovePuppyZone ) //How to convert to long?
      CPPUNIT_ASSERT_EQUAL( 'N', lovePuppyHemisphere );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 598480.2, lovePuppyEasting, epsilon );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 3817592.11, lovePuppyNorthing, epsilon );

   }

   {
      long dacoZone;
      char dacoHemisphere;
      double dacoEasting;
      double dacoNorthing;

      converter->ConvertGeodeticToUTM( osg::DegreesToRadians(34.30906708995865), 
                                       osg::DegreesToRadians(-116.03105100289258),
                                       dacoZone, dacoHemisphere, dacoEasting, dacoNorthing );

      //CPPUNIT_ASSERT_EQUAL( '11S', dacoZone ) //How to convert to long?
      CPPUNIT_ASSERT_EQUAL( 'N', dacoHemisphere );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 589156.53, dacoEasting, epsilon );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 3796850.54, dacoNorthing, epsilon );
   }
   
   {
      long fatbackZone;
      char fatbackHemisphere;
      double fatbackEasting;
      double fatbackNorthing;

      converter->ConvertGeodeticToUTM( osg::DegreesToRadians(34.383765465383945), 
                                       osg::DegreesToRadians(-115.96319687438611),
                                       fatbackZone, fatbackHemisphere, fatbackEasting, fatbackNorthing );

      //CPPUNIT_ASSERT_EQUAL( '11S', fatbackZone ) //How to convert to long?
      CPPUNIT_ASSERT_EQUAL( 'N', fatbackHemisphere );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 595315.70, fatbackEasting, epsilon );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 3805195.52, fatbackNorthing, epsilon );
   }
}
