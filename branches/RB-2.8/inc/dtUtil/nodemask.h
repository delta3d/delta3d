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


      static NodeMask PRE_PROCESS;              //0x10000000

      static NodeMask POST_PROCESS;             //0x20000000

      static NodeMask BACKGROUND;               //0x40000000

      static NodeMask FOREGROUND;               //0x80000000

      static NodeMask MULTIPASS;                //0x01000000

      static NodeMask RENDER_STATE_MASKS;       //0xF1000000


      static NodeMask TRANSPARENT_EFFECTS;      //0x02000000

      static NodeMask WATER;                    //0x04000000

      static NodeMask VOLUMETRIC_EFFECTS;       //0x08000000


      static NodeMask IGNORE_RAYCAST;           //0x0F000000


      static NodeMask TERRAIN_GEOMETRY;         //0x00800000

      static NodeMask NON_TRANSPARENT_GEOMETRY; //0x00F00000

      static NodeMask TRANSPARENT_GEOMETRY;     //0x000F0000

      static NodeMask DEFAULT_GEOMETRY;         //0x00FF0000 contains transparent and non transparent, ie not sorted



      //material states
      static NodeMask MAT_DIFFUSE_MAP;          //0x00000001
      static NodeMask MAT_VERTEX_COLOR;         //0x00000002
      static NodeMask MAT_REFLECTION;           //0x00000004
      static NodeMask MAT_SPECULAR_MAP;         //0x00000008
      static NodeMask MAT_ILLUMINATION_MAP;     //0x00000010
      static NodeMask MAT_NORMAL_MAP;           //0x00000020
      static NodeMask MAT_REFRACTION;           //0x00000040
      static NodeMask MAT_PARALLAX_MAP;         //0x00000080

      static NodeMask MAT_MASKS;                //0x000000FF

      //light states
      static NodeMask LIGHT_DYNAMIC;            //0x00000100
      static NodeMask LIGHT_STATIC;             //0x00000200
      static NodeMask LIGHT_SUN;                //0x00000400
      static NodeMask LIGHT_MOON;               //0x00000800

      //shadow states
      static NodeMask SHADOW_RECEIVE;           //0x00001000
      static NodeMask SHADOW_CAST;              //0x00002000


      static NodeMask LIGHT_MASKS;              //0x0000FF00



      /**
       *  @return the actual node mask value
       */
      unsigned int GetNodeMask() const;

      /***
       *
       *  Use this to override the default node masks
       *     this must happen before the scene is setup.
       */
      void SetNodeMask(unsigned int);


      //this is to allow setting without having to call GetNodeMask()
      operator unsigned int () const
      {
         return GetNodeMask();
      }

   protected:
      NodeMask(const std::string& name, unsigned int nodeMask);

   private:
      unsigned int mNodeMask;
   };
}

#endif // DELTA_NODEMASK_H
