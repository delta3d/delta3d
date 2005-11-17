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

#include "dtCore/light.h"
#include "dtCore/transformable.h"

namespace dtCore
{
   /**
   * A transformable light. This is the class to use for omni-directional
   * lights that have actual locations within the scene.
   */
   class DT_CORE_EXPORT PositionalLight : public Light, public Transformable
   {
      DECLARE_MANAGEMENT_LAYER(PositionalLight)
         
         public:
      
      /**
      *Constructor
      *
      *@param number: the light number, 0-7, this will overright any other light with that number
      *@param name: a name for the light, defaulted to defaultPositonalLight
      *@param mode: specifys a lighting mode, GLOBAL effects whole scene, LOCAL only effects children
      */
      PositionalLight( int number, const std::string& name = "defaultPositonalLight", LightingMode mode = GLOBAL );

      ///Copy constructor from an osg::LightSource
      PositionalLight( const osg::LightSource& source, const std::string& name = "defaultPositonalLight", LightingMode mode = GLOBAL );
      virtual ~PositionalLight();

      virtual osg::Node* GetOSGNode() { return mNode.get(); }

      ///Set that values that control how fast light fades as one moves away from the light
      void SetAttenuation( float constant, float linear, float quadratic );

      ///gets the value of the attenuation which controls how the light fades as objects get farther away
      void GetAttenuation( float& constant, float& linear, float& quadratic );
      
      ///adds a drawable as a child of this node
      virtual bool AddChild( DeltaDrawable *child ); 
      ///removes a drawable as a child of this node
      virtual void RemoveChild( DeltaDrawable *child );

      ///Adds the scene to this light, which makes the whole scene use this light
      virtual void AddedToScene( Scene *scene ) { Light::AddedToScene( scene ); }

      ///this class renders the light as a sphere for debugging purposes
      virtual void RenderProxyNode( bool enable = true ) { Transformable::RenderProxyNode( enable ); }

   };
}

#endif // DELTA_POSITIONAL_LIGHT
