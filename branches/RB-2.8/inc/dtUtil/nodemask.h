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
#ifndef DELTA_NODEMASK_H
#define DELTA_NODEMASK_H

#include <dtUtil/export.h>
#include <dtUtil/enumeration.h>

namespace dtUtil
{         
      /*
      *
      *  These types define the different sub scenes which
      *     can be created.  
      */
      class DT_UTIL_EXPORT NodeMask : public dtUtil::Enumeration 
      {
         DECLARE_ENUM(NodeMask);
      public:


         static NodeMask NOTHING;                  //0

         static NodeMask EVERYTHING;               //~0

         static NodeMask BACKGROUND;               //0x10000000
         
         static NodeMask FOREGROUND;               //0x20000000

         static NodeMask TRANSPARENT_EFFECTS;      //0x40000000

         static NodeMask WATER;                    //0x80000000
         
         static NodeMask IGNORE_RAYCAST;           //0xF0000000

         static NodeMask TERRAIN_FEATURES;         //0x0F000000
         
         static NodeMask DEFAULT_GEOMETRY;         //0x00FF0000 contains transparent and non transparent, ie not sorted

         static NodeMask NON_TRANSPARENT_GEOMETRY; //0x00F00000

         static NodeMask TRANSPARENT_GEOMETRY;     //0x000F0000
         
         static NodeMask SHADOW_RECEIVE;           //0x00001000
         
         static NodeMask SHADOW_CAST;              //0x00002000

         

         //cull masks , do we need a separate class?
         static NodeMask MAIN_CAMERA_CULL_MASK;//0xFFFFFFFF         
         
         static NodeMask SCENE_INTERSECT_MASK;     //0x0FFFFFFF

         //additional camera cull mask removes the UI or foreground bit
         //this supports multiple views or screens where the UI is only on the center
         static NodeMask ADDITIONAL_CAMERA_CULL_MASK;//0xDFFFFFFF

         static NodeMask NON_TRANSPARENT_SCENE_CULL_MASK;//0x1FF00000
         
         static NodeMask TRANSPARENT_ONLY_CULL_MASK;//0xC00F0000



         /**
         *  @return the actual node mask value
         */
         unsigned int GetNodeMask() const;


         //this is to allow setting without having to call GetNodeMask()
         operator unsigned int () const
         {
            return GetNodeMask();
         }

      protected:         
         NodeMask(const std::string &name, unsigned int nodeMask);

      private:
         const unsigned int mNodeMask;         
      };
}

#endif // DELTA_NODEMASK_H
