/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2004-2006 MOVES Institute
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
 * Bradley Anderegg 06/07/2006
 */

#ifndef __DELTA_WAYPOINT_H__
#define __DELTA_WAYPOINT_H__

#include <dtAI/export.h>
#include <osg/Vec3>

namespace dtAI 
{
   class WaypointActor;

   /**
    * This class encapsulates a Vec3 as a position
    * and is a place holder for other potential data
    * someone might want to store in a waypoint
    */
   class DT_AI_EXPORT Waypoint
   {
      public:

         //render flags will determine how we draw our waypoints
         enum RenderFlag{RENDER_DEFAULT = 0, RENDER_RED, RENDER_GREEN, RENDER_BLUE};

      public:
   
         Waypoint();
         Waypoint(const WaypointActor* pActor);
         Waypoint(const osg::Vec3& pPos);
         ~Waypoint();

         bool operator==(const Waypoint& pWay) const;
         bool operator!=(const Waypoint& pWay) const;

         void Set(const WaypointActor* pActor);
   
         const osg::Vec3& GetPosition() const;
         void SetPosition(const osg::Vec3& pVec);

         void SetID(unsigned pID);
         unsigned GetID()const;

         void SetRenderFlag(RenderFlag rf) const{mRenderFlag = rf;}
         RenderFlag GetRenderFlag() const {return mRenderFlag;}

         ///Set a color scaling value.  Used for rendering (0.0..1.0)
         void SetGradient(float pGradient) const;
         float GetGradient() const;

         ///Set an alpha transparency value.  Used for rendering (0.0..1.0)
         void SetAlpha(float newAlpha) const;
         float GetAlpha() const;

      private:        
         
         unsigned mID;

         mutable RenderFlag mRenderFlag;
         mutable float mGradient;
         mutable float mAlpha;

         osg::Vec3 mPosition;
   };
}//namespace 

#endif // __DELTA_WAYPOINT_H__
