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

#ifndef DTANIM_ATTACHMENTCONTROLLER_H_
#define DTANIM_ATTACHMENTCONTROLLER_H_

#include <osg/Referenced>

#include <dtCore/refptr.h>
#include <dtCore/observerptr.h>

#include <dtAnim/export.h>
#include <dtAnim/basemodelwrapper.h>
#include <dtUtil/hotspotdefinition.h>
#include <vector>


namespace dtCore
{
   class Transformable;
   class HotSpotAttachment;
}

namespace dtAnim
{
   typedef std::pair<dtCore::RefPtr<dtCore::Transformable>, dtUtil::HotSpotDefinition >
      AttachmentPair;

   /**
    * Stores a list of attachments for a cal model and can update their positions
    * based each frame based on the position of the bones.
    *
    * It may be subclassed to add additional features.
    */
   class DT_ANIM_EXPORT AttachmentController : public osg::Referenced
   {
   public:
      typedef std::vector<AttachmentPair> AttachmentContainer;

      AttachmentController();

      /**
       * Adds a hot spot attachment to the skeleton.  This will move the
       * attachment each time the skeleton is updated.
       * @see dtCore::HotSpotAttachment
       */
      void AddAttachment(dtCore::Transformable& attachment, const dtUtil::HotSpotDefinition& spot);

      /**
       * Removes a previously added hot spot attachment.  If the spot is not
       * in the container, this call is a no-op.
       * @see dtCore::HotSpotAttachment
       */
      void RemoveAttachment(const dtCore::Transformable& attachment);

      /// @return an immutable container holding the current set of hot spots.
      const AttachmentContainer& GetAttachments() const;

      // @return a given attachment.
      AttachmentPair* GetAttachment(unsigned which);

      unsigned GetNumAttachments() const;

      /// Remove all attachments
      void Clear();

      /**
       * Update the attachments to the new positions based on the model.
       *
       * This may be overridden in a subclass to modify or update the behavior.
       */
      virtual void Update(dtAnim::BaseModelWrapper& model);

   protected:
      virtual ~AttachmentController();

   private:
      AttachmentContainer mAttachments;
   };

   /**
    * This is a helper functor for moving attachments.  It's exposed to allow
    * use in other classes.
    */
   class AttachmentMover
   {
   public:
      AttachmentMover(const dtAnim::BaseModelWrapper& model);

      AttachmentMover(const AttachmentMover& same);

      AttachmentMover& operator =(const AttachmentMover& same);

      /**
       * For use with dtCore::HotSpotAttachments using for_each or similar
       * This method takes a ref ptr because containers tend to hold then, and
       * it is for use with for_each or something similar.  The ref ptr is
       * takes by reference, so it is not copied.
       */
      void operator()(dtCore::RefPtr<dtCore::HotSpotAttachment>& attachment);

      /**
       * For use with AttachmentPair typedef above using for_each or similar.
       * This method technically passes a refptr as a parameter, but it won't be copied
       * because its a member of the pair which is passed by reference.
       */
      void operator()(AttachmentPair& attachment);

   private:
      dtCore::ObserverPtr<dtAnim::BaseModelWrapper> mModel;
   };
} // namespace dtAnim

#endif // DTANIM_ATTACHMENTCONTROLLER_H_
