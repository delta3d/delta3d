/* -*-c++-*-
 * dtPhysics
 * Copyright 2010, Alion Science and Technology
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
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 * 
 * David Guthrie
 */

#ifndef PHYSICSACTORCOMPONENT_H_
#define PHYSICSACTORCOMPONENT_H_

#include <dtPhysics/physicsexport.h>
#include <dtPhysics/physicsobject.h>
#include <dtPhysics/physicstypes.h>
#include <dtPhysics/primitivetype.h>
#include <dtPhysics/collisioncontact.h>
#include <dtPhysics/action.h>
#include <dtPhysics/raycast.h>

#include <dtGame/actorcomponent.h>

#include <dtCore/uniqueid.h>
#include <dtCore/transformable.h>
#include <dtUtil/functor.h>
#include <dtUtil/refstring.h>

namespace dtCore
{
   class DeltaDrawable;
}

namespace dtCore
{
   class BaseActorObject;
}

namespace dtGame
{
   class GameActorProxy;
}

namespace dtPhysics
{
   class MaterialActor;

   /////////////////////////////////////////////////////////////////////////////
   // Class:
   /////////////////////////////////////////////////////////////////////////////
   class DT_PHYSICS_EXPORT PhysicsActComp: public dtGame::ActorComponent
   {
      public:
         static const dtGame::ActorComponent::ACType TYPE;

         static const dtUtil::RefString PROPERTY_PHYSICS_NAME;
         static const dtUtil::RefString PROPERTY_PHYSICS_MASS;
         static const dtUtil::RefString PROPERTY_PHYSICS_DIMENSIONS;
         static const dtUtil::RefString PROPERTY_COLLISION_GROUP;
         static const dtUtil::RefString PROPERTY_MATERIAL_ACTOR;
         static const dtUtil::RefString PROPERTY_AUTO_CREATE;

         typedef dtUtil::Functor<void, TYPELIST_0()> UpdateCallback;
         typedef dtUtil::Functor<void, TYPELIST_1(float)> ActionUpdateCallback;
         typedef std::vector<dtCore::RefPtr<PhysicsObject> > PhysicsObjectArray;

      public:
         // constructors
         PhysicsActComp();

         /// set the name of this set of physics objects, 1 container class
         void SetName(const dtUtil::RefString& n);

         /// @return the name of the helper
         const dtUtil::RefString& GetName() const;

         /// add a physics object to this helper. Setting makeMain to true will force it to be the main physics object.
         void AddPhysicsObject(PhysicsObject&, bool makeMain = false);

         /// @return the first, or main physics object
         const PhysicsObject* GetMainPhysicsObject() const;

         /// @return the first, or main physics object
         PhysicsObject* GetMainPhysicsObject();

         /// get a physics object if it exists
         const PhysicsObject* GetPhysicsObject(const std::string& name) const;

         /// get a physics object if it exists
         PhysicsObject* GetPhysicsObject(const std::string& name);

         /// remove a physics object from the helper, dont need it anymore
         void RemovePhysicsObject(const std::string& name);

         /// remove a physics object from the helper, dont need it anymore
         void RemovePhysicsObject(const PhysicsObject& objectToRemove);

         /// clear all the physics objects from the helper
         void ClearAllPhysicsObjects();

         void GetAllPhysicsObjects(std::vector<PhysicsObject*>& toFill);

         /**
          * This virtual method allows the user to move a complete set of perhaps
          * even jointed bodies based on this given transform.  It can be overridden
          * to support whatever system you need to to make the bodies warp to a new position
          * correctly.  You may also chose to zero out some forces, reset some states, etc.
          *
          * @note By default it just sets the transform on the main physics object.
          */
         virtual void SetMultiBodyTransform(const TransformType& xform);

         /**
          * @return the multi-body transform, the origin of this group of bodies.
          * @note By default it just gets the transform on the main physics object.
          */
         virtual void GetMultiBodyTransform(TransformType& xform);

         /**
          * Like SetMultiBodyTransform, but this one should work using the SetTransfromAsVisual.
          * This one will iterate over all the physics object and call SetTransfromAsVisual
          * by default.  Because the developer would have control over that transform, in most
          * cases this method should not need to be overridden, at least not as often
          * as SetMultiBodyTransform unless the bodies are moving independent of each other,
          * you want to reset some other state, or you chose not to implement it
          * using the standard as visual calls.
          */
         virtual void SetMultiBodyTransformAsVisual(const TransformType& xform);

         /**
          * By default just returns the result of GetTransformAsVisual on the main
          * physics object. Should be the inverse of SetMultiBodyTransformAsVisual.
          */
         virtual void GetMultiBodyTransformAsVisual(TransformType& xform);

         template <typename Func>
         void ForEachPhysicsObject(Func func)
         {
            std::for_each(mPhysicsObjects.begin(), mPhysicsObjects.end(), func);
         }

         /**
          * Called when the this is removed from the physics component.  It clears all physics objects and removes
          * all the callbacks into the physics engine.
          */
         virtual void CleanUp();

         /*virtual*/ void OnEnteredWorld();
         /*virtual*/ void OnRemovedFromWorld();

         /*virtual*/ void OnAddedToActor(dtCore::BaseActorObject& /*actor*/);
         /*virtual*/ void OnRemovedFromActor(dtCore::BaseActorObject& /*actor*/);

