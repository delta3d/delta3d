/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation
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
 * Allen Danklefsen
 * David Guthrie
 */

#include <dtPhysics/physicsactcomp.h>
#include <dtPhysics/physicsmaterialactor.h>
#include <dtPhysics/palphysicsworld.h>
#include <dtPhysics/action.h>
#include <dtPhysics/raycast.h>
#include <dtPhysics/palutil.h>
#include <dtGame/gameactor.h>
#include <dtCore/enginepropertytypes.h>

#include <dtPhysics/physicscomponent.h>

#include <dtPhysics/customraycastcallbacks.h>

namespace dtPhysics
{
   const dtGame::ActorComponent::ACType PhysicsActComp::TYPE(new dtCore::ActorType("PhysicsActComp", "ActorComponents",
         "Physics subsystem actor component.  Requires a GM level PhysicsComponent",
         dtGame::ActorComponent::BaseActorComponentType));

   const dtUtil::RefString PhysicsActComp::PROPERTY_PHYSICS_NAME("Physics Name");
   const dtUtil::RefString PhysicsActComp::PROPERTY_PHYSICS_MASS("Physics Mass");
   const dtUtil::RefString PhysicsActComp::PROPERTY_PHYSICS_DIMENSIONS("Physics Dimensions");
   const dtUtil::RefString PhysicsActComp::PROPERTY_COLLISION_GROUP("Default Collision Group");
   const dtUtil::RefString PhysicsActComp::PROPERTY_AUTO_CREATE("Auto-Create Physics Objects");
   const dtUtil::RefString PhysicsActComp::PROPERTY_MATERIAL_ACTOR("Material Actor");


   /////////////////////////////////////////////////////////////////////////////
   class PhysicsActCompAction : public Action
   {
   public:
      PhysicsActCompAction(PhysicsActComp& helper)
   : mHelper(helper)
   {
   }

      virtual void operator()(Real timeStep)
      {
         mHelper.ActionUpdate(timeStep);
      }

      PhysicsActComp& mHelper;
   };

   /////////////////////////////////////////////////////////////////////////////
   PhysicsActComp::PhysicsActComp()
   : dtGame::ActorComponent(PhysicsActComp::TYPE)
   , mMaterialActorId("")
   , mMass(0.0f)
   , mDefaultCollisionGroup(0)
   , mDefaultPrimitiveType(&PrimitiveType::BOX)
   , mAutoCreateOnEnteringWorld(false)
   , mIsRemote(false)
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   PhysicsActComp::~PhysicsActComp()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsActComp::SetName(const dtUtil::RefString& n)
   {
      mName = n;
   }

   /////////////////////////////////////////////////////////////////////////////
   const dtUtil::RefString& PhysicsActComp::GetName() const
   {
      return mName;
   }

