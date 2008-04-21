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

#include <dtAI/waypoint.h>
#include <dtAI/waypointactor.h>
#include <dtCore/transformable.h>
#include <osg/Matrix>
#include <osg/MatrixTransform>
#include <dtUtil/mathdefines.h>
#include <dtUtil/log.h>

namespace dtAI
{
   Waypoint::Waypoint()
   : mID(0)
   , mRenderFlag(Waypoint::RENDER_DEFAULT)
   , mGradient(1.0f)
   , mAlpha(1.0f)
   , mPosition()
   {
   }

   Waypoint::Waypoint(const WaypointActor* pActor)
   {
      Set(pActor);
   }

   Waypoint::Waypoint(const osg::Vec3& pPos)
   : mID(0)
   , mRenderFlag(Waypoint::RENDER_DEFAULT)
   , mGradient(1.0f)
   , mAlpha(1.0f)
   {
      mPosition = pPos;
   }
   
   Waypoint::~Waypoint()
   {
   }

   bool Waypoint::operator==(const Waypoint& pWay) const
   {
      return dtUtil::Equivalent(mPosition, pWay.mPosition);
   }

   bool Waypoint::operator!=(const Waypoint& pWay) const
   {
      return !(operator==(pWay));
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

   void Waypoint::SetRenderFlag(RenderFlag rf) const
   {
      mRenderFlag = rf;
      SetColorFromRenderFlag();      
   }

   void Waypoint::SetColor(const osg::Vec3 &color)
   {
      mColor = color;

      // Since we are setting a custom color, 
      // change the render flag to match
      mRenderFlag = RENDER_CUSTOM;
   }

   const osg::Vec3& Waypoint::GetColor() const
   {
      return mColor;
   }

   void Waypoint::SetGradient(float pGradient) const
   {
      mGradient = pGradient;

      if (mRenderFlag != RENDER_CUSTOM)
      {
         SetColorFromRenderFlag();
      }
      else
      {
         LOG_WARNING("Setting gradient with the RENDER_CUSTOM flag has no effect");
      }
   }

   float Waypoint::GetGradient() const
   {
      return mGradient;
   }
     
   void Waypoint::SetAlpha(float newAlpha) const
   {
      mAlpha = newAlpha;
   }

   float Waypoint::GetAlpha() const
   {
      return mAlpha;
   }

   void Waypoint::SetColorFromRenderFlag() const
   {
      switch(mRenderFlag)
      {
         case RENDER_DEFAULT:
         {
            mColor = osg::Vec3(1.0f, 1.0f, 1.0f) * mGradient;
            break;
         }
         case RENDER_RED:
         {
            mColor = osg::Vec3(1.0f, 0.0f, 0.0f) * mGradient;
            break;
         }
         case RENDER_GREEN:
         {
            mColor = osg::Vec3(0.0f, 1.0f, 0.0f) * mGradient;
            break;
         }
         case RENDER_BLUE:
         {
            mColor = osg::Vec3(0.0f, 0.0f, 1.0f) * mGradient;
            break;
         }
         case RENDER_CUSTOM:
         {
            LOG_WARNING("There is no need to manually set the render flag to RENDER_CUSTOM");
            break;
         }
         default:
            LOG_WARNING("Trying to set unknown render flag.");
      }
   }

}//namespace 
