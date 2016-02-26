#include <prefix/dtcoreprefix.h>
#include <dtCore/tripod.h>

#include <dtCore/camera.h>
#include <dtCore/system.h>
#include <dtCore/transform.h>
#include <dtUtil/log.h>

#include <osg/Matrix>

using namespace dtCore;
using namespace dtUtil;

IMPLEMENT_MANAGEMENT_LAYER(Tripod)

Tripod::Tripod(Transformable* child, Transformable* parent)
: mTetherMode(TETHER_PARENT_REL)
{
   RegisterInstance(this);

   dtCore::System::GetInstance().TickSignal.connect_slot(this, &Tripod::OnSystem);
   SetChild(child);
   SetAttachToTransformable(parent);

   mPosition.set(0.0f, -10.0f, 2.0f);
   mHPR.set(0.0f, 0.0f, 0.0f);
   mXYZScale.set(1.0f, 1.0f, 1.0f);
   mHPRScale.set(1.0f, 1.0f, 1.0f);
}

Tripod::~Tripod(void)
{
   DeregisterInstance(this);
}

void Tripod::SetCamera(Camera* cam)
{
   DEPRECATE("void SetCamera(Camera*)",
             "void SetChild(Transformable*)");

   SetChild(cam);
}

void Tripod::SetCamera(const std::string& camName)
{
   DEPRECATE("void SetCamera(const std::string&)",
             "void SetChild(const std::string&)");

   if (camName.size() > 0)
   {
      Camera* cam = Camera::GetInstance(camName);
      if (cam != NULL)
      {
         SetChild(cam);
      }
      else
      {
         Log::GetInstance().LogMessage(Log::LOG_WARNING, __FILE__,
         "Tripod: Can't find Camera %s", camName.c_str());
      }
   }
}

Camera* Tripod::GetCamera()
{
   DEPRECATE("Camera* GetCamera()",
             "Transformable* GetChild()");

   return dynamic_cast<Camera*>(GetChild());
}

void Tripod::SetChild(Transformable* child)
{
   mChild = child;
}

void Tripod::SetChild(const std::string& childName)
{
   if (childName.size() > 0)
   {
      Transformable *child = Transformable::GetInstance(childName);
      if (child != NULL)
      {
         SetChild(child);
      }
      else
      {
         Log::GetInstance().LogMessage(Log::LOG_WARNING, __FILE__,
         "Tripod: Can't find Transformable %s", childName.c_str());
      }
   }
}

Transformable* Tripod::GetChild()
{
   return mChild.get();
}

/** The supplied Transformable will be used as the basis for where the Tripod
  * should be positioned.  The offset values and scale values will then be used
  * to compute the final Camera position/attitude.
  */
void Tripod::SetAttachToTransformable(Transformable *parent)
{
   mParentTrans = parent;
}

void Tripod::SetAttachToTransformable( const std::string& parentName )
{
   if (parentName.size()>0)
   {
      Transformable *trans = Transformable::GetInstance(parentName);
      if (trans!=NULL)
      {
         SetAttachToTransformable(trans);
      }
      else Log::GetInstance().LogMessage(Log::LOG_WARNING, __FILE__,
         "Tripod: Can't find Transformable %s", parentName.c_str());
   }
}

/** Must be in the range (0.0,1.0).  A value of 0 will cause the Camera to not be
 *  updated on that particular degree of freedom.  A value of 1 will have no lag.
*/
void Tripod::SetScale(float x, float y, float z, float h, float p, float r)
{
   osg::Vec3 xyz(x,y,z);
   osg::Vec3 hpr(h,p,r);
   SetScale(xyz, hpr);
}


///Override to receive messages
void Tripod::OnSystem(const dtUtil::RefString& str, double deltaSim, double /*deltaReal*/)

{
   if (str == dtCore::System::MESSAGE_PRE_FRAME)
   {
      Update( deltaSim );
   }
}

/** Calculates where the Camera should be using the parent Transformable,
  * the delta frame time, the offset values, the scale values, and the curent
  * position/attitude.
  */
void Tripod::Update(double deltaFrameTime) //virtual
{
   //get parent's matrix
   if (!mParentTrans.valid() || !mChild.valid()) return;

   Transform parentXform;
   mParentTrans->GetTransform(parentXform);
   osg::Matrix parentMat;
   parentXform.Get(parentMat);

   Transform currXform;
   mChild->GetTransform(currXform);
   osg::Matrix currMat;
   currXform.Get(currMat);

   osg::Matrix newMat, offsetMat;
   Transform trans;
   trans.Set(mPosition, mHPR);
   trans.Get(offsetMat);

   switch(mTetherMode)
   {
   case TETHER_PARENT_REL:
      {
         //transform offset through parent's matrix to get new abs coord
         //sgMultMat4(newMat, parentMat, offsetMat);
         newMat = offsetMat * parentMat;
      }
      break;
   case TETHER_WORLD_REL:
      {
         newMat(3,0) = offsetMat(3,0) + parentMat(3,0);
         newMat(3,1) = offsetMat(3,1) + parentMat(3,1);
         newMat(3,2) = offsetMat(3,2) + parentMat(3,2);
      }
      break;
   default: break;
   }


   //set the lookat
   if (mLookAtTarget.valid())
   {
      Transform targetXform, lookatXform;
      mLookAtTarget.get()->GetTransform(targetXform);
      osg::Vec3 lookAtXYZ, upVec;
      upVec.set(0.0f, 0.0f, 1.0f);
      targetXform.GetTranslation(lookAtXYZ);
      lookatXform.Set(osg::Vec3(newMat(3,0), newMat(3,1), newMat(3,2)), lookAtXYZ, upVec);
      lookatXform.Get(newMat);
   }

   const osg::Vec3 ident(1.0f, 1.0f, 1.0f);
   if (mXYZScale != ident)
   {
      //adjust the new xyz using the xyzScale values
      osg::Vec3 xyzDiff(newMat(3,0) - currMat(3,0), newMat(3,1) - currMat(3,1), newMat(3,2) - currMat(3,2));
      xyzDiff[0] *= (mXYZScale[0]*deltaFrameTime)/deltaFrameTime;
      xyzDiff[1] *= (mXYZScale[1]*deltaFrameTime)/deltaFrameTime;
      xyzDiff[2] *= (mXYZScale[2]*deltaFrameTime)/deltaFrameTime;
      newMat(3,0) = xyzDiff[0] + currMat(3,0);
      newMat(3,1) = xyzDiff[1] + currMat(3,1);
      newMat(3,2) = xyzDiff[2] + currMat(3,2);
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
   mChild->SetTransform(parentXform);
}

/** The Camera will always point toward the look-at target if one is supplied.
* the roll of the Camera will always remain fixed (the Camera up pointed toward the sky).
* @param target: The target the Camera should point toward
*/
void Tripod::SetLookAtTarget(Transformable *target)
{
   mLookAtTarget = target;
}
