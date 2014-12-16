/* -*-c++-*-
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2008, Alion Science and Technology, BMH Operation
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
#include <dtPhysics/charactercontroller.h>

#include <dtPhysics/palutil.h>
#include <dtCore/enginepropertytypes.h>
#include <dtUtil/exception.h>

#include <pal/palCharacter.h>
#include <pal/palGeometry.h>

namespace dtPhysics
{
   class CharacterControllerImpl
   {
   public:
      CharacterControllerImpl(Geometry& shape)
      :  mShape(&shape)
      ,  mPalCharCon(NULL)
      {
      }

      ~CharacterControllerImpl()
      {
         delete mPalCharCon;
      }

      /** The geometry description to use when creating this controller.
       * It also gets the initial position from this geometry.
       */
      dtCore::RefPtr<Geometry> mShape;

      palCharacterController* mPalCharCon;
   };

   /////////////////////////////////////////////////////////////////////////////
   CharacterController::CharacterController(Geometry& shape)
   : mJumpHeight(Real(1.00))
   , mJumpSpeed(Real(20.00))
   , mFallSpeed(Real(55.00))
   , mStepHeight(Real(0.3))
   , mMaxInclineAngle(Real(45.0))
   , mSkinWidth(Real(0.04))
   , mCollisionGroup(0)
   , mImpl(new CharacterControllerImpl(shape))
   {
   }

   dtCore::RefPtr<dtCore::ObjectType> CharacterController::TYPE(new dtCore::ObjectType("CharacterController", "dtPhysics"));

   /*override*/ const dtCore::ObjectType& CharacterController::GetObjectType() const { return *TYPE; }


   /////////////////////////////////////////////////////////////////////////////
   dtPhysics::Geometry& CharacterController::GetGeometry()
   {
      return *mImpl->mShape;
   }

   /////////////////////////////////////////////////////////////////////////////
   const dtPhysics::Geometry& CharacterController::GetGeometry() const
   {
      return *mImpl->mShape;
   }

   /////////////////////////////////////////////////////////////////////////////
   CharacterController::~CharacterController()
   {
      delete mImpl;
      mImpl = NULL;
   }

   /////////////////////////////////////////////////////////////////////////////
   void CharacterController::Init()
   {
      mImpl->mPalCharCon = dynamic_cast<palCharacterController*>(palFactory::GetInstance()->CreateObject("palCharacterController"));
      bool success = false;
      if (mImpl->mPalCharCon != NULL)
      {
         palCharacterControllerDesc desc;
         desc.m_pShape = &mImpl->mShape->GetPalGeometry();
         desc.m_Group = mCollisionGroup;
         desc.m_fFallSpeed = mFallSpeed;
         desc.m_fJumpHeight = mJumpHeight;
         desc.m_fJumpSpeed = mJumpSpeed;
         desc.m_fStepHeight = mStepHeight;
         desc.m_fMaxInclineAngle = mMaxInclineAngle;
         if (mImpl->mPalCharCon->Init(desc))
         {
            mImpl->mPalCharCon->SetSkinWidth(mSkinWidth);
            success = true;
         }

      }
      if (!success)
      {
         throw dtUtil::Exception("Failed to create a pal character controller.  Check the input data for validity.", __FILE__, __LINE__);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void CharacterController::BuildPropertyMap()
   {
      static const dtUtil::RefString GROUP("CharacterController");
      //InitDefaults();
   }

   /////////////////////////////////////////////////////////////////////////////
   void CharacterController::Move(const VectorType& displacement)
   {
      palVector3 palVec;
      VectorTypeToPalVec(palVec, displacement);
      mImpl->mPalCharCon->Move(palVec);
   }

   /////////////////////////////////////////////////////////////////////////////
   void CharacterController::Walk(const VectorType& walkVelocity, Real timeInterval)
   {
      palVector3 palVec;
      VectorTypeToPalVec(palVec, walkVelocity);
      mImpl->mPalCharCon->Walk(palVec, timeInterval);

   }

   /////////////////////////////////////////////////////////////////////////////
   void CharacterController::Jump()
   {
      //mImpl->mPalCharCon->Jump();
   }

   /////////////////////////////////////////////////////////////////////////////
   void CharacterController::WalkClear()
   {
      mImpl->mPalCharCon->WalkClear();
   }

   /////////////////////////////////////////////////////////////////////////////
   void CharacterController::Warp(const VectorType& worldPos)
   {
      palVector3 palVec;
      VectorTypeToPalVec(palVec, worldPos);
      mImpl->mPalCharCon->Warp(palVec);
   }

   /////////////////////////////////////////////////////////////////////////////
   VectorType CharacterController::GetTranslation() const
   {
      VectorType trans;
      palVector3 palVec;
      mImpl->mPalCharCon->GetPosition(palVec);
      PalVecToVectorType(trans, palVec);
      return trans;
   }

   /////////////////////////////////////////////////////////////////////////////
   DT_IMPLEMENT_ACCESSOR(CharacterController, Real, JumpHeight)
   DT_IMPLEMENT_ACCESSOR(CharacterController, Real, JumpSpeed)
   DT_IMPLEMENT_ACCESSOR(CharacterController, Real, FallSpeed)
   DT_IMPLEMENT_ACCESSOR(CharacterController, Real, StepHeight)
   DT_IMPLEMENT_ACCESSOR(CharacterController, Real, MaxInclineAngle)
   DT_IMPLEMENT_ACCESSOR(CharacterController, Real, SkinWidth)
   DT_IMPLEMENT_ACCESSOR(CharacterController, CollisionGroup, CollisionGroup)

   CharacterController::CharacterController(const CharacterController&) {}
   CharacterController& CharacterController::operator=(const CharacterController&) { return *this; }

}
