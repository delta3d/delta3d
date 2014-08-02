/* -*-c++-*-
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2014, Caper Holdings, LLC
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

#include <prefix/dtutilprefix.h>
#include <dtUtil/nodemask.h>

namespace dtUtil
{

   //////////////////////////////////////////////////////////////////////////
   IMPLEMENT_ENUM(NodeMask);

   NodeMask NodeMask::NOTHING("NOTHING", 0);
   NodeMask NodeMask::EVERYTHING("EVERYTHING", ~0);
   NodeMask NodeMask::BACKGROUND("BACKGROUND", 0x10000000);
   NodeMask NodeMask::FOREGROUND("FOREGROUND", 0x20000000);
   NodeMask NodeMask::TRANSPARENT_EFFECTS("TRANSPARENT_EFFECTS", 0x40000000);
   NodeMask NodeMask::WATER("WATER", 0x80000000);
   NodeMask NodeMask::IGNORE_RAYCAST("IGNORE_RAYCAST", 0xF0000000);
   NodeMask NodeMask::SCENE_INTERSECT_MASK("SCENE_INTERSECT_MASK", 0x0FFFFFFF);
   NodeMask NodeMask::TERRAIN_FEATURES("TERRAIN_FEATURES", 0x0F000000);
   NodeMask NodeMask::DEFAULT_GEOMETRY("DEFAULT_GEOMETRY", 0x00FF0000);
   NodeMask NodeMask::NON_TRANSPARENT_GEOMETRY("NON_TRANSPARENT_GEOMETRY", 0x00F00000);
   NodeMask NodeMask::TRANSPARENT_GEOMETRY("TRANSPARENT_GEOMETRY", 0x000F0000);
   NodeMask NodeMask::SHADOW_RECEIVE("SHADOW_RECEIVE", 0x00001000);
   NodeMask NodeMask::SHADOW_CAST("SHADOW_CAST", 0x00002000);
   NodeMask NodeMask::MAIN_CAMERA_CULL_MASK("MAIN_CAMERA_CULL_MASK", 0xFFFFFFFF);
   NodeMask NodeMask::ADDITIONAL_CAMERA_CULL_MASK("ADDITIONAL_CAMERA_CULL_MASK", 0xDFFFFFFF);
   NodeMask NodeMask::NON_TRANSPARENT_SCENE_CULL_MASK("NON_TRANSPARENT_SCENE_CULL_MASK", 0x1FF00000);
   NodeMask NodeMask::TRANSPARENT_ONLY_CULL_MASK("TRANSPARENT_ONLY_CULL_MASK", 0xC00F0000);


   NodeMask::NodeMask(const std::string &name, unsigned int mask) 
      : dtUtil::Enumeration(name)
      , mNodeMask(mask)
   {
      AddInstance(this);
   }

   unsigned int NodeMask::GetNodeMask() const
   {
      return mNodeMask;
   }


   
}//namespace dtRender