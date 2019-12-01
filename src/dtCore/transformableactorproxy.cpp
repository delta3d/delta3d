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
   {
      SetClassName("dtCore::Transformable");
   }

   /////////////////////////////////////////////////////////////////////////////
   void TransformableActorProxy::BuildPropertyMap()
   {
      dtCore::BaseActorObject::BuildPropertyMap();

      static const dtUtil::RefString GROUPNAME = "Transformable";

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
          newProp->SetSendInPartialUpdate(true);
          AddProperty(newProp);
      }

      if (IsTranslationPropertyShown())
      {
         newProp = new Vec3ActorProperty(PROPERTY_TRANSLATION, PROPERTY_TRANSLATION,
                  Vec3ActorProperty::SetFuncType(this, &TransformableActorProxy::SetTranslation),
                  Vec3ActorProperty::GetFuncType(this, &TransformableActorProxy::GetTranslation),
                  "Sets the location of a transformable in 3D space.",
                  GROUPNAME);
         newProp->SetSendInPartialUpdate(true);
         AddProperty(newProp);
      }

      AddProperty(new BooleanActorProperty(PROPERTY_NORMAL_RESCALING, PROPERTY_NORMAL_RESCALING,
               BooleanActorProperty::SetFuncType(trans, &dtCore::Transformable::SetNormalRescaling),
               BooleanActorProperty::GetFuncType(trans, &dtCore::Transformable::GetNormalRescaling),
               "Enables the automatic scaling of normals when a Transformable is scaled",
               GROUPNAME));


      static const dtUtil::RefString RENDER_PROXY_NODE_DESC("Enables the rendering of the proxy node for this Transformable");
      AddProperty(new BooleanActorProperty(PROPERTY_RENDER_PROXY_NODE, PROPERTY_RENDER_PROXY_NODE,
               BooleanActorProperty::SetFuncType(trans, &dtCore::Transformable::RenderProxyNode),
               BooleanActorProperty::GetFuncType(trans, &dtCore::Transformable::GetIsRenderingProxyNode),
               RENDER_PROXY_NODE_DESC,
               GROUPNAME));

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

}
