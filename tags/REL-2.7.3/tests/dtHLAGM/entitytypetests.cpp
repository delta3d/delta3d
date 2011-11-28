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
 * @author Olen A. Bruce
 * @author David Guthrie
 */
#include <prefix/unittestprefix.h>
#include <cppunit/extensions/HelperMacros.h>

#include <vector>
#include <string>
#include <iostream>
#include <dtUtil/coordinates.h>
#include <dtCore/datatype.h>
#include <dtGame/messagetype.h>
#include <dtHLAGM/objecttoactor.h>
#include <dtHLAGM/interactiontomessage.h>
#include <dtHLAGM/attributetoproperty.h>
#include <dtHLAGM/parametertoparameter.h>
#include <dtHLAGM/onetoonemapping.h>
#include <dtHLAGM/distypes.h>

class EntityTypeTests : public CPPUNIT_NS::TestFixture 
{
   CPPUNIT_TEST_SUITE(EntityTypeTests);
  
   CPPUNIT_TEST(TestGetSet);
   CPPUNIT_TEST(TestAssign);
   CPPUNIT_TEST(TestEncodeDecode);

   CPPUNIT_TEST_SUITE_END();

   public:
      void setUp();
      void tearDown();
      void TestGetSet();
      void TestAssign();
      void TestEncodeDecode();
   
   private:
      dtHLAGM::EntityType et;
      void SetValues(dtHLAGM::EntityType& et);
      void CheckValues(dtHLAGM::EntityType& et);
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(EntityTypeTests);

void EntityTypeTests::setUp()
{
   SetValues(et);
}

void EntityTypeTests::tearDown()
{
}

void EntityTypeTests::SetValues(dtHLAGM::EntityType& et)
{
   et.SetKind(3);
   et.SetDomain(4);
   et.SetCountry(9000);
   et.SetCategory(100);
   et.SetSubcategory(49);
   et.SetSpecific(30);
   et.SetExtra(92);
}

void EntityTypeTests::CheckValues(dtHLAGM::EntityType& et)
{
   CPPUNIT_ASSERT(et.GetKind() == 3);
   CPPUNIT_ASSERT(et.GetDomain() == 4);
   CPPUNIT_ASSERT(et.GetCountry() == 9000);
   CPPUNIT_ASSERT(et.GetCategory() == 100);
   CPPUNIT_ASSERT(et.GetSubcategory() == 49);
   CPPUNIT_ASSERT(et.GetSpecific() == 30);
   CPPUNIT_ASSERT(et.GetExtra() == 92);
}

void EntityTypeTests::TestGetSet()
{
   CheckValues(et);
}

void EntityTypeTests::TestAssign()
{
   dtHLAGM::EntityType et2;
   et2 = et;
   CheckValues(et2);
   
   CPPUNIT_ASSERT(et2 == et);
}

void EntityTypeTests::TestEncodeDecode()
{
   char buffer[8];
   et.Encode(buffer);
   
   dtHLAGM::EntityType et2;
   et2.Decode(buffer);
   
   CheckValues(et2);
   CPPUNIT_ASSERT(et2.EncodedLength() == 8);
}
