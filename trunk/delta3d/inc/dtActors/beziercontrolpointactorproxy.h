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
#ifndef DELTA_BEZIER_CONTROL_POINT_ACTOR_PROXY
#define DELTA_BEZIER_CONTROL_POINT_ACTOR_PROXY

#include <dtDAL/transformableactorproxy.h>
#include <dtDAL/plugin_export.h>
#include <dtDAL/exceptionenum.h>
#include <dtCore/transformable.h>
#include <dtABC/beziercontrolpoint.h>
#include <dtABC/beziernode.h>



namespace dtActors
{
   class DT_PLUGIN_EXPORT BezierControlPointActorProxy : public dtDAL::TransformableActorProxy
   {
      public:
         /// Constructor
         BezierControlPointActorProxy()
         {
            SetClassName("dtABC::BezierControlPoint");
         }

         /**
           * Builds the properties associated with this actor proxy
           */
         void BuildPropertyMap();

         /**
         * Cameras can be placed in a scene
         */
         virtual bool IsPlaceable() const { return true; }


         /**
          * Sets the bezier node of this actor proxy
          * @param node The proxy that contains the node
          */
         void SetBezierNode(ActorProxy *node);

         /**
          * Gets the bezier node of this actor proxy
          * @return The bezier node
          */
         const dtABC::BezierNode* GetBezierNode() const;


         dtDAL::ActorProxyIcon * GetBillBoardIcon();
         const dtDAL::ActorProxy::RenderMode& GetRenderMode();

         /*virtual*/ void OnScale(const osg::Vec3 &oldValue, const osg::Vec3 &newValue);
         /*virtual*/ void OnRotation(const osg::Vec3 &oldValue, const osg::Vec3 &newValue);
         /*virtual*/ void OnTranslation(const osg::Vec3 &oldValue, const osg::Vec3 &newValue);

      protected:
         /// Destructor
         virtual ~BezierControlPointActorProxy()
         {
         }

         /// Creates the actor this proxy abstracts
         void CreateActor();


         static int mNumControlPoints;
    };
}

#endif
