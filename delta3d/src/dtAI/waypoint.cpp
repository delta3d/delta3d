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
 * @author Bradley Anderegg 06/07/2006
 */

#include <dtAI/waypoint.h>
#include <dtAI/waypointactor.h>
#include <dtCore/transformable.h>
#include <osg/Matrix>
#include <osg/MatrixTransform>


namespace dtAI
{
   Waypoint::Waypoint(): mRenderFlag(Waypoint::RENDER_DEFUALT)
   {
   }

   Waypoint::Waypoint(const WaypointActor* pActor)
   {
      Set(pActor);
   }

   Waypoint::Waypoint(osg::Vec3 pPos)
   {
      mPosition = pPos;
   }
   
   Waypoint::~Waypoint()
   {
   }


   void Waypoint::Set(const WaypointActor* pActor)
   {
      osg::Matrix tranform = pActor->GetMatrixNode()->getMatrix();
      mPosition[0] = tranform(3,0);
      mPosition[1] = tranform(3,1);
      mPosition[2] = tranform(3,2);
   }

   const osg::Vec3& Waypoint::GetPosition() const
   {
      return mPosition;
   }

   void Waypoint::SetPosition(const osg::Vec3& pVec)
   {
      mPosition = pVec;
   }

   void Waypoint::SetID(unsigned pId)
   {
      mID = pId;
   }

   unsigned Waypoint::GetID() const
   {
      return mID;
   }
}//namespace 
