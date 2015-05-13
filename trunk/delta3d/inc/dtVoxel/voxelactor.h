/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2015, Caper Holdings, LLC
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
 */

#ifndef INC_DTVOXEL_VOXELACTOR_H_
#define INC_DTVOXEL_VOXELACTOR_H_

#include <dtVoxel/export.h>
#include <dtGame/gameactorproxy.h>

namespace dtVoxel
{

   class DT_VOXEL_EXPORT VoxelActor: public dtGame::GameActorProxy
   {
   public:
      VoxelActor();

      /*override*/ void BuildPropertyMap();

   protected:
      virtual ~VoxelActor();
      /*override*/ void CreateDrawable();
   };

} /* namespace dtVoxel */

#endif /* INC_DTVOXEL_VOXELACTOR_H_ */
