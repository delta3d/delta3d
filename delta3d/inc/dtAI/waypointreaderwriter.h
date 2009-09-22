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

#ifndef DELTA_WAYPOINTREADERWRITER_H
#define DELTA_WAYPOINTREADERWRITER_H

#include <dtAI/export.h>
#include <dtAI/primitives.h>
#include <dtAI/waypointcollection.h>

#include <osg/Referenced>

#include <vector>
#include <string>
#include <utility>

namespace dtAI
{
   class WaypointInterface;
   class AIPluginInterface;
   class WaypointCollection;
   class NavMesh;

   class DT_AI_EXPORT WaypointReaderWriter : public osg::Referenced
   {
      public:
         WaypointReaderWriter(AIPluginInterface& aiInterface);

         void Clear();

         bool LoadWaypointFile(const std::string& filename);

         bool SaveWaypointFile(const std::string& filename);

      protected:
         /*virtual*/ ~WaypointReaderWriter();

         typedef std::vector<const dtAI::WaypointInterface*> ConstWaypointArray;

         void CreateWaypointCollectionEdges(ConstWaypointArray& wps, const NavMesh& nm);
         void Insert(WaypointInterface* wi, int searchLevel);
         void AssignChildren();
         void AssignChildEdges();

         AIPluginInterface* mAIInterface;

         typedef std::pair<WaypointID, WaypointID> WaypointIDPair;
         typedef std::vector<WaypointIDPair> WaypointIDPairArray;
         WaypointIDPairArray mCollectionChildren;
   };


} // namespace dtAI

#endif // DELTA_WAYPOINTREADERWRITER_H
