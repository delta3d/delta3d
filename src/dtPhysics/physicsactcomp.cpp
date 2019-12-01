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

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtPhysics/physicsactcomp.h>
#include <dtPhysics/physicsmaterialactor.h>
#include <dtPhysics/palphysicsworld.h>
#include <dtPhysics/action.h>
#include <dtPhysics/raycast.h>
#include <dtPhysics/palutil.h>
#include <dtGame/gameactor.h>
#include <dtCore/arrayactorpropertycomplex.h>
#include <dtCore/enginepropertytypes.h>
#include <dtCore/propertycontaineractorproperty.h>

#include <dtPhysics/physicscomponent.h>

#include <dtPhysics/customraycastcallbacks.h>

namespace dtPhysics
{
   /////////////////////////////////////////////////////////////////////////////
   // CONSTANTS
   /////////////////////////////////////////////////////////////////////////////
   const dtUtil::RefString PhysicsActComp::PROPERTY_PHYSICS_NAME("Physics Name");
   const dtUtil::RefString PhysicsActComp::PROPERTY_PHYSICS_MASS("Physics Mass");
   const dtUtil::RefString PhysicsActComp::PROPERTY_PHYSICS_OBJECT("Physics Object");
   const dtUtil::RefString PhysicsActComp::PROPERTY_PHYSICS_OBJECT_ARRAY("Physics Objects");
   const dtUtil::RefString PhysicsActComp::PROPERTY_COLLISION_GROUP("Default Collision Group");
   const dtUtil::RefString PhysicsActComp::PROPERTY_AUTO_CREATE("Auto-Create Physics Objects");
   const dtUtil::RefString PhysicsActComp::PROPERTY_MATERIAL_ACTOR("Material Actor");

