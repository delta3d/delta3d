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
#include <dtActors/beziernodeactorproxy.h>

#include <dtCore/actoractorproperty.h>
#include <dtCore/actorproxyicon.h>
#include <dtCore/floatactorproperty.h>
#include <dtCore/functor.h>
#include <dtCore/exceptionenum.h>
#include <sstream>

namespace dtActors
{

   int BezierNodeActorProxy::mNumNodes = 0;


   void BezierNodeActorProxy::CreateDrawable()
   {
      SetDrawable(*new dtABC::BezierNode);

      std::ostringstream ss;
      ss << "BezierNode" << mNumNodes++;
      SetName(ss.str());
   }


   void BezierNodeActorProxy::BuildPropertyMap()
   {
      dtABC::BezierNode *bn = static_cast<dtABC::BezierNode*> (GetDrawable());

      dtCore::TransformableActorProxy::BuildPropertyMap();

      AddProperty(new dtCore::FloatActorProperty("Time to Next", "Time to Next",
         dtCore::FloatActorProperty::SetFuncType(bn, &dtABC::BezierNode::SetTimeToNext),
         dtCore::FloatActorProperty::GetFuncType(bn, &dtABC::BezierNode::GetTimeToNext),
         "Time it takes to get from this node to the next"));

      AddProperty(new dtCore::FloatActorProperty("Time Step", "Time Step",
         dtCore::FloatActorProperty::SetFuncType(bn, &dtABC::BezierNode::SetStep),
         dtCore::FloatActorProperty::GetFuncType(bn, &dtABC::BezierNode::GetStep),
         "The Time step in seconds to interpolate between this node and the next"));

      AddProperty(new dtCore::ActorActorProperty(*this, "Entry Control Point", "Entry Control Point",
         dtCore::ActorActorProperty::SetFuncType(this, &BezierNodeActorProxy::SetBezierEntryControlPoint),
         dtCore::ActorActorProperty::GetFuncType(this, &BezierNodeActorProxy::GetBezierEntryControlPoint),
         "dtABC::BezierControlPoint",
         "Sets the control point on this node from entry"));

      AddProperty(new dtCore::ActorActorProperty(*this, "Exit Control Point", "Exit Control Point",
         dtCore::ActorActorProperty::SetFuncType(this, &BezierNodeActorProxy::SetBezierExitControlPoint),
         dtCore::ActorActorProperty::GetFuncType(this, &BezierNodeActorProxy::GetBezierExitControlPoint),
         "dtABC::BezierControlPoint",
         "Sets the control point on this node on exit"));

      AddProperty(new dtCore::ActorActorProperty(*this, "Next Bezier Node", "Next Bezier Node",
         dtCore::ActorActorProperty::SetFuncType(this, &BezierNodeActorProxy::SetNextBezierNode),
         dtCore::ActorActorProperty::GetFuncType(this, &BezierNodeActorProxy::GetNextBezierNode),
         "dtABC::BezierNode",
         "Sets the next node on this node"));

      AddProperty(new dtCore::ActorActorProperty(*this, "Previous Bezier Node", "Previous Bezier Node",
         dtCore::ActorActorProperty::SetFuncType(this, &BezierNodeActorProxy::SetPreviousBezierNode),
         dtCore::ActorActorProperty::GetFuncType(this, &BezierNodeActorProxy::GetPreviousBezierNode),
         "dtABC::BezierNode",
         "Sets the previous node on this node"));
   }

   void BezierNodeActorProxy::SetBezierEntryControlPoint(dtCore::BaseActorObject* controlPoint)
   {
      dtCore::BaseActorObject* old = mEntryCp;
      mEntryCp = controlPoint;

      if (old != NULL)
      {
         if (old == controlPoint)
         {
            //only return if the node is not NULL because if it is NULL
            //we want to make sure the actor is NULL too.
            return;
         }
         else
         {
            //set the value to NULL to clear out the internal data.
            static_cast<dtCore::ActorActorProperty*>(old->GetProperty("Bezier Node"))->SetValue(NULL);
         }
      }

      //old and node are both non-NULL but are different.
      if (controlPoint != NULL)
      {
         static_cast<dtCore::ActorActorProperty*>(controlPoint->GetProperty("Bezier Node"))->SetValue(this);
      }

      //Get our actual actor to set
      dtABC::BezierNode *bn = static_cast<dtABC::BezierNode*> (GetDrawable());

      dtABC::BezierControlPoint* bcp = NULL;

      if (controlPoint)
      {
         bcp = dynamic_cast<dtABC::BezierControlPoint*> (controlPoint->GetDrawable());
         if (bcp == NULL)
            throw dtCore::InvalidActorException(
               "Actor should be type dtABC::BezierControlPoint", __FILE__, __LINE__);
      }
      //set the actual node on the control point.
      bn->SetEntry(bcp);
      //whew! (wipes sweat from brow)
   }


   void BezierNodeActorProxy::SetBezierExitControlPoint(dtCore::BaseActorObject* controlPoint)
   {
      dtCore::BaseActorObject* old = mExitCp;

      if (old != NULL)
      {
         if (old == controlPoint)
         {
            //only return if the node is not NULL because if it is NULL
            //we want to make sure the actor is NULL too.
            return;
         }
         else
         {
            //set the value to NULL to clear out the internal data.
            static_cast<dtCore::ActorActorProperty*>(old->GetProperty("Bezier Node"))->SetValue(NULL);
         }
      }

      //old and node are both non-NULL but are different.
      if (controlPoint != NULL)
      {
         static_cast<dtCore::ActorActorProperty*>(controlPoint->GetProperty("Bezier Node"))->SetValue(this);
      }

      //Get our actual actor to set
      dtABC::BezierNode *bn = static_cast<dtABC::BezierNode*> (GetDrawable());

      dtABC::BezierControlPoint* bcp = NULL;

      if (controlPoint)
      {
         bcp = dynamic_cast<dtABC::BezierControlPoint*> (controlPoint->GetDrawable());
         if (bcp == NULL)
            throw dtCore::InvalidActorException(
               "Actor should be type dtABC::BezierControlPoint", __FILE__, __LINE__);
      }

      //set the actual node on the control point.
      bn->SetExit(bcp);
      //whew! (wipes sweat from brow)
   }


