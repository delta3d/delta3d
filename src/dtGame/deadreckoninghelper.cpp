/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation.
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
 * David Guthrie, Curtiss Murphy
 */
#include <prefix/dtgameprefix.h>
#include <dtGame/deadreckoninghelper.h>

#include <dtCore/booleanactorproperty.h>
#include <dtCore/propertymacros.h>

#include <dtGame/deadreckoningcomponent.h>
#include <dtGame/gameactor.h>

#include <dtUtil/log.h>
#include <dtUtil/matrixutil.h>
#include <dtUtil/mathdefines.h>

#include <osg/io_utils>  //for Vec3 streaming
#include <osgSim/DOFTransform>

namespace dtGame
{
   IMPLEMENT_ENUM(DeadReckoningAlgorithm);
   DeadReckoningAlgorithm DeadReckoningAlgorithm::NONE("None");
   DeadReckoningAlgorithm DeadReckoningAlgorithm::STATIC("Static");
   DeadReckoningAlgorithm DeadReckoningAlgorithm::VELOCITY_ONLY("Velocity Only");
   DeadReckoningAlgorithm DeadReckoningAlgorithm::VELOCITY_AND_ACCELERATION("Velocity and Acceleration");
   DeadReckoningAlgorithm::DeadReckoningAlgorithm(const std::string& name) : dtUtil::Enumeration(name)
   {
      AddInstance(this);
   }

   const float DeadReckoningActorComponent::DEFAULT_MAX_SMOOTHING_TIME_ROT = 2.0f;
   const float DeadReckoningActorComponent::DEFAULT_MAX_SMOOTHING_TIME_POS = 4.0f;

   const std::string DeadReckoningActorComponent::DeadReckoningDOF::REPRESENATION_POSITION("Position");
   const std::string DeadReckoningActorComponent::DeadReckoningDOF::REPRESENATION_POSITIONRATE("PositionRate");
   const std::string DeadReckoningActorComponent::DeadReckoningDOF::REPRESENATION_EXTENSION("Extension");
   const std::string DeadReckoningActorComponent::DeadReckoningDOF::REPRESENATION_EXTENSIONRATE("ExtensionRate");
   const std::string DeadReckoningActorComponent::DeadReckoningDOF::REPRESENATION_X("LocationX");
   const std::string DeadReckoningActorComponent::DeadReckoningDOF::REPRESENATION_XRATE("LocationXRate");
   const std::string DeadReckoningActorComponent::DeadReckoningDOF::REPRESENATION_Y("LocationY");
   const std::string DeadReckoningActorComponent::DeadReckoningDOF::REPRESENATION_YRATE("LocationYRate");
   const std::string DeadReckoningActorComponent::DeadReckoningDOF::REPRESENATION_Z("LocationZ");
   const std::string DeadReckoningActorComponent::DeadReckoningDOF::REPRESENATION_ZRATE("LocationZRate");
   const std::string DeadReckoningActorComponent::DeadReckoningDOF::REPRESENATION_AZIMUTH("Azimuth");
   const std::string DeadReckoningActorComponent::DeadReckoningDOF::REPRESENATION_AZIMUTHRATE("AzimuthRate");
   const std::string DeadReckoningActorComponent::DeadReckoningDOF::REPRESENATION_ELEVATION("Elevation");
   const std::string DeadReckoningActorComponent::DeadReckoningDOF::REPRESENATION_ELEVATIONRATE("ElevationRate");
   const std::string DeadReckoningActorComponent::DeadReckoningDOF::REPRESENATION_ROTATION("Rotation");
   const std::string DeadReckoningActorComponent::DeadReckoningDOF::REPRESENATION_ROTATIONRATE("RotationRate");

   IMPLEMENT_ENUM(DeadReckoningActorComponent::UpdateMode);
   DeadReckoningActorComponent::UpdateMode DeadReckoningActorComponent::UpdateMode::AUTO("AUTO");
   DeadReckoningActorComponent::UpdateMode DeadReckoningActorComponent::UpdateMode::CALCULATE_ONLY("CALCULATE_ONLY");
   DeadReckoningActorComponent::UpdateMode
      DeadReckoningActorComponent::UpdateMode::CALCULATE_AND_MOVE_ACTOR("CALCULATE_AND_MOVE_ACTOR");
   DeadReckoningActorComponent::UpdateMode::UpdateMode(const std::string& name) : dtUtil::Enumeration(name)
   {
      AddInstance(this);
   }


   //////////////////////////////////////////////////////////////////////
   DeadReckoningActorComponent::DeadReckoningActorComponent()
   : ActorComponent(TYPE)
   , mAutoRegisterWithGMComponent(true)
   , mGroundClampType(&GroundClampTypeEnum::KEEP_ABOVE)
   , mLastRotationUpdatedTime(0.0)
   , mAverageTimeBetweenRotationUpdates(0.0f)
   , mMaxRotationSmoothingTime(DEFAULT_MAX_SMOOTHING_TIME_ROT)
   , mUseFixedSmoothingTime(false)
   , mFixedSmoothingTime(1.0f)
   , mRotationElapsedTimeSinceUpdate(0.0f)
   , mRotationEndSmoothingTime(0.0f)
   , mMinDRAlgorithm(&DeadReckoningAlgorithm::STATIC)
   , mUpdateMode(&DeadReckoningActorComponent::UpdateMode::AUTO)
   , mRotationInitiated(false)
   , mUpdated(false)
   , mRotationUpdated(false)
   //, mFlying(false)
   , mRotationResolved(true)
   , mExtraDataUpdated(false)
   , mForceUprightRotation(false)
   , mCurTimeDelta(0.0f)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   DeadReckoningActorComponent::~DeadReckoningActorComponent()
   {
   }

   DT_IMPLEMENT_ACCESSOR(DeadReckoningActorComponent, bool, AutoRegisterWithGMComponent);

   // GROUND CLAMP TYPE PROPERTY
   DT_IMPLEMENT_ACCESSOR_GETTER(DeadReckoningActorComponent, dtUtil::EnumerationPointer<GroundClampTypeEnum>, GroundClampType);


   //////////////////////////////////////////////////////////////////////
   void DeadReckoningActorComponent::OnEnteredWorld()
   {
      mExtraDataUpdated = false;

      if (mAutoRegisterWithGMComponent)
      {
         RegisterWithGMComponent();
      }
   }

