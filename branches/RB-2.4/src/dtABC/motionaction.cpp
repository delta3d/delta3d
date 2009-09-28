#include <dtABC/motionaction.h>
#include <dtUtil/matrixutil.h>
#include <dtUtil/log.h>
#include <dtABC/pathpoint.h>
#include <dtCore/transform.h>
#include <osg/MatrixTransform>

namespace dtABC
{


MotionAction::MotionAction()
   : mTargetObject(NULL)
   , mParentRelation(NO_RELATION)
{

}

MotionAction::~MotionAction()
{

}

void MotionAction::SetParentAndRelation(dtCore::Transformable* pParent, PARENT_RELATION pRelation)
{
   mParent = pParent;
   mParentRelation = pRelation;

   dtCore::Transform trans;

   mParent->GetTransform(trans);
   trans.GetTranslation(mInitialParentPos);
}


void MotionAction::SetParent(dtCore::Transformable* pParent)
{
   mParent = pParent;
   if (mParent.valid())
   {
      dtCore::Transform trans;
      mParent->GetTransform(trans);
      trans.GetTranslation(mInitialParentPos);
   }
}

void MotionAction::SetParentRelation(PARENT_RELATION pRelation)
{
   mParentRelation = pRelation;
}

void MotionAction::RemoveParent()
{
   mParent = NULL;
   mParentRelation = NO_RELATION;
}

void MotionAction::StepObject(const PathPoint& cp)
{
   osg::Matrix pTransform;
   pTransform.makeScale(osg::Vec3(1.0f, 1.0f, 1.0f));

   if (!mParent.valid() && mParentRelation != NO_RELATION)
   {
      LOGN_ERROR("motionaction.cpp", "Motion Action has no parent, and the parent relation set to something other than NO_RELATION, so stepping won't work.");
      return;
   }

   switch (mParentRelation)
   {
   case TRACK_PARENT:
      {
         dtCore::Transform trans;
         osg::Vec3 parentPos;

         mParent->GetTransform(trans);
         trans.GetTranslation(parentPos);

         pTransform.makeRotate(cp.GetOrientation());

         SetTrack(parentPos - cp.GetPosition(), pTransform);

         dtUtil::MatrixUtil::SetRow(pTransform, cp.GetPosition(), 3);
         break;
      }

   case FOLLOW_PARENT:
      {
         pTransform.makeRotate(cp.GetOrientation());

         dtCore::Transform trans;
         osg::Vec3 parentPos, newPos;

         mParent->GetTransform(trans);
         trans.GetTranslation(parentPos);

         newPos = cp.GetPosition() + (parentPos - mInitialParentPos);

         dtUtil::MatrixUtil::SetRow(pTransform, newPos, 3);

         break;
      }

   case TRACK_AND_FOLLOW:
      {
         dtCore::Transform trans;
         osg::Vec3 parentPos, newPos;

         mParent->GetTransform(trans);
         trans.GetTranslation(parentPos);

         newPos = cp.GetPosition() + (parentPos - mInitialParentPos);

         pTransform.makeRotate(cp.GetOrientation());
         SetTrack(parentPos - newPos, pTransform);

         dtUtil::MatrixUtil::SetRow(pTransform, newPos, 3);


         break;
      }

      //this is when no target is set and mTargetRelation should be 0
   default:
      {
         pTransform.makeRotate(cp.GetOrientation());
         dtUtil::MatrixUtil::SetRow(pTransform, cp.GetPosition(), 3);
      }
      break;
   }

   osg::Matrix local;
   mLocalTransform.Get(local);
   if (mTargetObject.valid())
   {
      mTargetObject->GetMatrixNode()->setMatrix(local * pTransform);
   }
}


void MotionAction::SetTrack(const osg::Vec3& forwardVector, osg::Matrix& mat)
{
   osg::Vec3 x,y,z;

   y = forwardVector;
   z = dtUtil::MatrixUtil::GetRow3(mat, 2);
   x = y ^ z;
   z = x ^ y;

   x.normalize();
   y.normalize();
   z.normalize();

   dtUtil::MatrixUtil::SetRow(mat, x, 0);
   dtUtil::MatrixUtil::SetRow(mat, y, 1);
   dtUtil::MatrixUtil::SetRow(mat, z, 2);
}


}//namespace dtABC

