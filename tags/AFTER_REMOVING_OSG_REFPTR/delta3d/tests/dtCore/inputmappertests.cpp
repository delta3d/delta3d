/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006 MOVES Institute 
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
 * @author John K. Grant
 */
#include <cppunit/extensions/HelperMacros.h>
#include <dtCore/inputmapper.h>

namespace dtTest
{
   /// unit tests for dtCore::Axis
   class InputMapperTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE( InputMapperTests );
      CPPUNIT_TEST( TestObservers );
      CPPUNIT_TEST_SUITE_END();

      public:
         void setUp();
         void tearDown();

         ///\todo callback triggering and data flow to observers
         void TestObservers();

      private:
   };
}

using namespace dtTest;

void InputMapperTests::setUp()
{
}

void InputMapperTests::tearDown()
{
}

void InputMapperTests::TestObservers()
{
}
