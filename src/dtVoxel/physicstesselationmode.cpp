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
#include <dtVoxel/physicstesselationmode.h>

namespace dtVoxel
{

   IMPLEMENT_ENUM(PhysicsTesselationMode)
   PhysicsTesselationMode PhysicsTesselationMode::BOX_2_TRI_PER_SIDE("BOX_2_TRI_PER_SIDE");
   PhysicsTesselationMode PhysicsTesselationMode::BOX_1_TRI_PER_SIDE("BOX_1_TRI_PER_SIDE");
   //TesselationMode TesselationMode::SINGLE_TRI("SINGLE_TRI");

   PhysicsTesselationMode::PhysicsTesselationMode(const std::string& name): dtUtil::Enumeration(name) { AddInstance(this); }
}
