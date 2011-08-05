/* 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2004-2005 MOVES Institute 
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

#include <dtCore/light.h>

namespace dtCore
{
   /**
    * Light located an infinite distance from origin and therefore only has direction.
    * It's direction is set by azimuth and altitude.
    *
    * NOTE: As of Delta3D 1.3, the Light class is a sub-class of Transformable (it was
    * previously a sub-class of DeltaDrawable). This means InfintieTerrain is now also
    * a Transformable and has all the Transform and collision-related API. These functions
    * are undefined for this class since InfiniteLight technically does not have a XYZ
    * location in the world. SetRotation is used for the direction of the light.    
    */
   class DT_CORE_EXPORT InfiniteLight : public Light
   {
      DECLARE_MANAGEMENT_LAYER(InfiniteLight)

   public:

      /**
       * Constructor
       *
       * @param number: the light number, 0-7, this will overright any other light with that number
       * @param name: a name for the light, defaulted to defaultInfiniteLight
       * @param mode: specifys a lighting mode, GLOBAL effects whole scene, LOCAL only effects children
       */
      InfiniteLight(int number, const std::string& name = "defaultInfiniteLight", LightingMode mode = GLOBAL);

      /**
       * Constructor for an osg::LightSource
       */
      InfiniteLight(const osg::LightSource& source, const std::string& name = "defaultInfiniteLight", LightingMode mode = GLOBAL);

   protected:

      virtual ~InfiniteLight();
   };

}

#endif // DELTA_INFINITE_LIGHT
