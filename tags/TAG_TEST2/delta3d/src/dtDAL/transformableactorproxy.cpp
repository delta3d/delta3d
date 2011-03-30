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
#include <prefix/dtdalprefix.h>
#include <dtDAL/transformableactorproxy.h>

#include <dtCore/scene.h>
#include <dtCore/transform.h>
#include <dtCore/transformable.h>

#include <dtDAL/actorproxyicon.h>
#include <dtDAL/booleanactorproperty.h>
#include <dtDAL/enumactorproperty.h>

#include <dtDAL/floatactorproperty.h>
#include <dtDAL/vectoractorproperties.h>

#include <dtUtil/matrixutil.h>

namespace dtDAL
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

   //////////////////////////////////////////////////////
   TransformableActorProxy::TransformableActorProxy()
   : mHideDTCorePhysicsProps(false)
   {
      SetClassName("dtCore::Transformable");
      mCollisionType = &dtCore::Transformable::CollisionGeomType::NONE;
      mCollisionRadius = mCollisionLength = 0.0f;
      mCollisionBoxDims = osg::Vec3(0,0,0);
   }

   //////////////////////////////////////////////////////
   void TransformableActorProxy::BuildPropertyMap()
   {
      dtDAL::BaseActorObject::BuildPropertyMap();

      static const dtUtil::RefString GROUPNAME = "Transformable";
      static const dtUtil::RefString COLLISION_GROUP = "ODE Collision";

      dtCore::Transformable* trans;
      GetActor(trans);

      if (IsRotationPropertyShown())
      {
         AddProperty(new Vec3ActorProperty(PROPERTY_ROTATION, PROPERTY_ROTATION,
                  Vec3ActorProperty::SetFuncType(this, &TransformableActorProxy::SetRotation),
                  Vec3ActorProperty::GetFuncType(this, &TransformableActorProxy::GetRotation),
                  "Sets the amount of rotation on a transformable. Represented with pitch, yaw, and roll.",
                  GROUPNAME));
      }

      if (IsTranslationPropertyShown())
      {
         AddProperty(new Vec3ActorProperty(PROPERTY_TRANSLATION, PROPERTY_TRANSLATION,
                  Vec3ActorProperty::SetFuncType(this, &TransformableActorProxy::SetTranslation),
                  Vec3ActorProperty::GetFuncType(this, &TransformableActorProxy::GetTranslation),
                  "Sets the location of a transformable in 3D space.",
                  GROUPNAME));
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

         AddProperty(new EnumActorProperty<dtCore::Transformable::CollisionGeomType>(
                  PROPERTY_COLLISION_TYPE,"ODE Collision Type",
                  EnumActorProperty<dtCore::Transformable::CollisionGeomType>::SetFuncType(this,&TransformableActorProxy::SetCollisionType),
                  EnumActorProperty<dtCore::Transformable::CollisionGeomType>::GetFuncType(this,&TransformableActorProxy::GetCollisionType),
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
      }

      static const dtUtil::RefString RENDER_PROXY_NODE_DESC("Enables the rendering of the proxy node for this Transformable");
      AddProperty(new BooleanActorProperty(PROPERTY_RENDER_PROXY_NODE, PROPERTY_RENDER_PROXY_NODE,
               BooleanActorProperty::SetFuncType(trans, &dtCore::Transformable::RenderProxyNode),
               BooleanActorProperty::GetFuncType(trans, &dtCore::Transformable::GetIsRenderingProxyNode),
               RENDER_PROXY_NODE_DESC,
               GROUPNAME));

   }

   //////////////////////////////////////////////////////
   void TransformableActorProxy::SetRotation(const osg::Vec3 &rotation)
   {
      dtCore::Transformable *t = static_cast<dtCore::Transformable*>(GetActor());

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
      if (GetRenderMode() == dtDAL::BaseActorObject::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON ||
               GetRenderMode() == dtDAL::BaseActorObject::RenderMode::DRAW_BILLBOARD_ICON)
      {
         ActorProxyIcon *billBoard = GetBillBoardIcon();
         if (billBoard != NULL)
            billBoard->SetActorRotation(osg::Vec3(hpr[2], hpr[0], hpr[1]));
      }

      OnRotation(osg::Vec3(oldValue[1], oldValue[2], oldValue[0]), hpr);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void TransformableActorProxy::SetRotationFromMatrix(const osg::Matrix &rotation)
   {
      osg::Vec3 hpr;
      dtUtil::MatrixUtil::MatrixToHpr(hpr,rotation);
      SetRotation(osg::Vec3(hpr[1],hpr[2],hpr[0]));
   }

   ///////////////////////////////////////////////////////////////////////////////
   osg::Vec3 TransformableActorProxy::GetRotation() const
   {
      const dtCore::Transformable *t = static_cast<const dtCore::Transformable*>(GetActor());

      dtCore::Transform trans;
      t->GetTransform(trans, dtCore::Transformable::REL_CS);

      osg::Vec3 hpr;
      trans.GetRotation(hpr);

      return osg::Vec3(hpr[1],hpr[2],hpr[0]);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void TransformableActorProxy::SetTranslation(const osg::Vec3 &translation)
   {
      dtCore::Transformable *t = static_cast<dtCore::Transformable*>(GetActor());

      dtCore::Transform trans;
      t->GetTransform(trans, dtCore::Transformable::REL_CS);
      osg::Vec3 oldTrans;
      trans.GetTranslation(oldTrans);
      trans.SetTranslation(translation[0], translation[1], translation[2]);
      t->SetTransform(trans, dtCore::Transformable::REL_CS);

      //If we have a billboard update its position as well.
      if (GetRenderMode() == dtDAL::BaseActorObject::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON ||
               GetRenderMode() == dtDAL::BaseActorObject::RenderMode::DRAW_BILLBOARD_ICON)
      {
         ActorProxyIcon *billBoard = GetBillBoardIcon();
         if (billBoard != NULL)
            billBoard->SetPosition(translation);
      }

      OnTranslation(oldTrans, translation);
   }

   ///////////////////////////////////////////////////////////////////////////////
   osg::Vec3 TransformableActorProxy::GetTranslation() const
   {
      const dtCore::Transformable *t = static_cast<const dtCore::Transformable*>(GetActor());

      dtCore::Transform trans;
      t->GetTransform(trans, dtCore::Transformable::REL_CS);
      osg::Vec3 result;
      trans.GetTranslation(result);
      return result;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void TransformableActorProxy::SetRenderCollisionGeometry(bool enable)
   {
      dtCore::Transformable *phys = static_cast<dtCore::Transformable*>(GetActor());

      phys->RenderCollisionGeometry(enable);
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool TransformableActorProxy::GetRenderCollisionGeometry() const
   {
      const dtCore::Transformable *phys = static_cast<const dtCore::Transformable*>(GetActor());

      return phys->GetRenderCollisionGeometry();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void TransformableActorProxy::SetCollisionType(dtCore::Transformable::CollisionGeomType &type)
   {
      dtCore::Transformable *phys = static_cast<dtCore::Transformable*>(GetActor());

      mCollisionType = &type;
      if (mCollisionType == &dtCore::Transformable::CollisionGeomType::NONE)
         phys->ClearCollisionGeometry();
      else if (mCollisionType == &dtCore::Transformable::CollisionGeomType::CUBE)
         SetBoxCollision();
      else if (mCollisionType == &dtCore::Transformable::CollisionGeomType::SPHERE)
         SetSphereCollision();
      else if (mCollisionType == &dtCore::Transformable::CollisionGeomType::CYLINDER)
         SetCylinderCollision();
      else if (mCollisionType == &dtCore::Transformable::CollisionGeomType::RAY)
         SetRayCollision();
      else if (mCollisionType == &dtCore::Transformable::CollisionGeomType::MESH)
         SetMeshCollision();
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtCore::Transformable::CollisionGeomType &TransformableActorProxy::GetCollisionType()
   {
      return *mCollisionType;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void TransformableActorProxy::SetCollisionRadius(float radius)
   {
      //dtCore::Transformable *phys = static_cast<dtCore::Transformable*>(GetActor());

      mCollisionRadius = radius;
      if (mCollisionType == &dtCore::Transformable::CollisionGeomType::CYLINDER)
         SetCylinderCollision();
      else if (mCollisionType == &dtCore::Transformable::CollisionGeomType::SPHERE)
         SetSphereCollision();
   }

   ///////////////////////////////////////////////////////////////////////////////
   float TransformableActorProxy::GetCollisionRadius() const
   {
      return mCollisionRadius;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void TransformableActorProxy::SetCollisionBoxDims(const osg::Vec3 &dims)
   {
      //dtCore::Transformable *phys = static_cast<dtCore::Transformable*>(GetActor());

      mCollisionBoxDims = dims;
      SetBoxCollision();
   }

   ///////////////////////////////////////////////////////////////////////////////
   osg::Vec3 TransformableActorProxy::GetCollisionBoxDims() const
   {
      return mCollisionBoxDims;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void TransformableActorProxy::SetCollisionLength(float length)
   {
      //dtCore::Transformable *phys = static_cast<dtCore::Transformable*>(GetActor());

      mCollisionLength = length;
      if (mCollisionType == &dtCore::Transformable::CollisionGeomType::CYLINDER)
         SetCylinderCollision();
      else if (mCollisionType == &dtCore::Transformable::CollisionGeomType::RAY)
         SetRayCollision();
   }

   ///////////////////////////////////////////////////////////////////////////////
   float TransformableActorProxy::GetCollisionLength() const
   {
      return mCollisionLength;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void TransformableActorProxy::SetBoxCollision()
   {
      if (mCollisionType != &dtCore::Transformable::CollisionGeomType::CUBE)
         return;

      dtCore::Transformable *phys = static_cast<dtCore::Transformable*>(GetActor());

      phys->ClearCollisionGeometry();
      if (mCollisionBoxDims.x() == 0.0f || mCollisionBoxDims.y() == 0.0f ||
               mCollisionBoxDims.z() == 0.0f)
      {
         phys->SetCollisionBox(NULL);
      }
      else
      {
         phys->SetCollisionBox(mCollisionBoxDims.x(),mCollisionBoxDims.y(),
                  mCollisionBoxDims.z());
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void TransformableActorProxy::SetSphereCollision()
   {
      if (mCollisionType != &dtCore::Transformable::CollisionGeomType::SPHERE)
         return;

      dtCore::Transformable *phys = static_cast<dtCore::Transformable*>(GetActor());

      phys->ClearCollisionGeometry();
      if (mCollisionRadius == 0.0f)
         phys->SetCollisionSphere((osg::Node *)NULL);
      else
         phys->SetCollisionSphere(mCollisionRadius);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void TransformableActorProxy::SetCylinderCollision()
   {
      if (mCollisionType != &dtCore::Transformable::CollisionGeomType::CYLINDER)
         return;

      dtCore::Transformable *phys = static_cast<dtCore::Transformable*>(GetActor());

      phys->ClearCollisionGeometry();
      if (mCollisionRadius == 0.0f || mCollisionLength == 0.0f)
         phys->SetCollisionCappedCylinder(NULL);
      else
         phys->SetCollisionCappedCylinder(mCollisionRadius,mCollisionLength);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void TransformableActorProxy::SetRayCollision()
   {
      if (mCollisionType != &dtCore::Transformable::CollisionGeomType::RAY)
         return;

      dtCore::Transformable *phys = static_cast<dtCore::Transformable*>(GetActor());

      phys->ClearCollisionGeometry();
      phys->SetCollisionRay(mCollisionLength);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void TransformableActorProxy::SetMeshCollision()
   {
      if (mCollisionType != &dtCore::Transformable::CollisionGeomType::MESH)
         return;

      dtCore::Transformable *phys = static_cast<dtCore::Transformable*>(GetActor());

      phys->ClearCollisionGeometry();
      phys->SetCollisionMesh(NULL);
   }

   DT_IMPLEMENT_ACCESSOR(TransformableActorProxy, bool, HideDTCorePhysicsProps);

}
