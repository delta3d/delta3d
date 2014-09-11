/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
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
 * William E. Johnson II
 */
#include <prefix/dtcoreprefix.h>
#include <dtCore/transformableactorproxy.h>

#include <dtCore/scene.h>
#include <dtCore/transform.h>
#include <dtCore/transformable.h>
#include <dtCore/collisioncategorydefaults.h>

#include <dtCore/propertymacros.h>
#include <dtCore/actorproxyicon.h>
#include <dtCore/booleanactorproperty.h>
#include <dtCore/enumactorproperty.h>
#include <dtCore/bitmaskactorproperty.h>

#include <dtCore/floatactorproperty.h>
#include <dtCore/vectoractorproperties.h>
#include <dtCore/project.h> //<To resolve ResourceDescriptor paths
#include <dtUtil/matrixutil.h>

namespace dtCore
{
   const dtUtil::RefString TransformableActorProxy::PROPERTY_ROTATION("Rotation");
   const dtUtil::RefString TransformableActorProxy::PROPERTY_TRANSLATION("Translation");
   const dtUtil::RefString TransformableActorProxy::PROPERTY_NORMAL_RESCALING("Normal Rescaling");
   const dtUtil::RefString TransformableActorProxy::PROPERTY_RENDER_PROXY_NODE("Render Proxy Node");
   const dtUtil::RefString TransformableActorProxy::PROPERTY_ENABLE_COLLISION("Enable Collision");
   const dtUtil::RefString TransformableActorProxy::PROPERTY_COLLISION_TYPE("Collision Type");
   const dtUtil::RefString TransformableActorProxy::PROPERTY_COLLISION_RADIUS("Collision Radius");
   const dtUtil::RefString TransformableActorProxy::PROPERTY_COLLISION_LENGTH("Collision Length");
   const dtUtil::RefString TransformableActorProxy::PROPERTY_COLLISION_BOX("Collision Box");
   const dtUtil::RefString TransformableActorProxy::PROPERTY_COLLISION_CATEGORY_MASK("Collision Category Mask");
   const dtUtil::RefString TransformableActorProxy::PROPERTY_COLLISION_COLLIDE_MASK("Collision Collide Mask");

   /////////////////////////////////////////////////////////////////////////////
   bool IsAltCollisionMeshSet(const dtCore::ResourceDescriptor& resource)
   {
      if (resource == dtCore::ResourceDescriptor::NULL_RESOURCE ||
          resource.IsEmpty())
      {
         return false;
      }

      return true;
   }

   /////////////////////////////////////////////////////////////////////////////
   TransformableActorProxy::TransformableActorProxy()
   : mHideDTCorePhysicsProps(false)
   , mAltCollisionMesh(dtCore::ResourceDescriptor::NULL_RESOURCE)
   {
      SetClassName("dtCore::Transformable");
      mCollisionType = &dtCore::CollisionGeomType::NONE;
      mCollisionRadius = mCollisionLength = 0.0f;
      mCollisionBoxDims = osg::Vec3(0,0,0);
   }

