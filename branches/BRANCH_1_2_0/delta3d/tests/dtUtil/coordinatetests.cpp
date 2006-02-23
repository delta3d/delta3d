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

#include <dtUtil/log.h>
#include <dtUtil/exception.h>
#include <dtUtil/coordinates.h>
#include <dtUtil/matrixutil.h>
#include <cppunit/extensions/HelperMacros.h>
#include <osg/Vec3>

/**
 * @class DeprecationMgrTests
 * @brief Unit tests for the deprecation manager
 */
class CoordinateTests : public CPPUNIT_NS::TestFixture {
   CPPUNIT_TEST_SUITE( CoordinateTests );
   CPPUNIT_TEST( TestConfigure );   
   CPPUNIT_TEST( TestGeocentricToCartesianConversions );   
   CPPUNIT_TEST( TestUTMToCartesianConversions );   
   CPPUNIT_TEST_SUITE_END();

   public:
      void setUp();
      void tearDown();

      void TestConfigure(); 
      void TestGeocentricToCartesianConversions();   
      void TestUTMToCartesianConversions();   

   private:
      
      dtUtil::Log* mLogger; 
      dtUtil::Coordinates* converter;
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


