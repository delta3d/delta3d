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

#ifndef DELTA_POSITIONAL_LIGHT
#define DELTA_POSITIONAL_LIGHT

#include <dtCore/light.h>
#include <dtCore/transformable.h>

namespace dtCore
{
   /**
   * A transformable light. This is the class to use for omni-directional
   * lights that have actual locations within the scene.
   */
   class DT_CORE_EXPORT PositionalLight : public Light
   {
      DECLARE_MANAGEMENT_LAYER(PositionalLight)
         
   public:
      
      /**
       * Constructor
       *
       * @param number: the light number, 0-7, this will overright any other light with that number
       * @param name: a name for the light, defaulted to defaultPositonalLight
       * @param mode: specifys a lighting mode, GLOBAL effects whole scene, LOCAL only effects children
       */
      PositionalLight( int number, const std::string& name = "defaultPositonalLight", LightingMode mode = GLOBAL );

      ///Copy constructor from an osg::LightSource
      PositionalLight( const osg::LightSource& source, const std::string& name = "defaultPositonalLight", LightingMode mode = GLOBAL );

   protected:

      virtual ~PositionalLight();

   public:

      ///Set that values that control how fast light fades as one moves away from the light
      void SetAttenuation( float constant, float linear, float quadratic );
      void SetAttenuation( const osg::Vec3& constantLinearQuadratic );

      ///gets the value of the attenuation which controls how the light fades as objects get farther away
      void GetAttenuation( float& constant, float& linear, float& quadratic ) const;
      osg::Vec3 GetAttenuation() const;

   };
}

#endif // DELTA_POSITIONAL_LIGHT