   /////////////////////////////////////////////////////////////////////////////
   void TransformableActorProxy::BuildPropertyMap()
   {
      dtCore::BaseActorObject::BuildPropertyMap();

      static const dtUtil::RefString GROUPNAME = "Transformable";
      static const dtUtil::RefString COLLISION_GROUP = "ODE Collision";

      typedef dtCore::PropertyRegHelper<TransformableActorProxy&, TransformableActorProxy> PropRegHelperType;
      PropRegHelperType propRegHelper(*this, this, COLLISION_GROUP);

      dtCore::Transformable* trans = NULL;
      GetDrawable(trans);
      // Can't have any of these properties unless it has a drawable
      if (trans == NULL) return;

      dtCore::RefPtr<dtCore::ActorProperty> newProp;
      if (IsRotationPropertyShown())
      {
          newProp = new Vec3ActorProperty(PROPERTY_ROTATION, PROPERTY_ROTATION,
                           Vec3ActorProperty::SetFuncType(this, &TransformableActorProxy::SetRotation),
                           Vec3ActorProperty::GetFuncType(this, &TransformableActorProxy::GetRotation),
                           "Sets the amount of rotation on a transformable. Represented with pitch, yaw, and roll.",
                           GROUPNAME);
         AddProperty(newProp);
      }

      if (IsTranslationPropertyShown())
      {
         newProp = new Vec3ActorProperty(PROPERTY_TRANSLATION, PROPERTY_TRANSLATION,
                  Vec3ActorProperty::SetFuncType(this, &TransformableActorProxy::SetTranslation),
                  Vec3ActorProperty::GetFuncType(this, &TransformableActorProxy::GetTranslation),
                  "Sets the location of a transformable in 3D space.",
                  GROUPNAME);
         AddProperty(newProp);
      }

      AddProperty(new BooleanActorProperty(PROPERTY_NORMAL_RESCALING, PROPERTY_NORMAL_RESCALING,
               BooleanActorProperty::SetFuncType(trans, &dtCore::Transformable::SetNormalRescaling),
               BooleanActorProperty::GetFuncType(trans, &dtCore::Transformable::GetNormalRescaling),
               "Enables the automatic scaling of normals when a Transformable is scaled",
               GROUPNAME));

      if (!mHideDTCorePhysicsProps)
      {
         //COLLISION PROPS...
         AddProperty(new BooleanActorProperty("Show Collision Geometry", "ODE Show Collision Geometry",
                  BooleanActorProperty::SetFuncType(this, &TransformableActorProxy::SetRenderCollisionGeometry),
                  BooleanActorProperty::GetFuncType(this, &TransformableActorProxy::GetRenderCollisionGeometry),
                  "Enables/Disables the rendering of collision geometry assigned to this actor (using ODE).",
                  COLLISION_GROUP));

         AddProperty(new EnumActorProperty<dtCore::CollisionGeomType>(
                  PROPERTY_COLLISION_TYPE,"ODE Collision Type",
                  EnumActorProperty<dtCore::CollisionGeomType>::SetFuncType(this,&TransformableActorProxy::SetCollisionType),
                  EnumActorProperty<dtCore::CollisionGeomType>::GetFuncType(this,&TransformableActorProxy::GetCollisionType),
                  "Sets the type of geometry to use for collision detection (using ODE)",
                  COLLISION_GROUP));

         AddProperty(new FloatActorProperty(PROPERTY_COLLISION_RADIUS,"ODE Collision Radius",
                  FloatActorProperty::SetFuncType(this, &TransformableActorProxy::SetCollisionRadius),
                  FloatActorProperty::GetFuncType(this, &TransformableActorProxy::GetCollisionRadius),
                  "Sets the radius for collision calculations (using ODE). This value is used differently "
                  "depending on the type of collision assigned to this actor.  For example, "
                  "if the collision type is set to SPHERE, this will be the sphere's radius.",
                  COLLISION_GROUP));

         AddProperty(new FloatActorProperty(PROPERTY_COLLISION_LENGTH,"ODE Collision Length",
                  FloatActorProperty::SetFuncType(this, &TransformableActorProxy::SetCollisionLength),
                  FloatActorProperty::GetFuncType(this, &TransformableActorProxy::GetCollisionLength),
                  "Sets the length of the collision geometry (using ODE). This value is used differently "
                  "depending on the type of collision assigned to this actor.  For example, "
                  "if the collision type is set to CYLINDER, this will be the cylinder's length.",
                  COLLISION_GROUP));

         AddProperty(new Vec3ActorProperty(PROPERTY_COLLISION_BOX,"ODE Collision Box",
                  Vec3ActorProperty::SetFuncType(this, &TransformableActorProxy::SetCollisionBoxDims),
                  Vec3ActorProperty::GetFuncType(this, &TransformableActorProxy::GetCollisionBoxDims),
                  "Sets the size of the bounding box used for collision detection (using ODE).  This property "
                  "is used if the collision type is set to BOX.",
                  COLLISION_GROUP));

         AddProperty(new BooleanActorProperty(PROPERTY_ENABLE_COLLISION, "ODE Collision Enable",
                  BooleanActorProperty::SetFuncType(trans, &dtCore::Transformable::SetCollisionDetection),
                  BooleanActorProperty::GetFuncType(trans, &dtCore::Transformable::GetCollisionDetection),
                  "Enables collision detection on this actor (using ODE).",
                  COLLISION_GROUP));

         AddProperty(new BitMaskActorProperty(PROPERTY_COLLISION_CATEGORY_MASK, "ODE Collision Category Bits",
                  BitMaskActorProperty::SetFuncType(this, &TransformableActorProxy::SetCollisionCategoryMask),
                  BitMaskActorProperty::GetFuncType(this, &TransformableActorProxy::GetCollisionCategoryMask),
                  BitMaskActorProperty::GetMaskListFuncType(this, &TransformableActorProxy::GetCollisionMaskList),
                  "Sets the collision mask that defines this actor (using ODE).",
                  COLLISION_GROUP));

         AddProperty(new BitMaskActorProperty(PROPERTY_COLLISION_COLLIDE_MASK, "ODE Collision Collide Bits",
                  BitMaskActorProperty::SetFuncType(this, &TransformableActorProxy::SetCollisionCollideMask),
                  BitMaskActorProperty::GetFuncType(this, &TransformableActorProxy::GetCollisionCollideMask),
                  BitMaskActorProperty::GetMaskListFuncType(this, &TransformableActorProxy::GetCollisionMaskList),
                  "Sets the collision mask that defines what this actor collides with (using ODE).",
                  COLLISION_GROUP));

         DT_REGISTER_RESOURCE_PROPERTY_WITH_NAME(DataType::STATIC_MESH, AltCollisionMesh, "AltCollisionMesh", "Alternate Collision Mesh",
                                                "An alternate mesh to use for collision detection shapes",
                                                 PropRegHelperType, propRegHelper);

      }

      static const dtUtil::RefString RENDER_PROXY_NODE_DESC("Enables the rendering of the proxy node for this Transformable");
      AddProperty(new BooleanActorProperty(PROPERTY_RENDER_PROXY_NODE, PROPERTY_RENDER_PROXY_NODE,
               BooleanActorProperty::SetFuncType(trans, &dtCore::Transformable::RenderProxyNode),
               BooleanActorProperty::GetFuncType(trans, &dtCore::Transformable::GetIsRenderingProxyNode),
               RENDER_PROXY_NODE_DESC,
               GROUPNAME));

   }

