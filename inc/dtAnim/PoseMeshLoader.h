#ifndef __POSE_MESH_LOADER_H__
#define __POSE_MESH_LOADER_H__

#include <vector>
#include <string>

#include "PoseMeshXML.h"

namespace dtAnim
{
   class PoseMeshLoader
   {
   public:
      typedef std::vector<PoseMeshData> MeshDataContainer;

      PoseMeshLoader();
      ~PoseMeshLoader();

      bool Load(const std::string& file, MeshDataContainer& toFill);
   };
}

#endif // __POSE_MESH_LOADER_H__