   /////////////////////////////////////////////////////////////////////////////
   struct CreateFromPropsPhysObj
   {
      void operator()(dtCore::RefPtr<PhysicsObject>& po)
      {
         if (po->GetMaterial() == NULL)
         {
            const MaterialActor* mat = mPac->LookupMaterialActor();
            if (mat != NULL)
            {
               po->SetMaterial(mat->GetMaterial());
            }
         }
         po->Create();
      }
      PhysicsActComp* mPac;
   };

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsActComp::OnEnteredWorld()
   {
      RegisterWithGMComponent();

      // Initialize the position
      if (!mPrePhysicsUpdate.valid())
      {
         DefaultPrePhysicsUpdate();
      }

      if (GetAutoCreateOnEnteringWorld())
      {
         CreateFromPropsPhysObj creatFunc;
         creatFunc.mPac = this;
         ForEachPhysicsObject(creatFunc);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsActComp::OnRemovedFromWorld()
   {
      UnregisterWithGMComponent();
      CleanUp();
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsActComp::OnAddedToActor(dtCore::BaseActorObject& actor)
   {
      dtCore::Transformable* xformable = NULL;
      actor.GetDrawable(xformable);
      mCachedTransformable = xformable;
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsActComp::OnRemovedFromActor(dtCore::BaseActorObject& actor)
   {
      mCachedTransformable = NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PhysicsActComp::RegisterWithGMComponent()
   {
      PhysicsComponent* comp = NULL;

      dtGame::GameActorProxy* act = NULL;
      GetOwner(act);

      mIsRemote = act->IsRemote();

      act->GetGameManager()->
            GetComponentByName(PhysicsComponent::DEFAULT_NAME, comp);

      if (comp != NULL)
      {
         comp->RegisterActorComp(*this);
      }
      else
      {
         dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
               "Actor \"%s\"\"%s\" unable to find PhysicsComponent.",
               act->GetName().c_str(), act->GetId().ToString().c_str());
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PhysicsActComp::UnregisterWithGMComponent()
   {
      PhysicsComponent* comp = NULL;

      dtGame::GameActorProxy* act = NULL;
      GetOwner(act);

      act->GetGameManager()->
            GetComponentByName(PhysicsComponent::DEFAULT_NAME, comp);

      if (comp != NULL)
      {
         comp->UnregisterActorComp(*this);
      }
      else
      {
         dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
               "Actor \"%s\"\"%s\" unable to find PhysicsComponent.",
               act->GetName().c_str(), act->GetId().ToString().c_str());
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsActComp::BuildPropertyMap()
   {
      static const dtUtil::RefString GROUP("PhysicsActComp");

      AddProperty(new dtCore::ActorIDActorProperty(PROPERTY_MATERIAL_ACTOR, PROPERTY_MATERIAL_ACTOR,
            dtCore::ActorIDActorProperty::SetFuncType(this, &PhysicsActComp::SetMaterialActor),
            dtCore::ActorIDActorProperty::GetFuncType(this, &PhysicsActComp::GetMaterialActor),
            dtUtil::RefString("dtPhysics::MaterialActor"),
            dtUtil::RefString("The actor that defines the material properties of this physics helper."), GROUP));

      static const dtUtil::RefString PROPERTY_PHYSICS_NAME_DESC("The physics reference name.");
      AddProperty(new dtCore::StringActorProperty(PROPERTY_PHYSICS_NAME, PROPERTY_PHYSICS_NAME, //ActorName
            dtCore::StringActorProperty::SetFuncType(this, &PhysicsActComp::SetNameByString),
            dtCore::StringActorProperty::GetFuncType(this, &PhysicsActComp::GetNameAsString),
            PROPERTY_PHYSICS_NAME_DESC, GROUP));

      static const dtUtil::RefString PROPERTY_PHYSICS_MASS_DESC("The physics mass.  This is a configuration value, "
            "and can be use however the developer intends.");
      AddProperty(new dtCore::FloatActorProperty(PROPERTY_PHYSICS_MASS, PROPERTY_PHYSICS_MASS, //ActorName
            dtCore::FloatActorProperty::SetFuncType(this, &PhysicsActComp::SetMass),
            dtCore::FloatActorProperty::GetFuncType(this, &PhysicsActComp::GetMass),
            PROPERTY_PHYSICS_MASS_DESC, GROUP));

      static const dtUtil::RefString PROPERTY_PHYSICS_DIMESNIONS_DESC("The collision dimensions.  This is a configuration value, "
            "and can be use however the developer intends.");
      AddProperty(new dtCore::Vec3ActorProperty(PROPERTY_PHYSICS_DIMENSIONS, PROPERTY_PHYSICS_DIMENSIONS, //ActorName
            dtCore::Vec3ActorProperty::SetFuncType(this, &PhysicsActComp::SetDimensions),
            dtCore::Vec3ActorProperty::GetFuncType(this, &PhysicsActComp::GetDimensions),
            PROPERTY_PHYSICS_DIMESNIONS_DESC, GROUP));

      static const dtUtil::RefString PROPERTY_PHYSICS_COLLISION_GROUP_DESC("The default group to use for the physics "
            "objects.  This is a configuration value, and can be use however the developer intends.");
      AddProperty(new dtCore::IntActorProperty(PROPERTY_COLLISION_GROUP, PROPERTY_COLLISION_GROUP,
            dtCore::IntActorProperty::SetFuncType(this, &PhysicsActComp::SetDefaultCollisionGroup),
            dtCore::IntActorProperty::GetFuncType(this, &PhysicsActComp::GetDefaultCollisionGroup),
            PROPERTY_PHYSICS_COLLISION_GROUP_DESC, GROUP));

      static const dtUtil::RefString PROPERTY_PHYSICS_AUTO_CREATE("Creates and initializes all the physics objects as configured when "
            "the actor enters the world.");
      AddProperty(new dtCore::BooleanActorProperty(PROPERTY_AUTO_CREATE, PROPERTY_AUTO_CREATE,
            dtCore::BooleanActorProperty::SetFuncType(this, &PhysicsActComp::SetAutoCreateOnEnteringWorld),
            dtCore::BooleanActorProperty::GetFuncType(this, &PhysicsActComp::GetAutoCreateOnEnteringWorld),
            PROPERTY_PHYSICS_COLLISION_GROUP_DESC, GROUP));

      //Get properties for the physics objects.
      PhysicsObjectArray::iterator i, iend;
      i = mPhysicsObjects.begin();
      iend = mPhysicsObjects.end();
      for(; i != iend; ++i)
      {
         PhysicsObject& physObj = **i;
         physObj.BuildPropertyMap();
         physObj.ForEachProperty(dtUtil::MakeFunctor(&PropertyContainer::AddProperty, this));
      }
   }

   //////////////////////////////////////////////////////////////////
   dtCore::RefPtr<dtCore::ActorProperty> PhysicsActComp::GetDeprecatedProperty(const std::string& name)
   {
      return NULL;
   }

   //////////////////////////////////////////////////////////////////
   void PhysicsActComp::SetPrePhysicsCallback(const UpdateCallback& uc)
   {
      mPrePhysicsUpdate = uc;
   }

   //////////////////////////////////////////////////////////////////
   void PhysicsActComp::SetPostPhysicsCallback(const UpdateCallback& uc)
   {
      mPostPhysicsUpdate = uc;
   }

   //////////////////////////////////////////////////////////////////
   void PhysicsActComp::SetActionUpdateCallback(const ActionUpdateCallback& uc)
   {
      mActionUpdate = uc;
   }

   //////////////////////////////////////////////////////////////////
   bool PhysicsActComp::IsPrePhysicsCallbackValid() const
   {
      return mPrePhysicsUpdate.valid();
   }

   //////////////////////////////////////////////////////////////////
   bool PhysicsActComp::IsPostPhysicsCallbackValid() const
   {
      return mPostPhysicsUpdate.valid();
   }

   //////////////////////////////////////////////////////////////////
   bool PhysicsActComp::IsActionCallbackValid() const
   {
      return mActionUpdate.valid();
   }

   //////////////////////////////////////////////////////////////////
   void PhysicsActComp::PrePhysicsUpdate()
   {
      if (mPrePhysicsUpdate.valid())
      {
         mPrePhysicsUpdate();
      }
      else if (mIsRemote)
      {
         DefaultPrePhysicsUpdate();
      }


      // Update the action in pre physics for several reasons.
      // Creating the helper and/or setting the callback for the action update may happen
      // in STAGE or another time when the world has not been initalized.
      // When PrePhysicsUpdate is called, that should all be sorted out, and this way
      // any changes will get picked up on the next tick, or immediately if done in prephysics
      if (mActionUpdate.valid())
      {
         if (!mHelperAction.valid())
         {
            mHelperAction = new PhysicsActCompAction(*this);
            PhysicsWorld::GetInstance().AddAction(*mHelperAction);
         }
      }
      else
      {
         if (mHelperAction.valid())
         {
            PhysicsWorld::GetInstance().RemoveAction(*mHelperAction);
            mHelperAction = NULL;
         }
      }
   }

   //////////////////////////////////////////////////////////////////
   void PhysicsActComp::PostPhysicsUpdate()
   {
      if (mPostPhysicsUpdate.valid())
      {
         mPostPhysicsUpdate();
      }
      else if (!mIsRemote)
      {
         DefaultPostPhysicsUpdate();
      }
   }

   //////////////////////////////////////////////////////////////////
   void PhysicsActComp::ActionUpdate(Real dt)
   {
      if (mActionUpdate.valid())
         mActionUpdate(dt);
   }

   //////////////////////////////////////////////////////////////////
   void  PhysicsActComp::SetMaterialActor(const dtCore::UniqueId& id)
   {
      mMaterialActorId = id;
   }

   //////////////////////////////////////////////////////////////////
   const dtCore::UniqueId& PhysicsActComp::GetMaterialActor() const
   {
      return mMaterialActorId;
   }

   //////////////////////////////////////////////////////////////////
   const MaterialActor* PhysicsActComp::LookupMaterialActor()
   {
      const MaterialActor* result = NULL;
      if (!GetMaterialActor().ToString().empty() )
      {
         dtGame::GameActorProxy* gap = NULL;
         GetOwner(gap);
         if (gap != NULL)
         {
            gap->GetGameManager()->FindActorById(GetMaterialActor(), result);
         }
      }
      return result;
   }

   //////////////////////////////////////////////////////////////////
   void PhysicsActComp::SetMass(Real mass)
   {
      mMass = mass;
   }

   //////////////////////////////////////////////////////////////////
   Real PhysicsActComp::GetMass() const
   {
      return mMass;
   }

   //////////////////////////////////////////////////////////////////
   void PhysicsActComp::SetDimensions(const VectorType& dim)
   {
      mDimensions = dim;
   }

   //////////////////////////////////////////////////////////////////
   const VectorType& PhysicsActComp::GetDimensions() const
   {
      return mDimensions;
   }

   //////////////////////////////////////////////////////////////////
   CollisionGroup PhysicsActComp::GetDefaultCollisionGroup() const
   {
      return mDefaultCollisionGroup;
   }

   //////////////////////////////////////////////////////////////////
   void PhysicsActComp::SetDefaultCollisionGroup(CollisionGroup group)
   {
      mDefaultCollisionGroup = group;
   }

   //////////////////////////////////////////////////////////////////
   void PhysicsActComp::SetDefaultPrimitiveType(PrimitiveType& p)
   {
      mDefaultPrimitiveType = &p;
   }

   //////////////////////////////////////////////////////////////////
   PrimitiveType& PhysicsActComp::GetDefaultPrimitiveType() const
   {
      return *mDefaultPrimitiveType;
   }

   //////////////////////////////////////////////////////////////////
   bool PhysicsActComp::GetAutoCreateOnEnteringWorld() const
   {
      return mAutoCreateOnEnteringWorld;
   }

   //////////////////////////////////////////////////////////////////
   void PhysicsActComp::SetAutoCreateOnEnteringWorld(bool autoCreate)
   {
      mAutoCreateOnEnteringWorld = autoCreate;
   }

   //////////////////////////////////////////////////////////////////
   void PhysicsActComp::AddPhysicsObject(PhysicsObject& po, bool makeMain)
   {
      if (po.GetUserData() != NULL)
      {
         // Should this throw an exception?
         LOG_ERROR(std::string("The user data on the passed in physics object nameed \"") +
               po.GetName() +
               "\" is not NULL.  Either it already is added to a helper, "
               "or it has a custom user data. Value will be overwritten.");
      }

      po.SetUserData(this);
      if (makeMain)
      {
         mPhysicsObjects.insert(mPhysicsObjects.begin(), &po);
      }
      else
      {
         mPhysicsObjects.push_back(&po);
      }
   }

   //////////////////////////////////////////////////////////////////
   const PhysicsObject* PhysicsActComp::GetMainPhysicsObject() const
   {
      if (mPhysicsObjects.size() > 0)
      {
         return mPhysicsObjects.front().get();
      }
      else
      {
         return NULL;
      }
   }

   //////////////////////////////////////////////////////////////////
   PhysicsObject* PhysicsActComp::GetMainPhysicsObject()
   {
      if (!mPhysicsObjects.empty())
      {
         return mPhysicsObjects.front().get();
      }
      else
      {
         return NULL;
      }
   }

   //////////////////////////////////////////////////////////////////
   const PhysicsObject* PhysicsActComp::GetPhysicsObject(const std::string& name) const
   {
      PhysicsObjectArray::const_iterator iter, iend;
      iter = mPhysicsObjects.begin();
      iend =  mPhysicsObjects.end();
      for(; iter != iend; ++iter)
      {
         if((*iter)->GetName() == name)
            return (*iter).get();
      }
      return NULL;
   }

   //////////////////////////////////////////////////////////////////
   PhysicsObject* PhysicsActComp::GetPhysicsObject(const std::string& name)
   {
      PhysicsObjectArray::iterator iter, iend;
      iter = mPhysicsObjects.begin();
      iend = mPhysicsObjects.end();
      for(; iter != iend; ++iter)
      {
         if((*iter)->GetName() == name)
            return (*iter).get();
      }
      return NULL;
   }

   //////////////////////////////////////////////////////////////////
   void PhysicsActComp::RemovePhysicsObject(const std::string& name)
   {
      PhysicsObjectArray::iterator iter, iend;
      iter = mPhysicsObjects.begin();
      iend = mPhysicsObjects.end();
      for(; iter != iend; ++iter)
      {
         PhysicsObject& po = **iter;
         if (po.GetName() == name)
         {
            po.SetUserData(NULL);
            mPhysicsObjects.erase(iter);
            return;
         }
      }
   }

   //////////////////////////////////////////////////////////////////
   void PhysicsActComp::RemovePhysicsObject(const PhysicsObject& objectToRemove)
   {
      PhysicsObjectArray::iterator iter, iend;
      iter = mPhysicsObjects.begin();
      iend = mPhysicsObjects.end();
      for(; iter != iend; ++iter)
      {
         PhysicsObject& po = **iter;
         if (&po == &objectToRemove)
         {
            po.SetUserData(NULL);
            mPhysicsObjects.erase(iter);
            return;
         }
      }
   }

   //////////////////////////////////////////////////////////////////
   void PhysicsActComp::ClearAllPhysicsObjects()
   {
      mPhysicsObjects.clear();
   }

   //////////////////////////////////////////////////////////////////
   void PhysicsActComp::GetAllPhysicsObjects(std::vector<PhysicsObject*>& toFill)
   {
      toFill.reserve(mPhysicsObjects.size() + toFill.size());
      PhysicsObjectArray::iterator i, iend;
      i = mPhysicsObjects.begin();
      iend = mPhysicsObjects.end();
      for (; i != iend; ++i)
      {
         toFill.push_back(i->get());
      }
   }

   //////////////////////////////////////////////////////////////////
   void PhysicsActComp::SetMultiBodyTransform(const TransformType& xform)
   {
      PhysicsObject* po = GetMainPhysicsObject();
      if (po != NULL)
      {
         po->SetTransform(xform);
      }
   }

   //////////////////////////////////////////////////////////////////
   void PhysicsActComp::GetMultiBodyTransform(TransformType& xform)
   {
      PhysicsObject* po = GetMainPhysicsObject();
      if (po != NULL)
      {
         po->GetTransform(xform);
      }
   }

   //////////////////////////////////////////////////////////////////
   void PhysicsActComp::SetMultiBodyTransformAsVisual(const TransformType& xform)
   {
      PhysicsObjectArray::iterator i, iend;
      i = mPhysicsObjects.begin();
      iend = mPhysicsObjects.end();
      for (; i != iend; ++i)
      {
         PhysicsObject& po = **i;
         po.SetTransformAsVisual(xform);
      }
   }

   //////////////////////////////////////////////////////////////////
   void PhysicsActComp::GetMultiBodyTransformAsVisual(TransformType& xform)
   {
      PhysicsObject* po = GetMainPhysicsObject();
      if (po != NULL)
      {
         po->GetTransformAsVisual(xform);
      }
   }

   //////////////////////////////////////////////////////////////////
   void PhysicsActComp::CleanUp()
   {
      PhysicsObjectArray::iterator i, iend;
      i = mPhysicsObjects.begin();
      iend = mPhysicsObjects.end();
      for (; i != iend; ++i)
      {
         PhysicsObject* curPo = *i;
         if (curPo->GetBodyWrapper() != NULL)
         {
            if (PhysicsWorld::GetInstance().IsBackgroundUpdateStepRunning())
            {
               LOG_ERROR("Cleaning up physics actor component while physics stepping");
            }
         }
         curPo->CleanUp();
      }

      if (mHelperAction.valid())
      {
         PhysicsWorld::GetInstance().RemoveAction(*mHelperAction);
         mHelperAction = NULL;
      }

      //      std::vector<dtCore::ActorProperty *> propList;
      //      GetPropertyList(propList);
      //      for (unsigned i = 0; i < propList.size(); ++i)
      //      {
      //         RemoveProperty(propList[i]->GetName());
      //      }
   }

   //////////////////////////////////////////////////////////////////
   void PhysicsActComp::SetNameByString(const std::string& name)
   {
      mName = name;
   }

   //////////////////////////////////////////////////////////////////
   const std::string& PhysicsActComp::GetNameAsString() const
   {
      return mName;
   }

   /// hiding copy constructor and operator=
   PhysicsActComp::PhysicsActComp(const PhysicsActComp&)
   : dtGame::ActorComponent(PhysicsActComp::TYPE)
   , mMaterialActorId("")
   , mMass(0.0f)
   , mDefaultCollisionGroup(0)
   , mDefaultPrimitiveType(&PrimitiveType::BOX)
   , mAutoCreateOnEnteringWorld(false)
   , mIsRemote(false)
   {
   }

   /// hiding copy constructor and operator=
   const PhysicsActComp& PhysicsActComp::operator=(const PhysicsActComp&)
   {
      return *this;
   }

   //////////////////////////////////////////////////////////////////
   void PhysicsActComp::TraceRay(RayCast& ray, std::vector<RayCast::Report>& hits)
   {
      // Create a raycast that should ignore this vehicle.
      // Pass in the game actor owner of our physics helper so we don't intersect with ourself!
      FindAllHitsCallback callback(ray.GetDirection().length(), this);
      PhysicsWorld::GetInstance().TraceRay(ray, callback);
      hits = callback.mHits;
   }

   //////////////////////////////////////////////////////////////////
   float PhysicsActComp::TraceRay(RayCast& ray, RayCast::Report& report)
   {
      float closestHitDistance = 0.0f;

      // Create a raycast that should ignore this vehicle.
      // Pass in the game actor owner of our physics helper so we don't intersect with ourself!
      FindClosestHitCallback closestHitCallback(ray.GetDirection().length(), this);
      PhysicsWorld::GetInstance().TraceRay(ray, closestHitCallback);

      if (closestHitCallback.mGotAHit)
      {
         PalRayHitToRayCastReport(report, closestHitCallback.mClosestHit);
         closestHitDistance = closestHitCallback.mClosestHit.m_fDistance;
      }

      // Returns 0 if no hit or distance to closest hit.  Actual hit loc is in outPoint
      return closestHitDistance;
   }

   //////////////////////////////////////////////////////////////////
   float PhysicsActComp::TraceRay(const VectorType& location,
         const VectorType& direction , VectorType& outPoint, CollisionGroupFilter groupFlags)
   {
      dtPhysics::RayCast ray;
      ray.SetOrigin(location);
      ray.SetDirection(direction * 10000.0); // basically, we want it to go out to infinity - should be a parameter probably.
      ray.SetCollisionGroupFilter(groupFlags);

      dtPhysics::RayCast::Report report;

      // Returns 0 if no hit or distance to closest hit.  Actual hit loc is in outPoint
      float result = TraceRay(ray, report);
      if (result > 0.0f)
      {
         outPoint = report.mHitPos;
      }
      return result;
   }

   //////////////////////////////////////////////////////////////////
   void PhysicsActComp::DefaultPrePhysicsUpdate()
   {
      if (!mCachedTransformable.valid())
         return;

      dtCore::Transform xform;
      mCachedTransformable->GetTransform(xform);
      if (xform.IsValid())
      {
         for (unsigned i = 0; i < mPhysicsObjects.size(); ++i)
         {
            dtPhysics::PhysicsObject* physObj = mPhysicsObjects[i];
            bool setTransform = physObj->GetMechanicsType() == MechanicsType::KINEMATIC;

            // It's in efficient to set the position of static objects
            // and it can mess up dynamic objects to set the position every frame
            // so we check to see if you moved it since the last update before setting the value.
            if (!setTransform)
            {
               dtCore::Transform xformP;
               physObj->GetTransformAsVisual(xformP);
               if (!xformP.EpsilonEquals(xform, 0.01))
               {
                  setTransform = true;
               }
            }
            if (setTransform)
               physObj->SetTransformAsVisual(xform);
         }
      }
      else
      {
         BaseActorObject* actor = NULL;
         GetOwner(actor);
         std::string debugInfo("Invalid transform on physics actor component: ");
         if (actor)
         {
            debugInfo += actor->GetName() + " " + actor->GetActorType().GetFullName();
         }
         LOGN_ERROR("physicsactcomp.cpp", "Invalid transform on physics actor component: ");
      }
   }

   //////////////////////////////////////////////////////////////////
   void PhysicsActComp::DefaultPostPhysicsUpdate()
   {
      if (!mCachedTransformable.valid())
         return;
      dtCore::Transform xform;
      dtPhysics::PhysicsObject* physObj = GetMainPhysicsObject();
      if (physObj == NULL || physObj->GetMechanicsType() == MechanicsType::STATIC)
         return;

      physObj->GetTransformAsVisual(xform);

      if (xform.IsValid())
      {
         mCachedTransformable->SetTransform(xform);
      }
      else
      {
         BaseActorObject* actor = NULL;
         GetOwner(actor);
         std::string debugInfo("Invalid transform on physics actor component: ");
         if (actor)
         {
            debugInfo += actor->GetName() + " " + actor->GetActorType().GetFullName();
         }
         LOGN_ERROR("physicsactcomp.cpp", debugInfo);
      }
   }

} // namespace dtPhysics
