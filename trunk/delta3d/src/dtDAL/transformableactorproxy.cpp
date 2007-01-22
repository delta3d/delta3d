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
#include <prefix/dtdalprefix-src.h>
#include <dtDAL/transformableactorproxy.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/exceptionenum.h>
#include <dtDAL/actorproxyicon.h>
#include <dtCore/transformable.h>
#include <dtCore/scene.h>
#include <dtUtil/matrixutil.h>

namespace dtDAL
{
   void TransformableActorProxy::BuildPropertyMap()
   {
      const std::string GROUPNAME = "Transformable";
      const std::string COLLISION_GROUP = "Collision";

      dtCore::Transformable *trans = static_cast<dtCore::Transformable*>(GetActor());
      
      AddProperty(new Vec3ActorProperty("Rotation", "Rotation",
                                        MakeFunctor(*this, &TransformableActorProxy::SetRotation),
                                        MakeFunctorRet(*this, &TransformableActorProxy::GetRotation),
                                        "Sets the amount of rotation on a transformable. Represented with pitch, yaw, and roll.",
                                        GROUPNAME));

      AddProperty(new Vec3ActorProperty("Translation", "Translation",
                                        MakeFunctor(*this, &TransformableActorProxy::SetTranslation),
                                        MakeFunctorRet(*this, &TransformableActorProxy::GetTranslation),
                                        "Sets the location of a transformable in 3D space.",
                                        GROUPNAME));

      AddProperty(new Vec3ActorProperty("Scale", "Scale",
                                        MakeFunctor(*this, &TransformableActorProxy::SetScale),
                                        MakeFunctorRet(*this, &TransformableActorProxy::GetScale),
                                        "Sets the scale of a transformable.",GROUPNAME));

      AddProperty(new BooleanActorProperty("Normal Rescaling", "Normal Rescaling",
                                           MakeFunctor(*trans, &dtCore::Transformable::SetNormalRescaling),
                                           MakeFunctorRet(*trans, &dtCore::Transformable::GetNormalRescaling),
                                           "Enables the automatic scaling of normals when a Transformable is scaled",
                                           GROUPNAME));

      //COLLISION PROPS...
      AddProperty(new BooleanActorProperty("Show Collision Geometry", "Show Collision Geometry",
                                           MakeFunctor(*this, &TransformableActorProxy::SetRenderCollisionGeometry),
                                           MakeFunctorRet(*this, &TransformableActorProxy::GetRenderCollisionGeometry),
                                           "Enables/Disables the rendering of collision geometry assigned to this actor.",
                                           COLLISION_GROUP));

      AddProperty(new EnumActorProperty<dtCore::Transformable::CollisionGeomType>(
                     "Collision Type","Collision Type",
                     MakeFunctor(*this,&TransformableActorProxy::SetCollisionType),
                     MakeFunctorRet(*this,&TransformableActorProxy::GetCollisionType),
                     "Sets the type of geometry to use for collision detection",
                     COLLISION_GROUP));

      AddProperty(new FloatActorProperty("Collision Radius","Collision Radius",
                                         MakeFunctor(*this,&TransformableActorProxy::SetCollisionRadius),
                                         MakeFunctorRet(*this,&TransformableActorProxy::GetCollisionRadius),
                                         "Sets the radius for collision calculations. This value is used differently "
                                         "depending on the type of collision assigned to this actor.  For example, "
                                         "if the collision type is set to SPHERE, this will be the sphere's radius.",
                                         COLLISION_GROUP));

      AddProperty(new FloatActorProperty("Collision Length","Collision Length",
                                         MakeFunctor(*this,&TransformableActorProxy::SetCollisionLength),
                                         MakeFunctorRet(*this,&TransformableActorProxy::GetCollisionLength),
                                         "Sets the length of the collision geometry. This value is used differently "
                                         "depending on the type of collision assigned to this actor.  For example, "
                                         "if the collision type is set to CYLINDER, this will be the cylinder's length.",
                                         COLLISION_GROUP));

      AddProperty(new Vec3ActorProperty("Collision Box","Collision Box",
                                        MakeFunctor(*this, &TransformableActorProxy::SetCollisionBoxDims),
                                        MakeFunctorRet(*this, &TransformableActorProxy::GetCollisionBoxDims),
                                        "Sets the size of the bounding box used for collision detection.  This property "
                                        "is used if the collision type is set to BOX.",
                                        COLLISION_GROUP));

      AddProperty(new BooleanActorProperty("Render Proxy Node", "Render Proxy Node",
         MakeFunctor(*trans, &dtCore::Transformable::RenderProxyNode),
         MakeFunctorRet(*trans, &dtCore::Transformable::GetIsRenderingProxyNode),
         "Enables the rendering of the proxy node for this Transformable",
         GROUPNAME));
   }

