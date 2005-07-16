#include "dtCore/tripod.h"
#include "dtCore/system.h"
#include "dtCore/camera.h"
#include "dtCore/notify.h"
#include "dtCore/transform.h"

#include <osg/Matrix>

using namespace dtCore;

IMPLEMENT_MANAGEMENT_LAYER(Tripod)

Tripod::Tripod(Camera *cam, Transformable *trans):
mTetherMode(TETHER_PARENT_REL)
{
   RegisterInstance(this);
   
   AddSender(System::GetSystem());
   if (cam != NULL) SetCamera(cam);
   SetAttachToTransformable(trans);

   mPosition.set(0.0f, -10.0f, 2.0f);
   mHPR.set(0.0f, 0.0f, 0.0f);
   mXYZScale.set(1.f, 1.f, 1.f);
   mHPRScale.set(1.f, 1.f, 1.f);
}

Tripod::~Tripod(void)
{
   DeregisterInstance(this);
   RemoveSender( System::GetSystem() );    
}

void Tripod::SetCamera(Camera *cam)
{
   mCamera = cam;
}

void Tripod::SetCamera(std::string camName)
{
   if (camName.size()>0)
   {
      Camera *cam = Camera::GetInstance(camName);
      if (cam!=NULL)
      {
         SetCamera(cam);
      }
      else Notify(WARN, "Tripod: Can't find Camera %s", camName.c_str());
   }
}

/** The supplied Transformable will be used as the basis for where the Tripod
  * should be positioned.  The offset values and scale values will then be used
  * to compute the final Camera position/attitude.
  */
void Tripod::SetAttachToTransformable(Transformable *trans)
{
   mParent = trans;
}

void Tripod::SetAttachToTransformable(std::string transName)
{
   if (transName.size()>0)
   {
      Transformable *trans = Transformable::GetInstance(transName);
      if (trans!=NULL)
      {
         SetAttachToTransformable(trans);
      }
      else Notify(WARN, "Tripod: Can't find Transformable %s", transName.c_str());
   }
}

void Tripod::SetOffset(float x, float y, float z, float h, float p, float r)
{
   sgCoord tmp = {x,y,z,h,p,r};
   SetOffset(&tmp);
}

/** Must be in the range (0.0,1.0)
*/
void Tripod::SetScale(float x, float y, float z, float h, float p, float r)
{
   osg::Vec3 xyz(x,y,z);
   osg::Vec3 hpr(h,p,r);
   SetScale(xyz, hpr);
}


///Override to receive messages
void Tripod::OnMessage(MessageData *data)
{
   if (data->message == "preframe")
   {
      Update( *(double*)data->userData );
   }
}

/** Calculates where the Camera should be using the parent Transformable,
  * the delta frame time, the offset values, the scale values, and the curent
  * position/attitude.
  */
void Tripod::Update(double deltaFrameTime) //virtual
{
   //get parent's matrix
   if (!mParent.valid() || !mCamera.valid()) return;

   Transform parentXform;
   mParent->GetTransform(&parentXform);
   osg::Matrix parentMat;
   parentXform.Get(parentMat);

   Transform currXform;
   mCamera->GetTransform(&currXform);
   osg::Matrix currMat;
   currXform.Get(currMat);

   osg::Matrix newMat, offsetMat;
   Transform trans;
   trans.Set(mPosition, mHPR, osg::Vec3(1.0f, 1.0f, 1.0f));
   trans.Get(offsetMat);
      
   switch(mTetherMode)
   {
   case TETHER_PARENT_REL:
      {
         //transform offset through parent's matrix to get new abs coord
         //sgMultMat4(newMat, parentMat, offsetMat);
         newMat = parentMat * offsetMat;
      }
      break;
   case TETHER_WORLD_REL:
      {
         newMat(0,3) = offsetMat(0,3) + parentMat(0,3);
         newMat(1,3) = offsetMat(1,3) + parentMat(1,3);
         newMat(2,3) = offsetMat(2,3) + parentMat(2,3);
      }
      break;
   default: break;
   }


   //set the lookat
   if (mLookAtTarget.valid())
   {
      Transform targetXform, lookatXform;
      mLookAtTarget.get()->GetTransform(&targetXform);
      osg::Vec3 lookAtXYZ, upVec;
      upVec.set(0.f, 0.f, 1.f);
      targetXform.GetTranslation(lookAtXYZ);
      lookatXform.SetLookAt(osg::Vec3(newMat(0,3), newMat(1,3), newMat(2,3)), lookAtXYZ, upVec);
      lookatXform.Get(newMat);
   }

   const osg::Vec3 ident(1.f, 1.f, 1.f);
    if (mXYZScale != ident)
   {
      //adjust the new xyz using the xyzScale values
      osg::Vec3 xyzDiff(newMat(0,3) - currMat(0,3), newMat(1,3) - currMat(1,3), newMat(2,3) - currMat(2,3));
      xyzDiff[0] *= (mXYZScale[0]*deltaFrameTime)/deltaFrameTime;
      xyzDiff[1] *= (mXYZScale[1]*deltaFrameTime)/deltaFrameTime;
      xyzDiff[2] *= (mXYZScale[2]*deltaFrameTime)/deltaFrameTime;
      newMat(0,3) = xyzDiff[0] + currMat(0,3);
      newMat(1,3) = xyzDiff[1] + currMat(1,3);
      newMat(2,3) = xyzDiff[2] + currMat(2,3);
   }

   if (mHPRScale != ident)
   {
      //sgLerpAnglesVec3 ( newMat[0], currMat[0], newMat[0], mHPRScale[0]);
      //sgLerpAnglesVec3 ( newMat[1], currMat[1], newMat[1], mHPRScale[1]);
      //sgLerpAnglesVec3 ( newMat[2], currMat[2], newMat[2], mHPRScale[2]);

      ///\todo- this lerp should not be hard coded
      newMat(0,0) = currMat(0,0) + mHPRScale[0] * (newMat(0,0) - currMat(0,0));
      newMat(1,0) = currMat(1,0) + mHPRScale[0] * (newMat(1,0) - currMat(1,0));
      newMat(2,0) = currMat(2,0) + mHPRScale[0] * (newMat(2,0) - currMat(2,0));

      newMat(0,1) = currMat(0,1) + mHPRScale[1] * (newMat(0,1) - currMat(0,1));
      newMat(1,1) = currMat(1,1) + mHPRScale[1] * (newMat(1,1) - currMat(1,1));
      newMat(2,1) = currMat(2,1) + mHPRScale[1] * (newMat(2,1) - currMat(2,1));

      newMat(0,2) = currMat(0,2) + mHPRScale[2] * (newMat(0,2) - currMat(0,2));
      newMat(1,2) = currMat(1,2) + mHPRScale[2] * (newMat(1,2) - currMat(1,2));
      newMat(2,2) = currMat(2,2) + mHPRScale[2] * (newMat(2,2) - currMat(2,2));
         
   }

   parentXform.Set(newMat);
   mCamera->SetTransform(&parentXform);
}

/** The Camera will always point toward the look-at target if one is supplied.
* the roll of the Camera will always remain fixed (the Camera up pointed toward the sky).
* @param target: The target the Camera should point toward
*/
void Tripod::SetLookAtTarget(Transformable *target)
{
   mLookAtTarget = target;
}
