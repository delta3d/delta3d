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
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/actorproxyicon.h>
#include <sstream>

namespace dtActors
{

  int BezierControlPointActorProxy::mNumControlPoints = 0;


   void BezierControlPointActorProxy::CreateActor()
   {
      SetActor(*new dtABC::BezierControlPoint); 
      
      std::ostringstream ss;
      ss << "ControlPoint" << mNumControlPoints++;
      SetName(ss.str());
   }

   void BezierControlPointActorProxy::SetBezierNode(ActorProxy *node)
   {

      //ActorProxy* old = GetLinkedActor("Bezier Node");
      //set the linked actor proxy for safe-keeping
      SetLinkedActor("Bezier Node", node);

      //if (old != NULL)
      //{
      //   if (old == node)
      //   {
      //      //only return if the node is not NULL because if it is NULL
      //      //we want to make sure the actor is NULL too.
      //      return;         
      //   }
      //   else
      //   {
      //      //clear out old the proxy's control point to make sure we don't recurse 
      //      old->SetLinkedActor("Control Point", NULL);
      //      //set the value to NULL to clear out the internal data.
      //      static_cast<dtDAL::ActorActorProperty*>(old->GetProperty("Control Point"))->SetValue(NULL);
      //   }
      //}

      ////old and node are both non-NULL but are different.
      //if (node != NULL)
      //{
      //   static_cast<dtDAL::ActorActorProperty*>(node->GetProperty("Control Point"))->SetValue(this);

      //}

      //Get our actual actor te set
      dtABC::BezierControlPoint *bcp = static_cast<dtABC::BezierControlPoint*> (GetActor());

      dtABC::BezierNode* bn = NULL;

      if(node != NULL)
      {
         bn = static_cast<dtABC::BezierNode*> (node->GetActor());
      }

      //set the actual node on the control point.
      bcp->SetParent(bn);
      //whew! (wipes sweat from brow)
   }

   dtCore::DeltaDrawable* BezierControlPointActorProxy::GetBezierNode()
   {
      dtABC::BezierControlPoint* bcp = static_cast<dtABC::BezierControlPoint*>( GetActor() );

      return bcp->GetParent();
   }

   void BezierControlPointActorProxy::BuildPropertyMap()
   {
      //dtABC::BezierControlPoint *bcp = static_cast<dtABC::BezierControlPoint*> (GetActor());

      dtDAL::TransformableActorProxy::BuildPropertyMap();

      AddProperty(new dtDAL::ActorActorProperty(*this, "Bezier Node", "Bezier Node", 
         dtDAL::MakeFunctor(*this, &BezierControlPointActorProxy::SetBezierNode),
         dtDAL::MakeFunctorRet(*this, &BezierControlPointActorProxy::GetBezierNode), 
         "dtABC::BezierNode", 
         "Sets the Bezier Node of this proxy"));
   }


   const dtDAL::ActorProxy::RenderMode& BezierControlPointActorProxy::GetRenderMode()
   {
         return dtDAL::ActorProxy::RenderMode::DRAW_BILLBOARD_ICON;
   }

   //////////////////////////////////////////////////////////////////////////
   dtDAL::ActorProxyIcon *BezierControlPointActorProxy::GetBillBoardIcon()
   {
      if(!mBillBoardIcon.valid())
      {
         mBillBoardIcon =
            new dtDAL::ActorProxyIcon(dtDAL::ActorProxyIcon::IconType::GENERIC);
      }

      return mBillBoardIcon.get();
   }


   void BezierControlPointActorProxy::OnScale(const osg::Vec3 &oldValue, const osg::Vec3 &newValue)
   {
      dtABC::BezierControlPoint *bcp = static_cast<dtABC::BezierControlPoint*> (GetActor());

      if (bcp->GetParent() != NULL)
      {
         bcp->GetParent()->SetDirtyFlag(true);
      }
   }

   void BezierControlPointActorProxy::OnRotation(const osg::Vec3 &oldValue, const osg::Vec3 &newValue)
   {
      dtABC::BezierControlPoint *bcp = static_cast<dtABC::BezierControlPoint*> (GetActor());
      
      if (bcp->GetParent() != NULL)
      {
         bcp->GetParent()->SetDirtyFlag(true);
      }
   }

   void BezierControlPointActorProxy::OnTranslation(const osg::Vec3 &oldValue, const osg::Vec3 &newValue)
   {
      dtABC::BezierControlPoint *bcp = static_cast<dtABC::BezierControlPoint*> (GetActor());

      if(bcp->GetParent() != NULL)
      {
         bcp->GetParent()->SetDirtyFlag(true);
      }
   }


}
