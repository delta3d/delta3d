/* -*-c++-*-
 * allTests - This source file (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2009 MOVES Institute
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
 * Erik Johnson
 */

#include <prefix/unittestprefix.h>
#include <cppunit/extensions/HelperMacros.h>
#include <dtCore/resourcedescriptor.h>
#include <sstream>

class ResourceDescriptorTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(ResourceDescriptorTests);
      CPPUNIT_TEST(TestNULLResource);
      CPPUNIT_TEST(TestCreatingNULLResource);
      CPPUNIT_TEST(TestStreamOperators);
   CPPUNIT_TEST_SUITE_END();

public:
   void setUp();
   void tearDown();

   void TestNULLResource();
   void TestCreatingNULLResource();
   void TestStreamOperators();
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(ResourceDescriptorTests);

void ResourceDescriptorTests::setUp()
{
}

void ResourceDescriptorTests::tearDown()
{
}

void ResourceDescriptorTests::TestNULLResource()
{
   CPPUNIT_ASSERT_EQUAL_MESSAGE("NULL_RESOURCE isn't empty",
      true, dtCore::ResourceDescriptor::NULL_RESOURCE.IsEmpty());

   CPPUNIT_ASSERT_EQUAL_MESSAGE("NULL_RESOURCE Display Name isn't empty",
      true, dtCore::ResourceDescriptor::NULL_RESOURCE.GetDisplayName().empty());

   CPPUNIT_ASSERT_EQUAL_MESSAGE("NULL_RESOURCE Resource Identifier isn't empty",
      true, dtCore::ResourceDescriptor::NULL_RESOURCE.GetResourceIdentifier().empty());
}

void ResourceDescriptorTests::TestCreatingNULLResource()
{
   dtCore::ResourceDescriptor resource("name", "id");
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Resource should not be NULL",
                                false, resource.IsEmpty());

   resource.Clear();
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Resource should be NULL after Clear()",
                                 true, resource.IsEmpty());

}

void ResourceDescriptorTests::TestStreamOperators()
{
   dtCore::ResourceDescriptor resource("name", "id");

   std::ostringstream ss;
   ss << resource;

   std::istringstream iss;
   iss.str(ss.str());

   dtCore::ResourceDescriptor resultResource;

   iss >> resultResource;

   CPPUNIT_ASSERT_EQUAL(resource, resultResource);

   resource = dtCore::ResourceDescriptor::NULL_RESOURCE;

   ss.str("");

   ss << resource;

   iss.str(ss.str());

   iss >> resultResource;
   CPPUNIT_ASSERT(resultResource.IsEmpty());
}
