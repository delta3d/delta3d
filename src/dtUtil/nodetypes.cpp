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
*
* Chris Rodgers
*/

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtUtil/nodetypes.h>
#include <map>


namespace dtUtil
{
   /////////////////////////////////////////////////////////////////////////////
   // GLOBALS
   /////////////////////////////////////////////////////////////////////////////
   typedef std::map<std::string, dtUtil::NodeType*> NodeClassToTypeMap;
   static NodeClassToTypeMap gNodeClassToTypeMap;



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   IMPLEMENT_ENUM(NodeType);

   const NodeType NodeType::UNKNOWN("Unknown", "Unknown");
   const NodeType NodeType::BILLBOARD("Billboard", "Billboard");
   const NodeType NodeType::BONE("Bone", "Bone");
   const NodeType NodeType::DOF("DOF", "DOFTransform");
   const NodeType NodeType::GEODE("Geode", "Geode");
   const NodeType NodeType::GEOMETRY("Geometry", "Geometry");
   const NodeType NodeType::GROUP("Group", "Group");
   const NodeType NodeType::MATRIX("Matrix", "MatrixTransform");
   const NodeType NodeType::LOD("LOD", "LOD");
   const NodeType NodeType::OCCLUDER("Occluder", "OccluderNode");
   const NodeType NodeType::OCCLUSION_QUERY("OcclusionQuery", "OcclusionQueryNode");
   const NodeType NodeType::SKELETON("Skeleton", "Skeleton");

   NodeType::NodeType(const std::string& name, const std::string& className)
      : BaseClass(name)
      , mClassName(className)
   {
      AddInstance(this);

      gNodeClassToTypeMap[className] = this;
   }

   const std::string& NodeType::GetClassName() const
   {
      return mClassName;
   }

   const NodeType* NodeType::GetNodeTypeByClassName(const std::string& className)
   {
      const NodeType* nodeType = nullptr;

      NodeClassToTypeMap::const_iterator foundIter = gNodeClassToTypeMap.find(className);

      if (foundIter != gNodeClassToTypeMap.end())
      {
         nodeType = foundIter->second;
      }

      return nodeType;
   }


}