   /////////////////////////////////////////////////////////////////////////////
   void TransformableActorProxy::OnMapLoadEnd()
   {
      SetCollisionCategoryMask(GetCollisionCategoryMask());
      SetCollisionCollideMask(GetCollisionCollideMask());
   }

   /////////////////////////////////////////////////////////////////////////////
   void TransformableActorProxy::SetRotation(const osg::Vec3& rotation)
   {
      dtCore::Transformable* t;
      GetDrawable(t);

      if (t == NULL) return;

      osg::Vec3 hpr = rotation;

      //Normalize the rotation.
      if (hpr.x() < 0.0f)
         hpr.x() += 360.0f;
      if (hpr.x() > 360.0f)
         hpr.x() -= 360.0f;

      if (hpr.y() < 0.0f)
         hpr.y() += 360.0f;
      if (hpr.y() > 360.0f)
         hpr.y() -= 360.0f;

      if (hpr.z() < 0.0f)
         hpr.z() += 360.0f;
      if (hpr.z() > 360.0f)
         hpr.z() -= 360.0f;

      dtCore::Transform trans;
      t->GetTransform(trans, dtCore::Transformable::REL_CS);
      osg::Vec3 oldValue;
      trans.GetRotation(oldValue);
      trans.SetRotation(osg::Vec3(hpr[2],hpr[0],hpr[1]));
      t->SetTransform(trans, dtCore::Transformable::REL_CS);

      //If we have a billboard update its rotation as well.
      if (GetRenderMode() == dtCore::BaseActorObject::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON ||
          GetRenderMode() == dtCore::BaseActorObject::RenderMode::DRAW_BILLBOARD_ICON)
      {
         ActorProxyIcon *billBoard = GetBillBoardIcon();
         if (billBoard != NULL)
            billBoard->SetActorRotation(osg::Vec3(hpr[2], hpr[0], hpr[1]));
      }

      OnRotation(osg::Vec3(oldValue[1], oldValue[2], oldValue[0]), hpr);
   }

   /////////////////////////////////////////////////////////////////////////////
   void TransformableActorProxy::SetRotationFromMatrix(const osg::Matrix& rotation)
   {
      osg::Vec3 hpr;
      dtUtil::MatrixUtil::MatrixToHpr(hpr,rotation);
      SetRotation(osg::Vec3(hpr[1],hpr[2],hpr[0]));
   }

   /////////////////////////////////////////////////////////////////////////////
   osg::Vec3 TransformableActorProxy::GetRotation() const
   {
      const dtCore::Transformable* t = GetDrawable<Transformable>();

      if (t == NULL) return osg::Vec3();

      dtCore::Transform trans;
      t->GetTransform(trans, dtCore::Transformable::REL_CS);

      osg::Vec3 hpr;
      trans.GetRotation(hpr);

      return osg::Vec3(hpr[1],hpr[2],hpr[0]);
   }