   DT_IMPLEMENT_ARRAY_ACCESSOR(PhysicsActComp, dtCore::RefPtr<JointDesc>, Joint, Joints, new JointDesc);
   DT_IMPLEMENT_ARRAY_ACCESSOR_WITH_ON_ADD_REMOVE(PhysicsActComp, TransformJointUpdaterPtr, TransformJointUpdater, TransformJointUpdaters, nullptr, \
         {value->AddProperties(*this, true/*send in partial update*/);\
         dtGame::GameActorProxy* parent = nullptr; \
         GetOwner(parent); \
         if (parent != nullptr) \
         {\
            parent->AddProperty(this->GetProperty(value->GetTranslationPropertyName()));\
            parent->AddProperty(this->GetProperty(value->GetRotationPropertyName()));\
         }\
         },\
         {mTransformJointUpdaters[idx]->RemoveProperties(*this); \
         dtCore::PropertyContainer* parent = nullptr; \
         GetOwner(parent); \
         if (parent != nullptr) \
            mTransformJointUpdaters[idx]->RemoveProperties(*this); }\
         );

   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
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
   PhysicsActComp::PhysicsActComp(const dtGame::ActorComponent::ACType& type)
   : dtGame::ActorComponent(type)
   , mMaterialActorId(false)
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
   void PhysicsActComp::OnEnteredWorld()
   {
      RegisterWithGMComponent();

      // Initialize the position
      if (!mPrePhysicsUpdate.valid())
      {
         DefaultPrePhysicsUpdate(0.00f);
      }

      ForEachPhysicsObject([&](dtCore::RefPtr<dtPhysics::PhysicsObject>& po)
         {
            if (po->GetMaterialId().IsNull())
            {
               po->SetMaterialId(GetMaterialActor());
            }
            if (GetAutoCreateOnEnteringWorld())
            {
               po->Create();
            }
         });
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
      dtCore::Transformable* xformable = nullptr;
      actor.GetDrawable(xformable);
      mCachedTransformable = xformable;
   }

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsActComp::OnRemovedFromActor(dtCore::BaseActorObject& actor)
   {
      mCachedTransformable = nullptr;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PhysicsActComp::RegisterWithGMComponent()
   {
      PhysicsComponent* comp = nullptr;

      dtGame::GameActorProxy* act = nullptr;
      GetOwner(act);

      mIsRemote = act->IsRemote();

      act->GetGameManager()->
            GetComponentByName(PhysicsComponent::DEFAULT_NAME, comp);

      if (comp != nullptr)
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
      PhysicsComponent* comp = nullptr;

      dtGame::GameActorProxy* act = nullptr;
      GetOwner(act);

      act->GetGameManager()->
            GetComponentByName(PhysicsComponent::DEFAULT_NAME, comp);

      if (comp != nullptr)
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
   // HELPER PRED
   struct PropertyNameCollectorPred
   {
      typedef dtCore::RefPtr<dtCore::ActorProperty> PropPtr;
      typedef dtUtil::Functor<void, TYPELIST_1(PropPtr)> VoidPropPtrFunc;

      typedef std::vector<std::string> StrArray;
      StrArray mNames;

      void operator() (PropPtr prop)
      {
         mNames.push_back(prop->GetName());
      }

      VoidPropPtrFunc GetFunc()
      {
         return dtUtil::MakeFunctor(&PropertyNameCollectorPred::operator(), this);
      }
   };

   class PhysicsObjectPropertyContainerActorProperty : public dtCore::PropertyContainerActorProperty<PhysicsObject>
   {
   public:
      typedef dtCore::PropertyContainerActorProperty<PhysicsObject> BaseClass;
      typedef BaseClass::SetFuncType SetFuncType;
      typedef BaseClass::GetFuncType GetFuncType;

      PhysicsObjectPropertyContainerActorProperty(
               const dtUtil::RefString& name,
               const dtUtil::RefString& label,
               SetFuncType set,
               GetFuncType get,
               const dtUtil::RefString& desc,
               const dtUtil::RefString& groupName)
      : BaseClass(name, label, set, get, desc, groupName)
      {
      }
	  /*override*/ void CreateNew() override { BaseClass::SetValue(PhysicsObject::CreateNew()); }
   protected:
      virtual ~PhysicsObjectPropertyContainerActorProperty() {}
   };

   /////////////////////////////////////////////////////////////////////////////
   void PhysicsActComp::BuildPropertyMap()
   {
      static const dtUtil::RefString GROUP("PhysicsActComp");

      using namespace dtCore;
      using namespace dtUtil;

      AddProperty(new ActorIDActorProperty(PROPERTY_MATERIAL_ACTOR, PROPERTY_MATERIAL_ACTOR,
            ActorIDActorProperty::SetFuncType(this, &PhysicsActComp::SetMaterialActor),
            ActorIDActorProperty::GetFuncType(this, &PhysicsActComp::GetMaterialActor),
            RefString("dtPhysics::MaterialActor"),
            RefString("The actor that defines the material properties of this physics helper."), GROUP));

      static const RefString PROPERTY_PHYSICS_NAME_DESC("The physics reference name.");
      AddProperty(new StringActorProperty(PROPERTY_PHYSICS_NAME, PROPERTY_PHYSICS_NAME, //ActorName
            StringActorProperty::SetFuncType(this, &PhysicsActComp::SetNameByString),
            StringActorProperty::GetFuncType(this, &PhysicsActComp::GetNameAsString),
            PROPERTY_PHYSICS_NAME_DESC, GROUP));

      static const RefString PROPERTY_PHYSICS_MASS_DESC("The physics mass.  This is a configuration value, "
            "and can be use however the developer intends.");
      AddProperty(new FloatActorProperty(PROPERTY_PHYSICS_MASS, PROPERTY_PHYSICS_MASS, //ActorName
            FloatActorProperty::SetFuncType(this, &PhysicsActComp::SetMass),
            FloatActorProperty::GetFuncType(this, &PhysicsActComp::GetMass),
            PROPERTY_PHYSICS_MASS_DESC, GROUP));

      static const RefString PROPERTY_PHYSICS_COLLISION_GROUP_DESC("The default group to use for the physics "
            "objects.  This is a configuration value, and can be use however the developer intends.");
      AddProperty(new IntActorProperty(PROPERTY_COLLISION_GROUP, PROPERTY_COLLISION_GROUP,
            IntActorProperty::SetFuncType(this, &PhysicsActComp::SetDefaultCollisionGroup),
            IntActorProperty::GetFuncType(this, &PhysicsActComp::GetDefaultCollisionGroup),
            PROPERTY_PHYSICS_COLLISION_GROUP_DESC, GROUP));

      static const RefString PROPERTY_PHYSICS_AUTO_CREATE("Creates and initializes all the physics objects as configured when "
            "the actor enters the world.");
      AddProperty(new BooleanActorProperty(PROPERTY_AUTO_CREATE, PROPERTY_AUTO_CREATE,
            BooleanActorProperty::SetFuncType(this, &PhysicsActComp::SetAutoCreateOnEnteringWorld),
            BooleanActorProperty::GetFuncType(this, &PhysicsActComp::GetAutoCreateOnEnteringWorld),
            PROPERTY_PHYSICS_COLLISION_GROUP_DESC, GROUP));

      typedef ArrayActorPropertyComplex<dtPhysics::PhysicsObjectPtr> PhysObjArrayProp;
      dtCore::RefPtr<PhysObjArrayProp> physObjArrayProp =
            new PhysObjArrayProp(
                  PROPERTY_PHYSICS_OBJECT_ARRAY,
                  PROPERTY_PHYSICS_OBJECT_ARRAY,
                  PhysObjArrayProp::SetFuncType(this, &PhysicsActComp::SetPhysicsObjectByIndex),
                  PhysObjArrayProp::GetFuncType(this, &PhysicsActComp::GetPhysicsObjectByIndex),
                  PhysObjArrayProp::GetSizeFuncType(this, &PhysicsActComp::GetPhysicsObjectCount),
                  PhysObjArrayProp::InsertFuncType(this, &PhysicsActComp::InsertNewPhysicsObject),
                  PhysObjArrayProp::RemoveFuncType(this, &PhysicsActComp::RemovePhysicsObjectByIndex),
                  RefString("Defines physics objects associated with the actor."),
                  GROUP);

      dtCore::RefPtr<PhysicsObjectPropertyContainerActorProperty> physObjProp =
            new PhysicsObjectPropertyContainerActorProperty(
                  PROPERTY_PHYSICS_OBJECT,
                  PROPERTY_PHYSICS_OBJECT,
                  PhysicsObjectPropertyContainerActorProperty::SetFuncType(physObjArrayProp.get(), &PhysObjArrayProp::SetCurrentValue),
                  PhysicsObjectPropertyContainerActorProperty::GetFuncType(physObjArrayProp.get(), &PhysObjArrayProp::GetCurrentValue),
                  RefString("Physics object properties"),
                  GROUP
                  );

      physObjArrayProp->SetArrayProperty(*physObjProp);
      physObjArrayProp->SetSendInFullUpdate(false);
      physObjArrayProp->SetSendInPartialUpdate(false);

      AddProperty(physObjArrayProp.get());


      typedef dtCore::ArrayActorPropertyComplex<JointDescPtr > JointArrayPropType;
      dtCore::RefPtr<JointArrayPropType> jointArrayProp =
          new JointArrayPropType
          ("Joints", "Joints",
                JointArrayPropType::SetFuncType(this, &PhysicsActComp::SetJoint),
                JointArrayPropType::GetFuncType(this, &PhysicsActComp::GetJoint),
                JointArrayPropType::GetSizeFuncType(this, &PhysicsActComp::GetNumJoints),
                JointArrayPropType::InsertFuncType(this, &PhysicsActComp::InsertJoint),
                JointArrayPropType::RemoveFuncType(this, &PhysicsActComp::RemoveJoint),
           "Array of Joint Descriptions",
           GROUP
           );

      dtCore::RefPtr<dtCore::BasePropertyContainerActorProperty> singleJointProp =
           new dtCore::SimplePropertyContainerActorProperty<JointDesc>("Joint", "Joint",
           dtCore::SimplePropertyContainerActorProperty<JointDesc>::SetFuncType(jointArrayProp.get(), &JointArrayPropType::SetCurrentValue),
           dtCore::SimplePropertyContainerActorProperty<JointDesc>::GetFuncType(jointArrayProp.get(), &JointArrayPropType::GetCurrentValue),
           "", GROUP);

      jointArrayProp->SetArrayProperty(*singleJointProp);
      jointArrayProp->SetSendInFullUpdate(false);
      jointArrayProp->SetSendInPartialUpdate(false);

      AddProperty(jointArrayProp.get());


   }

   //////////////////////////////////////////////////////////////////
   dtCore::RefPtr<dtCore::ActorProperty> PhysicsActComp::GetDeprecatedProperty(const std::string& name)
   {
      dtCore::RefPtr<dtCore::ActorProperty> result;
      size_t idx = name.find(": ");
      if ( idx != std::string::npos && idx + 2 < name.length())
      {
         PhysicsObject* po = GetPhysicsObject(name.substr(0, idx));
         if (po != nullptr)
         {
            result = po->GetProperty(name.substr(idx + 2));
         }
      }
      return result;
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
   void PhysicsActComp::PrePhysicsUpdate(Real simDt)
   {
      if (mPrePhysicsUpdate.valid())
      {
         mPrePhysicsUpdate();
      }
      else
      {
         DefaultPrePhysicsUpdate(simDt);
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
            mHelperAction = nullptr;
         }
      }
   }

   //////////////////////////////////////////////////////////////////
   void PhysicsActComp::PostPhysicsUpdate(Real simDt)
   {
      if (mPostPhysicsUpdate.valid())
      {
         mPostPhysicsUpdate();
      }
      else if (!mIsRemote)
      {
         DefaultPostPhysicsUpdate(simDt);
      }
      struct CallUpdate
      {
         void operator() (dtCore::RefPtr<TransformJointUpdater>& callback)
         {
            (*callback)();
         }
      };
      CallUpdate call;
      std::for_each(mTransformJointUpdaters.begin(), mTransformJointUpdaters.end(), call);
   }

   //////////////////////////////////////////////////////////////////
   void PhysicsActComp::ActionUpdate(Real dt)
   {
      if (mActionUpdate.valid())
      {
         mActionUpdate(dt);
      }
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
   const MaterialActor* PhysicsActComp::LookupMaterialActor(const dtCore::UniqueId& id)
   {
      const MaterialActor* result = nullptr;
      if (!id.IsNull())
      {
         dtGame::GameActorProxy* gap = nullptr;
         GetOwner(gap);
         if (gap != nullptr)
         {
            gap->GetGameManager()->FindActorById(id, result);
         }
      }
      return result;
   }

   //////////////////////////////////////////////////////////////////
   const MaterialActor* PhysicsActComp::LookupMaterialActor(const std::string& matName)
   {
      const MaterialActor* result = nullptr;
      dtGame::GameActorProxy* gap = nullptr;
      GetOwner(gap);
      if (gap != nullptr)
      {
         gap->GetGameManager()->FindActorByName(matName, result);
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
   void PhysicsActComp::SetAllActive(bool active)
   {
      std::for_each(mPhysicsObjects.begin(), mPhysicsObjects.end(), [active] (PhysicsObjectPtr& po)
            {
               po->SetActive(active);
            });
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
      if (po.GetUserData() != nullptr)
      {
         // Should this throw an exception?
         LOG_ERROR(std::string("The user data on the passed in physics object nameed \"") +
               po.GetName() +
               "\" is not nullptr.  Either it already is added to a helper, "
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
         return nullptr;
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
         return nullptr;
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
      return nullptr;
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
      return nullptr;
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
            po.SetUserData(nullptr);
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
            po.SetUserData(nullptr);
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
   void PhysicsActComp::SetPhysicsObjectByIndex(unsigned index, PhysicsObject* obj)
   {
      if (obj != nullptr && mPhysicsObjects.size() > size_t(index))
      {
         // TODO we need a clone
         mPhysicsObjects[index] = PhysicsObject::CreateNew(obj->GetName());
         mPhysicsObjects[index]->CopyPropertiesFrom(*obj);
         mPhysicsObjects[index]->SetUserData(this);
      }
   }

   //////////////////////////////////////////////////////////////////
   PhysicsObject* PhysicsActComp::GetPhysicsObjectByIndex(unsigned index)
   {
      PhysicsObject* obj = nullptr;

      if (size_t(index) < mPhysicsObjects.size())
      {
         obj = mPhysicsObjects[index];
      }

      return obj;
   }

   //////////////////////////////////////////////////////////////////
   void PhysicsActComp::InsertNewPhysicsObject(unsigned index)
   {
      if (size_t(index) <= mPhysicsObjects.size())
      {
         mPhysicsObjects.insert(mPhysicsObjects.begin() + index, PhysicsObject::CreateNew());
         mPhysicsObjects[index]->SetUserData(this);
      }
   }

   //////////////////////////////////////////////////////////////////
   void PhysicsActComp::RemovePhysicsObjectByIndex(unsigned index)
   {
      if (size_t(index) < mPhysicsObjects.size())
      {
         mPhysicsObjects[index]->SetUserData(nullptr);
         mPhysicsObjects.erase(mPhysicsObjects.begin() + index);
      }
   }

   //////////////////////////////////////////////////////////////////
   size_t PhysicsActComp::GetPhysicsObjectCount() const
   {
      return mPhysicsObjects.size();
   }

   //////////////////////////////////////////////////////////////////
   palLink* PhysicsActComp::CreateJoint(dtPhysics::JointDesc& desc, osg::Transform* refNode, const osg::Node* rootNode, bool setBody2VisualToBody)
   {
      TransformType frameParent, visualToBody;
      dtPhysics::PhysicsObject* body1 = GetPhysicsObject(desc.GetBody1Name());
      dtPhysics::PhysicsObject* body2 = GetPhysicsObject(desc.GetBody2Name());
      if (body1 == nullptr || body1->GetBodyWrapper() == nullptr || body2 == nullptr || body2->GetBodyWrapper() == nullptr)
         return nullptr;
      if (refNode != nullptr)
      {
         body1->GetVisualToBodyTransform(visualToBody);
         // This is not right yet.
//         if (rootNode == nullptr)
//         {
//            dtGame::GameActorProxy* gap = GetOwner<dtGame::GameActorProxy>();
//            dtCore::DeltaDrawable* dd = gap != nullptr ? gap->GetDrawable() : nullptr;
//            rootNode = dd != nullptr ? dd->GetOSGNode() : nullptr;
//         }
         ComputeLocalOffsetMatrixForNode(frameParent, *refNode, visualToBody, rootNode);
         desc.SetBody1Frame(frameParent);
      }

      if (setBody2VisualToBody)
      {
         dtCore::Transform xform, frame1, frame2, temp;
         body1->GetTransform(xform);
         desc.GetBody1Frame(frame1);
         frame1.Multiply(xform, temp);
         desc.GetBody2Frame(frame2);
         frame2.Invert();
         frame2.Multiply(temp, xform);
         body2->SetTransform(xform);

         // Just using frame1 as a temp here.
         body1->GetTransformAsVisual(frame1);
         frame1.Invert();
         xform.Multiply(frame1, temp);
         body2->SetVisualToBodyTransform(temp);
      }

      return dtPhysics::PhysicsObject::CreateJoint(*body1, *body2, desc);
   }

   //////////////////////////////////////////////////////////////////
   void PhysicsActComp::ComputeLocalOffsetMatrixForNode(TransformType& frameOut, const osg::Node& node, const TransformType& visualToBodyTransform, const osg::Node* root, const osg::Vec3& externalScale)
   {
      osg::Matrix wcMatrix, vtb;
      frameOut.Get(wcMatrix);

      if (root != nullptr)
      {
         dtCore::Transformable::GetAbsoluteMatrix(&node, wcMatrix, root);
      }
      else if (node.getNumParents() > 0)
      {
         const osg::Transform* parentNode = node.getParent(0)->asTransform();
         if (parentNode != nullptr)
         {
            parentNode->computeLocalToWorldMatrix(wcMatrix, nullptr);
         }
      }

      osg::Vec3d unscaledTrans = wcMatrix.getTrans();
      wcMatrix.setTrans(osg::Vec3d(unscaledTrans.x() * externalScale.x(),  unscaledTrans.y() * externalScale.y(), unscaledTrans.z() * externalScale.z()));

      visualToBodyTransform.Get(vtb);
      vtb.invert(vtb);
      //wcMatrix.invert(wcMatrix);
      wcMatrix.postMult(vtb);
      frameOut.Set(wcMatrix);
   }

   //////////////////////////////////////////////////////////////////
   void PhysicsActComp::SetMultiBodyTransform(const TransformType& xform)
   {
      PhysicsObject* po = GetMainPhysicsObject();
      if (po != nullptr)
      {
         po->SetTransform(xform);
      }
   }

   //////////////////////////////////////////////////////////////////
   void PhysicsActComp::GetMultiBodyTransform(TransformType& xform)
   {
      PhysicsObject* po = GetMainPhysicsObject();
      if (po != nullptr)
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
      if (po != nullptr)
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
         if (curPo->GetBodyWrapper() != nullptr)
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
         mHelperAction = nullptr;
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
   void PhysicsActComp::DefaultPrePhysicsUpdate(Real simDt)
   {
      if (!mCachedTransformable.valid())
         return;

      // I only want to read the absolute transform if it needs it.
      bool hasReadTransform = false;
      dtCore::Transform xform, xformP;
      for (auto i = mPhysicsObjects.begin(), iend = mPhysicsObjects.end(); i!=iend; ++i)
      {
         PhysicsObjectPtr physObj = *i;

         if (!hasReadTransform)
         {
            mCachedTransformable->GetTransform(xform);
            if (!xform.IsValid())
            {
               BaseActorObject* actor = nullptr;
               GetOwner(actor);
               std::string debugInfo("Invalid transform on physics actor component: ");
               if (actor)
               {
                  debugInfo += actor->GetName() + " " + actor->GetActorType().GetFullName();
               }
               LOGN_ERROR("physicsactcomp.cpp", "Invalid transform on physics actor component: ");
               break;
            }
            hasReadTransform = true;
         }

         dtPhysics::MechanicsType& mechanics = physObj->GetMechanicsType();
         bool updateDynamic = mIsRemote && mechanics == MechanicsType::DYNAMIC;
         bool checkChanged = mechanics == MechanicsType::DYNAMIC || mechanics == MechanicsType::STATIC;
         dtCore::Transform xformCopy = xform;

         bool setTransform = mechanics == MechanicsType::KINEMATIC;

         if (checkChanged)
         {
            physObj->GetTransformAsVisual(xformP);
            setTransform = !xformP.EpsilonEquals(xform, 0.001);
         }

         if (setTransform && updateDynamic && simDt > std::numeric_limits<Real>::epsilon() && physObj->GetBodyWrapper() != nullptr)
         {
            osg::Vec3 velocity = (xformCopy.GetTranslation() - xformP.GetTranslation()) / simDt;
            physObj->SetLinearVelocity(velocity);
            // only change the rotation.  The velocity will correct the position, otherwise there is discontinuous motion and all sorts of things go wrong.
            // Setting the angular velocity was causing problems, so I had to set it correctly
            // but it would be better to do that.
            xformCopy.SetTranslation(xformP.GetTranslation());
         }

         if (setTransform)
         {
            physObj->SetTransformAsVisual(xformCopy);
         }

      };
   }

   //////////////////////////////////////////////////////////////////
   void PhysicsActComp::DefaultPostPhysicsUpdate(Real)
   {
      if (!mCachedTransformable.valid())
         return;
      dtCore::Transform xform;
      dtPhysics::PhysicsObject* physObj = GetMainPhysicsObject();
      if (physObj == nullptr || physObj->GetMechanicsType() == MechanicsType::STATIC || physObj->GetBodyWrapper() == nullptr)
         return;

      physObj->GetTransformAsVisual(xform);

      if (xform.IsValid())
      {
         mCachedTransformable->SetTransform(xform);
      }
      else
      {
         BaseActorObject* actor = nullptr;
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
