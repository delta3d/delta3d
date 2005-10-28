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
#ifndef DELTA_BEZIER_CONTROL_POINT_ACTOR_PROXY
#define DELTA_BEZIER_CONTROL_POINT_ACTOR_PROXY

#include <dtDAL/transformableactorproxy.h>
#include <dtDAL/plugin_export.h>
#include <dtDAL/exceptionenum.h>
#include <dtCore/transformable.h>

// Foward declarations
class BezierNode;

// Temp class
class BezierControlPoint : public dtCore::Transformable
{
   public:
      /// Constructor
      BezierControlPoint() : mBezierNode(NULL) { }
   
      /// Destructor
      virtual ~BezierControlPoint() {}

      /**
        * Sets the BezierNode of this BezierControlPoint
        * @param node The new node
        */
      void SetBezierNode(BezierNode *node) { mBezierNode = node; }

      /**
        * Gets the BezierNode of this BezierNode
        * @return The node
        */
      const BezierNode* GetBezierNode() const { return mBezierNode; }
       
   private:
      BezierNode *mBezierNode;
};

namespace dtActors
{
   class DT_PLUGIN_EXPORT BezierControlPointActorProxy : public dtDAL::TransformableActorProxy
   {
      public:
         /// Constructor
         BezierControlPointActorProxy()
         {
            SetClassName("BezierControlPoint");
         }

         /**
           * Builds the properties associated with this actor proxy
           */
         void BuildPropertyMap();

         /**
          * Sets the bezier node of this actor proxy
          * @param node The proxy that contains the node
          */
         void SetBezierNode(ActorProxy *node);

         /**
          * Gets the bezier node of this actor proxy
          * @return The bezier node
          */
         const BezierNode* GetBezierNode() const;

      protected:
         /// Destructor
         virtual ~BezierControlPointActorProxy()
         {
         }

         /// Creates the actor this proxy abstracts
         void CreateActor() { mActor = new BezierControlPoint; }
    };
}

#endif
