#ifndef DELTA_CLOUDPLANE
#define DELTA_CLOUDPLANE


#include "dtCore/enveffect.h"
#include "sg.h"

#include <osg/Group>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Transform>
#include <osg/Texture2D>
#include <osgUtil/CullVisitor>
#include <osg/Fog>

namespace dtCore
{
   class DT_EXPORT CloudPlane : public dtCore::EnvEffect
   {
   public:
      
       DECLARE_MANAGEMENT_LAYER(CloudPlane)
       CloudPlane(int  octaves,
                 float cutoff,
                 int   frequency,
                 float amp,
                 float persistence,
                 float density,
                 int   texSize,
                 float height,
                 std::string name = "CloudPlane");
      
      ~CloudPlane();

      virtual osg::Group *GetNode(void) {return mNode.get();}

      virtual void Repaint(sgVec4 sky_color, sgVec4 fog_color, 
          double sun_angle, double sunAzimuth,
          double vis);

      float getHeight(void) { return mHeight; };

   private:
       class MoveEarthySkyWithEyePointTransform : public osg::Transform
       {
       public:

           /** Get the transformation matrix which moves from local coords to world coords.*/
           virtual bool computeLocalToWorldMatrix(osg::Matrix& matrix,osg::NodeVisitor* nv) const 
           {
               osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
               if (cv)
               {
                   osg::Vec3 eyePointLocal = cv->getEyeLocal();
                   matrix.preMult(osg::Matrix::translate(eyePointLocal.x(),eyePointLocal.y(),eyePointLocal.z()));
               }
               return true;
           }

           /** Get the transformation matrix which moves from world coords to local coords.*/
           virtual bool computeWorldToLocalMatrix(osg::Matrix& matrix,osg::NodeVisitor* nv) const
           {    
               osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
               if (cv)
               {
                   osg::Vec3 eyePointLocal = cv->getEyeLocal();
                   matrix.postMult(osg::Matrix::translate(-eyePointLocal.x(),-eyePointLocal.y(),-eyePointLocal.z()));
               }
               return true;
           }
       };

       void Create( void );
       osg::Texture2D *createPerlinTexture(void);
       virtual void OnMessage(MessageData *data);
       void Update(const double deltaFrameTime);
       osg::Geometry *CloudPlane::createPlane(float, float);


       osg::ref_ptr<osg::Group> mNode;
       osg::ref_ptr<osg::Geode> mGeode;
       osg::ref_ptr<osg::Geometry> mPlane;
       osg::ref_ptr<osg::Image> mImage;
       osg::ref_ptr<osg::Texture2D> mCloudTexture;
       osg::ref_ptr<osg::Fog> mFog; ///< The fog adjuster
       int mOctaves;
       float mCutoff;
       int   mFrequency;
       float mAmplitude;
       float mPersistence;
       float mDensity;
       float mHeight;
       int   mTexSize;
       sgVec4 mSkyColor;
       osg::Vec2 *mWind;
       osg::Vec4 *mCloudColor;
       osg::Vec2Array *mTexCoords;
       osg::Vec4Array *mColors;
       osg::ref_ptr<MoveEarthySkyWithEyePointTransform> mXform;

   };

}


#endif // DELTA_CLOUDPLANE
