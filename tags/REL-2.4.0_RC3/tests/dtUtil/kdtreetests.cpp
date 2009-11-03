/* -*-c++-*-
* allTests - This source file (.h & .cpp) - Using 'The MIT License'
* Copyright (C) 2006-2008, MOVES Institute
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
* Bradley Anderegg
*/
#include <prefix/dtgameprefix-src.h>
#include <cppunit/extensions/HelperMacros.h>
#include <dtUtil/kdtree.h>
#include <dtCore/refptr.h>
#include <osg/Vec3>

namespace dtUtil
{
   struct KDHolder
   {
      typedef float value_type;

      KDHolder(const osg::Vec3& pos)
      {
         d[0] = pos[0];
         d[1] = pos[1];
         d[2] = pos[2];
      }

      KDHolder(const osg::Vec3& pos, int id)
      {
         d[0] = pos[0];
         d[1] = pos[1];
         d[2] = pos[2];
         
         mID = id;
      }

      KDHolder(value_type a, value_type b, value_type c)
      {
         d[0] = a;
         d[1] = b;
         d[2] = c;
      }

      KDHolder(const KDHolder& x)
      {
         d[0] = x.d[0];
         d[1] = x.d[1];
         d[2] = x.d[2];
         mID = x.mID;
      }

      operator osg::Vec3()
      {
         return osg::Vec3(d[0], d[1], d[2]);
      }
      
      inline value_type operator[](size_t const N) const { return d[N]; }

      int mID;
      value_type d[3];      
   };

   inline float tac(KDHolder t, size_t k ) { return t[k]; }

   class KdTreeTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE(KdTreeTests);
         CPPUNIT_TEST( TestAddRemove );
      CPPUNIT_TEST_SUITE_END();

      public:
         void setUp();
         void tearDown();

         void TestAddRemove();

      private:
         typedef dtUtil::KDTree<3, KDHolder, std::pointer_to_binary_function<KDHolder, size_t, float> > TreeType;
         TreeType* mTree;
   };

   // Registers the fixture into the 'registry'
   CPPUNIT_TEST_SUITE_REGISTRATION( KdTreeTests );

   void KdTreeTests::setUp()
   {
      mTree = new TreeType(std::ptr_fun(tac));
   }

   void KdTreeTests::tearDown()
   {
      mTree->clear();
      delete mTree;
   }

   void KdTreeTests::TestAddRemove()
   {
      osg::Vec3 origin(0.0f, 0.0f, 0.0f);
      osg::Vec3 pos(1.0f, 0.0f, 0.0f);

      KDHolder h(pos, 100);      

      mTree->insert(h);
      
      
      typedef std::pair<TreeType::const_iterator, float> find_result;
      find_result found = mTree->find_nearest(origin, 2.0f);

      CPPUNIT_ASSERT(found.first != mTree->end());
      CPPUNIT_ASSERT(found.first->mID == 100);
      
   }

}
