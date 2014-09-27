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
#include <dtUtil/cullmask.h>

namespace dtUtil
{

   //////////////////////////////////////////////////////////////////////////
   IMPLEMENT_ENUM(CullMask);

   CullMask CullMask::NOTHING("NOTHING", 0);
   CullMask CullMask::EVERYTHING("EVERYTHING", ~0);
   
   CullMask CullMask::SCENE_INTERSECT_MASK("SCENE_INTERSECT_MASK",                    0x00FFFFFF);

   CullMask CullMask::MAIN_CAMERA_SINGLEPASS("MAIN_CAMERA_SINGLEPASS",                 0xFFFFFFFF);
   CullMask CullMask::MAIN_CAMERA_MULTIPASS("MAIN_CAMERA_MULTIPASS",                   0xB100FFFF);
   CullMask CullMask::MAIN_CAMERA_MASK("MAIN_CAMERA_MASK",                             0xFFFFFFFF);
   
   CullMask CullMask::ADDITIONAL_CAMERA_SINGLEPASS("ADDITIONAL_CAMERA_SINGLEPASS",     0x7FFFFFFF);
   CullMask CullMask::ADDITIONAL_CAMERA_MULTIPASS("ADDITIONAL_CAMERA_MULTIPASS",       0x4EFFFFFF);
   CullMask CullMask::ADDITIONAL_CAMERA_MASK("ADDITIONAL_CAMERA_MASK",                 0x7FFFFFFF);
   
   CullMask CullMask::MULTIPASS_DEPTH_ONLY_MASK("MULTIPASS_DEPTH_ONLY_MASK",           0x00800000);
   CullMask CullMask::MULTIPASS_REFLECTION_MASK("MULTIPASS_REFLECTION_MASK",           0x4AFFFFFF);
   CullMask CullMask::MULTIPASS_NON_TRANSPARENT_MASK("MULTIPASS_NON_TRANSPARENT_MASK", 0x40F0FFFF);
   CullMask CullMask::MULTIPASS_TRANSPARENT_MASK("MULTIPASS_TRANSPARENT_MASK",         0x0E0FFFFF);


   CullMask::CullMask(const std::string &name, unsigned int mask) 
      : dtUtil::Enumeration(name)
      , mCullMask(mask)
   {
      AddInstance(this);
   }

   unsigned int CullMask::GetCullMask() const
   {
      return mCullMask;
   }

   void CullMask::SetCullMask( unsigned int i)
   {
      mCullMask = i;
   }


   
}//namespace dtRender
