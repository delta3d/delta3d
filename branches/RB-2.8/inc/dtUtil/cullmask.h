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
#ifndef DELTA_CULLMASK_H
#define DELTA_CULLMASK_H

#include <dtUtil/export.h>
#include <dtUtil/enumeration.h>

namespace dtUtil
{         
      
      class DT_UTIL_EXPORT CullMask : public dtUtil::Enumeration 
      {
         DECLARE_ENUM(CullMask);
      public:

         static CullMask NOTHING;                  //0

         static CullMask EVERYTHING;               //~0

         static CullMask SCENE_INTERSECT_MASK;           //0x00FFFFFF


         static CullMask MAIN_CAMERA_SINGLEPASS;         //0xFFFFFFFF         
         static CullMask MAIN_CAMERA_MULTIPASS;          //0xB100FFFF ( PRE_PROCESS, MULTIPASS, FOREGROUND, POST_PROCESS )      
         static CullMask MAIN_CAMERA_MASK;               //0xFFFFFFHFF (Set for actor property will choose appropriate mode.)
                  
         static CullMask ADDITIONAL_CAMERA_SINGLEPASS;   //0x7FFFFFFF (removes FOREGROUND for multiple screen rendering)        
         static CullMask ADDITIONAL_CAMERA_MULTIPASS;    //0x4EFFFFFF ( removes PRE_PROCESS, MULTIPASS, FOREGROUND, POST_PROCESS )      
         static CullMask ADDITIONAL_CAMERA_MASK;         //0x7FFFFFFF (Set for actor property will choose appropriate mode.)         
         

         static CullMask MULTIPASS_DEPTH_ONLY_MASK;      //0x00800000 (TERRAIN_GEOMETRY)
         static CullMask MULTIPASS_REFLECTION_MASK;      //0x4AFFFFFF (ADDITIONAL_CAMERA_MULTIPASS ^ WATER)
         static CullMask MULTIPASS_NON_TRANSPARENT_MASK; //0x40F0FFFF (BACKGROUND, and NON_TRANSPARENT_GEOMETRY)
         static CullMask MULTIPASS_TRANSPARENT_MASK;     //0x0E0FFFFF (WATER, TRANSPARENT_EFFECTS, VOLUMETRIC_EFFECTS, TRANSPARENT_GEOMETRY)
         


         /**
         *  @return the actual node mask value
         */
         unsigned int GetCullMask() const;
         void SetCullMask(unsigned int);


         //this is to allow setting without having to call GetCullMask()
         operator unsigned int () const
         {
            return GetCullMask();
         }

      protected:         
         CullMask(const std::string &name, unsigned int CullMask);

      private:
         unsigned int mCullMask;         
      };
}

#endif // DELTA_CULLMASK_H
