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
#include <dtPhysics/jointdesc.h>
#include <dtPhysics/transformjointupdater.h>

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
   // TYPE DEFINITIONS
   /////////////////////////////////////////////////////////////////////////////
   typedef dtCore::RefPtr<dtPhysics::PhysicsObject> PhysicsObjectPtr;
   typedef std::vector<PhysicsObjectPtr> PhysicsObjectArray;



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_PHYSICS_EXPORT PhysicsActComp: public dtGame::ActorComponent
   {
      public:
         static const dtGame::ActorComponent::ACType TYPE;

         static const dtUtil::RefString PROPERTY_PHYSICS_NAME;
         static const dtUtil::RefString PROPERTY_PHYSICS_MASS;
         static const dtUtil::RefString PROPERTY_PHYSICS_OBJECT;
         static const dtUtil::RefString PROPERTY_PHYSICS_OBJECT_ARRAY;
         static const dtUtil::RefString PROPERTY_COLLISION_GROUP;
         static const dtUtil::RefString PROPERTY_MATERIAL_ACTOR;
         static const dtUtil::RefString PROPERTY_AUTO_CREATE;

         typedef dtUtil::Functor<void, TYPELIST_0()> UpdateCallback;
         typedef dtUtil::Functor<void, TYPELIST_1(float)> ActionUpdateCallback;

      public:
         // constructors
         PhysicsActComp(const dtGame::ActorComponent::ACType& type = TYPE);

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
          * Method used by the physics object array property to assign
          * an object at a specified index.
          */
         void SetPhysicsObjectByIndex(unsigned index, PhysicsObject* obj);

         /**
          * Method used by the physics object array property to acquire
          * an object at a specified index.
          */
         PhysicsObject* GetPhysicsObjectByIndex(unsigned index);

         /**
          * Method used by the physics object array property to create
          * a new object at a specified index.
          */
         void InsertNewPhysicsObject(unsigned index);

         /**
          * Method used by the physics object array property to remove
          * an object at a specified index.
          */
         void RemovePhysicsObjectByIndex(unsigned index);

         /**
          * Method used by the physics object array property to determine
          * the number of objects in the array.
          */
         size_t GetPhysicsObjectCount() const;

         /**
          * List of joint descriptions.
          */
         DT_DECLARE_ARRAY_ACCESSOR(JointDescPtr, Joint, Joints);

         /**
          * List of updater objects to use to update the transforms on nodes.
          */
         DT_DECLARE_ARRAY_ACCESSOR(TransformJointUpdaterPtr, TransformJointUpdater, TransformJointUpdaters);

         /**
          *  Creates a joint object based on the description, sets the parent frame to match that on the given node.
          *  It's templated on the updater type so it can be subclassed, though it must be a subclass of TransformJointUpdater.
          *  This version will add the properties of the updater to the actor
          *  @param rootNode root node is the node on which to stop when computing the local transform of the refNode.
          */
         template<typename UpdaterType /*= TransformJointUpdater*/>
         UpdaterType* CreateJointAndRegisterUpdater(dtPhysics::JointDesc& desc, osg::Transform& refNodeToUpdate, const osg::Node* rootNode = nullptr)
         {
            dtCore::RefPtr<UpdaterType> result;
            palLink* newLink = CreateJoint(desc, &refNodeToUpdate, rootNode);
            if (newLink == nullptr)
               return nullptr;

            result = new UpdaterType(refNodeToUpdate,*newLink);

            AddTransformJointUpdater(result);

            return result;
         }

         /**
          * Computes a matrix that is the offset from the given node with its parent transform and a visual to body transform
          * which offsets the frame to the body transform, rather than what is the visual origin. It does not incorporate any
          * transform on the node itself because that is assumed to be a starting position.  That is, if this frame is used to create a joint,
          * then the joint parent frame would be from the body to the provided node if it was the equivalent of an identity matrix.
          * A scale may be provided, and it will be applied before the visualToBodyTransform, which is assumed to be used unscaled.
          * The scale does NOT create a scaled matrix, since the physics won't like that.  It just multiplies into the translation prior to applying the visualToBodyTransform.  That
          * means that the visualToBodyTransform will be used as entered in the property values, but the visual model can be still scaled without messing
          * up the joints, visually speaking.
          */
         static void ComputeLocalOffsetMatrixForNode(TransformType& frameOut, const osg::Node& node, const TransformType& visualToBodyTransform, const osg::Node* root = nullptr, const osg::Vec3& externalScale = osg::Vec3(1.0f,1.0f,1.0f));

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
         void PrePhysicsUpdate(Real simDt);
         void PostPhysicsUpdate(Real simDt);

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
         const MaterialActor* LookupMaterialActor() { return LookupMaterialActor(GetMaterialActor()); }

         /// Finds a material actor given an id.
         const MaterialActor* LookupMaterialActor(const dtCore::UniqueId& id);

         // Looks up the material actor object by name.
         const MaterialActor* LookupMaterialActor(const std::string& matName);

         /// Changes the configured mass.  This is for reference only so that code may be data driven.
         void SetMass(Real mass);
         /// @return the configured mass.  This is for reference only so that code may be data driven.
         Real GetMass() const;

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

         /**
          * Convenience function to force all physics objects in this actor component to be active or inactive.
          * @see PhysicsObjects::SetActive
          */
         void SetAllActive(bool active);


         // build property maps, for editor.
         virtual void BuildPropertyMap();

         /// For now, this is used to make the old dtPhysX properties map in.
         virtual dtCore::RefPtr<dtCore::ActorProperty> GetDeprecatedProperty(const std::string& name);

         /**
          * This is like the CreateJoint on PhysicsObject except that it looks up the physics objects, and set
          * the joint description body1frame to match the transform in the ref node.
          * @param rootNode the node on which to stop when computing the local transfrom of the ref node.  The root node transform, if it has one, is not included.
          * @param setBody2VisualToBody if true, This code assumes the visual to body transform of body 1 is correct,
          * and if the visual to body transform of body 2 so that it will put it at the joint origin position.
          * That way, if the transform of the system is set, the body will be warped in a reset position.
          */
         palLink* CreateJoint(dtPhysics::JointDesc& desc, osg::Transform* refNode = nullptr, const osg::Node* rootNode = nullptr, bool setBody2VisualToBody = true);

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

      protected:
         ~PhysicsActComp();

         /**
          * If you don't have a prephysics update, and it's remote, it calls this.
          */
         virtual void DefaultPrePhysicsUpdate(Real simDt);
         /**
          * If you don't have a postphysics update, it calls this.
          */
         virtual void DefaultPostPhysicsUpdate(Real simDt);

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

   typedef dtCore::RefPtr<PhysicsActComp> PhysicsActCompPtr;
} // namespace


#endif /* PHYSICSACTORCOMPONENT_H_ */
