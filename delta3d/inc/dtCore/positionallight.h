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

#ifndef DELTA_POSITIONAL_LIGHT
#define DELTA_POSITIONAL_LIGHT

#include "dtCore/light.h"
#include "dtCore/transformable.h"

namespace dtCore
{
   class DT_EXPORT PositionalLight :  public Light,  public Transformable
   {
      DECLARE_MANAGEMENT_LAYER(PositionalLight)
         
         public:
      
      PositionalLight( int number, const std::string name = "defaultPositonalLight", const LightingMode mode = GLOBAL );
      PositionalLight( osg::LightSource* const source, const std::string name = "defaultPositonalLight", const LightingMode mode = GLOBAL );
      virtual ~PositionalLight();

      virtual osg::Node* GetOSGNode() { return mNode.get(); } 
      
      // attenuation factor = 1 / ( k_c + k_l*(d) + k_q*(d^2) )
      // where k_c = constant, k_l = linear, k_q = quadractric
      void SetAttenuation( const float constant, const float linear, const float quadratic );
      void GetAttenuation( float* constant, float* linear, float* quadratic );
      
      // Add a DeltaDrawable child
      virtual void AddChild( DeltaDrawable *child ); 
         
      // Remove a DeltaDrawable child
      virtual void RemoveChild( DeltaDrawable *child );

      virtual void AddedToScene( Scene *scene ) { Light::AddedToScene( scene ); }
      virtual void RenderProxyNode( const bool enable = true ) { Transformable::RenderProxyNode( enable ); }

   };
}

#endif // DELTA_POSITIONAL_LIGHT