   /////////////////////////////////////////////////////////////////////////////
   void TransformableActorProxy::SetTranslation(const osg::Vec3& translation)
   {
      dtCore::Transformable* t = GetDrawable<dtCore::Transformable>();

      if (t == NULL) return;

      dtCore::Transform trans;
      t->GetTransform(trans, dtCore::Transformable::REL_CS);
      osg::Vec3 oldTrans;
      trans.GetTranslation(oldTrans);
      trans.SetTranslation(translation[0], translation[1], translation[2]);
      t->SetTransform(trans, dtCore::Transformable::REL_CS);

      //If we have a billboard update its position as well.
      if (GetRenderMode() == dtCore::BaseActorObject::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON ||
               GetRenderMode() == dtCore::BaseActorObject::RenderMode::DRAW_BILLBOARD_ICON)
      {
         ActorProxyIcon* billBoard = GetBillBoardIcon();
         if (billBoard != NULL)
         {
            billBoard->SetPosition(translation);
         }
      }

      OnTranslation(oldTrans, translation);
   }

   /////////////////////////////////////////////////////////////////////////////
   osg::Vec3 TransformableActorProxy::GetTranslation() const
   {
      const dtCore::Transformable* t = GetDrawable<Transformable>();

      if (t == NULL) return osg::Vec3();

      dtCore::Transform trans;
      t->GetTransform(trans, dtCore::Transformable::REL_CS);
      osg::Vec3 result;
      trans.GetTranslation(result);
      return result;
   }

   /////////////////////////////////////////////////////////////////////////////
   void TransformableActorProxy::SetRenderCollisionGeometry(bool enable)
   {
      dtCore::Transformable* phys = GetDrawable<Transformable>();

      if (phys == NULL) return;

      phys->RenderCollisionGeometry(enable);
   }

   /////////////////////////////////////////////////////////////////////////////
   bool TransformableActorProxy::GetRenderCollisionGeometry() const
   {
      const dtCore::Transformable* phys = GetDrawable<Transformable>();
      if (phys == NULL) return false;

      return phys->GetRenderCollisionGeometry();
   }

