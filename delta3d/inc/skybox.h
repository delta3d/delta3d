#pragma once
#include "enveffect.h"
#include "skydome.h"
namespace dtCore
{

   ///A six-sided textured cube to represent distant scenery

   /** The SkyBox is an Environmental Effect that is very useful in certain
   * applications.  Essentially, its a large 6-sided cube with the Camera
   * centered in the middle.  Each side of the cube has a texture applied to
   * it representing far away scenery (sky, clouds, buildings, hills, etc.).
   * To use this class, create an instance of it, pass in the texture filenames
   * using SetTextureFilename(),
   * then add it to a Environment using Environment::AddEffect().  The terms
   * "front", "right", etc. map to compass headings.  For example, front is 
   * north, right is east, top is up.
   *
   * The resolution of the textures should be based on the screen resolution
   * and the field of view ( texRes = screenRes / tan(fov/2) ) for a
   * texel per pixel correlation.
   * \code
   SkyBox *box = new SkyBox("skyBox");
   box->SetTextureFilename( SkyBox::SKYBOX_FRONT, "front.bmp");
   box->SetTextureFilename( SkyBox::SKYBOX_BACK, "back.bmp");
   ...
   Environment *env = new Environment("myEnv");
   env->AddEffect( box );
   * \endcode
   *
   * Typically, this would be the only environmental effect added to the
   * Environment.  Adding fog, sun, and clouds typically make things look a 
   * little weird.
   */
class DT_EXPORT SkyBox :  public EnvEffect
{
   DECLARE_MANAGEMENT_LAYER(SkyBox)

public:
   SkyBox(std::string name="SkyBox");
   virtual ~SkyBox(void);

   typedef enum   {
      SKYBOX_FRONT = 0,
      SKYBOX_RIGHT,
      SKYBOX_BACK,      
      SKYBOX_LEFT,
      SKYBOX_TOP,
      SKYBOX_BOTTOM
   } SkyBoxSideEnum;

   virtual osg::Group *GetNode(void) {return mNode.get();}

   /// Must override this to supply the repainting routine
   virtual void Repaint(sgVec3 skyColor, sgVec3 fogColor,
      double sunAngle, double sunAzimuth,
      double visibility);

   virtual void OnMessage(MessageData *data);

   /// Supply the filename of the texture to load for this side
   void SetTextureFilename(SkyBoxSideEnum side, std::string filename);

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
           // matrix.preMult(osg::Matrix::rotate(osg::DegreesToRadians(-mAzimuth-90.f), 0.f, 0.f, 1.f));
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
   
   MoveEarthySkyWithEyePointTransform *mXform;
   osg::ref_ptr<osg::Group> mNode;
   osg::ref_ptr<osg::Geode> mGeode;
   void Config(void);
   osg::Node* MakeBox(void);
   std::map<short, std::string> mTextureFilenameMap; ///<maps side to filename
};

}