   /// Called when the parent actor leaves the "world".
   //////////////////////////////////////////////////////////////////////
   void DeadReckoningActorComponent::OnRemovedFromWorld()
   {
      if (mAutoRegisterWithGMComponent)
      {
         UnregisterWithGMComponent();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DeadReckoningActorComponent::RegisterWithGMComponent()
   {
      dtGame::DeadReckoningComponent* drc = NULL;

      GameActorProxy* act = NULL;
      GetOwner(act);

      act->GetGameManager()->
         GetComponentByName(dtGame::DeadReckoningComponent::DEFAULT_NAME, drc);

      if (drc != NULL)
      {
         drc->RegisterActor(*act, *this);
      }
      else
      {
         dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
            "Actor \"%s\"\"%s\" unable to find DeadReckoningComponent.",
            act->GetName().c_str(), act->GetId().ToString().c_str());
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DeadReckoningActorComponent::UnregisterWithGMComponent()
   {
      dtGame::DeadReckoningComponent* drc = NULL;

      GameActorProxy* act = NULL;
      GetOwner(act);
      // This is false in some delete cases.
      if (act != NULL)
      {
         act->GetGameManager()->
            GetComponentByName(dtGame::DeadReckoningComponent::DEFAULT_NAME, drc);

         if (drc != NULL)
         {
            drc->UnregisterActor(*act);
         }
         else
         {
            dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
               "Actor \"%s\"\"%s\" unable to find DeadReckoningComponent.",
               act->GetName().c_str(), act->GetId().ToString().c_str());
         }
      }
   }

   //////////////////////////////////////////////////////////////////////
   DeadReckoningActorComponent::UpdateMode& DeadReckoningActorComponent::GetEffectiveUpdateMode(bool isRemote) const
   {
      if (*mUpdateMode == DeadReckoningActorComponent::UpdateMode::AUTO)
      {
         if (isRemote && GetDeadReckoningAlgorithm() != DeadReckoningAlgorithm::NONE)
            return DeadReckoningActorComponent::UpdateMode::CALCULATE_AND_MOVE_ACTOR;
         else
            return DeadReckoningActorComponent::UpdateMode::CALCULATE_ONLY;
      }
      return *mUpdateMode;
   }

   //////////////////////////////////////////////////////////////////////
   bool DeadReckoningActorComponent::IsExtraDataUpdated()
   {
      return mExtraDataUpdated;
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningActorComponent::SetExtraDataUpdated(bool newValue)
   {
      mExtraDataUpdated = newValue;
   }

   //////////////////////////////////////////////////////////////////////
   bool DeadReckoningActorComponent::IsFlyingDeprecatedProperty()
   {
      bool result = (GroundClampTypeEnum::NONE == GetGroundClampType());
      return result;
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningActorComponent::SetFlyingDeprecatedProperty(bool newFlying)
   {
      // THIS METHOD SUPPORTS THE DEPRECATED PROPERTY IN EXISTING MAPS. USE SetGroundClampType() instead.
      (newFlying) ?
         (SetGroundClampType(GroundClampTypeEnum::NONE)) :
         (SetGroundClampType(GroundClampTypeEnum::FULL));
   }

   //////////////////////////////////////////////////////////////////////
   DEPRECATE_FUNC bool DeadReckoningActorComponent::IsFlying()
   {
      DEPRECATE("void DeadReckoningActorComponent::IsFlying()", 
         " GroundClampTypeEnum DeadReckoningActorComponent::GetGroundClampType()");
      return IsFlyingDeprecatedProperty();
   }

   //////////////////////////////////////////////////////////////////////
   DEPRECATE_FUNC void DeadReckoningActorComponent::SetFlying(bool newFlying)
   {
      DEPRECATE("void DeadReckoningActorComponent::SetFlying(bool)", 
         " bool DeadReckoningActorComponent::SetGroundClampType(GroundClampTypeEnum)");
      SetFlyingDeprecatedProperty(newFlying);
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningActorComponent::SetGroundClampType(GroundClampTypeEnum& newType)
   {
      if (newType == GetGroundClampType())
         return;

      mGroundClampingData.SetGroundClampType(newType);
      mGroundClampType = newType;
      mUpdated = true;
      mExtraDataUpdated = true;
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningActorComponent::SetAdjustRotationToGround(bool newAdjust)
   {
      if (mGroundClampingData.GetAdjustRotationToGround() == newAdjust)
         return;
      mGroundClampingData.SetAdjustRotationToGround(newAdjust);
      mUpdated = true;
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningActorComponent::SetDeadReckoningAlgorithm(DeadReckoningAlgorithm& newAlgorithm)
   {
      if (mMinDRAlgorithm == &newAlgorithm)
         return;
      mMinDRAlgorithm = &newAlgorithm;
      mUpdated = true;

      mExtraDataUpdated = true;
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningActorComponent::SetLastKnownTranslation(const osg::Vec3& vec)
   {
      mTranslation.SetLastKnownTranslation(vec);
      mUpdated = true;
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningActorComponent::SetLastKnownRotation(const osg::Vec3& newRot)
   {
      // Some objects only care about heading, so we zero out pitch/yaw.
      osg::Vec3 vec = (!mForceUprightRotation) ? (newRot) : (osg::Vec3(newRot.x(), 0.0f, 0.0f));

      dtCore::Transform xform;
      xform.SetRotation(vec);
      xform.GetRotation(mLastRotationMatrix);

      if (mRotationInitiated)
      {
         mRotQuatBeforeLastUpdate = mCurrentDeadReckonedRotation;
      }
      else
      {
         mLastRotationMatrix.get(mRotQuatBeforeLastUpdate);
      }

      mLastRotation = vec;
      mLastRotationMatrix.get(mLastQuatRotation);
      mRotationElapsedTimeSinceUpdate = 0.0;
      mRotationInitiated = true;
      mRotationUpdated = true;
      mUpdated = true;
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningActorComponent::SetLastKnownVelocity(const osg::Vec3& vec)
   {
      mTranslation.SetLastKnownVelocity(vec);
      mUpdated = true;
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningActorComponent::SetLastKnownAcceleration(const osg::Vec3& vec)
   {
      mTranslation.mAcceleration = vec;
      mUpdated = true;
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningActorComponent::SetLastKnownAngularVelocity(const osg::Vec3& vec)
   {
      mAngularVelocityVector = vec;
      mUpdated = true;
   }

   //////////////////////////////////////////////////////////////////////
   const osg::Vec3& DeadReckoningActorComponent::GetCurrentInstantVelocity() const
   {
      return mTranslation.mPreviousInstantVel;
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningActorComponent::SetForceUprightRotation(bool newValue)
   {
      mForceUprightRotation = newValue;
   }

   //////////////////////////////////////////////////////////////////////
   bool DeadReckoningActorComponent::GetForceUprightRotation() const
   {
      return mForceUprightRotation;
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningActorComponent::SetGroundOffset(float newOffset)
   {
      mGroundClampingData.SetGroundOffset(newOffset);
      mUpdated = true;

      mExtraDataUpdated = true;
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningActorComponent::SetLastTranslationUpdatedTime(double newUpdatedTime)
   {
      mTranslation.SetLastUpdatedTime(newUpdatedTime);
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningActorComponent::SetLastRotationUpdatedTime(double newUpdatedTime)
   {
      //the average of the last average and the current time since an update.
      float timeDelta = float(newUpdatedTime - mLastRotationUpdatedTime);
      mAverageTimeBetweenRotationUpdates = 0.5f * timeDelta + 0.5f * mAverageTimeBetweenRotationUpdates;
      mLastRotationUpdatedTime = newUpdatedTime;
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningActorComponent::SetTranslationElapsedTimeSinceUpdate(float value)
   { 
      /// Compute time delta for this step of DR. Should be the same as DeltaTime in the component
      mCurTimeDelta = dtUtil::Max(value - mTranslation.mElapsedTimeSinceUpdate, 0.0f);
      mTranslation.mElapsedTimeSinceUpdate = value; 
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningActorComponent::SetModelDimensions(const osg::Vec3& newDimensions)
   {
      mGroundClampingData.SetModelDimensions(newDimensions);
      SetUseModelDimensions(true);
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningActorComponent::AddToDeadReckonDOF(const std::string& dofName,
      const osg::Vec3& position,
      const osg::Vec3& rateOverTime,
      const std::string& metricName)
   {
      dtCore::RefPtr<DeadReckoningDOF> newDOF = new DeadReckoningDOF();
      newDOF->mNext = NULL;
      newDOF->mPrev = NULL;
      newDOF->mName = dofName;
      newDOF->mMetricName = metricName;
      newDOF->mCurrentTime = 0;
      newDOF->mRateOverTime.set( rateOverTime );
      newDOF->mStartLocation.set( position );

      std::list<dtCore::RefPtr<DeadReckoningDOF> >::iterator iter;
      for (iter = mDeadReckonDOFS.begin(); iter != mDeadReckonDOFS.end(); ++iter)
      {
         // does the linking of the object to ADD, so we add the next pointer and validate it
         // no matter what the dead reckon dof gets pushed onto the list at the end
         // of the function.
         if((*iter)->mName == dofName)
         {
            DeadReckoningDOF* currentDOF = (*iter).get();

            while(currentDOF != NULL)
            {
               if(currentDOF->mNext == NULL)
               {
                  break;
               }
               currentDOF = currentDOF->mNext;
            }

            newDOF->mPrev = currentDOF;
            newDOF->mNext = NULL;

            currentDOF->mCurrentTime = 0;
            currentDOF->mNext = newDOF.get();
            currentDOF->mStartLocation = mDOFDeadReckoning->GetDOFTransform(currentDOF->mName)->getCurrentHPR();

            break;
         }
      }

      mDeadReckonDOFS.push_back(newDOF);
   }

   //////////////////////////////////////////////////////////////////////
   // Used for removing area by name.
   struct RemoveByName
   {
   public:
      RemoveByName(const std::string&  name) : mName(name)
      {

      }

      bool operator()(dtCore::RefPtr<DeadReckoningActorComponent::DeadReckoningDOF>& toCheck)
      {
         return (mName == toCheck->mName);
      }

   private:
      const std::string& mName;
   };

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningActorComponent::RemoveAllDRDOFByName(const std::string& removeName)
   {
      mDeadReckonDOFS.remove_if(RemoveByName(removeName));
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningActorComponent::RemoveDRDOF(DeadReckoningDOF& obj)
   {
      std::list<dtCore::RefPtr<DeadReckoningDOF> >::iterator iterDOF;
      for (iterDOF = mDeadReckonDOFS.begin();iterDOF != mDeadReckonDOFS.end(); ++iterDOF)
      {
         // If the same pointer or the values match, then remove this object from the list.
         if ( &obj == *iterDOF
            ||((obj.mCurrentTime   == (*iterDOF)->mCurrentTime)
            && (obj.mName          == (*iterDOF)->mName)
            && (obj.mRateOverTime  == (*iterDOF)->mRateOverTime)
            && (obj.mStartLocation == (*iterDOF)->mStartLocation)))
         {
            RemoveDRDOF(iterDOF);
            return;
         }
      }
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningActorComponent::RemoveDRDOF(std::list<dtCore::RefPtr<DeadReckoningDOF> >::iterator& iter)
   {
      // if theres more after it
      if ((*iter)->mNext != NULL)
      {
         // if its in the middle of two objects
         if ((*iter)->mPrev != NULL)
         {
            (*iter)->mPrev->mNext = (*iter)->mNext;
            (*iter)->mNext->mPrev = (*iter)->mPrev;
         }
         // else its in the front
         else
         {
            (*iter)->mNext->mPrev = NULL;
         }
      }
      // if there is more before it
      else if ((*iter)->mPrev != NULL)
      {
         // its last
        (*iter)->mPrev->mNext = NULL;
      }
      // none before or after the object, first one in list
      else
      {
         // dont need to link objects
      }
      mDeadReckonDOFS.erase(iter);
      return;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DeadReckoningActorComponent::OnAddedToActor(dtCore::BaseActorObject& actor)
   {
      // publishing the position and rotation properties while dead reckoning is enabled will
      // cause the deadreckoning to fail to function properly, so this disables the publishing.
      dtCore::ActorProperty* prop = actor.GetProperty(dtCore::TransformableActorProxy::PROPERTY_ROTATION);
      if (prop != NULL)
      {
         prop->SetSendInFullUpdate(false);
         prop->SetSendInPartialUpdate(false);
      }
      prop = actor.GetProperty(dtCore::TransformableActorProxy::PROPERTY_TRANSLATION);
      if (prop != NULL)
      {
         prop->SetSendInFullUpdate(false);
         prop->SetSendInPartialUpdate(false);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DeadReckoningActorComponent::OnRemovedFromActor(dtCore::BaseActorObject& actor)
   {
      dtCore::ActorProperty* prop = actor.GetProperty(dtCore::TransformableActorProxy::PROPERTY_ROTATION);
      // It would be more correct to set these back to the original values before this actor component was added
      // but since the code that removes the component will then have to figure out what it wants these values to be, it's
      // probably just safest to set them to the default.
      if (prop != NULL)
      {
         prop->SetSendInFullUpdate(true);
         prop->SetSendInPartialUpdate(true);
      }
      prop = actor.GetProperty(dtCore::TransformableActorProxy::PROPERTY_TRANSLATION);
      if (prop != NULL)
      {
         prop->SetSendInFullUpdate(true);
         prop->SetSendInPartialUpdate(true);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   // PROPERTY NAME DECLARATIONS
   ////////////////////////////////////////////////////////////////////////////////
   const dtUtil::RefString DeadReckoningActorComponent::PROPERTY_LAST_KNOWN_TRANSLATION("Last Known Translation");
   const dtUtil::RefString DeadReckoningActorComponent::PROPERTY_LAST_KNOWN_ROTATION("Last Known Rotation");
   const dtUtil::RefString DeadReckoningActorComponent::PROPERTY_VELOCITY_VECTOR("Velocity Vector");
   const dtUtil::RefString DeadReckoningActorComponent::PROPERTY_ACCELERATION_VECTOR("Acceleration Vector");
   const dtUtil::RefString DeadReckoningActorComponent::PROPERTY_ANGULAR_VELOCITY_VECTOR("Angular Velocity Vector");
   const dtUtil::RefString DeadReckoningActorComponent::PROPERTY_DEAD_RECKONING_ALGORITHM("Dead Reckoning Algorithm");
   const dtUtil::RefString DeadReckoningActorComponent::PROPERTY_FLYING("Flying");
   const dtUtil::RefString DeadReckoningActorComponent::PROPERTY_GROUND_CLAMP_TYPE("GroundClampType");
   const dtUtil::RefString DeadReckoningActorComponent::PROPERTY_GROUND_OFFSET("Ground Offset");

   ////////////////////////////////////////////////////////////////////////////////
   void DeadReckoningActorComponent::BuildPropertyMap()
   {
      static const dtUtil::RefString DEADRECKONING_GROUP = "Dead Reckoning";
      typedef dtCore::PropertyRegHelper<DeadReckoningActorComponent> PropRegType;
      PropRegType propRegHelper(*this, this, DEADRECKONING_GROUP);//"Dead Reckoning");


      DT_REGISTER_PROPERTY_WITH_NAME(LastKnownTranslation, PROPERTY_LAST_KNOWN_TRANSLATION, 
         "Sets the last know position of this Entity", PropRegType, propRegHelper);
      GetProperty(PROPERTY_LAST_KNOWN_TRANSLATION)->SetSendInPartialUpdate(true);

      // Last Known Rotation - See the header for GetInternalLastKnownRotationInXYZ() for why this is wierd.
      DT_REGISTER_PROPERTY_WITH_NAME(InternalLastKnownRotationInXYZ, PROPERTY_LAST_KNOWN_ROTATION, 
         "Sets the last known rotation of this Entity", PropRegType, propRegHelper);
      GetProperty(PROPERTY_LAST_KNOWN_ROTATION)->SetSendInPartialUpdate(true);

      // Note - the member vars were changed to LastKnownXYZ, but the properties were left the same
      // so as to not break MANY maps in production.
      DT_REGISTER_PROPERTY_WITH_NAME(LastKnownVelocity, PROPERTY_VELOCITY_VECTOR, 
         "Sets the last known velocity vector of this Entity", PropRegType, propRegHelper);
      GetProperty(PROPERTY_VELOCITY_VECTOR)->SetSendInPartialUpdate(true);

      // Note - the member vars were changed to LastKnownXYZ, but the properties were left the same
      // so as to not break MANY maps in production.
      DT_REGISTER_PROPERTY_WITH_NAME(LastKnownAcceleration, PROPERTY_ACCELERATION_VECTOR, 
         "Sets the last known acceleration vector of this Entity", PropRegType, propRegHelper);
      GetProperty(PROPERTY_ACCELERATION_VECTOR)->SetSendInPartialUpdate(true);

      // Note - the member vars were changed to LastKnownXYZ, but the properties were left the same
      // so as to not break MANY maps in production.
      DT_REGISTER_PROPERTY_WITH_NAME(LastKnownAngularVelocity, PROPERTY_ANGULAR_VELOCITY_VECTOR, 
         "Sets the last known angular velocity vector of this Entity", PropRegType, propRegHelper);
      GetProperty(PROPERTY_ANGULAR_VELOCITY_VECTOR)->SetSendInPartialUpdate(true);

      DT_REGISTER_PROPERTY_WITH_NAME(DeadReckoningAlgorithm, PROPERTY_DEAD_RECKONING_ALGORITHM, 
         "Sets the enumerated dead reckoning algorithm to use.", PropRegType, propRegHelper);
      GetProperty(PROPERTY_DEAD_RECKONING_ALGORITHM)->SetSendInPartialUpdate(true);


      // Doesn't use macro cause the Getter is called IsFlying
      DT_REGISTER_PROPERTY_WITH_NAME_AND_LABEL(GroundClampType, PROPERTY_GROUND_CLAMP_TYPE, "Ground Clamp Type",
         "What type of ground clamping should be performed. This replaced the Flying property.", 
         PropRegType, propRegHelper);

      DT_REGISTER_PROPERTY_WITH_NAME(GroundOffset, PROPERTY_GROUND_OFFSET, 
         "Sets the offset from the ground this entity should have.  This only matters if it is not flying.", PropRegType, propRegHelper);

   }

   /////////////////////////////////////////////////////////////////////////////////
   void DeadReckoningActorComponent::IncrementTimeSinceUpdate(float simTimeDelta, double curSimulationTime)
   {
      //Pretend we were updated on the last tick so we have time delta to work with
      //when calculating movement.
      if (IsTranslationUpdated())
      {
         SetLastTranslationUpdatedTime(curSimulationTime - simTimeDelta);
         SetTranslationElapsedTimeSinceUpdate(0.0);
      }

      if (IsRotationUpdated() )
      {
         SetLastRotationUpdatedTime(curSimulationTime - simTimeDelta );
         SetRotationElapsedTimeSinceUpdate(0.0);
         SetRotationResolved( false );
      }

      //We want to do this every time. make sure it's greater than 0 in case of time being set.

      // Translation
      float transElapsedTime = GetTranslationElapsedTimeSinceUpdate() + simTimeDelta;
      if (transElapsedTime < 0.0) transElapsedTime = 0.0f;
      SetTranslationElapsedTimeSinceUpdate(transElapsedTime);

      // Rotation
      float rotElapsedTime = GetRotationElapsedTimeSinceUpdate() + simTimeDelta;
      if (rotElapsedTime < 0.0) rotElapsedTime = 0.0f;
      SetRotationElapsedTimeSinceUpdate(rotElapsedTime);
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool DeadReckoningActorComponent::DoDR(dtCore::Transformable& txable, dtCore::Transform& xform,
         dtUtil::Log* pLogger, BaseGroundClamper::GroundClampRangeType*& gcType)
   {
      bool returnValue = false; // indicates we changed the transform
      if (GetGroundClampType() == GroundClampTypeEnum::NONE)
      {
         gcType = &BaseGroundClamper::GroundClampRangeType::NONE;
      }
      else if (GetGroundClampingData().GetAdjustRotationToGround())
      {
         gcType = &BaseGroundClamper::GroundClampRangeType::RANGED;
      }
      else
      {
         gcType = &BaseGroundClamper::GroundClampRangeType::INTERMITTENT_SAVE_OFFSET;
      }

      if (GetDeadReckoningAlgorithm() == DeadReckoningAlgorithm::NONE)
      {
         if (pLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            pLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                  "Dead Reckoning Algorithm set to NONE, "
                  "setting the transform to match the actor's current position.");
         }
         txable.GetTransform(xform, dtCore::Transformable::REL_CS);
         // It's set to NONE, so no ground clamping..
         gcType = &BaseGroundClamper::GroundClampRangeType::NONE;
      }
      else if (GetDeadReckoningAlgorithm() == DeadReckoningAlgorithm::STATIC)
      {
         if (IsUpdated())
         {
            DRStatic(txable, xform, pLogger);
            returnValue = true;
         }
         else if (mTranslation.mPreviousInstantVel.length2() > FLT_EPSILON)
         {
            mTranslation.mPreviousInstantVel.set(0.0f, 0.0f, 0.0f);
         }
      }
      else
      {
         returnValue = DRVelocityAcceleration(txable, xform, pLogger);
      }

      return returnValue;
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningActorComponent::DRStatic(dtCore::Transformable& txable, dtCore::Transform& xform, dtUtil::Log* pLogger)
   {
      if (pLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         std::ostringstream ss;
         ss << "Dead Reckoning actor as STATIC.  New position is " <<  mTranslation.mLastValue
            << ".  New Rotation is " << mLastRotation << ".";
         pLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, ss.str().c_str());
      }

      xform.SetTranslation(mTranslation.mLastValue);
      xform.SetRotation(mLastRotationMatrix);

      mTranslation.mPreviousInstantVel = mCurTimeDelta <= 0.0 ?  mTranslation.mPreviousInstantVel : (mTranslation.mLastValue - mTranslation.mValueBeforeLastUpdate) / mCurTimeDelta;
      mTranslation.mValueBeforeLastUpdate = mTranslation.mLastValue;
      mTranslation.mCurrentDeadReckonedValue = mTranslation.mLastValue;

      mRotQuatBeforeLastUpdate = mLastQuatRotation;
      mRotationResolved = true;

      xform.GetRotation(mCurrentAttitudeVector);
      mLastRotationMatrix.get(mCurrentDeadReckonedRotation);

   }

   //////////////////////////////////////////////////////////////////////
   bool DeadReckoningActorComponent::DRVelocityAcceleration(dtCore::Transformable& txable, dtCore::Transform& xform, dtUtil::Log* pLogger)
   {
      bool returnValue = false; // indicates that we made a change to the transform
      osg::Vec3 pos;
      xform.GetTranslation(pos);
      osg::Matrix rot;
      xform.GetRotation(rot);

      osg::Vec3 unclampedTranslation = pos;

      //avoid the sqrtf by using length2.
      //we went to see if all this dr and ground clamping stuff has to be done.
      if (IsUpdated() ||
         mTranslation.mLastValue != unclampedTranslation ||
         !mRotationResolved ||
         mTranslation.mLastVelocity.length2() > 1e-2f ||
         (GetDeadReckoningAlgorithm() == DeadReckoningAlgorithm::VELOCITY_AND_ACCELERATION
            && mTranslation.mAcceleration.length2() > 1e-2f)||
         (GetDeadReckoningAlgorithm() == DeadReckoningAlgorithm::VELOCITY_AND_ACCELERATION
            && mAngularVelocityVector.length2() > 1e-5f))
      {
         if (pLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            LogDeadReckonStarted(unclampedTranslation, rot, pLogger);

         // if we got an update, then we need to recalculate our smoothing
         if (IsUpdated())
         {
            CalculateSmoothingTimes(xform);

//            // If doing Cubic splines, we have to pre-compute some values
//            if (mUseCubicSplineTransBlend && GetDeadReckoningAlgorithm() == DeadReckoningAlgorithm::VELOCITY_AND_ACCELERATION)
//            {  // Use Accel
//               //mTranslation.RecomputeTransSplineValues(mTranslation.mAcceleration);
//            }
//            else if (mUseCubicSplineTransBlend) // No accel
//            {
//               osg::Vec3 zeroAccel;
//               mTranslation.RecomputeTransSplineValues(zeroAccel);
//            }

            if (pLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            {
               std::ostringstream ss;
               ss << "Actor " << txable.GetUniqueId() << " - " << txable.GetName() << " got an update " << std::endl
                  << "      Rotation \"" << mLastRotation  << "\" " << std::endl
                  << "      Position \"" << mTranslation.mLastValue << "\" " << std::endl;
               pLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, ss.str().c_str());
            }
         }

         // RESOLVE ROTATION
         DeadReckonRotation(xform);

         // POSITION SMOOTHING
         bool useAcceleration = GetDeadReckoningAlgorithm() == DeadReckoningAlgorithm::VELOCITY_AND_ACCELERATION;
         mTranslation.DeadReckonPosition(pos, pLogger, txable,
            useAcceleration, mCurTimeDelta);
         xform.SetTranslation(pos);

         returnValue = true;
      }
      else
      {
         mTranslation.mPreviousInstantVel.set(0.0f, 0.0f, 0.0f);

         if (pLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            pLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Entity does not need to be Dead Reckoned");
      }

      return returnValue;
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningActorComponent::CalculateSmoothingTimes(const dtCore::Transform& xform)
   {
      // Dev Note - When the blend time changes drastically (ex 0.1 one time then 
      // 1.0 the next), it can inject significant issues when DR'ing - whether with 
      // catmull-rom/Bezier splines or linear. Recommended use case is for mAlwaysUseMaxSmoothingTime = true

      // ROTATION
      if (mUseFixedSmoothingTime)
      {
         mRotationEndSmoothingTime = GetFixedSmoothingTime();
      }
      else 
      {
         mRotationEndSmoothingTime = GetMaxRotationSmoothingTime();
         // Use our avg update time if it's smaller than our max
         if (GetMaxRotationSmoothingTime() > mAverageTimeBetweenRotationUpdates)
            mRotationEndSmoothingTime = mAverageTimeBetweenRotationUpdates;

         // Way-Out-Of-Bounds check For angular acceleration. 
         if (GetDeadReckoningAlgorithm() == DeadReckoningAlgorithm::VELOCITY_AND_ACCELERATION &&
            (mAngularVelocityVector.length2() * (mRotationEndSmoothingTime * mRotationEndSmoothingTime)) <
            0.1 * ((mLastQuatRotation-mCurrentDeadReckonedRotation).length2()))
         {
            mRotationEndSmoothingTime = std::min(1.0f, mRotationEndSmoothingTime);
            //mRotationEndSmoothingTime = std::min(1.0f, mAverageTimeBetweenRotationUpdates);
         }
      }

      // TRANSLATION
      if (mUseFixedSmoothingTime)
      {
         mTranslation.mEndSmoothingTime = GetFixedSmoothingTime();
      }
      else 
      {
         mTranslation.mEndSmoothingTime = GetMaxTranslationSmoothingTime();
         // Use our avg update time if it's smaller than our max
         if (GetMaxTranslationSmoothingTime() > mTranslation.mAvgTimeBetweenUpdates)
         {
            mTranslation.mEndSmoothingTime = mTranslation.mAvgTimeBetweenUpdates;
         }

         osg::Vec3 pos;
         xform.GetTranslation(pos);

         //Order of magnitude check - if the entity could not possibly get to the new position
         // in max smoothing time based on the magnitude of it's velocity, then smooth quicker (ie 1 second).
         if (mTranslation.mLastVelocity.length2() * (mTranslation.mEndSmoothingTime*mTranslation.mEndSmoothingTime) 
            < (mTranslation.mLastValue - pos).length2() )
         {
            mTranslation.mEndSmoothingTime = std::min(1.0f, mTranslation.mEndSmoothingTime);
         }
      }
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningActorComponent::DeadReckonRotation(dtCore::Transform& xform)
   {
      osg::Quat newRot;
      osg::Quat drQuat = mLastQuatRotation; // velocity only just uses the last.
      bool isRotationChangedByAccel = false;
      osg::Quat startRotation = mRotQuatBeforeLastUpdate;

      if (!mRotationResolved) // mRotationResolved is never set when using Accel
      {

         // For vel and Accel, we use the angular velocity to compute a dead reckoning matrix to slerp to
         // assuming that we have an angular velocity at all...
         if (GetDeadReckoningAlgorithm() == DeadReckoningAlgorithm::VELOCITY_AND_ACCELERATION &&
            mAngularVelocityVector.length2() > 1e-6)
         {
            // if we're here, we had some sort of change, however small
            isRotationChangedByAccel = true;

            // Compute The change in the rotation based Dead Reckoning matrix
            // The Dead Reckoning Matrix
            osg::Matrix angularRotation;
            float actualRotationTime = std::min(mRotationElapsedTimeSinceUpdate, mRotationEndSmoothingTime);

            osg::Vec3 angVelAxis(mAngularVelocityVector);
            float angVelMag = angVelAxis.normalize(); // returns the length
            // rotation around the axis is magnitude of ang vel * time.
            float rotationAngle = angVelMag * actualRotationTime;
            osg::Quat rotationFromAngVel(rotationAngle, angVelAxis);
            
            // Expected DR'ed rotation - Take the last rot and add the change over time
            drQuat = rotationFromAngVel * mLastQuatRotation; // The current DR'ed rotation
            // Previous DR'ed rotation - same, but uses where we were before the last update, so we can smooth it out...
            startRotation = rotationFromAngVel * mRotQuatBeforeLastUpdate; // The current DR'ed rotation
         }

         // If there is a difference in the rotations and we still have time to smooth, then
         // slerp between the two quats: 1) the old rotation plus the expected change using angular
         //    velocity and 2) the desired new rotation
         if ((mRotationEndSmoothingTime > 0.0f) && (mRotationElapsedTimeSinceUpdate <  mRotationEndSmoothingTime))
         {
            float smoothingFactor = mRotationElapsedTimeSinceUpdate/mRotationEndSmoothingTime;
            dtUtil::Clamp(smoothingFactor, 0.0f, 1.0f);
            newRot.slerp(smoothingFactor, startRotation, drQuat);
         }
         else // Either smoothing time is done or the current rotation equals the desired rotation
         {
            newRot = drQuat;
            mRotationResolved = !isRotationChangedByAccel; //true;
         }

         // we finished DR, so update the rotation values on the helper and transform
         xform.SetRotation(newRot);
         mCurrentDeadReckonedRotation = newRot;
         xform.GetRotation(mCurrentAttitudeVector);
      }
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningActorComponent::LogDeadReckonStarted(osg::Vec3& unclampedTranslation, osg::Matrix& rot, dtUtil::Log* pLogger)
   {
      std::ostringstream ss;
      ss << "Actor passed optimization checks: fully dead-reckoning actor.\n"
         << "  mLastTranslation:                     " << mTranslation.mLastValue << std::endl
         << "  unclampedTranslation:                 " << unclampedTranslation << std::endl
         << "  mLastVelocity.length2():     " << mTranslation.mLastVelocity.length2() << std::endl
         << "  mAccelerationVector.length2(): " << mTranslation.mAcceleration.length2() << std::endl
         << "  rot Matrix is: " << rot << std::endl
         << "  mLastRotationMatrix is: " << mLastRotationMatrix << std::endl;

      if (!mRotationResolved)
      {
         ss << "rot Matrix is: " << rot << std::endl;
         ss << "mLastRotationMatrix is: " << mLastRotationMatrix << std::endl;
      }

      pLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, ss.str().c_str());
   }

   ////////////////////////////////////////////////////////////////////////////////////
   void DeadReckoningActorComponent::SetInternalLastKnownRotationInXYZ(const osg::Vec3& vec)
   {
      SetLastKnownRotation(osg::Vec3(vec[2], vec[0], vec[1]));
   }

   ////////////////////////////////////////////////////////////////////////////////////
   osg::Vec3 DeadReckoningActorComponent::GetInternalLastKnownRotationInXYZ() const
   {
      const osg::Vec3& result = GetLastKnownRotation();
      return osg::Vec3(result[1], result[2], result[0]);
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<dtCore::ActorProperty> DeadReckoningActorComponent::GetDeprecatedProperty(const std::string& name)
   {
      static const dtUtil::RefString DEADRECKONING_GROUP = "Dead Reckoning";
      dtCore::RefPtr<dtCore::ActorProperty> result;
      if (name == PROPERTY_FLYING)
      {
         result = new dtCore::BooleanActorProperty(name, "Should Not Follow the Ground",
            dtCore::BooleanActorProperty::SetFuncType(this, &DeadReckoningActorComponent::SetFlyingDeprecatedProperty),
            dtCore::BooleanActorProperty::GetFuncType(this, &DeadReckoningActorComponent::IsFlyingDeprecatedProperty),
            "DEPRECATED - USE GROUNDCLAMPTYPE INSTEAD! ", DEADRECKONING_GROUP);
      }
      return result;
   }

   //////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////


   //////////////////////////////////////////////////////////////////////
   DeadReckoningActorComponent::DRVec3Util::DRVec3Util()
      : mLastUpdatedTime(0.0)
      , mAvgTimeBetweenUpdates(0.0f)
      , mMaxSmoothingTime(DEFAULT_MAX_SMOOTHING_TIME_POS)
      , mElapsedTimeSinceUpdate(0.0f)
      , mEndSmoothingTime(0.0f)
      , mInitialized(false)
      , mUpdated(false)
   {

   }

   //////////////////////////////////////////////////////////////////////
   DeadReckoningActorComponent::DRVec3Util::~DRVec3Util() 
   { 
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningActorComponent::DRVec3Util::SetLastKnownTranslation(const osg::Vec3& vec)
   {
      if (mInitialized)
      {
         mValueBeforeLastUpdate = mCurrentDeadReckonedValue;
      }
      else
      {
         mValueBeforeLastUpdate = vec;
      }

      mInitialized = true;
      mLastValue = vec;
      mElapsedTimeSinceUpdate = 0.0;
      mUpdated = true;
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningActorComponent::DRVec3Util::SetLastKnownVelocity(const osg::Vec3& vec)
   {
      // Clamp the instant velocity to never be more than 4x the last velocity. 
      // This prevents a rare issue that occurs when toggling between 
      // different DR modes where the values are bogus and it shoots WAY off. 
      float lengthInstantVel2 = mPreviousInstantVel.length2();
      float lengthLastVelocity2 = mLastVelocity.length2();
      if (lengthInstantVel2 > 0.0001f && lengthInstantVel2 > 16.0f * lengthLastVelocity2)
      {
         // Cut it down to twice the magnitude of the new velocity at most.
         mVelocityBeforeLastUpdate = mPreviousInstantVel * 2.0f * std::sqrt(lengthLastVelocity2 / lengthInstantVel2);
      }
      else 
      {
         mVelocityBeforeLastUpdate = mPreviousInstantVel; 
      }


      mLastVelocity = vec;
      mElapsedTimeSinceUpdate = 0.0;
      // If velocity is updated, the effect is the same as if the trans was updated
      mUpdated = true;
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningActorComponent::DRVec3Util::SetLastUpdatedTime(double newUpdatedTime)
   {
      //the average of the last average and the current time since an update.
      float timeDelta = float(newUpdatedTime - mLastUpdatedTime);
      mAvgTimeBetweenUpdates = 0.5f * (timeDelta + mAvgTimeBetweenUpdates);
      mLastUpdatedTime = newUpdatedTime;
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningActorComponent::DRVec3Util::DeadReckonPosition
      (osg::Vec3& pos, dtUtil::Log* pLogger, dtCore::Transformable& txable,
         bool useAcceleration, float curTimeDelta)
   {
      // Do we just need to project or do we need more smoothing?
      bool pastTheSmoothingTime = (mEndSmoothingTime <= 0.0f) || 
         (mElapsedTimeSinceUpdate >= mEndSmoothingTime);

      if (pastTheSmoothingTime) 
      {  
         // no need to blend, so just project forward using trivial equation for motion. 
         // Simple formula - P' = P + V*T + 1/2*A*T
         osg::Vec3 accelerationEffect;
         if (useAcceleration)
         {
            accelerationEffect = mAcceleration * 0.5f * mElapsedTimeSinceUpdate * mElapsedTimeSinceUpdate;
         }
         pos = mLastValue + mLastVelocity * mElapsedTimeSinceUpdate + accelerationEffect;

      }
      // Still Smoothing - PROJECTIVE VELOCITY BLENDING
      else 
      {
         DeadReckonUsingLinearBlend(pos, pLogger, txable, useAcceleration);
      }

      // Compute our instantaneous velocity for this frame == change in pos / time. Used when we get a new vel update.
      if (curTimeDelta > 0.0f) // if delta <= 0 then just use prev values
      {
         osg::Vec3 instantVel = (pos - mCurrentDeadReckonedValue) / curTimeDelta;
         mPreviousInstantVel = instantVel;
      }

      mCurrentDeadReckonedValue = pos;

      // DEBUG STUFF
      if (pLogger != NULL && pLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         std::ostringstream ss;
         ss << "Actor " << txable.GetUniqueId() << " - " << txable.GetName() << " current pos "
            << "\"" << pos << "\", temp\"" << mLastUpdatedTime + mElapsedTimeSinceUpdate << "\"";
         pLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, ss.str().c_str());
      }
   }

   //////////////////////////////////////////////////////////////////////
   void DeadReckoningActorComponent::DRVec3Util::DeadReckonUsingLinearBlend
      (osg::Vec3& pos, dtUtil::Log* pLogger, dtCore::Transformable& txable, bool useAcceleration)
   {
      float smoothingFactor = mElapsedTimeSinceUpdate/mEndSmoothingTime;

      // This DR is very simple. Use velocity/Accel to project forward from published loc and 
      // from our last actual DR pos, then blend between them over time. 
      // For more information about this algorithm, see the article in Game Engine Gems 2 (Mar '11)

      // COMPUTE LAST KNOWN CHANGE - as if we just used the best known position & velocity.
      osg::Vec3 lastKnownPosChange = mLastValue + mLastVelocity * mElapsedTimeSinceUpdate; // Add Accel later. 

      // COMPUTE BLENDED VELOCITY - Lerp the two velocities and use that for movement. 
      // This majorly reduces the oscillations. 
      osg::Vec3 mBlendedVelocity = mVelocityBeforeLastUpdate + 
         (mLastVelocity - mVelocityBeforeLastUpdate) * smoothingFactor;
      osg::Vec3 velBlendedPos = mValueBeforeLastUpdate + mBlendedVelocity * mElapsedTimeSinceUpdate;// Add Accel later;

      // BLEND THE TWO - lerp between the last known and blended velocity 
      pos = velBlendedPos + (lastKnownPosChange - velBlendedPos) * smoothingFactor;
      // ADD ACCEL - do at end because it applies to both projections anyway.
      if (useAcceleration)
      {
         pos += mAcceleration * 0.5f * mElapsedTimeSinceUpdate * mElapsedTimeSinceUpdate;
      }

      if (pLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         std::ostringstream ss;
         ss << "Actor \"" << txable.GetUniqueId() << " - " << txable.GetName() << "\" has pos " << "\"" << pos << "\"";
         pLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, ss.str().c_str());
      }
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////////////
   // Splines (Below) don't work very well with dead-reckoning, and they are much more expensive mathematically.
   ////////////////////////////////////////////////////////////////////////////////////////////////////////

   //////////////////////////////////////////////////////////////////////
//   void DeadReckoningActorComponent::DRVec3Util::DeadReckonUsingSplines
//      (osg::Vec3& pos, dtUtil::Log* pLogger, dtCore::Transformable& txable)
//   {
//      //////////////////////////////////////////////////
//      // With cubic splines, we work with 4 points and several pre-computed values
//      // Then, we interpret along the cubic spline based on time T.
//      // See RecomputeTransSplineValues() for the actual algorithm
//      //////////////////////////////////////////////////
//
//      // The formula for X, Y, and Z is ... x = A*t^3 + B*t^2 + C*t + D.
//      // Note - t is normalized between 0 and 1.
//      // Note - Acceleration is accounted for in RecomputeTransSplineValues()
//      // Note - if mTranslationEndSmoothingTime changes often, this may cause anomalies
//      // due to the velocity of the changes in T. Set mRotationEndSmoothingTime to improve.
//      float timeT = mElapsedTimeSinceUpdate/(mEndSmoothingTime);
//      float timeTT = timeT * timeT;
//      float timeTTT = timeTT * timeT;
//
//      // The following is for Bezier Cubic Splines
//      float timeTInverse = (1.0f - timeT);
//      float timeTTInverse = timeTInverse * timeTInverse;
//      float timeTTTInverse = timeTInverse * timeTTInverse;
//      pos.x() = timeTTTInverse * mPosSplineXA + 3.0f * timeTTInverse * timeT * mPosSplineXB +
//         3.0f * timeTInverse * timeTT * mPosSplineXC + timeTTT * mPosSplineXD;
//      pos.y() = timeTTTInverse * mPosSplineYA + 3.0f * timeTTInverse * timeT * mPosSplineYB +
//         3.0f * timeTInverse * timeTT * mPosSplineYC + timeTTT * mPosSplineYD;
//      pos.z() = timeTTTInverse * mPosSplineZA + 3.0f * timeTTInverse * timeT * mPosSplineZB +
//         3.0f * timeTInverse * timeTT * mPosSplineZC + timeTTT * mPosSplineZD;
//
//      // The following is left for testing with Catmull-Rom Cubic Splines to DR the trans.
//      /*
//      pos.x() = mPosSplineXA * timeTTT + mPosSplineXD +
//         mPosSplineXB * timeTT + mPosSplineXC * timeT;
//      pos.y() = mPosSplineYA * timeTTT + mPosSplineYD +
//         mPosSplineYB * timeTT + mPosSplineYC * timeT;
//      pos.z() = mPosSplineZA * timeTTT + mPosSplineZD +
//         mPosSplineZB * timeTT + mPosSplineZC * timeT;
//      */
//
//      if (pLogger != NULL && pLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
//      {
//         std::ostringstream ss;
//         ss << "Actor \"" << txable.GetUniqueId() << " - " << txable.GetName() << "\" has pos " << "\"" << pos << "\"";
//         pLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, ss.str().c_str());
//      }
//
//      // Note - at the end of the bezier cubic spline, it should match the mLastVelocity.
//   }
//
//
//   //////////////////////////////////////////////////////////////////////
//   void DeadReckoningActorComponent::DRVec3Util::RecomputeTransSplineValues(const osg::Vec3& currentAccel)
//   {
//      //const osg::Vec3& transBeforeLastUpdate, const osg::Vec3& velBeforeLastUpdate,
//      //   const osg::Vec3& lastTrans, const osg::Vec3& lastVel, float transEndSmoothingTime,;
//      //mTranslation.mValueBeforeLastUpdate,
//      //   mTranslation.mVelocityBeforeLastUpdate, mTranslation.mLastValue,
//      //   mTranslation.mLastVelocity, mTranslation.mEndSmoothingTime, ;;
//
//      osg::Vec3 distanceFromAcceleration = currentAccel * 0.5f * mEndSmoothingTime * mEndSmoothingTime;
//      osg::Vec3 projectForwardDistance = distanceFromAcceleration + mLastVelocity * mEndSmoothingTime;
//      osg::Vec3 splineEndLocation = mLastValue + projectForwardDistance;
//
//      ///////////////////////////////////////////////////
//      // CUBIC BEZIER SPLINE IMPLEMENTATION
//      float velNormalizer = (mEndSmoothingTime > 0.0f) ? 1.0 * mEndSmoothingTime : 1.0f;
//      osg::Vec3 coord0; // Start pos
//      osg::Vec3 coord1; // First Control - Start Pos + 1/3 velBeforeLastUpdate
//      osg::Vec3 coord3; // End Pos
//      osg::Vec3 coord2; // Second Control - End Pos - 1/3 lastvel
//      coord0 = mValueBeforeLastUpdate;
//      coord1 = mValueBeforeLastUpdate + (mVelocityBeforeLastUpdate * velNormalizer) * 0.33333f;
//      coord3 = splineEndLocation; //mValueBeforeLastUpdate + projectForwardDistance;
//      osg::Vec3 estimatedVel = mLastVelocity + currentAccel * mEndSmoothingTime;
//      coord2 = coord3 - (estimatedVel * velNormalizer * 0.33333f);
//
//      // Note, the use XA, XB, XC, is no longer directly needed, but is setup that way
//      // to support multiple algorithms... (see below)
//      mPosSplineXA = coord0.x();
//      mPosSplineXB = coord1.x();
//      mPosSplineXC = coord2.x();
//      mPosSplineXD = coord3.x();
//
//      mPosSplineYA = coord0.y();
//      mPosSplineYB = coord1.y();
//      mPosSplineYC = coord2.y();
//      mPosSplineYD = coord3.y();
//
//      mPosSplineZA = coord0.z();
//      mPosSplineZB = coord1.z();
//      mPosSplineZC = coord2.z();
//      mPosSplineZD = coord3.z();
//
//   }

}
