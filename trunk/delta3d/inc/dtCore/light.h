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

#ifndef DELTA_LIGHT
#define DELTA_LIGHT

#include <osg/LightSource>
#include <osg/LightModel>
#include <osg/Light>

#include "dtCore/base.h"
#include "dtCore/export.h"
#include "dtCore/deltadrawable.h"
#include "dtCore/transformable.h"

namespace dtCore
{

   class DT_EXPORT Light : public DeltaDrawable,
                           public Transformable
   {
      DECLARE_MANAGEMENT_LAYER(Light)

   public:

      enum LightingMode
      {
         GLOBAL = 0,
         LOCAL  = 1
      };

      /*
      enum DirPos
      {
         DIRECTIONAL = 0,
         POSITONAL   = 1
      };
      */

      enum AttenuationType
      {
         CONSTANT  = 0,
         LINEAR    = 1,
         QUADRATIC = 2
      };

      Light( int number, const std::string name, const LightingMode mode );

      Light( osg::LightSource* const source, const std::string name, const LightingMode mode );
      virtual ~Light();
      
     /**
      * Notifies this light object that it has been added to
      * a scene.
      *
      * @param scene the scene to which this light object has
      * been added
      */
      virtual void AddedToScene( Scene* scene );    
      
      /**
      * Notifies this light object that it has been removed from
      * a scene.
      *
      * @param scene the scene to which this light object has
      * been removed
      */
      virtual void RemovedFromScene( Scene* scene ); 

      //get lightsource?
      inline osg::Node* GetOSGNode()
      { return mLightSource.get(); }

      inline void SetLightingMode( const LightingMode mode )
      { mLightingMode = mode; }
      
      inline LightingMode GetLightingMode() const
      { return mLightingMode; }

      void SetDirectionalLighting( bool directional );
      bool GetDirectionalLighting();
      
      inline void SetLightModel( osg::LightModel* model )
      { mLightSource->getOrCreateStateSet()->setAttributeAndModes( model, osg::StateAttribute::ON ); }

      inline void SetNumber( const int number )
      { mLightSource->getLight()->setLightNum( number ); }

      inline int GetNumber() const
      { return mLightSource->getLight()->getLightNum(); }

      inline void SetAmbient( const float r, const float g, const float b, const float a )
      { mLightSource->getLight()->setAmbient( osg::Vec4( r, g, b, a) ); }
      
      inline void GetAmbient( float* r, float* g, float* b, float* a ) const
      { 
         osg::Vec4f color = mLightSource->getLight()->getAmbient();
         *r = color[0]; *g = color[1]; *b = color[2]; *a = color[3];
      }
      
      inline void SetDiffuse( const float r, const float g, const float b, const float a )
      { mLightSource->getLight()->setDiffuse( osg::Vec4( r, g, b, a) ); }
      
      inline void GetDiffuse( float* r, float* g, float* b, float* a ) const
      {
         osg::Vec4f color = mLightSource->getLight()->getDiffuse();
         *r = color[0]; *g = color[1]; *b = color[2]; *a = color[3];
      }
      
      inline void SetSpecular( const float r, const float g, const float b, const float a )
      { mLightSource->getLight()->setSpecular( osg::Vec4( r, g, b, a) ); }
      
      inline void GetSpecular( float* r, float* g, float* b, float* a ) const
      {
         osg::Vec4f color = mLightSource->getLight()->getSpecular();
         *r = color[0]; *g = color[1]; *b = color[2]; *a = color[3];
      }

      // attenuation factor = 1 / ( k_c + k_l*(d) + k_q*(d^2) )
      // where k_c = constant, k_l = linear, k_q = quadractric
      void SetAttenuation( AttenuationType type, float value );
      float GetAttenuation( AttenuationType type );

      inline void SetSpotExponent( float spot_exponent )
      { mLightSource->getLight()->setSpotExponent( spot_exponent ); }
      
      inline float GetSpotExponent() const
      { return mLightSource->getLight()->getSpotExponent(); }

      inline void SetSpotCutoff( float spot_cutoff )
      { mLightSource->getLight()->setSpotCutoff( spot_cutoff ); }
      
      inline float GetSpotCutoff() const
      { return mLightSource->getLight()->getSpotCutoff(); }

   private:
      LightingMode mLightingMode;
      osg::ref_ptr<osg::LightSource> mLightSource;

   };

}

#endif // DELTA_LIGHT
