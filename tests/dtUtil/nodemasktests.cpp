/* -*-c++-*-
 * allTests - This source file (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2014, Caper Holdings, LLC
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
 * @author Bradley Anderegg
 */

#include <prefix/unittestprefix.h>
#include <cppunit/extensions/HelperMacros.h>
#include <dtUtil//nodemask.h>

namespace dtUtil
{

/**
 * @class NodeMaskTests
 */
class NodeMaskTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(NodeMaskTests);
   CPPUNIT_TEST(TestNodeMask);   
   CPPUNIT_TEST_SUITE_END();

   public:
      void setUp();
      void tearDown();

      void TestNodeMask();

      
   private:
      

};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(NodeMaskTests);

///////////////////////////////////////////////////////////////////////////////
void NodeMaskTests::setUp()
{
}

///////////////////////////////////////////////////////////////////////////////
void NodeMaskTests::tearDown()
{
}

///////////////////////////////////////////////////////////////////////////////
void NodeMaskTests::TestNodeMask()
{

   //everything or nothing
   CPPUNIT_ASSERT_MESSAGE("Everything and nothing should return false",
      !(dtUtil::NodeMask::NOTHING.GetNodeMask() & 
      dtUtil::NodeMask::EVERYTHING.GetNodeMask()));

   //everything
   CPPUNIT_ASSERT_MESSAGE("Everything and Background should return true",
      (dtUtil::NodeMask::EVERYTHING.GetNodeMask() & 
      dtUtil::NodeMask::BACKGROUND.GetNodeMask()));

   CPPUNIT_ASSERT_MESSAGE("Everything and Foreground should return true",
      (dtUtil::NodeMask::EVERYTHING.GetNodeMask() & 
      dtUtil::NodeMask::FOREGROUND.GetNodeMask()));

   CPPUNIT_ASSERT_MESSAGE("Everything and Transparent Effects should return true",
      (dtUtil::NodeMask::EVERYTHING.GetNodeMask() & 
      dtUtil::NodeMask::TRANSPARENT_EFFECTS.GetNodeMask()));

   CPPUNIT_ASSERT_MESSAGE("Everything and Water should return true",
      (dtUtil::NodeMask::EVERYTHING.GetNodeMask() & 
      dtUtil::NodeMask::WATER.GetNodeMask()));

   CPPUNIT_ASSERT_MESSAGE("Everything and Terrain Features should return true",
      (dtUtil::NodeMask::EVERYTHING.GetNodeMask() & 
      dtUtil::NodeMask::TERRAIN_FEATURES.GetNodeMask()));

   CPPUNIT_ASSERT_MESSAGE("Everything and DEFAULT_GEOMETRY should return true",
      (dtUtil::NodeMask::EVERYTHING.GetNodeMask() & 
      dtUtil::NodeMask::DEFAULT_GEOMETRY.GetNodeMask()));


   CPPUNIT_ASSERT_MESSAGE("Background and Foreground should return false",
      !(dtUtil::NodeMask::BACKGROUND.GetNodeMask() & 
      dtUtil::NodeMask::FOREGROUND.GetNodeMask()));
   
   CPPUNIT_ASSERT_MESSAGE("Foreground and Transparent Effects should return false", 
      !(dtUtil::NodeMask::FOREGROUND.GetNodeMask() & 
      dtUtil::NodeMask::TRANSPARENT_EFFECTS.GetNodeMask()));
   
   CPPUNIT_ASSERT_MESSAGE("Transparent Effects and Water should return false", 
      !(dtUtil::NodeMask::TRANSPARENT_EFFECTS.GetNodeMask() & 
      dtUtil::NodeMask::WATER.GetNodeMask()));

   //IGNORE_RAYCAST

   CPPUNIT_ASSERT_MESSAGE("Background should ignore raycast", 
      (dtUtil::NodeMask::BACKGROUND.GetNodeMask() | 
      dtUtil::NodeMask::IGNORE_RAYCAST.GetNodeMask()));

   CPPUNIT_ASSERT_MESSAGE("Foreground should ignore raycast", 
      (dtUtil::NodeMask::FOREGROUND.GetNodeMask() |  
      dtUtil::NodeMask::IGNORE_RAYCAST.GetNodeMask()));

   CPPUNIT_ASSERT_MESSAGE("Transparent effects should ignore raycast", 
      (dtUtil::NodeMask::TRANSPARENT_EFFECTS.GetNodeMask() |
      dtUtil::NodeMask::IGNORE_RAYCAST.GetNodeMask()));

   CPPUNIT_ASSERT_MESSAGE("Water should ignore raycast", 
      (dtUtil::NodeMask::WATER.GetNodeMask() |
      dtUtil::NodeMask::IGNORE_RAYCAST.GetNodeMask()));

   CPPUNIT_ASSERT_MESSAGE("Terrain features should not ignore raycast", 
      !(dtUtil::NodeMask::TERRAIN_FEATURES.GetNodeMask() &
      dtUtil::NodeMask::IGNORE_RAYCAST.GetNodeMask()));

   CPPUNIT_ASSERT_MESSAGE("Default geometry should not ignore raycast", 
      !(dtUtil::NodeMask::DEFAULT_GEOMETRY.GetNodeMask() &
      dtUtil::NodeMask::IGNORE_RAYCAST.GetNodeMask()));

   CPPUNIT_ASSERT_MESSAGE("Non Transparent geometry should not ignore raycast", 
      !(dtUtil::NodeMask::NON_TRANSPARENT_GEOMETRY.GetNodeMask() &
      dtUtil::NodeMask::IGNORE_RAYCAST.GetNodeMask()));

   CPPUNIT_ASSERT_MESSAGE("Transparent geometry should not ignore raycast", 
      !(dtUtil::NodeMask::TRANSPARENT_GEOMETRY.GetNodeMask() &
      dtUtil::NodeMask::IGNORE_RAYCAST.GetNodeMask()));

   //SCENE_INTERSECT_MASK

   CPPUNIT_ASSERT_MESSAGE("Background should not intersect with scene", 
      !(dtUtil::NodeMask::BACKGROUND.GetNodeMask() &
      dtUtil::NodeMask::SCENE_INTERSECT_MASK.GetNodeMask()));

   CPPUNIT_ASSERT_MESSAGE("Foreground should not intersect with scene", 
      !(dtUtil::NodeMask::FOREGROUND.GetNodeMask() &  
      dtUtil::NodeMask::SCENE_INTERSECT_MASK.GetNodeMask()));

   CPPUNIT_ASSERT_MESSAGE("Transparent effects should not intersect with scene", 
      !(dtUtil::NodeMask::TRANSPARENT_EFFECTS.GetNodeMask() &
      dtUtil::NodeMask::SCENE_INTERSECT_MASK.GetNodeMask()));

   CPPUNIT_ASSERT_MESSAGE("Water should not intersect with scene", 
      !(dtUtil::NodeMask::WATER.GetNodeMask() &
      dtUtil::NodeMask::SCENE_INTERSECT_MASK.GetNodeMask()));

   CPPUNIT_ASSERT_MESSAGE("Terrain features should intersect with scene", 
      (dtUtil::NodeMask::TERRAIN_FEATURES.GetNodeMask() |
      dtUtil::NodeMask::SCENE_INTERSECT_MASK.GetNodeMask()));

   CPPUNIT_ASSERT_MESSAGE("Default geometry should intersect with scene", 
      (dtUtil::NodeMask::DEFAULT_GEOMETRY.GetNodeMask() |
      dtUtil::NodeMask::SCENE_INTERSECT_MASK.GetNodeMask()));

   CPPUNIT_ASSERT_MESSAGE("Non Transparent geometry should intersect with scene", 
      (dtUtil::NodeMask::NON_TRANSPARENT_GEOMETRY.GetNodeMask() |
      dtUtil::NodeMask::SCENE_INTERSECT_MASK.GetNodeMask()));

   CPPUNIT_ASSERT_MESSAGE("Transparent geometry should intersect with scene", 
      (dtUtil::NodeMask::TRANSPARENT_GEOMETRY.GetNodeMask() |
      dtUtil::NodeMask::SCENE_INTERSECT_MASK.GetNodeMask()));

   //default geometry
   CPPUNIT_ASSERT_MESSAGE("Default geometry should contain non transparent geometry", 
      (dtUtil::NodeMask::DEFAULT_GEOMETRY.GetNodeMask() |
      dtUtil::NodeMask::NON_TRANSPARENT_GEOMETRY.GetNodeMask()));

   CPPUNIT_ASSERT_MESSAGE("Default geometry should contain transparent geometry", 
      (dtUtil::NodeMask::DEFAULT_GEOMETRY.GetNodeMask() |
      dtUtil::NodeMask::TRANSPARENT_GEOMETRY.GetNodeMask()));

   //transparent - non- transparent
   CPPUNIT_ASSERT_MESSAGE("Transparent and non transparent should be 0", 
      !(dtUtil::NodeMask::NON_TRANSPARENT_GEOMETRY.GetNodeMask() &
      dtUtil::NodeMask::TRANSPARENT_GEOMETRY.GetNodeMask()));


   //cull mask


   CPPUNIT_ASSERT_MESSAGE("Main cull camera mask should contain everything", 
      (dtUtil::NodeMask::MAIN_CAMERA_CULL_MASK.GetNodeMask() &
       dtUtil::NodeMask::EVERYTHING.GetNodeMask()));


   CPPUNIT_ASSERT_MESSAGE("Additional camera mask should not contain Foreground", 
      !(dtUtil::NodeMask::ADDITIONAL_CAMERA_CULL_MASK.GetNodeMask() &
       dtUtil::NodeMask::FOREGROUND.GetNodeMask()));

   CPPUNIT_ASSERT_MESSAGE("Non Transparent Scene should not contain Foreground", 
      !(dtUtil::NodeMask::NON_TRANSPARENT_SCENE_CULL_MASK.GetNodeMask() &
       dtUtil::NodeMask::FOREGROUND.GetNodeMask()));

   CPPUNIT_ASSERT_MESSAGE("Non Transparent Scene should contain Background", 
      (dtUtil::NodeMask::NON_TRANSPARENT_SCENE_CULL_MASK.GetNodeMask() &
       dtUtil::NodeMask::BACKGROUND.GetNodeMask()));

   CPPUNIT_ASSERT_MESSAGE("Non Transparent Scene should not contain the Foreground", 
      !(dtUtil::NodeMask::NON_TRANSPARENT_SCENE_CULL_MASK.GetNodeMask() &
      dtUtil::NodeMask::FOREGROUND.GetNodeMask()));

   CPPUNIT_ASSERT_MESSAGE("Non Transparent Scene should not contain the Water", 
      !(dtUtil::NodeMask::NON_TRANSPARENT_SCENE_CULL_MASK.GetNodeMask() &
      dtUtil::NodeMask::WATER.GetNodeMask()));

   CPPUNIT_ASSERT_MESSAGE("Non Transparent Scene should not contain the Transparent Effects", 
      !(dtUtil::NodeMask::NON_TRANSPARENT_SCENE_CULL_MASK.GetNodeMask() &
      dtUtil::NodeMask::TRANSPARENT_EFFECTS.GetNodeMask()));

   CPPUNIT_ASSERT_MESSAGE("Non Transparent Scene should not contain the Transparent Geometry", 
      !(dtUtil::NodeMask::NON_TRANSPARENT_SCENE_CULL_MASK.GetNodeMask() &
      dtUtil::NodeMask::TRANSPARENT_GEOMETRY.GetNodeMask()));

   CPPUNIT_ASSERT_MESSAGE("Transparent Scene should contain the Transparent Geometry", 
      (dtUtil::NodeMask::TRANSPARENT_ONLY_CULL_MASK.GetNodeMask() &
      dtUtil::NodeMask::TRANSPARENT_GEOMETRY.GetNodeMask()));

   CPPUNIT_ASSERT_MESSAGE("Transparent Scene should contain the Transparent Effects", 
      (dtUtil::NodeMask::TRANSPARENT_ONLY_CULL_MASK.GetNodeMask() &
      dtUtil::NodeMask::TRANSPARENT_EFFECTS.GetNodeMask()));

   CPPUNIT_ASSERT_MESSAGE("Transparent Scene should contain Water", 
      (dtUtil::NodeMask::TRANSPARENT_ONLY_CULL_MASK.GetNodeMask() &
      dtUtil::NodeMask::WATER.GetNodeMask()));

   CPPUNIT_ASSERT_MESSAGE("Transparent Scene should not contain Background", 
      !(dtUtil::NodeMask::TRANSPARENT_ONLY_CULL_MASK.GetNodeMask() &
      dtUtil::NodeMask::BACKGROUND.GetNodeMask()));

   CPPUNIT_ASSERT_MESSAGE("Transparent Scene should not contain Foreground", 
      !(dtUtil::NodeMask::TRANSPARENT_ONLY_CULL_MASK.GetNodeMask() &
      dtUtil::NodeMask::FOREGROUND.GetNodeMask()));

   CPPUNIT_ASSERT_MESSAGE("Transparent Scene should not contain Non Transparent Geometry", 
      !(dtUtil::NodeMask::TRANSPARENT_ONLY_CULL_MASK.GetNodeMask() &
      dtUtil::NodeMask::NON_TRANSPARENT_GEOMETRY.GetNodeMask()));

   CPPUNIT_ASSERT_MESSAGE("Transparent Scene should not contain Background", 
      !(dtUtil::NodeMask::TRANSPARENT_ONLY_CULL_MASK.GetNodeMask() &
      dtUtil::NodeMask::BACKGROUND.GetNodeMask()));


   CPPUNIT_ASSERT_MESSAGE("Everything should include the background scene", 
      dtUtil::NodeMask::BACKGROUND.GetNodeMask() & 
      dtUtil::NodeMask::EVERYTHING);

}

} //namespace dtUtil
