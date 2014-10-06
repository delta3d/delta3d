/* -*-c++-*-
 * dtPhysics
 * Copyright 2007-2008, Alion Science and Technology
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
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 * david
 */

#ifndef CUSTOMRAYCASTCALLBACKS_H_
#define CUSTOMRAYCASTCALLBACKS_H_

//////////////////////////////////////////////
///////// Avoid including this from a header
//////////////////////////////////////////////

#include <pal/palCollision.h>
#include <dtPhysics/physicsobject.h>

namespace dtPhysics
{
   /////////////////////////////////////////////////////////////////////////////////////////////
   // FindClosestHitCallback - a simple callback that allows you to exclude yourself, check for
   //    groups, and find the closest hit along a raycast.
   /////////////////////////////////////////////////////////////////////////////////////////////

   class FindClosestHitCallback : public palRayHitCallback
   {
   public:
      /////////////////////////////////////////////////////////////////////////////////////////////
      FindClosestHitCallback(float rayLength, osg::Referenced* ownerToIgnore = NULL)
         : mGotAHit(false)
         , mHitCount(0)
         , mOwnerToIgnore(ownerToIgnore)
         , mRayLength(rayLength)
      {  }

      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual ~FindClosestHitCallback(){}

      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual Float AddHit(palRayHit& hit)
      {
         if (hit.m_pBody != NULL)
         {
            ++mHitCount;
            dtPhysics::PhysicsObject* physObject = reinterpret_cast<dtPhysics::PhysicsObject*>(hit.m_pBody->GetUserData());

            // Ignore the hit if it is an object in this helper.
            if (physObject != NULL && physObject->GetUserData() != mOwnerToIgnore)
            {
               if (!mGotAHit || mClosestHit.m_fDistance > hit.m_fDistance)
               {
                  mGotAHit = true;
                  mClosestHit = hit;
                  mRayLength = mClosestHit.m_fDistance;
               }
            }
         }

         return mRayLength;
      }

   public:
      bool mGotAHit;
      palRayHit mClosestHit;
      unsigned mHitCount;
      osg::Referenced* mOwnerToIgnore;
      float mRayLength;
   };

   /////////////////////////////////////////////////////////////////////////////////////////////
   class FindAllHitsCallback : public palRayHitCallback
   {
   public:
      /////////////////////////////////////////////////////////////////////////////////////////////
      FindAllHitsCallback(float rayLength, osg::Referenced* ownerToIgnore = NULL)
         : mOwnerToIgnore(ownerToIgnore)
         , mRayLength(rayLength)
      {  }

      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual ~FindAllHitsCallback(){}

      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual Float AddHit(palRayHit& hit)
      {
         if (hit.m_pBody != NULL)
         {
            dtPhysics::PhysicsObject* physObject = reinterpret_cast<dtPhysics::PhysicsObject*>(hit.m_pBody->GetUserData());

            if (physObject != NULL && (physObject->GetUserData() == NULL || physObject->GetUserData() != mOwnerToIgnore))
            {
               dtPhysics::RayCast::Report report;
               dtPhysics::PalRayHitToRayCastReport(report, hit);
               mHits.push_back(report);
            }
         }
         return mRayLength;
      }

   public:
      std::vector<dtPhysics::RayCast::Report> mHits;
      osg::Referenced* mOwnerToIgnore;
      float mRayLength;
   };

}


#endif /* CUSTOMRAYCASTCALLBACKS_H_ */
