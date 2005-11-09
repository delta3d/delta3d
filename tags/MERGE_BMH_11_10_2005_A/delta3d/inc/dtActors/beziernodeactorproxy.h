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
#ifndef DELTA_BEZIER_NODE_ACTOR_PROXY
#define DELTA_BEZIER_NODE_ACTOR_PROXY

#include <dtDAL/transformableactorproxy.h>
#include <dtDAL/plugin_export.h>
#include <dtDAL/exceptionenum.h>
#include <dtCore/transformable.h>
#include <dtABC/beziernode.h>
#include <dtABC/beziercontrolpoint.h>
#include "dtActors/beziercontrolpointactorproxy.h"


namespace dtActors
{
   class DT_PLUGIN_EXPORT BezierNodeActorProxy : public dtDAL::TransformableActorProxy
   {
      public:
         /// Constructor
         BezierNodeActorProxy()
         {
            SetClassName("dtABC::BezierNode");
         }

         /**
           * Builds the properties associated with this proxy
           */
         virtual void BuildPropertyMap();

         /**
         * Cameras can be placed in a scene
         */
         virtual bool IsPlaceable() const { return true; }


         /**
          * Sets the bezier control point on enter for this proxy's actor
          * @param controlPoint The entry controlPoint to set
          */
         void SetBezierEntryControlPoint(ActorProxy *controlPoint);

         /**
         * Sets the bezier control point on exit for this proxy's actor
         * @param controlPoint The exit controlPoint to set
         */
         void SetBezierExitControlPoint(ActorProxy *controlPoint);

         /**
          * Sets the next bezier node on this proxy's actor
          * @param node The node to set
          */
         void SetNextBezierNode(ActorProxy *node);

         /**
          * Sets the previous bezier node on this proxy's actor
          * @param node The node to set
          */
         void SetPreviousBezierNode(ActorProxy *node);

         /**
          * Gets the next node on this proxy's actor
          * @return The node
          */
         const dtABC::BezierNode* GetNextBezierNode() const 
         {
            const dtABC::BezierNode *bn = dynamic_cast<const dtABC::BezierNode*> (mActor.get());
            if(bn == NULL)
               EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtABC::BezierNode");
            
            return bn->GetNext()->GetBezierInterface();
         }

         /**
          * Gets the previous node on this proxy's actor
          * @return The node
          */
         const dtABC::BezierNode* GetPreviousBezierNode() const 
         {
            const dtABC::BezierNode *bn = dynamic_cast<const dtABC::BezierNode*> (mActor.get());
            if(bn == NULL)
               EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtABC::BezierNode");

            return bn->GetPrev()->GetBezierInterface();
         }

         /**
          * Gets the entry bezier control point on this proxy's actor
          * @return controlPoint The entry controlPoint for this proxy's actor
          */
         const dtABC::BezierControlPoint* GetBezierEntryControlPoint() const 
         {
            const dtABC::BezierNode *bn = dynamic_cast<const dtABC::BezierNode*> (mActor.get());
            if(bn == NULL)
               EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtABC::BezierNode");

            return bn->GetEntry();
         }

         /**
         * Gets the exit bezier control point on this proxy's actor
         * @return controlPoint The exit controlPoint for this proxy's actor
         */
         const dtABC::BezierControlPoint* GetBezierExitControlPoint() const 
         {
            const dtABC::BezierNode *bn = dynamic_cast<const dtABC::BezierNode*> (mActor.get());
            if(bn == NULL)
               EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtABC::BezierNode");

            return bn->GetExit();
         }

         dtDAL::ActorProxyIcon * GetBillBoardIcon();


         const dtDAL::ActorProxy::RenderMode& GetRenderMode();

         /*virtual*/ void OnScale(const osg::Vec3 &oldValue, const osg::Vec3 &newValue);
         /*virtual*/ void OnRotation(const osg::Vec3 &oldValue, const osg::Vec3 &newValue);
         /*virtual*/ void OnTranslation(const osg::Vec3 &oldValue, const osg::Vec3 &newValue);



      protected:

         /// Destructor
         virtual ~BezierNodeActorProxy()
         {
         }
      
         /// Creates the actor that this proxy abstracts
         virtual void CreateActor();

         static int mNumNodes;
   };
}

#endif
