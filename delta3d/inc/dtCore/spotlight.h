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

#ifndef DELTA_SPOT_LIGHT
#define DELTA_SPOT_LIGHT

#include "dtCore/positionallight.h"

namespace dtCore
{

   class DT_EXPORT SpotLight : public PositionalLight
   {
      DECLARE_MANAGEMENT_LAYER(SpotLight)

   public:

      SpotLight( int number, const std::string name, const LightingMode mode );

      SpotLight( osg::LightSource* const source, const std::string name, const LightingMode mode );
      virtual ~SpotLight();

      inline void SetSpotCutoff( float spot_cutoff )
      { mLightSource->getLight()->setSpotCutoff( spot_cutoff ); }

      inline float GetSpotCutoff() const
      { return mLightSource->getLight()->getSpotCutoff(); }

      inline void SetSpotExponent( float spot_exponent )
      { mLightSource->getLight()->setSpotExponent( spot_exponent ); }

      inline float GetSpotExponent() const
      { return mLightSource->getLight()->getSpotExponent(); }

   };

}

#endif // DELTA_SPOT_LIGHT
