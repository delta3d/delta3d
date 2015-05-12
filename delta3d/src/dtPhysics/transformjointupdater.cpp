/*
 * transformjointupdater.cpp
 *
 *  Created on: May 4, 2015
 *      Author: david
 */

#include <dtPhysics/transformjointupdater.h>
#include <dtPhysics/physicstypes.h>
#include <pal/palLinks.h>
#include <dtCore/transform.h>
#include <osg/MatrixTransform>
#include <osgSim/DOFTransform>

namespace dtPhysics
{
   TransformJointUpdater::TransformJointUpdater(palLink& mJoint, osg::Transform& mTransformNode)
   : mDeleteJoint(true)
   , mJoint(&mJoint)
   , mTransformNode(&mTransformNode)
   {
   }

   TransformJointUpdater::~TransformJointUpdater()
   {
      if (mDeleteJoint)
      {
         delete mJoint;
      }
      mJoint = NULL;
   }

   DT_IMPLEMENT_ACCESSOR(TransformJointUpdater, bool, DeleteJoint);

   void TransformJointUpdater::operator()()
   {
      dtCore::Transform xform;
      if (!mJoint->SupportsParameters()) return;

      if (!mJoint->SupportsParametersPerAxis())
      {
         Float pos = mJoint->GetParam(PAL_LINK_RELATIVE_BODY_POS_OR_ANGLE);
         switch (mJoint->GetLinkType())
         {
            case PAL_LINK_REVOLUTE:
            {
               xform.SetRotation(0.0f, 0.0f, pos);
               break;
            }
            case PAL_LINK_PRISMATIC:
            {
               xform.SetRotation(pos, 0.0f, 0.0f);
               break;
            }
         }
      }
      else
      {
         dtPhysics::VectorType trans;
         dtPhysics::VectorType rotation;
         for (unsigned i = 0; i < 3; ++i)
         {
            trans[i]    = mJoint->GetParam(PAL_LINK_RELATIVE_BODY_POS_OR_ANGLE, i);
            rotation[i] = mJoint->GetParam(PAL_LINK_RELATIVE_BODY_POS_OR_ANGLE, i + 3);
         }
      }
      osg::MatrixTransform* mt = mTransformNode->asMatrixTransform();
      if (mt != NULL)
      {
         osg::Matrix mat;
         xform.Get(mat);
         mt->setMatrix(mat);
         return;
      }
      osgSim::DOFTransform* doft = dynamic_cast<osgSim::DOFTransform*>(mTransformNode.get());
      if (doft != NULL)
      {
         doft->setCurrentHPR(xform.GetRotation());
         doft->setCurrentTranslate(xform.GetTranslation());
      }
   }

} /* namespace dtPhysics */
