#include <dtAnim/hotspotdriver.h>
#include <dtAnim/cal3dmodelwrapper.h>
#include <dtAnim/attachmentcontroller.h>
#include <dtCore/hotspotattachment.h>
#include <dtCore/transformable.h>

#include <algorithm>
#include <cstddef>

namespace dtAnim
{
   HotSpotDriver::HotSpotDriver(Cal3DAnimator* animator)
      : mAnimator(animator), 
      mHotSpots()
   {
   }

   HotSpotDriver::~HotSpotDriver()
   {
   }

   void HotSpotDriver::Update(double dt)
   {
      AttachmentMover mover( *mAnimator->GetWrapper() );
      std::for_each( mHotSpots.begin(), mHotSpots.end(), mover);
   }

   void HotSpotDriver::SetAnimator(dtAnim::Cal3DAnimator* animator)
   {
      mAnimator = animator;
   }

   void HotSpotDriver::AddHotSpot(dtCore::HotSpotAttachment* spot)
   {
      mHotSpots.push_back( spot );
   }

   void HotSpotDriver::RemoveHotSpot(const dtCore::HotSpotAttachment* spot)
   {
      // And they smote the tents of those who had cattle,
      // and carried away sheep in abundance and camels.
      HotSpotContainer::iterator newEnd = std::remove( mHotSpots.begin(), mHotSpots.end(), spot );
      mHotSpots.erase( newEnd, mHotSpots.end() );
   }

   const HotSpotDriver::HotSpotContainer& HotSpotDriver::GetHotSpots() const
   {
      return mHotSpots;
   }

}