   /////////////////////////////////////////////////////////////////////////////
   void TransformableActorProxy::SetCollisionType(dtCore::CollisionGeomType &type)
   {
      dtCore::Transformable* phys = GetDrawable<Transformable>();
      if (phys == NULL) return;

      mCollisionType = &type;
      if (mCollisionType == &dtCore::CollisionGeomType::NONE)
      {
         phys->ClearCollisionGeometry();
      }
      else if (mCollisionType == &dtCore::CollisionGeomType::CUBE)
      {
         SetBoxCollision();
      }
      else if (mCollisionType == &dtCore::CollisionGeomType::SPHERE)
      {
         SetSphereCollision();
      }
      else if (mCollisionType == &dtCore::CollisionGeomType::CYLINDER ||
               mCollisionType == &dtCore::CollisionGeomType::CCYLINDER)
      {
         SetCapsuleCollision();
      }
      else if (mCollisionType == &dtCore::CollisionGeomType::RAY)
      {
         SetRayCollision();
      }
      else if (mCollisionType == &dtCore::CollisionGeomType::MESH)
      {
         SetMeshCollision();
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   dtCore::CollisionGeomType& TransformableActorProxy::GetCollisionType()
   {
      return *mCollisionType;
   }

   /////////////////////////////////////////////////////////////////////////////
   void TransformableActorProxy::SetCollisionRadius(float radius)
   {

      mCollisionRadius = radius;
      if (mCollisionType == &dtCore::CollisionGeomType::CYLINDER ||
          mCollisionType == &dtCore::CollisionGeomType::CCYLINDER)
      {
         SetCapsuleCollision();
      }
      else if (mCollisionType == &dtCore::CollisionGeomType::SPHERE)
      {
         SetSphereCollision();
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   float TransformableActorProxy::GetCollisionRadius() const
   {
      return mCollisionRadius;
   }

   /////////////////////////////////////////////////////////////////////////////
   void TransformableActorProxy::SetCollisionBoxDims(const osg::Vec3& dims)
   {
      mCollisionBoxDims = dims;
      SetBoxCollision();
   }

   /////////////////////////////////////////////////////////////////////////////
   osg::Vec3 TransformableActorProxy::GetCollisionBoxDims() const
   {
      return mCollisionBoxDims;
   }

   /////////////////////////////////////////////////////////////////////////////
   void TransformableActorProxy::SetCollisionLength(float length)
   {

      mCollisionLength = length;
      if (mCollisionType == &dtCore::CollisionGeomType::CYLINDER ||
          mCollisionType == &dtCore::CollisionGeomType::CCYLINDER)
      {
         SetCapsuleCollision();
      }
      else if (mCollisionType == &dtCore::CollisionGeomType::RAY)
      {
         SetRayCollision();
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   float TransformableActorProxy::GetCollisionLength() const
   {
      return mCollisionLength;
   }

   /////////////////////////////////////////////////////////////////////////////
   void TransformableActorProxy::SetCollisionCategoryMask(unsigned int mask)
   {
      dtCore::Transformable* trans;
      GetDrawable(trans);
      if (trans == NULL) return;

      trans->SetCollisionCategoryBits((unsigned long)mask);
   }

   /////////////////////////////////////////////////////////////////////////////
   unsigned int TransformableActorProxy::GetCollisionCategoryMask() const
   {
      const dtCore::Transformable* trans = NULL;
      GetDrawable(trans);
      if (trans == NULL) return 0;

      return (unsigned int)trans->GetCollisionCategoryBits();
   }

   /////////////////////////////////////////////////////////////////////////////
   void TransformableActorProxy::SetCollisionCollideMask(unsigned int mask)
   {
      dtCore::Transformable* trans;
      GetDrawable(trans);
      if (trans == NULL) return;

      trans->SetCollisionCollideBits((unsigned long)mask);
   }

   /////////////////////////////////////////////////////////////////////////////
   unsigned int TransformableActorProxy::GetCollisionCollideMask() const
   {
      const dtCore::Transformable* trans;
      GetDrawable(trans);
      if (trans == NULL) return 0;

      return (unsigned int)trans->GetCollisionCollideBits();
   }

   /////////////////////////////////////////////////////////////////////////////
   void TransformableActorProxy::GetCollisionMaskList(std::vector<std::string>& names, std::vector<unsigned int>& values) const
   {
      names.push_back("Proximity Trigger");
      values.push_back(COLLISION_CATEGORY_MASK_PROXIMITYTRIGGER);

      names.push_back("Camera");
      values.push_back(COLLISION_CATEGORY_MASK_CAMERA);

      names.push_back("Compass");
      values.push_back(COLLISION_CATEGORY_MASK_COMPASS);

      names.push_back("Infinite Terrain");
      values.push_back(COLLISION_CATEGORY_MASK_INFINITETERRAIN);

      names.push_back("ISector");
      values.push_back(COLLISION_CATEGORY_MASK_ISECTOR);

      names.push_back("Object");
      values.push_back(COLLISION_CATEGORY_MASK_OBJECT);

      names.push_back("Particle System");
      values.push_back(COLLISION_CATEGORY_MASK_PARTICLESYSTEM);

      names.push_back("Physical");
      values.push_back(COLLISION_CATEGORY_MASK_PHYSICAL);

      names.push_back("Point Axis");
      values.push_back(COLLISION_CATEGORY_MASK_POINTAXIS);

      names.push_back("Positional Light");
      values.push_back(COLLISION_CATEGORY_MASK_POSITIONALLIGHT);

      names.push_back("Spot Light");
      values.push_back(COLLISION_CATEGORY_MASK_SPOTLIGHT);

      names.push_back("Transformable");
      values.push_back(COLLISION_CATEGORY_MASK_TRANSFORMABLE);

      names.push_back("Listener");
      values.push_back(COLLISION_CATEGORY_MASK_LISTENER);

      names.push_back("Sound");
      values.push_back(COLLISION_CATEGORY_MASK_SOUND);

      names.push_back("Entity");
      values.push_back(COLLISION_CATEGORY_MASK_ENTITY);

      names.push_back("Terrain");
      values.push_back(COLLISION_CATEGORY_MASK_TERRAIN);

      names.push_back("Defaults");
      values.push_back(COLLISION_CATEGORY_MASK_ALLDEFAULTS);

      names.push_back("All");
      values.push_back(COLLISION_CATEGORY_MASK_ALL);
   }

   /////////////////////////////////////////////////////////////////////////////
   void TransformableActorProxy::SetBoxCollision()
   {
      if (mCollisionType == &dtCore::CollisionGeomType::CUBE)
      {
         dtCore::Transformable* trans = GetDrawable<dtCore::Transformable>();
         if (trans == NULL) return;

         trans->ClearCollisionGeometry();

         if (mCollisionBoxDims.x() == 0.0f ||
            mCollisionBoxDims.y() == 0.0f ||
            mCollisionBoxDims.z() == 0.0f)
         {
            osg::Node* node = NULL;
            if (IsAltCollisionMeshSet(GetAltCollisionMesh()))
            {
               node = LoadAltCollisionMesh(GetAltCollisionMesh());
            }

            trans->SetCollisionBox(node);
         }
         else
         {
            trans->SetCollisionBox(mCollisionBoxDims.x(),
               mCollisionBoxDims.y(),
               mCollisionBoxDims.z());
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void TransformableActorProxy::SetSphereCollision()
   {
      if (mCollisionType != &dtCore::CollisionGeomType::SPHERE)
         return;

      dtCore::Transformable* trans;
      GetDrawable(trans);
      if (trans == NULL) return;

      trans->ClearCollisionGeometry();
      if (mCollisionRadius == 0.0f)
      {
         osg::Node* node = NULL;
         if (IsAltCollisionMeshSet(GetAltCollisionMesh()))
         {
            node = LoadAltCollisionMesh(GetAltCollisionMesh());
         }

         trans->SetCollisionSphere(node);
      }
      else
      {
         trans->SetCollisionSphere(mCollisionRadius);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void TransformableActorProxy::SetCapsuleCollision()
   {
      if (mCollisionType != &dtCore::CollisionGeomType::CYLINDER ||
          mCollisionType != &dtCore::CollisionGeomType::CCYLINDER)
      {
         return;
      }

      dtCore::Transformable* trans;
      GetDrawable(trans);
      if (trans == NULL) return;

      trans->ClearCollisionGeometry();
      if (mCollisionRadius == 0.0f || mCollisionLength == 0.0f)
      {
         osg::Node* node = NULL;

         if (IsAltCollisionMeshSet(GetAltCollisionMesh()))
         {
            node = LoadAltCollisionMesh(GetAltCollisionMesh());
         }

         trans->SetCollisionCappedCylinder(node);
      }
      else
      {
         trans->SetCollisionCappedCylinder(mCollisionRadius,mCollisionLength);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void TransformableActorProxy::SetRayCollision()
   {
      if (mCollisionType != &dtCore::CollisionGeomType::RAY)
         return;

      dtCore::Transformable* phys = GetDrawable<dtCore::Transformable>();
      if (phys == NULL) return;

      phys->ClearCollisionGeometry();
      phys->SetCollisionRay(mCollisionLength);
   }

   /////////////////////////////////////////////////////////////////////////////
   void TransformableActorProxy::SetMeshCollision()
   {
      if (mCollisionType != &dtCore::CollisionGeomType::MESH)
         return;

      dtCore::Transformable* trans = GetDrawable<dtCore::Transformable>();
      if (trans == NULL) return;

      trans->ClearCollisionGeometry();

      osg::Node* node = NULL;

      if (IsAltCollisionMeshSet(GetAltCollisionMesh()))
      {
         node = LoadAltCollisionMesh(GetAltCollisionMesh());
      }

      trans->SetCollisionMesh(node);
   }

   /////////////////////////////////////////////////////////////////////////////
   void TransformableActorProxy::SetAltCollisionMesh(const dtCore::ResourceDescriptor& value)
   {
      mAltCollisionMesh = value;
      SetCollisionType(*mCollisionType);
   }

   /////////////////////////////////////////////////////////////////////////////
   const dtCore::ResourceDescriptor& TransformableActorProxy::GetAltCollisionMesh() const
   {
      return mAltCollisionMesh;
   }

   /////////////////////////////////////////////////////////////////////////////
   osg::Node* TransformableActorProxy::LoadAltCollisionMesh(const dtCore::ResourceDescriptor& resource)
   {
      mAltCollisionGeometry = NULL;

      if (dtCore::Project::GetInstance().IsContextValid())
      {
         const std::string resourcePath = dtCore::Project::GetInstance().GetResourcePath(resource);
         if (!resourcePath.empty())
         {
            mAltCollisionGeometry = dtUtil::FileUtils::GetInstance().ReadNode(resourcePath);
            if (mAltCollisionGeometry == NULL)
            {
               LOG_ERROR("Can't load the alternate collision mesh: " + resource.GetDisplayName());
            }
         }
      }
      else
      {
         LOG_WARNING("No ProjectContext is set. Cannot resolve the alternate collision mesh ResourceDescriptor");
      }

      return mAltCollisionGeometry.get();
   }

   DT_IMPLEMENT_ACCESSOR(TransformableActorProxy, bool, HideDTCorePhysicsProps);

}
