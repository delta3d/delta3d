/* 
* Delta3D Open Source Game and Simulation Engine 
* Copyright (C) 2004 MOVES Institute 
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
*/

#ifndef DELTA_RENDER_SURFACE
#define DELTA_RENDER_SURFACE

#include <Producer/RenderSurface>
#include "dtCore/export.h"

namespace dtCore
{
   /* Delta3D Implementation of Producer's RenderSurface. Producer does
    * account for the user changing the screen resolution at runtime
    * so we must add that functionality.
    */

   class DT_EXPORT DeltaRenderSurface : public Producer::RenderSurface
   {
   public:

         virtual void GetScreenWidthHeight( unsigned int* width, unsigned int* height );
         virtual void SetScreenWidthHeight( unsigned int width, unsigned int height );
   };
};

#endif // DELTA_RENDER_SURFACE