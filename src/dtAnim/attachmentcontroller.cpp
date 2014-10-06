/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2007, Alion Science and Technology
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
 * David Guthrie
 */

#include <dtAnim/attachmentcontroller.h>
#include <dtCore/transformable.h>
#include <dtCore/transform.h>
#include <dtCore/hotspotattachment.h>
#include <dtUtil/log.h>

#include <osg/Quat>
#include <osg/Vec3>
#include <osg/Matrix>

#include <algorithm>

namespace dtAnim
{

   /////////////////////////////////////////////////////////////////////////////////
   AttachmentController::AttachmentController()
   {
   }
   /////////////////////////////////////////////////////////////////////////////////
   AttachmentController::~AttachmentController()
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   void AttachmentController::AddAttachment(dtCore::Transformable& actor, const dtUtil::HotSpotDefinition& spot)
   {
      mAttachments.push_back(std::make_pair(&actor, spot));
   }

   /////////////////////////////////////////////////////////////////////////////////
   class IsActor
   {
      public:
         IsActor(const dtCore::Transformable& actor) : mActor(actor) {}

         bool operator()(const AttachmentPair& val)
         {
            return val.first.get() == &mActor;
         }

      private:
         const dtCore::Transformable& mActor;
   };

   /////////////////////////////////////////////////////////////////////////////////
   void AttachmentController::RemoveAttachment(const dtCore::Transformable& actor)
   {
      mAttachments.erase(std::remove_if (mAttachments.begin(), mAttachments.end(), IsActor(actor)), mAttachments.end());
   }

   /////////////////////////////////////////////////////////////////////////////////
   const AttachmentController::AttachmentContainer& AttachmentController::GetAttachments() const
   {
      return mAttachments;
   }

   /////////////////////////////////////////////////////////////////////////////////
   AttachmentPair* AttachmentController::GetAttachment(unsigned which)
   {
      if (which >= mAttachments.size()) { return NULL; }
      return &mAttachments[which];
   }

   /////////////////////////////////////////////////////////////////////////////////
   unsigned AttachmentController::GetNumAttachments() const
   {
      return mAttachments.size();
   }


   /////////////////////////////////////////////////////////////////////////////////
   void AttachmentController::Clear()
   {
      mAttachments.clear();
   }

   /////////////////////////////////////////////////////////////////////////////////
   void AttachmentController::Update(BaseModelWrapper& model)
   {
      AttachmentMover mover(model);
      std::for_each(mAttachments.begin(), mAttachments.end(), mover);
   }


   /////////////////////////////////////////////////////////////////////////////////
   AttachmentMover::AttachmentMover(const dtAnim::BaseModelWrapper& model)
      : mModel(const_cast<dtAnim::BaseModelWrapper*>(&model))
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   AttachmentMover::AttachmentMover(const AttachmentMover& same)
      : mModel(same.mModel)
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   AttachmentMover& AttachmentMover::operator =(const AttachmentMover& same)
   {
      mModel = same.mModel;
      return *this;
   }

   /////////////////////////////////////////////////////////////////////////////////
   void AttachmentMover::operator()(dtCore::RefPtr<dtCore::HotSpotAttachment>& attachment)
   {
      AttachmentPair aPair(dtCore::RefPtr<dtCore::Transformable>(attachment.get()), attachment->GetDefinition());
      operator()(aPair);
   }

   /////////////////////////////////////////////////////////////////////////////////
   void AttachmentMover::operator()(AttachmentPair& attachment)
   {
      dtCore::Transformable& xformable = *attachment.first;
      dtUtil::HotSpotDefinition& spotDef = attachment.second;

      // find out if the bone exists
      dtAnim::BoneInterface* bone = mModel->GetBone(spotDef.mParentName);
      
      // there was no bone with this name
      if (bone == NULL)
      {
         dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
               "Ignoring update on character attached actor \"%s\" because bone named \"%s\" does not exist.",
               xformable.GetName().c_str(), spotDef.mParentName.c_str());
         return;
      }

      // find the total transformation for the bone
      osg::Quat parentRot = bone->GetAbsoluteRotation();
      osg::Quat bodyRotation = spotDef.mLocalRotation * parentRot;

      osg::Vec3 boneTrans = bone->GetAbsoluteTranslation();

      // transform the local point by the total transformation
      // and store result in the absolute point
      osg::Vec3 bodyTranslation = (boneTrans + (parentRot * spotDef.mLocalTranslation)) * mModel->GetScale();

      dtCore::Transform x;
      x.Set(bodyTranslation, osg::Matrix(bodyRotation));
      xformable.SetTransform(x, dtCore::Transformable::REL_CS);
   }

} // namespace dtAnim
