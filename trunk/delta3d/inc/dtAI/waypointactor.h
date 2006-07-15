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

#ifndef __DELTA_WAYPOINTACTOR_H__
#define __DELTA_WAYPOINTACTOR_H__

#include <dtAI/export.h>
#include <dtCore/transformable.h>

namespace dtAI 
{

   /**
   * This class encapsulates a Waypoint
   */
   class DT_AI_EXPORT WaypointActor: public dtCore::Transformable
   {
   public:
      WaypointActor();
   protected:
      virtual ~WaypointActor();
   public:

      void SetIndex(unsigned pIndex){mIndex = pIndex;}
      unsigned GetIndex() const{return mIndex;}

   private:
      unsigned mIndex;

   };
}//namespace 

#endif // __DELTA_WAYPOINTACTOR_H__
