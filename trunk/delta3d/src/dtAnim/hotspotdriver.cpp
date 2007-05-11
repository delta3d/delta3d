#include <dtAnim/hotspotdriver.h>
#include <dtAnim/cal3dmodelwrapper.h>
#include <dtCore/hotspotattachment.h>
#include <dtCore/transformable.h>

#include <algorithm>
#include <cstddef>

namespace dtAnim
{
   ///\ignore
   namespace details
   {
      struct BoneOffsetApplicator
      {
      public:
         BoneOffsetApplicator(const dtAnim::Cal3DModelWrapper* model)
                              : mModel(model)
         {
         }

         BoneOffsetApplicator(const BoneOffsetApplicator& same)
                              : mModel( same.mModel )
         {
         }

         BoneOffsetApplicator& operator =(const BoneOffsetApplicator& same)
         {
            mModel = same.mModel;
            return *this;
         }

         void operator()(dtCore::RefPtr<dtCore::HotSpotAttachment> spot)
         {
            // find out if the bone exists
            int bone_id = mModel->GetCoreBoneID( spot->GetDefinition().mParentName );

            // there was no bone with this name
            if( bone_id == dtAnim::Cal3DModelWrapper::NULL_BONE )
            {
               ///\todo log this
               return;
            }

            // find the total transformation for the bone
            osg::Quat parent_rot = mModel->GetBoneAbsoluteRotation( bone_id );
            osg::Quat mBodyRotation = spot->GetDefinition().mLocalRotation * parent_rot;

            osg::Vec3 bone_trans = mModel->GetBoneAbsoluteTranslation( bone_id );

            // transform the local point by the total transformation
            // and store result in the absolute point
            osg::Vec3 mBodyTranslation = bone_trans + (parent_rot * spot->GetDefinition().mLocalTranslation);

            dtCore::Transform x;
            osg::Vec3 scale(1.f,1.f,1.f);
            x.Set( mBodyTranslation, osg::Matrix(mBodyRotation), scale );
            spot->SetTransform( x, dtCore::Transformable::REL_CS );
         }

      private:
         BoneOffsetApplicator(); ///< not implemented by design

         const dtAnim::Cal3DModelWrapper* mModel;
      };

   }  // end namespace details
}  // end namespace PD

using namespace dtAnim;

HotSpotDriver::HotSpotDriver(const dtAnim::Cal3DModelWrapper* model)
   : mModel(model)
   , mHotSpots()
{
}

HotSpotDriver::~HotSpotDriver()
{
}

void HotSpotDriver::Update(double dt)
{
   details::BoneOffsetApplicator applicator( mModel );
   std::for_each( mHotSpots.begin(), mHotSpots.end(), applicator );
}

void HotSpotDriver::SetWrapper(dtAnim::Cal3DModelWrapper* model)
{
   mModel = model;
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

