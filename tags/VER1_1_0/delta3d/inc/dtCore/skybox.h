/* 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2004-2005 MOVES Institute 
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
*/

#ifndef DELTA_SKYBOX
#define DELTA_SKYBOX

#include "dtCore/enveffect.h"
#include "dtCore/skydome.h"
#include "dtCore/refptr.h"

#include <osg/Vec3>
#include <osg/Texture2D>
#include <dtUtil/deprecationmgr.h>

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
   box->SetTexture( SkyBox::SKYBOX_FRONT, "front.bmp");
   box->SetTexture( SkyBox::SKYBOX_BACK, "back.bmp");
   ...
   Environment *env = new Environment("myEnv");
   env->AddEffect( box );
   * \endcode
   *
   * Typically, this would be the only environmental effect added to the
   * Environment.  Adding fog, sun, and clouds typically make things look a 
   * little weird.
   */
class DT_CORE_EXPORT SkyBox :  public EnvEffect
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

   //virtual osg::Group *GetNode(void) {return mNode.get();}

   /// Must override this to supply the repainting routine
   virtual void Repaint(osg::Vec3 skyColor, osg::Vec3 fogColor,
      double sunAngle, double sunAzimuth,
      double visibility);

   virtual void OnMessage(MessageData *data);

   /// Set the texture for this side of the skybox
   void SetTexture(SkyBoxSideEnum side, std::string filename);


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
   //dtCore::RefPtr<osg::Group> mNode;
   dtCore::RefPtr<osg::Geode> mGeode;
   void Config(void);
   osg::Node* MakeBox(void);
   //std::map<short, std::string> mTextureFilenameMap; ///<maps side to filename
   osg::ref_ptr<osg::Texture2D> mTextureList[6];
};

}

#endif // DELTA_SKYBOX
