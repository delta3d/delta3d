#ifndef DT_PHYSICS_TESSELATION_MODE
#define DT_PHYSICS_TESSELATION_MODE
/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2016, Caper Holdings, LLC
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

#include <dtUtil/enumeration.h>
#include <dtVoxel/export.h>

namespace dtVoxel
{
   /**
    * Defines the mode of tesselation for physics geometry for the voxels.
    */
   class DT_VOXEL_EXPORT PhysicsTesselationMode : public dtUtil::Enumeration
   {
   public:
      DECLARE_ENUM(PhysicsTesselationMode)

      static PhysicsTesselationMode BOX_2_TRI_PER_SIDE;
      static PhysicsTesselationMode BOX_1_TRI_PER_SIDE;
      //TesselationMode SINGLE_TRI;
   protected:
      PhysicsTesselationMode(const std::string& name);
   };
}

#endif // DT_PHYSICS_TESSELATION_MODE
