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
* @author Bradley Anderegg
*/
#include "dtActors/beziernodeactorproxy.h"
#include <dtDAL/actorproxyicon.h>
#include <dtDAL/enginepropertytypes.h>

namespace dtActors
{

   int BezierNodeActorProxy::mNumNodes = 0;


   void BezierNodeActorProxy::CreateActor()
   {
      mActor = new dtABC::BezierNode;
      
      std::ostringstream ss;
      ss << "BezierNode" << mNumNodes++;
      SetName(ss.str());
   }  


   void BezierNodeActorProxy::BuildPropertyMap()
   {
      dtABC::BezierNode *bn = dynamic_cast<dtABC::BezierNode*> (mActor.get());
      if(bn == NULL)
         EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtABC::BezierNode");

      dtDAL::TransformableActorProxy::BuildPropertyMap();

      AddProperty(new dtDAL::FloatActorProperty("Time to Next", "Time to Next",
         dtDAL::MakeFunctor(*bn, &dtABC::BezierNode::SetTimeToNext), 
         dtDAL::MakeFunctorRet(*bn, &dtABC::BezierNode::GetTimeToNext), 
         "Time it takes to get from this node to the next"));

      AddProperty(new dtDAL::FloatActorProperty("Time Step", "Time Step",
         dtDAL::MakeFunctor(*bn, &dtABC::BezierNode::SetStep), 
         dtDAL::MakeFunctorRet(*bn, &dtABC::BezierNode::GetStep), 
         "The Time step in seconds to interpolate between this node and the next"));

      AddProperty(new dtDAL::ActorActorProperty(*this, "Entry Control Point", "Entry Control Point", 
         dtDAL::MakeFunctor(*this, &BezierNodeActorProxy::SetBezierEntryControlPoint),
         dtDAL::MakeFunctorRet(*this, &BezierNodeActorProxy::GetBezierEntryControlPoint),
         "dtABC::BezierControlPoint", 
         "Sets the control point on this node from entry"));

      AddProperty(new dtDAL::ActorActorProperty(*this, "Exit Control Point", "Exit Control Point", 
         dtDAL::MakeFunctor(*this, &BezierNodeActorProxy::SetBezierExitControlPoint),
         dtDAL::MakeFunctorRet(*this, &BezierNodeActorProxy::GetBezierExitControlPoint),
         "dtABC::BezierControlPoint", 
         "Sets the control point on this node on exit"));

      AddProperty(new dtDAL::ActorActorProperty(*this, "Next Bezier Node", "Next Bezier Node", 
         dtDAL::MakeFunctor(*this, &BezierNodeActorProxy::SetNextBezierNode), 
         dtDAL::MakeFunctorRet(*this, &BezierNodeActorProxy::GetNextBezierNode), 
         "dtABC::BezierNode",
         "Sets the next node on this node"));

      AddProperty(new dtDAL::ActorActorProperty(*this, "Previous Bezier Node", "Previous Bezier Node", 
         dtDAL::MakeFunctor(*this, &BezierNodeActorProxy::SetPreviousBezierNode), 
         dtDAL::MakeFunctorRet(*this, &BezierNodeActorProxy::GetPreviousBezierNode), 
         "dtABC::BezierNode",
         "Sets the previous node on this node"));
   }

   void BezierNodeActorProxy::SetBezierEntryControlPoint(ActorProxy *controlPoint)   
   {

      ActorProxy* old = GetLinkedActor("Entry Control Point");
      //set the linked actor proxy for safe-keeping
      SetLinkedActor("Entry Control Point", controlPoint);

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
      dtABC::BezierNode *bn = dynamic_cast<dtABC::BezierNode*> (GetActor());
      if(bn == NULL)
         EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtABC::BezierNode");

      dtABC::BezierControlPoint* bcp = NULL;

      if(controlPoint)
      {
         bcp = dynamic_cast<dtABC::BezierControlPoint*> (controlPoint->GetActor());
         if(bcp == NULL)
            EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtABC::BezierControlPoint");
      }
      //set the actual node on the control point.
      bn->SetEntry(bcp);
      //whew! (wipes sweat from brow)
   }


   void BezierNodeActorProxy::SetBezierExitControlPoint(ActorProxy *controlPoint)   
   {
      ActorProxy* old = GetLinkedActor("Exit Control Point");
      //set the linked actor proxy for safe-keeping
      SetLinkedActor("Exit Control Point", controlPoint);

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
      dtABC::BezierNode *bn = dynamic_cast<dtABC::BezierNode*> (GetActor());
      if(bn == NULL)
         EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtABC::BezierNode");

      dtABC::BezierControlPoint* bcp = NULL;

      if(controlPoint)
      {
         bcp = dynamic_cast<dtABC::BezierControlPoint*> (controlPoint->GetActor());
         if(bcp == NULL)
            EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtABC::BezierControlPoint");
      }

      //set the actual node on the control point.     
      bn->SetExit(bcp);
      //whew! (wipes sweat from brow)
   }


