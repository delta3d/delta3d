#include "dtCore/tripod.h"
#include "dtCore/system.h"
#include "dtCore/camera.h"

using namespace dtCore;

IMPLEMENT_MANAGEMENT_LAYER(Tripod)

Tripod::Tripod(Camera *cam, Transformable *trans):
mTetherMode(TETHER_PARENT_REL)
{
   RegisterInstance(this);
   
   AddSender(System::GetSystem());
   if (cam != NULL) SetCamera(cam);
   SetAttachToTransformable(trans);

   sgSetCoord(&mOffsetCoord, 0.f, -10.f, 2.f, 0.f, 0.f, 0.f);
   sgSetVec3(mXYZScale, 1.f, 1.f, 1.f);
   sgSetVec3(mHPRScale, 1.f, 1.f, 1.f);
}

Tripod::~Tripod(void)
{
    DeregisterInstance(this);
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
   sgVec3 xyz = {x,y,z};
   sgVec3 hpr = {h,p,r};
   SetScale(xyz, hpr);
}

/** Must be in the range (0.0,1.0)
*/
void Tripod::SetScale(sgVec3 xyz, sgVec3 hpr)
{
   sgCopyVec3(mXYZScale, xyz);
   sgCopyVec3(mHPRScale, hpr);
}

void Tripod::GetScale(sgVec3 xyz, sgVec3 hpr)
{
   sgCopyVec3(xyz, mXYZScale);
   sgCopyVec3(hpr, mHPRScale);
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
   sgMat4 parentMat;
   parentXform.Get(parentMat);

   Transform currXform;
   mCamera->GetTransform(&currXform);
   sgMat4 currMat;
   currXform.Get(currMat);

   sgMat4 newMat;
   sgMat4 offsetMat;
   sgMakeCoordMat4(offsetMat, &mOffsetCoord);

   switch(mTetherMode)
   {
   case TETHER_PARENT_REL:
      {
         //transform offset through parent's matrix to get new abs coord
         sgMultMat4(newMat, parentMat, offsetMat);
      }
      break;
   case TETHER_WORLD_REL:
      {
         sgCopyMat4(newMat, offsetMat);
         sgAddVec3(newMat[3], offsetMat[3], parentMat[3]);
      }
      break;
   default: break;
   }


   //set the lookat
   if (mLookAtTarget.valid())
   {
      Transform targetXform, lookatXform;
      mLookAtTarget.get()->GetTransform(&targetXform);
      sgVec3 lookAtXYZ, upVec;
      sgSetVec3(upVec, 0.f, 0.f, 1.f);
      targetXform.GetTranslation(lookAtXYZ);
      lookatXform.SetLookAt(newMat[3], lookAtXYZ, upVec);
      lookatXform.Get(newMat);
   }

   const sgVec3 ident = {1.f, 1.f, 1.f};
   if (!sgEqualVec3(mXYZScale,ident))
   {
      //adjust the new xyz using the xyzScale values
      sgVec3 xyzDiff;
      sgSubVec3(xyzDiff, newMat[3], currMat[3]);
      xyzDiff[0] *= (mXYZScale[0]*deltaFrameTime)/deltaFrameTime;
      xyzDiff[1] *= (mXYZScale[1]*deltaFrameTime)/deltaFrameTime;
      xyzDiff[2] *= (mXYZScale[2]*deltaFrameTime)/deltaFrameTime;
      sgAddVec3(newMat[3], xyzDiff, currMat[3]);
   }

   if (!sgEqualVec3(mHPRScale, ident))
   {
      sgLerpAnglesVec3 ( newMat[0], currMat[0], newMat[0], mHPRScale[0]);
      sgLerpAnglesVec3 ( newMat[1], currMat[1], newMat[1], mHPRScale[1]);
      sgLerpAnglesVec3 ( newMat[2], currMat[2], newMat[2], mHPRScale[2]);
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