   ///////////////////////////////////////////////////////////////////////////////
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
      t->GetTransform(trans);
      osg::Vec3 oldValue;
      trans.GetRotation(oldValue);
      trans.SetRotation(osg::Vec3(hpr[2],hpr[0],hpr[1]));
      t->SetTransform(trans);

      //If we have a billboard update its rotation as well.
      if (GetRenderMode() == ActorProxy::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON ||
          GetRenderMode() == ActorProxy::RenderMode::DRAW_BILLBOARD_ICON)
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
   osg::Vec3 TransformableActorProxy::GetRotation()
   {
      dtCore::Transformable *t = static_cast<dtCore::Transformable*>(GetActor());

      dtCore::Transform trans;
      t->GetTransform(trans);

      osg::Vec3 hpr;
      trans.GetRotation(hpr);

      return osg::Vec3(hpr[1],hpr[2],hpr[0]);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void TransformableActorProxy::SetTranslation(const osg::Vec3 &translation)
   {
      dtCore::Transformable *t = static_cast<dtCore::Transformable*>(GetActor());
      
      dtCore::Transform trans;
      t->GetTransform(trans);
      osg::Vec3 oldTrans;
      trans.GetTranslation(oldTrans);
      trans.SetTranslation(translation[0], translation[1], translation[2]);
      t->SetTransform(trans);

      //If we have a billboard update its position as well.
      if (GetRenderMode() == ActorProxy::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON ||
          GetRenderMode() == ActorProxy::RenderMode::DRAW_BILLBOARD_ICON)
      {
         ActorProxyIcon *billBoard = GetBillBoardIcon();
         if (billBoard != NULL)
            billBoard->SetPosition(translation);
      }

      OnTranslation(oldTrans, translation);
   }

   ///////////////////////////////////////////////////////////////////////////////
   osg::Vec3 TransformableActorProxy::GetTranslation()
   {
      dtCore::Transformable *t = static_cast<dtCore::Transformable*>(GetActor());
      
      dtCore::Transform trans;
      t->GetTransform(trans);
      float x, y, z;
      trans.GetTranslation(x, y, z);
      return osg::Vec3(x, y, z);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void TransformableActorProxy::SetScale(const osg::Vec3 &scale)
   {
      dtCore::Transformable *t = static_cast<dtCore::Transformable*>(GetActor());
      
      dtCore::Transform trans;
      t->GetTransform(trans);
      osg::Vec3 oldScale;
      trans.GetScale(oldScale);
      trans.SetScale(scale[0], scale[1], scale[2]);
      t->SetTransform(trans);

      //If we have a billboard update its scale as well.
      if (GetRenderMode() == ActorProxy::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON ||
          GetRenderMode() == ActorProxy::RenderMode::DRAW_BILLBOARD_ICON)
      {
         ActorProxyIcon *billBoard = GetBillBoardIcon();
         if (billBoard != NULL)
            billBoard->SetScale(scale);
      }

      OnScale(oldScale, scale);
   }

   ///////////////////////////////////////////////////////////////////////////////
   osg::Vec3 TransformableActorProxy::GetScale()
   {
      dtCore::Transformable *t = static_cast<dtCore::Transformable*>(GetActor());
      
      dtCore::Transform trans;
      t->GetTransform(trans);
      float x, y, z;
      trans.GetScale(x, y, z);
      return osg::Vec3(x, y, z);
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
}
