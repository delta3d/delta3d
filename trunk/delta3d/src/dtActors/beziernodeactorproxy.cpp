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
 * @author William E. Johnson II
 */
#include "dtActors/beziernodeactorproxy.h"
#include <dtDAL/enginepropertytypes.h>

namespace dtActors
{
   void BezierNodeActorProxy::BuildPropertyMap()
   {
      BezierNode *bn = dynamic_cast<BezierNode*> (mActor.get());
      if(bn == NULL)
         EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type BezierNode");

      dtDAL::TransformableActorProxy::BuildPropertyMap();

      AddProperty(new dtDAL::FloatActorProperty("Time", "Time",
         dtDAL::MakeFunctor(*bn, &BezierNode::SetTime), 
         dtDAL::MakeFunctorRet(*bn, &BezierNode::GetTime), 
         "Time it takes to get from this node to the next"));

      AddProperty(new dtDAL::ActorActorProperty(*this, "Control Point", "Control Point", 
         dtDAL::MakeFunctor(*this, &BezierNodeActorProxy::SetBezierControlPoint), "BezierControlPoint", 
         "Sets the control point on this node"));

      AddProperty(new dtDAL::ActorActorProperty(*this, "Next Bezier Node", "Next Bezier Node", 
         dtDAL::MakeFunctor(*this, &BezierNodeActorProxy::SetNextBezierNode), "BezierNode",
         "Sets the next node on this node"));

      AddProperty(new dtDAL::ActorActorProperty(*this, "Previous Bezier Node", "Previous Bezier Node", 
         dtDAL::MakeFunctor(*this, &BezierNodeActorProxy::SetPreviousBezierNode), "BezierNode",
         "Sets the previous node on this node"));
   }
   
   void BezierNodeActorProxy::SetBezierControlPoint(ActorProxy *controlPoint)   
   {
      BezierControlPoint* bcp = NULL;

      ActorProxy* old = GetLinkedActor("Control Point");
      //set the linked actor proxy for safe-keeping
      SetLinkedActor("Control Point", controlPoint);

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
            //clear out old the old proxy's bezier node to make sure we don't recurse 
            old->SetLinkedActor("Bezier Node", NULL);
            //set the value to NULL to clear out the internal data.
            static_cast<dtDAL::ActorActorProperty*>(old->GetProperty("Bezier Node"))->SetValue(NULL);
         }
      }

      //old and node are both non-NULL but are different.
      if (controlPoint != NULL)
      {
         static_cast<dtDAL::ActorActorProperty*>(controlPoint->GetProperty("Bezier Node"))->SetValue(this);
      }

      //Get our actual actor to set
      BezierNode *bn = dynamic_cast<BezierNode*> (GetActor());
      if(bn == NULL)
         EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type BezierNode");

      //set the actual node on the control point.
      bn->SetBezierControlPoint(bcp);
      //whew! (wipes sweat from brow)
   }

   void BezierNodeActorProxy::SetNextBezierNode(ActorProxy *node)
   {
      BezierNode* nbn = NULL;

      ActorProxy* old = GetLinkedActor("Next Bezier Node");
      SetLinkedActor("Next Bezier Node", node);
      
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
            //clear out old the control point to make sure we don't recurse 
            old->SetLinkedActor("Previous Bezier Node", NULL);
            //set the value to NULL to clear out the internal data.
            static_cast<dtDAL::ActorActorProperty*>(old->GetProperty("Previous Bezier Node"))->SetValue(NULL);
         }
      }

      //old and node are both non-NULL but are different.
      if (node != NULL)
      {
         //set the linked actor proxy for safe-keeping

         static_cast<dtDAL::ActorActorProperty*>(node->GetProperty("Previous Bezier Node"))->SetValue(this);
      }

      //Get our actual actor to set
      BezierNode *bn = dynamic_cast<BezierNode*> (GetActor());
      if(bn == NULL)
         EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type BezierNode");

      //set the actual node on the control point.
      bn->SetNextBezierNode(nbn);
      //whew! (wipes sweat from brow)
   }

   void BezierNodeActorProxy::SetPreviousBezierNode(ActorProxy *node)
   {
      BezierNode* pbn = NULL;

      ActorProxy* old = GetLinkedActor("Previous Bezier Node");
      //set the linked actor proxy for safe-keeping
      SetLinkedActor("Previous Bezier Node", node);

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

            //clear out old the control point to make sure we don't recurse 
            old->SetLinkedActor("Next Bezier Node", NULL);
            //set the value to NULL to clear out the internal data.
            static_cast<dtDAL::ActorActorProperty*>(old->GetProperty("Next Bezier Node"))->SetValue(NULL);
         }
      }

      //old and node are both non-NULL but are different.
      if (node != NULL)
      {
         static_cast<dtDAL::ActorActorProperty*>(node->GetProperty("Next Bezier Node"))->SetValue(this);
      }

      //Get our actual actor to set
      BezierNode *bn = dynamic_cast<BezierNode*> (GetActor());
      if(bn == NULL)
         EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type BezierNode");

      //set the actual node on the control point.
      bn->SetPreviousBezierNode(pbn);
      //whew! (wipes sweat from brow)
   }
   
}