         void RegisterWithGMComponent();
         void UnregisterWithGMComponent();

         bool IsPrePhysicsCallbackValid() const;
         bool IsPostPhysicsCallbackValid() const;
         bool IsActionCallbackValid() const;

         //callbacks
         void SetPrePhysicsCallback(const UpdateCallback& uc);
         void SetPostPhysicsCallback(const UpdateCallback& uc);
         void SetActionUpdateCallback(const ActionUpdateCallback& uc);

         // call the call backs
         void PrePhysicsUpdate();
         void PostPhysicsUpdate();

         /**
          * Action updates are called on the physics thread either before the full update or between each substep
          * depending on which physics engine is being used.  This allows for both offloading physics code to another thread,
          * and running physics code at the full rate of the physics engine.
          */
         void ActionUpdate(Real dt);

         /// set and get for material properties
         void SetMaterialActor(const dtCore::UniqueId& id);

         /// set and get for material properties
         const dtCore::UniqueId& GetMaterialActor() const;

         // Looks up the material actor object using the stored id.
         const MaterialActor* LookupMaterialActor();

         /// Changes the configured mass.  This is for reference only so that code may be data driven.
         void SetMass(Real mass);
         /// @return the configured mass.  This is for reference only so that code may be data driven.
         Real GetMass() const;

         /// Changes the configured collision dimensions.  This is for reference only so that code may be data driven.
         void SetDimensions(const VectorType& dim);
         /// @return the configured collision dimensions.  This is for reference only so that code may be data driven.
         const VectorType& GetDimensions() const;

         /// @return the collision group for reference in code only.
         CollisionGroup GetDefaultCollisionGroup() const;
         /// Sets a collision group for reference in code only.
         void SetDefaultCollisionGroup(CollisionGroup group);

         /// @return the auto create value
         bool GetAutoCreateOnEnteringWorld() const;
         /// Initializes all the physics geometry when the object enters the world when true.
         void SetAutoCreateOnEnteringWorld(bool);

         void SetDefaultPrimitiveType(PrimitiveType& p);
         PrimitiveType& GetDefaultPrimitiveType() const;

         // build property maps, for editor.
         virtual void BuildPropertyMap();

         /// For now, this is used to make dtPhysX properties map in.
         virtual dtCore::RefPtr<dtCore::ActorProperty> GetDeprecatedProperty(const std::string& name);

         //////////////////////////////////////////////////////////////////////////////////////
         //                                    Utilities                                     //
         //////////////////////////////////////////////////////////////////////////////////////

         /**
          * trace a ray output all of the hits.
          */
         void TraceRay(RayCast& ray, std::vector<RayCast::Report>& hits);

         /**
          * trace a ray output the closest hits.
          * @return the range of the closest hit that is not one of the physics objects in this helper or 0.0 if nothing is hit.
          */
         float TraceRay(RayCast& ray, RayCast::Report& report);

         /**
          * Trace in a direction from a location and get the closest hit. Returns
          * the distance to the hit. If distance > 0.0, then outPoint will have full details.
          * Pass in the group flags you want to consider.
          * @return the closest hit that is not one of the physics objects is this helper.
          */
         float TraceRay(const VectorType& location,
            const VectorType& direction , VectorType& outPoint, CollisionGroupFilter groupFlags);

         /// @see TraceRay
         float FindClosestIntersectionUsingDirection(const VectorType& location,
                  const VectorType& direction , VectorType& outPoint, CollisionGroupFilter groupFlags)
         {
            return TraceRay(location, direction, outPoint, groupFlags);
         }

      protected:
         ~PhysicsActComp();

         /**
          * If you don't have a prephysics update, it calls this.
          */
         virtual void DefaultPrePhysicsUpdate();
         /**
          * If you don't have a postphysics update, it calls this.
          */
         virtual void DefaultPostPhysicsUpdate();

         //For the actor property
         void SetNameByString(const std::string& name);
         //For the actor property
         const std::string& GetNameAsString() const;

      private:

         /// name of the physics helper
         dtUtil::RefString mName;

         dtCore::UniqueId mMaterialActorId;

         /// The configured mass of the actor
         Real mMass;

         CollisionGroup mDefaultCollisionGroup;
         PrimitiveType* mDefaultPrimitiveType;

         /// The configured collision dimensions of the actor
         VectorType mDimensions;

         /// all of our objects contained by this helper
         PhysicsObjectArray mPhysicsObjects;

         /// the call back for the pre physics update
         UpdateCallback mPrePhysicsUpdate;

         /// the call back for the post physics update
         UpdateCallback mPostPhysicsUpdate;

         /// Called each frame by the physics engine on the physics thread
         ActionUpdateCallback mActionUpdate;

         dtCore::RefPtr<Action> mHelperAction;

         dtCore::ObserverPtr<dtCore::Transformable> mCachedTransformable;

         bool mAutoCreateOnEnteringWorld;
         bool mIsRemote;

         /// hiding copy constructor and operator=
         PhysicsActComp(const PhysicsActComp&);
         /// hiding copy constructor and operator=
         const PhysicsActComp& operator=(const PhysicsActComp&);

   };

} // namespace


#endif /* PHYSICSACTORCOMPONENT_H_ */
