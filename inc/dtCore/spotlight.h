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

#ifndef DELTA_SPOT_LIGHT
#define DELTA_SPOT_LIGHT

#include <dtCore/positionallight.h>

namespace dtCore
{
   /** 
    * A light that has a position in the scene and also spotlight properties.
    * By maniupulating the SpotCutoff and SpotExponent you can get lights that
    * look like flashlights, lamps, or even pseudo-laser beams.
    */
   class DT_CORE_EXPORT SpotLight : public PositionalLight
   {
      DECLARE_MANAGEMENT_LAYER(SpotLight)

   public:

      /**
       * Constructor
       *
       * @param number: the light number, 0-7, this will overright any other light with that number
       * @param name: a name for the light, defaulted to defaultPositonalLight
       * @param mode: specifys a lighting mode, GLOBAL effects whole scene, LOCAL only effects children
       */
      SpotLight( int number, const std::string& name = "defaultSpotLight", LightingMode mode = GLOBAL );

      ///Copy constructor from an osg::LightSource
      SpotLight( const osg::LightSource& source, const std::string& name = "defaultSpotLight", LightingMode mode = GLOBAL  );

   protected:

      virtual ~SpotLight();

   public:

      ///SpotCutoff is half the angle the SpotLight's cone, default is 22.5
      void SetSpotCutoff( float spot_cutoff );

      float GetSpotCutoff() const;

      ///The higher the SpotExponent, the more concentrated the light will be in the center of the cone, default is 1.0
      void SetSpotExponent( float spot_exponent );

      float GetSpotExponent() const;
   };
}

#endif // DELTA_SPOT_LIGHT
