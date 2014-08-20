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
   
   NodeMask NodeMask::PRE_PROCESS("PRE_PROCESS",                           0x10000000);
   NodeMask NodeMask::POST_PROCESS("POST_PROCESS",                         0x20000000);
   NodeMask NodeMask::BACKGROUND("BACKGROUND",                             0x40000000);
   NodeMask NodeMask::FOREGROUND("FOREGROUND",                             0x80000000);
   NodeMask NodeMask::MULTIPASS("MULTIPASS",                               0x01000000);
   NodeMask NodeMask::RENDER_STATE_MASKS("RENDER_STATE_MASKS",             0xF1000000);
   
   NodeMask NodeMask::TRANSPARENT_EFFECTS("TRANSPARENT_EFFECTS",           0x02000000);
   NodeMask NodeMask::WATER("WATER",                                       0x04000000);
   NodeMask NodeMask::VOLUMETRIC_EFFECTS("VOLUMETRIC_EFFECTS",             0x08000000);

   NodeMask NodeMask::IGNORE_RAYCAST("IGNORE_RAYCAST",                     0x0F000000);

   NodeMask NodeMask::TERRAIN_GEOMETRY("TERRAIN_GEOMETRY",                 0x00800000);
   NodeMask NodeMask::NON_TRANSPARENT_GEOMETRY("NON_TRANSPARENT_GEOMETRY", 0x00F00000);
   NodeMask NodeMask::TRANSPARENT_GEOMETRY("TRANSPARENT_GEOMETRY",         0x000F0000);
   NodeMask NodeMask::DEFAULT_GEOMETRY("DEFAULT_GEOMETRY",                 0x00FF0000);


   NodeMask NodeMask::MAT_DIFFUSE_MAP("MAT_DIFFUSE_MAP",                   0x00000001);
   NodeMask NodeMask::MAT_VERTEX_COLOR("MAT_VERTEX_COLOR",                  0x00000002);
   NodeMask NodeMask::MAT_REFLECTION("MAT_REFLECTION",                      0x00000004);
   NodeMask NodeMask::MAT_SPECULAR_MAP("MAT_SPECULAR_MAP",                  0x00000008);
   NodeMask NodeMask::MAT_ILLUMINATION_MAP("MAT_ILLUMINATION_MAP",          0x00000010);
   NodeMask NodeMask::MAT_NORMAL_MAP("MAT_NORMAL_MAP",                      0x00000020);
   NodeMask NodeMask::MAT_REFRACTION("MAT_REFRACTION",                      0x00000040);
   NodeMask NodeMask::MAT_PARALLAX_MAP("MAT_PARALLAX_MAP",                  0x00000080);
   NodeMask NodeMask::MAT_MASKS("MAT_MASKS",                                0x000000FF);


   NodeMask NodeMask::LIGHT_DYNAMIC("LIGHT_DYNAMIC",                        0x00000100);
   NodeMask NodeMask::LIGHT_STATIC("LIGHT_STATIC",                          0x00000200);
   NodeMask NodeMask::LIGHT_SUN("LIGHT_SUN",                                0x00000400);
   NodeMask NodeMask::LIGHT_MOON("LIGHT_MOON",                             0x00000800);
   
   NodeMask NodeMask::SHADOW_RECEIVE("SHADOW_RECEIVE",                     0x00001000);
   NodeMask NodeMask::SHADOW_CAST("SHADOW_CAST",                           0x00002000);
   
   NodeMask NodeMask::LIGHT_MASKS("LIGHT_MASKS",                           0x0000FF00);


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

   void NodeMask::SetNodeMask( unsigned int i)
   {
      mNodeMask = i;
   }


   
}//namespace dtRender
