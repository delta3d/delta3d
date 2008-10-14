/* -*-c++-*-
 * testBumpMap - bumpmapdrawable (.h & .cpp) - Using 'The MIT License'
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
 */

#ifndef __BUMP_MAP_DRAWABLE_H__
#define __BUMP_MAP_DRAWABLE_H__

#include <osg/Group>
#include <osg/PositionAttitudeTransform>
#include <osg/Drawable>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Image>
#include <osg/Texture2D>
#include <osg/PrimitiveSet>
#include <osg/Program>
#include <osg/Uniform>
#include <osg/Vec3>
#include <osg/Vec4>

#include <dtCore/deltadrawable.h>

using namespace dtCore;

class BumpMapDrawable: public DeltaDrawable
{

public:
   BumpMapDrawable();
   ~BumpMapDrawable();

   void Initialize();

   void SetUniforms(const osg::Vec3& pLightPos, const osg::Vec3& pEyePos);
   void SetWireframe(bool pWireframe);


   ///required by DeltaDrawable
   osg::Node* GetOSGNode() { return mNode.get(); }
   const osg::Node* GetOSGNode() const { return mNode.get(); }

private:

   void CreateGeometry();
   void EnableShaders();

   RefPtr<osg::Node>     mNode;
   RefPtr<osg::Geometry> mGeometry;
   RefPtr<osg::Geode>    mGeode;

   RefPtr<osg::Program>  mProg;
   RefPtr<osg::Uniform>  mLightPos;
   RefPtr<osg::Uniform>  mEyePos;

};

#endif //__BUMP_MAP_DRAWABLE_H__
