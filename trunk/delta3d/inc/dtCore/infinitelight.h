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

#ifndef DELTA_INFINITE_LIGHT
#define DELTA_INFINITE_LIGHT

#include "dtCore/base.h"
#include "dtCore/light.h"

namespace dtCore
{

   class DT_EXPORT InfiniteLight : public Base, public Light
   {
      DECLARE_MANAGEMENT_LAYER(InfiniteLight)

   public:

      InfiniteLight( int number, const std::string name = "defaultInfiniteLight", const LightingMode mode = GLOBAL );
      InfiniteLight( osg::LightSource* const source, const std::string name = "defaultInfiniteLight", const LightingMode mode = GLOBAL  );
      virtual ~InfiniteLight();

      void SetDirection( const float h, const float p, const float r );
      void GetDirection( float* h, float* p, float* r ) const;

   };

}

#endif // DELTA_INFINITE_LIGHT
