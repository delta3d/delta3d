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

#ifndef DELTA_BEZIER_NODE_ACTOR_PROXY
#define DELTA_BEZIER_NODE_ACTOR_PROXY

#include <dtCore/transformableactorproxy.h>
#include <dtCore/plugin_export.h>
#include <dtCore/transformable.h>
#include <dtABC/beziernode.h>
#include <dtABC/beziercontrolpoint.h>
#include "dtActors/beziercontrolpointactorproxy.h"


namespace dtActors
{
   class DT_PLUGIN_EXPORT BezierNodeActorProxy : public dtCore::TransformableActorProxy
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
         void BuildPropertyMap() override;

         /**
          * Cameras can be placed in a scene
          */
         bool IsPlaceable() const override { return true; }


         /**
          * Sets the bezier control point on enter for this proxy's actor
          * @param controlPoint The entry controlPoint to set
          */
         void SetBezierEntryControlPoint(dtCore::BaseActorObject* controlPoint);

         /**
          * Sets the bezier control point on exit for this proxy's actor
          * @param controlPoint The exit controlPoint to set
          */
         void SetBezierExitControlPoint(dtCore::BaseActorObject* controlPoint);

         /**
          * Sets the next bezier node on this proxy's actor
          * @param node The node to set
          */
         void SetNextBezierNode(dtCore::BaseActorObject* node);

         /**
          * Sets the previous bezier node on this proxy's actor
          * @param node The node to set
          */
         void SetPreviousBezierNode(dtCore::BaseActorObject* node);

         /**
          * Gets the next node on this proxy's actor
          * @return The node
          */
         dtCore::DeltaDrawable* GetNextBezierNode();

         /**
          * Gets the previous node on this proxy's actor
          * @return The node
          */
         dtCore::DeltaDrawable* GetPreviousBezierNode();

         /**
          * Gets the entry bezier control point on this proxy's actor
          * @return controlPoint The entry controlPoint for this proxy's actor
          */
         dtCore::DeltaDrawable* GetBezierEntryControlPoint();

         /**
         * Gets the exit bezier control point on this proxy's actor
         * @return controlPoint The exit controlPoint for this proxy's actor
         */
         dtCore::DeltaDrawable* GetBezierExitControlPoint();

         dtCore::ActorProxyIcon* GetBillBoardIcon() override;


         const dtCore::BaseActorObject::RenderMode& GetRenderMode() override;

         void OnRotation(const osg::Vec3& oldValue, const osg::Vec3& newValue) override;
         void OnTranslation(const osg::Vec3& oldValue, const osg::Vec3& newValue) override;



      protected:

         /// Destructor
         virtual ~BezierNodeActorProxy()
         {
         }

         /// Creates the actor that this proxy abstracts
         virtual void CreateDrawable();

         dtCore::RefPtr<BaseActorObject> mBezierNode, mEntryCp, mExitCp, mNextBezierNode, mPrevBezierNode;

         static int mNumNodes;
   };
} // namespace dtActors

#endif // DELTA_BEZIER_NODE_ACTOR_PROXY
