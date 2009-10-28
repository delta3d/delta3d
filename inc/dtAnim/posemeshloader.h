/* 
* Delta3D Open Source Game and Simulation Engine 
* Copyright (C) 2004-2008 MOVES Institute 
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
* Michael Guerrero
*/

#ifndef __POSE_MESH_LOADER_H__
#define __POSE_MESH_LOADER_H__

#include <vector>
#include <string>

#include "posemeshxml.h"

namespace dtAnim
{
   class PoseMeshLoader
   {
   public:
      typedef std::vector<PoseMeshData> MeshDataContainer;

      PoseMeshLoader();
      ~PoseMeshLoader();

      /*
      * Load Loads Pose Mesh specifications from a file
      * @param file the name of the pose mesh xml file
      * @param toFill the container where all loaded data will go
      */
      bool Load(const std::string& file, MeshDataContainer& toFill);
   };
}

#endif // __POSE_MESH_LOADER_H__
