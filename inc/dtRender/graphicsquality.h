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
#ifndef DELTA_GRAPHICS_QUALITY_H
#define DELTA_GRAPHICS_QUALITY_H

#include <dtRender/dtrenderexport.h>
#include <dtUtil/enumeration.h>

namespace dtRender
{
   class DT_RENDER_EXPORT GraphicsQuality : public dtUtil::Enumeration 
   {
      DECLARE_ENUM(GraphicsQuality);
      public:
         
         static GraphicsQuality NO_GRAPHICS_SUPPORT;   //USED FOR HEADLESS RUNTIME
         static GraphicsQuality FASTEST;  //DOWNSCALE GRAPHICS TO SPEED UP APPLICATION
         static GraphicsQuality COMPATIBILITY_MODE;    //OPENGL 1.2- FIXED FUNCTION SUPPORT
         static GraphicsQuality MEDIUM;  //MEDIUM, GL 2, BASIC SHADER SUPPORT
         static GraphicsQuality HIGH;  //HIGH, , GL 3, GEOMETRY SHADERS AND SUCH
         static GraphicsQuality HIGHEST;  //HIGHEST, GL 4, TESSELATION, AND EVALUATION

         static GraphicsQuality DEFAULT;

      protected:         
         GraphicsQuality(const std::string &name);
   };
      
}

#endif // DELTA_GRAPHICS_QUALITY_H
