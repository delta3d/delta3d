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
 * Bradley Anderegg
 */
#include <dtActors/beziercontrolpointactorproxy.h>
#include <dtActors/beziernodeactorproxy.h>

#include <dtCore/actoractorproperty.h>
#include <dtCore/actorproxyicon.h>

#include <sstream>

namespace dtActors
{

  int BezierControlPointActorProxy::mNumControlPoints = 0;


   void BezierControlPointActorProxy::CreateDrawable()
   {
      SetDrawable(*new dtABC::BezierControlPoint);

      std::ostringstream ss;
      ss << "ControlPoint" << mNumControlPoints++;
      SetName(ss.str());
   }

   void BezierControlPointActorProxy::SetBezierNode(dtCore::BaseActorObject* node)
   {

      dtABC::BezierControlPoint *bcp = static_cast<dtABC::BezierControlPoint*> (GetDrawable());

      dtABC::BezierNode* bn = NULL;

      if (node != NULL)
      {
         bn = static_cast<dtABC::BezierNode*> (node->GetDrawable());
      }

      //set the actual node on the control point.
      bcp->SetParent(bn);
      //whew! (wipes sweat from brow)
   }

   dtCore::DeltaDrawable* BezierControlPointActorProxy::GetBezierNode()
   {
      dtABC::BezierControlPoint* bcp = static_cast<dtABC::BezierControlPoint*>( GetDrawable() );

      return bcp->GetParent();
   }

   void BezierControlPointActorProxy::BuildPropertyMap()
   {
      //dtABC::BezierControlPoint *bcp = static_cast<dtABC::BezierControlPoint*> (GetDrawable());

      dtCore::TransformableActorProxy::BuildPropertyMap();

      AddProperty(new dtCore::ActorActorProperty(*this, "Bezier Node", "Bezier Node",
               dtCore::ActorActorProperty::SetFuncType(this, &BezierControlPointActorProxy::SetBezierNode),
               dtCore::ActorActorProperty::GetFuncType(this, &BezierControlPointActorProxy::GetBezierNode),
               "dtABC::BezierNode",
               "Sets the Bezier Node of this proxy"));
   }


   const dtCore::BaseActorObject::RenderMode& BezierControlPointActorProxy::GetRenderMode()
   {
         return dtCore::BaseActorObject::RenderMode::DRAW_BILLBOARD_ICON;
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::ActorProxyIcon *BezierControlPointActorProxy::GetBillBoardIcon()
   {
      if (!mBillBoardIcon.valid())
      {
         mBillBoardIcon = new dtCore::ActorProxyIcon(dtCore::ActorProxyIcon::IMAGE_BILLBOARD_PATHNODE);
      }

      return mBillBoardIcon.get();
   }


   void BezierControlPointActorProxy::OnScale(const osg::Vec3& oldValue, const osg::Vec3& newValue)
   {
      dtABC::BezierControlPoint *bcp = static_cast<dtABC::BezierControlPoint*> (GetDrawable());

      if (bcp->GetParent() != NULL)
      {
         bcp->GetParent()->SetDirtyFlag(true);
      }
   }

   void BezierControlPointActorProxy::OnRotation(const osg::Vec3& oldValue, const osg::Vec3& newValue)
   {
      dtABC::BezierControlPoint *bcp = static_cast<dtABC::BezierControlPoint*> (GetDrawable());

      if (bcp->GetParent() != NULL)
      {
         bcp->GetParent()->SetDirtyFlag(true);
      }
   }

   void BezierControlPointActorProxy::OnTranslation(const osg::Vec3& oldValue, const osg::Vec3& newValue)
   {
      dtABC::BezierControlPoint *bcp = static_cast<dtABC::BezierControlPoint*> (GetDrawable());

      if (bcp->GetParent() != NULL)
      {
         bcp->GetParent()->SetDirtyFlag(true);
      }
   }


}
