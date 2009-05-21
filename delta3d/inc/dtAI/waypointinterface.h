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
 * Bradley Anderegg 
 */

#ifndef __DELTA_WAYPOINTINTERFACE_H__
#define __DELTA_WAYPOINTINTERFACE_H__

#include <dtAI/export.h>
#include <dtAI/primitives.h>
#include <osg/Vec3>

namespace dtAI
{

   class DT_AI_EXPORT WaypointInterface
   {
   public:
      WaypointInterface();
      virtual ~WaypointInterface();

      WaypointID GetID() const;

      bool operator==(const WaypointInterface& pWay) const;
      bool operator!=(const WaypointInterface& pWay) const;

      virtual const osg::Vec3& GetPosition() const = 0;
      virtual void SetPosition(const osg::Vec3& pVec) = 0;

   protected:
      void SetID(WaypointID pID);

   private:
      WaypointID mID;

      static WaypointID mIDCounter;
   };

} // namespace dtAI

#endif // __DELTA_WAYPOINTINTERFACE_H__