   void BezierNodeActorProxy::SetNextBezierNode(ActorProxy *node)
   {
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
      dtABC::BezierNode *bn = dynamic_cast<dtABC::BezierNode*> (GetActor());
      if(bn == NULL)
         EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtABC::BezierNode");

      dtABC::BezierNode* nbn = NULL;
      if(node)
      {
         nbn = dynamic_cast<dtABC::BezierNode*> (node->GetActor());
         if(nbn == NULL)
            EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtABC::BezierNode");
      }

      //set the actual node on the control point.      
      bn->SetNext(nbn);
      //whew! (wipes sweat from brow)
   }

   void BezierNodeActorProxy::SetPreviousBezierNode(ActorProxy *node)
   {
      
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
      dtABC::BezierNode *bn = dynamic_cast<dtABC::BezierNode*> (GetActor());
      if(bn == NULL)
         EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtABC::BezierNode");

      dtABC::BezierNode* pbn = NULL;
      if(node)
      {
         pbn = dynamic_cast<dtABC::BezierNode*> (node->GetActor());
         if(pbn == NULL)
            EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtABC::BezierNode");
      }
      //set the actual node on the control point.      
      bn->SetPrev(pbn);
      //whew! (wipes sweat from brow)
   }

   dtCore::DeltaDrawable* BezierNodeActorProxy::GetNextBezierNode()
   {
      dtABC::BezierNode* bn = dynamic_cast< dtABC::BezierNode* >( GetActor() );
      if( bn == 0 )
      {
         EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtABC::BezierNode");
      }

      return bn->GetNext();
   }

   dtCore::DeltaDrawable* BezierNodeActorProxy::GetPreviousBezierNode()
   {  
      dtABC::BezierNode* bn = dynamic_cast< dtABC::BezierNode* >( GetActor() );
      if( bn == 0 )
      {
         EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtABC::BezierNode");
      }

      return bn->GetPrev();
   }

   dtCore::DeltaDrawable* BezierNodeActorProxy::GetBezierEntryControlPoint()
   {
      dtABC::BezierNode* bn = dynamic_cast< dtABC::BezierNode* >( GetActor() );
      if( bn == 0 )
      {
         EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtABC::BezierNode");
      }

      return bn->GetEntry();
   }

   dtCore::DeltaDrawable* BezierNodeActorProxy::GetBezierExitControlPoint()
   {
      dtABC::BezierNode* bn = dynamic_cast< dtABC::BezierNode* >( GetActor() );
      if( bn == 0 )
      {
         EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtABC::BezierNode");
      }

      return bn->GetExit();
   }

   const dtDAL::ActorProxy::RenderMode& BezierNodeActorProxy::GetRenderMode()
   {
         return dtDAL::ActorProxy::RenderMode::DRAW_BILLBOARD_ICON;
   }

   //////////////////////////////////////////////////////////////////////////
   dtDAL::ActorProxyIcon *BezierNodeActorProxy::GetBillBoardIcon()
   {
      if(!mBillBoardIcon.valid())
      {
         mBillBoardIcon =
            new dtDAL::ActorProxyIcon(dtDAL::ActorProxyIcon::IconType::GENERIC);
      }

      return mBillBoardIcon.get();
   }




   void BezierNodeActorProxy::OnScale(const osg::Vec3 &oldValue, const osg::Vec3 &newValue)
   {
      dtABC::BezierNode *bn = dynamic_cast<dtABC::BezierNode*> (mActor.get());
      if(bn == NULL)
         EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtABC::BezierNode");

      bn->SetDirtyFlag(true);
   }

   void BezierNodeActorProxy::OnRotation(const osg::Vec3 &oldValue, const osg::Vec3 &newValue)
   {
      dtABC::BezierNode *bn = dynamic_cast<dtABC::BezierNode*> (mActor.get());
      if(bn == NULL)
         EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtABC::BezierNode");
      
      bn->SetDirtyFlag(true);
   }

   void BezierNodeActorProxy::OnTranslation(const osg::Vec3 &oldValue, const osg::Vec3 &newValue)
   {
      dtABC::BezierNode *bn = dynamic_cast<dtABC::BezierNode*> (mActor.get());
      if(bn == NULL)
         EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtABC::BezierNode");

      bn->SetDirtyFlag(true);
   }



}
