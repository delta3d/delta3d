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
   TransformJointUpdater::TransformJointUpdater(osg::Transform& transformNode, palLink& mJoint)
   : BaseClass(transformNode)
   , mDeleteJoint(true)
   , mJoint(&mJoint)
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
      if (!mJoint->SupportsParameters()) return;

      if (!mJoint->SupportsParametersPerAxis())
      {
         Float pos = osg::RadiansToDegrees(mJoint->GetParam(PAL_LINK_RELATIVE_BODY_POS_OR_ANGLE));
         switch (mJoint->GetLinkType())
         {
            case PAL_LINK_REVOLUTE:
            {
               SetRotation(osg::Vec3(0.0f, 0.0f, pos));
               break;
            }
            case PAL_LINK_PRISMATIC:
            {
               SetRotation(osg::Vec3(pos, 0.0f, 0.0f));
               break;
            }
         }
      }
      else
      {
         osg::Vec3 trans;
         osg::Vec3 rotation;
         for (unsigned i = 0; i < 3; ++i)
         {
            trans[i]    = mJoint->GetParam(PAL_LINK_RELATIVE_BODY_POS_OR_ANGLE, i);
            // joint angles are all reversed.
            rotation[i] = -osg::RadiansToDegrees(mJoint->GetParam(PAL_LINK_RELATIVE_BODY_POS_OR_ANGLE, i + 3));
         }
         rotation.set(rotation.z(), rotation.x(), rotation.y());
         SetTranslation(trans);
         SetRotation(rotation);
      }
   }

} /* namespace dtPhysics */
