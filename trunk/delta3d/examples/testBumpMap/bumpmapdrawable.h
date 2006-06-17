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
#include <osgDB/ReadFile>   
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
   osg::Node* GetOSGNode(){return mNode.get();}
   const osg::Node* GetOSGNode() const{return mNode.get();}

private:

   void CreateGeometry();
   void EnableShaders();

   RefPtr<osg::Node>                         mNode;
   RefPtr<osg::Geometry>                     mGeometry;
   RefPtr<osg::Geode>		                  mGeode;

   RefPtr<osg::Program>                      mProg; 
   RefPtr<osg::Uniform>                      mLightPos; 
   RefPtr<osg::Uniform>                      mEyePos;

};

#endif //__BUMP_MAP_DRAWABLE_H__
