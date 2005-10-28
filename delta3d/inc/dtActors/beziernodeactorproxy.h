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
#ifndef DELTA_BEZIER_NODE_ACTOR_PROXY
#define DELTA_BEZIER_NODE_ACTOR_PROXY

#include <dtDAL/transformableactorproxy.h>
#include <dtDAL/plugin_export.h>
#include <dtDAL/exceptionenum.h>
#include <dtCore/transformable.h>
#include "dtActors/beziercontrolpointactorproxy.h"

class BezierNode : public dtCore::Transformable
{
   public:
      /// Constructor
      BezierNode() : mControlPoint(NULL), mNext(NULL), mPrevious(NULL), mTime(0.0f) 
      { 
      }
      /// Destructor
      virtual ~BezierNode() { }

      /**
        * Sets the time of this BezierNode
        * @param time The new time
        */
      void SetTime(float time) { mTime = time; }

      /**
        * Gets the time of this BezierNode
        * @return The time
        */
      float GetTime() const { return mTime; }

      /**
        * Gets the control point of this BezierNode
        * @return The control point
        */
      const BezierControlPoint* GetBezierControlPoint() const { return mControlPoint.get(); }

      /**
        * Gets the next node of this
        * @return The next node
        */
      const BezierNode* GetNextBezierNode() const { return mNext.get(); }

      /**
        * Gets the next node of this
        * @return The next node
        */
      const BezierNode* GetPreviousBezierNode() const { return mPrevious.get(); }

      // These functions are actor properties and should only be set by the proxy
      /**
       * Sets the control point of this BezierNode
       * @param controlPoint The new control point
       */
      void SetBezierControlPoint(BezierControlPoint *controlPoint) 
      { 
         if (mControlPoint != NULL)
            mControlPoint->SetBezierNode(NULL);
         mControlPoint = controlPoint;
         if (mControlPoint != NULL)
            mControlPoint->SetBezierNode(this);
      }
      /**
       * Sets the next node of this BeizerNode
       * @param node The new node
       */
      void SetNextBezierNode(BezierNode *node) 
      {
         mNext = node;
      }
      /**
       * Sets the next node of this BeizerNode
       * @param node The new node
       */
      void SetPreviousBezierNode(BezierNode *node) 
      { 
         mPrevious = node;
      }
   private:
      dtCore::RefPtr<BezierControlPoint> mControlPoint;
      dtCore::RefPtr<BezierNode> mNext, mPrevious;
      float mTime;
};

namespace dtActors
{
   class DT_PLUGIN_EXPORT BezierNodeActorProxy : public dtDAL::TransformableActorProxy
   {
      public:
         /// Constructor
         BezierNodeActorProxy()
         {
            SetClassName("BezierNode");
         }

         /**
           * Builds the properties associated with this proxy
           */
         virtual void BuildPropertyMap();

         /**
          * Sets the bezier control point on this proxy's actor
          * @param controlPoint The controlPoint to set
          */
         void SetBezierControlPoint(ActorProxy *controlPoint);

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
         const BezierNode* GetNextBezierNode() const 
         {
            const BezierNode *bn = dynamic_cast<const BezierNode*> (mActor.get());
            if(bn == NULL)
               EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type BezierNode");
            
            return bn->GetNextBezierNode();
         }

         /**
          * Gets the previous node on this proxy's actor
          * @return The node
          */
         const BezierNode* GetPreviousBezierNode() const 
         {
            const BezierNode *bn = dynamic_cast<const BezierNode*> (mActor.get());
            if(bn == NULL)
               EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type BezierNode");

            return bn->GetPreviousBezierNode();
         }

         /**
          * Gets the bezier control point on this proxy's actor
          * @param controlPoint The controlPoint to set
          */
         const BezierControlPoint* GetBezierControlPoint() const 
         {
            const BezierNode *bn = dynamic_cast<const BezierNode*> (mActor.get());
            if(bn == NULL)
               EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type BezierNode");
            
            return bn->GetBezierControlPoint();
         }

      protected:

         /// Destructor
         virtual ~BezierNodeActorProxy()
         {
         }
      
         /// Creates the actor that this proxy abstracts
         virtual void CreateActor()
         {
            mActor = new BezierNode;
         }  
   };
}

#endif
