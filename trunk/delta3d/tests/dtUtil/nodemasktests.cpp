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
#include <dtUtil//cullmask.h>
#include <dtUtil//bits.h>

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
   bool ContainsBits(unsigned int mask, unsigned int bits);      

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
bool NodeMaskTests::ContainsBits(unsigned int mask, unsigned int bits)
{
   return dtUtil::Bits::Has(mask, bits);
}

///////////////////////////////////////////////////////////////////////////////
void NodeMaskTests::TestNodeMask()
{

   //everything or nothing
   CPPUNIT_ASSERT_MESSAGE("Everything and nothing should return false",
      !(dtUtil::NodeMask::NOTHING.GetNodeMask() & 
      dtUtil::NodeMask::EVERYTHING.GetNodeMask()));

   //everything
   CPPUNIT_ASSERT_MESSAGE("Everything should contain Background.",
      ContainsBits(dtUtil::NodeMask::EVERYTHING.GetNodeMask(),
      dtUtil::NodeMask::BACKGROUND.GetNodeMask()));

   CPPUNIT_ASSERT_MESSAGE("Everything should contain Foreground.",
      ContainsBits(dtUtil::NodeMask::EVERYTHING.GetNodeMask(),
      dtUtil::NodeMask::FOREGROUND.GetNodeMask()));

   CPPUNIT_ASSERT_MESSAGE("Everything should contain Transparent Effects",
      ContainsBits(dtUtil::NodeMask::EVERYTHING.GetNodeMask(),
      dtUtil::NodeMask::TRANSPARENT_EFFECTS.GetNodeMask()));

   CPPUNIT_ASSERT_MESSAGE("Everything should contain Water.",
      ContainsBits(dtUtil::NodeMask::EVERYTHING.GetNodeMask(),
      dtUtil::NodeMask::WATER.GetNodeMask()));

   CPPUNIT_ASSERT_MESSAGE("Everything should contain Terrain.",
      ContainsBits(dtUtil::NodeMask::EVERYTHING.GetNodeMask(),
      dtUtil::NodeMask::TRANSPARENT_GEOMETRY.GetNodeMask()));

   CPPUNIT_ASSERT_MESSAGE("Everything should contain DEFAULT_GEOMETRY",
      ContainsBits(dtUtil::NodeMask::EVERYTHING.GetNodeMask(),
      dtUtil::NodeMask::DEFAULT_GEOMETRY.GetNodeMask()));


   CPPUNIT_ASSERT_MESSAGE("Background should not contain Foreground",
      !ContainsBits(dtUtil::NodeMask::BACKGROUND.GetNodeMask(),
      dtUtil::NodeMask::FOREGROUND.GetNodeMask()));
   
   CPPUNIT_ASSERT_MESSAGE("Foreground should not contain Transparent Effects", 
      !ContainsBits(dtUtil::NodeMask::FOREGROUND, 
      dtUtil::NodeMask::TRANSPARENT_EFFECTS.GetNodeMask()));
   
   CPPUNIT_ASSERT_MESSAGE("Transparent Effects should not should contatin Water", 
      !ContainsBits(dtUtil::NodeMask::TRANSPARENT_EFFECTS,
      dtUtil::NodeMask::WATER.GetNodeMask()));

   //IGNORE_RAYCAST

   CPPUNIT_ASSERT_MESSAGE("Background should ignore raycast", 
      !ContainsBits(dtUtil::CullMask::SCENE_INTERSECT_MASK,
      dtUtil::NodeMask::BACKGROUND.GetNodeMask()));

   CPPUNIT_ASSERT_MESSAGE("Foreground should ignore raycast", 
      !ContainsBits(dtUtil::CullMask::SCENE_INTERSECT_MASK,
      dtUtil::NodeMask::FOREGROUND.GetNodeMask()));

   CPPUNIT_ASSERT_MESSAGE("Transparent effects should ignore raycast", 
      !ContainsBits(dtUtil::CullMask::SCENE_INTERSECT_MASK,
      dtUtil::NodeMask::TRANSPARENT_EFFECTS));

   CPPUNIT_ASSERT_MESSAGE("Water should ignore raycast", 
      !ContainsBits(dtUtil::CullMask::SCENE_INTERSECT_MASK,
      dtUtil::NodeMask::WATER));

   CPPUNIT_ASSERT_MESSAGE("Terrain features should not ignore raycast", 
      ContainsBits(dtUtil::CullMask::SCENE_INTERSECT_MASK,
      dtUtil::NodeMask::TRANSPARENT_GEOMETRY));

   CPPUNIT_ASSERT_MESSAGE("Default geometry should not ignore raycast", 
      ContainsBits(dtUtil::CullMask::SCENE_INTERSECT_MASK,
      dtUtil::NodeMask::DEFAULT_GEOMETRY));

   CPPUNIT_ASSERT_MESSAGE("Non Transparent geometry should not ignore raycast", 
      ContainsBits(dtUtil::CullMask::SCENE_INTERSECT_MASK,
      dtUtil::NodeMask::NON_TRANSPARENT_GEOMETRY));

   CPPUNIT_ASSERT_MESSAGE("Transparent geometry should not ignore raycast", 
      ContainsBits(dtUtil::CullMask::SCENE_INTERSECT_MASK,
      dtUtil::NodeMask::TRANSPARENT_GEOMETRY));
  
   //default geometry
   CPPUNIT_ASSERT_MESSAGE("Default geometry should contain non transparent geometry", 
      ContainsBits(dtUtil::NodeMask::DEFAULT_GEOMETRY,
      dtUtil::NodeMask::NON_TRANSPARENT_GEOMETRY));

   CPPUNIT_ASSERT_MESSAGE("Default geometry should contain transparent geometry", 
      ContainsBits(dtUtil::NodeMask::DEFAULT_GEOMETRY,
      dtUtil::NodeMask::TRANSPARENT_GEOMETRY));

   //transparent - non- transparent
   CPPUNIT_ASSERT_MESSAGE("Transparent and non transparent should not overlap", 
      !ContainsBits(dtUtil::NodeMask::NON_TRANSPARENT_GEOMETRY,
      dtUtil::NodeMask::TRANSPARENT_GEOMETRY));


   //cull mask, single pass


   CPPUNIT_ASSERT_MESSAGE("Main camera mask should contain PreProcess", 
      ContainsBits(dtUtil::CullMask::MAIN_CAMERA_SINGLEPASS,
      dtUtil::NodeMask::PRE_PROCESS));

   CPPUNIT_ASSERT_MESSAGE("Main camera mask should contain Background", 
      ContainsBits(dtUtil::CullMask::MAIN_CAMERA_SINGLEPASS,
      dtUtil::NodeMask::BACKGROUND));

   CPPUNIT_ASSERT_MESSAGE("Main camera mask should contain Terrain", 
      ContainsBits(dtUtil::CullMask::MAIN_CAMERA_SINGLEPASS,
      dtUtil::NodeMask::TERRAIN_GEOMETRY));

   CPPUNIT_ASSERT_MESSAGE("Main camera mask should contain Water", 
      ContainsBits(dtUtil::CullMask::MAIN_CAMERA_SINGLEPASS,
      dtUtil::NodeMask::WATER));

   CPPUNIT_ASSERT_MESSAGE("Main camera mask should contain Transparent Effects", 
      ContainsBits(dtUtil::CullMask::MAIN_CAMERA_SINGLEPASS,
      dtUtil::NodeMask::TRANSPARENT_EFFECTS));

   CPPUNIT_ASSERT_MESSAGE("Main camera mask should contain Volumetric Effects", 
      ContainsBits(dtUtil::CullMask::MAIN_CAMERA_SINGLEPASS,
      dtUtil::NodeMask::VOLUMETRIC_EFFECTS));

   CPPUNIT_ASSERT_MESSAGE("Main camera mask should contain Non Transparent Geometry", 
      ContainsBits(dtUtil::CullMask::MAIN_CAMERA_SINGLEPASS,
      dtUtil::NodeMask::NON_TRANSPARENT_GEOMETRY));

   CPPUNIT_ASSERT_MESSAGE("Main camera mask should contain Default Geometry", 
      ContainsBits(dtUtil::CullMask::MAIN_CAMERA_SINGLEPASS,
      dtUtil::NodeMask::DEFAULT_GEOMETRY));

   CPPUNIT_ASSERT_MESSAGE("Main camera mask should contain Transparent Geometry", 
      ContainsBits(dtUtil::CullMask::MAIN_CAMERA_SINGLEPASS,
      dtUtil::NodeMask::TRANSPARENT_GEOMETRY));

   CPPUNIT_ASSERT_MESSAGE("Main camera mask should contain Foreground", 
      ContainsBits(dtUtil::CullMask::MAIN_CAMERA_SINGLEPASS,
      dtUtil::NodeMask::FOREGROUND));


   CPPUNIT_ASSERT_MESSAGE("Main camera mask should contain Post Process", 
      ContainsBits(dtUtil::CullMask::MAIN_CAMERA_SINGLEPASS,
      dtUtil::NodeMask::POST_PROCESS));


   CPPUNIT_ASSERT_MESSAGE("Additional camera mask should not contain Foreground", 
      !ContainsBits(dtUtil::CullMask::ADDITIONAL_CAMERA_SINGLEPASS,
       dtUtil::NodeMask::FOREGROUND));

   
   //cull mask, multi pass, main camera

   CPPUNIT_ASSERT_MESSAGE("Main camera multipass mask should contain PreProcess", 
      ContainsBits(dtUtil::CullMask::MAIN_CAMERA_MULTIPASS,
      dtUtil::NodeMask::PRE_PROCESS));

   CPPUNIT_ASSERT_MESSAGE("Main camera multipass mask should contain Multipass", 
      ContainsBits(dtUtil::CullMask::MAIN_CAMERA_MULTIPASS,
      dtUtil::NodeMask::MULTIPASS));

   CPPUNIT_ASSERT_MESSAGE("Main camera multipass mask should not contain Background", 
      !ContainsBits(dtUtil::CullMask::MAIN_CAMERA_MULTIPASS,
      dtUtil::NodeMask::BACKGROUND));

   CPPUNIT_ASSERT_MESSAGE("Main camera multipass mask should not contain Terrain", 
      !ContainsBits(dtUtil::CullMask::MAIN_CAMERA_MULTIPASS,
      dtUtil::NodeMask::TERRAIN_GEOMETRY));

   CPPUNIT_ASSERT_MESSAGE("Main camera multipass mask should not contain Water", 
      !ContainsBits(dtUtil::CullMask::MAIN_CAMERA_MULTIPASS,
      dtUtil::NodeMask::WATER));

   CPPUNIT_ASSERT_MESSAGE("Main camera multipass mask should not contain Transparent Effects", 
      !ContainsBits(dtUtil::CullMask::MAIN_CAMERA_MULTIPASS,
      dtUtil::NodeMask::TRANSPARENT_EFFECTS));

   CPPUNIT_ASSERT_MESSAGE("Main camera multipass mask should not contain Volumetric Effects", 
      !ContainsBits(dtUtil::CullMask::MAIN_CAMERA_MULTIPASS,
      dtUtil::NodeMask::VOLUMETRIC_EFFECTS));

   CPPUNIT_ASSERT_MESSAGE("Main camera multipass mask should not contain Non Transparent Geometry", 
      !ContainsBits(dtUtil::CullMask::MAIN_CAMERA_MULTIPASS,
      dtUtil::NodeMask::NON_TRANSPARENT_GEOMETRY));

   CPPUNIT_ASSERT_MESSAGE("Main camera multipass mask should not contain Default Geometry", 
      !ContainsBits(dtUtil::CullMask::MAIN_CAMERA_MULTIPASS,
      dtUtil::NodeMask::DEFAULT_GEOMETRY));

   CPPUNIT_ASSERT_MESSAGE("Main camera multipass mask should contain not Transparent Geometry", 
      !ContainsBits(dtUtil::CullMask::MAIN_CAMERA_MULTIPASS,
      dtUtil::NodeMask::TRANSPARENT_GEOMETRY));

   CPPUNIT_ASSERT_MESSAGE("Main camera multipass mask should contain Foreground", 
      ContainsBits(dtUtil::CullMask::MAIN_CAMERA_MULTIPASS,
      dtUtil::NodeMask::FOREGROUND));

   CPPUNIT_ASSERT_MESSAGE("Main camera multipass mask should contain Post Process", 
      ContainsBits(dtUtil::CullMask::MAIN_CAMERA_MULTIPASS,
      dtUtil::NodeMask::POST_PROCESS));


   //cull masks additional camera multipass

   //cull mask, multi pass, main camera

   CPPUNIT_ASSERT_MESSAGE("Additional camera multipass mask should not contain PreProcess", 
      !ContainsBits(dtUtil::CullMask::ADDITIONAL_CAMERA_MULTIPASS,
      dtUtil::NodeMask::PRE_PROCESS));

   CPPUNIT_ASSERT_MESSAGE("Additional camera multipass mask should not contain Multipass", 
      !ContainsBits(dtUtil::CullMask::ADDITIONAL_CAMERA_MULTIPASS,
      dtUtil::NodeMask::MULTIPASS));

   CPPUNIT_ASSERT_MESSAGE("Additional camera multipass mask should contain Background", 
      ContainsBits(dtUtil::CullMask::ADDITIONAL_CAMERA_MULTIPASS,
      dtUtil::NodeMask::BACKGROUND));

   CPPUNIT_ASSERT_MESSAGE("Additional camera multipass mask should contain Terrain", 
      ContainsBits(dtUtil::CullMask::ADDITIONAL_CAMERA_MULTIPASS,
      dtUtil::NodeMask::TERRAIN_GEOMETRY));

   CPPUNIT_ASSERT_MESSAGE("Additional camera multipass mask should contain Water", 
      ContainsBits(dtUtil::CullMask::ADDITIONAL_CAMERA_MULTIPASS,
      dtUtil::NodeMask::WATER));

   CPPUNIT_ASSERT_MESSAGE("Additional camera multipass mask should contain Transparent Effects", 
      ContainsBits(dtUtil::CullMask::ADDITIONAL_CAMERA_MULTIPASS,
      dtUtil::NodeMask::TRANSPARENT_EFFECTS));

   CPPUNIT_ASSERT_MESSAGE("Additional camera multipass mask should contain Volumetric Effects", 
      ContainsBits(dtUtil::CullMask::ADDITIONAL_CAMERA_MULTIPASS,
      dtUtil::NodeMask::VOLUMETRIC_EFFECTS));

   CPPUNIT_ASSERT_MESSAGE("Additional camera multipass mask should contain Non Transparent Geometry", 
      ContainsBits(dtUtil::CullMask::ADDITIONAL_CAMERA_MULTIPASS,
      dtUtil::NodeMask::NON_TRANSPARENT_GEOMETRY));

   CPPUNIT_ASSERT_MESSAGE("Additional camera multipass mask should contain Default Geometry", 
      ContainsBits(dtUtil::CullMask::ADDITIONAL_CAMERA_MULTIPASS,
      dtUtil::NodeMask::DEFAULT_GEOMETRY));

   CPPUNIT_ASSERT_MESSAGE("Additional camera multipass mask should contain not Transparent Geometry", 
      ContainsBits(dtUtil::CullMask::ADDITIONAL_CAMERA_MULTIPASS,
      dtUtil::NodeMask::TRANSPARENT_GEOMETRY));

   CPPUNIT_ASSERT_MESSAGE("Additional camera multipass mask should not contain Foreground", 
      !ContainsBits(dtUtil::CullMask::ADDITIONAL_CAMERA_MULTIPASS,
      dtUtil::NodeMask::FOREGROUND));

   CPPUNIT_ASSERT_MESSAGE("Additional camera multipass mask should nost contain Post Process", 
      !ContainsBits(dtUtil::CullMask::ADDITIONAL_CAMERA_MULTIPASS,
      dtUtil::NodeMask::POST_PROCESS));


   //cull masks and non transparent masks
   
   CPPUNIT_ASSERT_MESSAGE("Non Transparent Scene should not contain the pre process scene", 
      !ContainsBits(dtUtil::CullMask::MULTIPASS_NON_TRANSPARENT_MASK,
      dtUtil::NodeMask::PRE_PROCESS));

   CPPUNIT_ASSERT_MESSAGE("Non Transparent Scene should not contain the post processs scene", 
      !ContainsBits(dtUtil::CullMask::MULTIPASS_NON_TRANSPARENT_MASK,
      dtUtil::NodeMask::POST_PROCESS));

   CPPUNIT_ASSERT_MESSAGE("Non Transparent Scene should not contain Foreground", 
      !ContainsBits(dtUtil::CullMask::MULTIPASS_NON_TRANSPARENT_MASK,
       dtUtil::NodeMask::FOREGROUND));

   CPPUNIT_ASSERT_MESSAGE("Non Transparent Scene should not contain the Multipass scene", 
      !ContainsBits(dtUtil::CullMask::MULTIPASS_NON_TRANSPARENT_MASK,
      dtUtil::NodeMask::MULTIPASS));


   CPPUNIT_ASSERT_MESSAGE("Non Transparent Scene should contain Background", 
      ContainsBits(dtUtil::CullMask::MULTIPASS_NON_TRANSPARENT_MASK,
       dtUtil::NodeMask::BACKGROUND));
  
   CPPUNIT_ASSERT_MESSAGE("Non Transparent Scene should not contain the Water", 
      !ContainsBits(dtUtil::CullMask::MULTIPASS_NON_TRANSPARENT_MASK,
      dtUtil::NodeMask::WATER));

   CPPUNIT_ASSERT_MESSAGE("Non Transparent Scene should not contain the Volumetric Effects", 
      !ContainsBits(dtUtil::CullMask::MULTIPASS_NON_TRANSPARENT_MASK,
      dtUtil::NodeMask::VOLUMETRIC_EFFECTS));

   CPPUNIT_ASSERT_MESSAGE("Non Transparent Scene should not contain the Transparent Effects", 
      !ContainsBits(dtUtil::CullMask::MULTIPASS_NON_TRANSPARENT_MASK,
      dtUtil::NodeMask::TRANSPARENT_EFFECTS));

   CPPUNIT_ASSERT_MESSAGE("Non Transparent Scene should not contain the Transparent Geometry", 
      !ContainsBits(dtUtil::CullMask::MULTIPASS_NON_TRANSPARENT_MASK,
      dtUtil::NodeMask::TRANSPARENT_GEOMETRY));

   CPPUNIT_ASSERT_MESSAGE("Non Transparent Scene should contain the Non Transparent Geometry", 
      ContainsBits(dtUtil::CullMask::MULTIPASS_NON_TRANSPARENT_MASK,
      dtUtil::NodeMask::NON_TRANSPARENT_GEOMETRY));

   CPPUNIT_ASSERT_MESSAGE("Non Transparent Scene should contain the Terrain Geometry", 
      ContainsBits(dtUtil::CullMask::MULTIPASS_NON_TRANSPARENT_MASK,
      dtUtil::NodeMask::TERRAIN_GEOMETRY));
   

   //transparent mask

   CPPUNIT_ASSERT_MESSAGE("Transparent Scene should not contain Background", 
      !ContainsBits(dtUtil::CullMask::MULTIPASS_TRANSPARENT_MASK,
      dtUtil::NodeMask::BACKGROUND));

   CPPUNIT_ASSERT_MESSAGE("Transparent Scene should not contain Foreground", 
      !ContainsBits(dtUtil::CullMask::MULTIPASS_TRANSPARENT_MASK,
      dtUtil::NodeMask::FOREGROUND));

   CPPUNIT_ASSERT_MESSAGE("Transparent Scene should not contain Pre Process", 
      !ContainsBits(dtUtil::CullMask::MULTIPASS_TRANSPARENT_MASK,
      dtUtil::NodeMask::PRE_PROCESS));
   
   CPPUNIT_ASSERT_MESSAGE("Transparent Scene should not contain Post Process", 
      !ContainsBits(dtUtil::CullMask::MULTIPASS_TRANSPARENT_MASK,
      dtUtil::NodeMask::POST_PROCESS));

   CPPUNIT_ASSERT_MESSAGE("Transparent Scene should not contain Multipass", 
      !ContainsBits(dtUtil::CullMask::MULTIPASS_TRANSPARENT_MASK,
      dtUtil::NodeMask::MULTIPASS));


   CPPUNIT_ASSERT_MESSAGE("Transparent Scene should contain the Transparent Geometry", 
      ContainsBits(dtUtil::CullMask::MULTIPASS_TRANSPARENT_MASK,
      dtUtil::NodeMask::TRANSPARENT_GEOMETRY));

   CPPUNIT_ASSERT_MESSAGE("Transparent Scene should contain the Transparent Effects", 
      ContainsBits(dtUtil::CullMask::MULTIPASS_TRANSPARENT_MASK,
      dtUtil::NodeMask::TRANSPARENT_EFFECTS));

   CPPUNIT_ASSERT_MESSAGE("Transparent Scene should contain Water", 
      ContainsBits(dtUtil::CullMask::MULTIPASS_TRANSPARENT_MASK,
      dtUtil::NodeMask::WATER));

   CPPUNIT_ASSERT_MESSAGE("Transparent Scene should contain Volumetric Effects", 
      ContainsBits(dtUtil::CullMask::MULTIPASS_TRANSPARENT_MASK,
      dtUtil::NodeMask::VOLUMETRIC_EFFECTS));

   CPPUNIT_ASSERT_MESSAGE("Transparent Scene should not contain Terrain", 
      !ContainsBits(dtUtil::CullMask::MULTIPASS_TRANSPARENT_MASK,
      dtUtil::NodeMask::TERRAIN_GEOMETRY));

   CPPUNIT_ASSERT_MESSAGE("Transparent Scene should not contain Non Transparent Geometry", 
      !ContainsBits(dtUtil::CullMask::MULTIPASS_TRANSPARENT_MASK,
      dtUtil::NodeMask::NON_TRANSPARENT_GEOMETRY));

}

} //namespace dtUtil