   void BezierNodeActorProxy::SetNextBezierNode(dtCore::BaseActorObject* node)
   {
      dtCore::BaseActorObject* old = mNextBezierNode;
      mNextBezierNode = node;

      if (old != NULL)
      {
         if (old == node)
         {
            //only return if the node is not NULL because if it is NULL
            //we want to make sure the actor is NULL too.
            return;
         }
         else
         {
            //set the value to NULL to clear out the internal data.
            static_cast<dtCore::ActorActorProperty*>(old->GetProperty("Previous Bezier Node"))->SetValue(NULL);
         }
      }

      //old and node are both non-NULL but are different.
      if (node != NULL)
      {
         //set the linked actor proxy for safe-keeping

         static_cast<dtCore::ActorActorProperty*>(node->GetProperty("Previous Bezier Node"))->SetValue(this);
      }

      //Get our actual actor to set
      dtABC::BezierNode *bn = static_cast<dtABC::BezierNode*> (GetDrawable());

      dtABC::BezierNode* nbn = NULL;
      if (node)
      {
         nbn = dynamic_cast<dtABC::BezierNode*> (node->GetDrawable());
         if (nbn == NULL)
            throw dtCore::InvalidActorException(
               "Actor should be type dtABC::BezierNode", __FILE__, __LINE__);
      }

      //set the actual node on the control point.
      bn->SetNext(nbn);
      //whew! (wipes sweat from brow)
   }

   //////////////////////////////////////////////////////////////////////////
   void BezierNodeActorProxy::SetPreviousBezierNode(dtCore::BaseActorObject* node)
   {

      dtCore::BaseActorObject* old = mPrevBezierNode;
      //set the linked actor proxy for safe-keeping
      mPrevBezierNode = node;

      if (old != NULL)
      {
         if (old == node)
         {
            //only return if the node is not NULL because if it is NULL
            //we want to make sure the actor is NULL too.
            return;
         }
         else
         {

            //set the value to NULL to clear out the internal data.
            static_cast<dtCore::ActorActorProperty*>(old->GetProperty("Next Bezier Node"))->SetValue(NULL);
         }
      }

      //old and node are both non-NULL but are different.
      if (node != NULL)
      {
         static_cast<dtCore::ActorActorProperty*>(node->GetProperty("Next Bezier Node"))->SetValue(this);
      }

      //Get our actual actor to set
      dtABC::BezierNode *bn = static_cast<dtABC::BezierNode*> (GetDrawable());

      dtABC::BezierNode* pbn = NULL;
      if (node)
      {
         pbn = static_cast<dtABC::BezierNode*> (node->GetDrawable());
      }
      //set the actual node on the control point.
      bn->SetPrev(pbn);
      //whew! (wipes sweat from brow)
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::DeltaDrawable* BezierNodeActorProxy::GetNextBezierNode()
   {
      dtABC::BezierNode* bn = static_cast< dtABC::BezierNode* >( GetDrawable() );

      return bn->GetNext();
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::DeltaDrawable* BezierNodeActorProxy::GetPreviousBezierNode()
   {
      dtABC::BezierNode* bn = static_cast< dtABC::BezierNode* >( GetDrawable() );

      return bn->GetPrev();
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::DeltaDrawable* BezierNodeActorProxy::GetBezierEntryControlPoint()
   {
      dtABC::BezierNode* bn = static_cast< dtABC::BezierNode* >( GetDrawable() );

      return bn->GetEntry();
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::DeltaDrawable* BezierNodeActorProxy::GetBezierExitControlPoint()
   {
      dtABC::BezierNode* bn = static_cast< dtABC::BezierNode* >( GetDrawable() );

      return bn->GetExit();
   }

   //////////////////////////////////////////////////////////////////////////
   const dtCore::BaseActorObject::RenderMode& BezierNodeActorProxy::GetRenderMode()
   {
      return dtCore::BaseActorObject::RenderMode::DRAW_BILLBOARD_ICON;
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::ActorProxyIcon *BezierNodeActorProxy::GetBillBoardIcon()
   {
      if (!mBillBoardIcon.valid())
      {
         mBillBoardIcon = new dtCore::ActorProxyIcon(dtCore::ActorProxyIcon::IMAGE_BILLBOARD_PATHNODE);
      }

      return mBillBoardIcon.get();
   }

   void BezierNodeActorProxy::OnScale(const osg::Vec3 &oldValue, const osg::Vec3 &newValue)
   {
      dtABC::BezierNode *bn = static_cast<dtABC::BezierNode*> (GetDrawable());

      bn->SetDirtyFlag(true);
   }

   void BezierNodeActorProxy::OnRotation(const osg::Vec3 &oldValue, const osg::Vec3 &newValue)
   {
      dtABC::BezierNode *bn = static_cast<dtABC::BezierNode*> (GetDrawable());

      bn->SetDirtyFlag(true);
   }

   void BezierNodeActorProxy::OnTranslation(const osg::Vec3 &oldValue, const osg::Vec3 &newValue)
   {
      dtABC::BezierNode *bn = static_cast<dtABC::BezierNode*> (GetDrawable());

      bn->SetDirtyFlag(true);
   }



}
