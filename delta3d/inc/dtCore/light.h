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

namespace dtCore
{

   class DT_EXPORT Light : public DeltaDrawable, public Base
   {
      DECLARE_MANAGEMENT_LAYER(Light)

   public:

      enum LightingMode
      {
         GLOBAL = 0,
         LOCAL  = 1
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

      inline osg::Node* GetOSGNode()
      { return mLightSource.get(); }

      inline void SetLightingMode( const LightingMode mode )
      { mLightingMode = mode; }
      
      inline LightingMode GetLightingMode() const
      { return mLightingMode; }
      
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

      inline void SetPosition( const float x, const float y, const float z )
      {
         mLightSource->getLight()->setPosition( osg::Vec4( x, y, z, 1.0 ) );
      }
      
      inline void GetPosition( float* x, float* y, float *z ) const
      {
         osg::Vec4f position = mLightSource->getLight()->getPosition();
         *x = position[0]; *y = position[1]; *z = position[2];
      }

      inline void SetDirection( const float x, const float y, const float z )
      { mLightSource->getLight()->setDirection( osg::Vec3( x, y, z ) ); }
      
      inline void GetDirection( float* x, float* y, float *z ) const
      {
         osg::Vec3f direction = mLightSource->getLight()->getDirection();
         *x = direction[0]; *y = direction[1]; *z = direction[2];
      }

      inline void SetConstantAttenuation( float constant_attenuation )
      { mLightSource->getLight()->setConstantAttenuation( constant_attenuation ); }
      
      inline float GetConstantAttenuation() const
      { return mLightSource->getLight()->getConstantAttenuation(); }

      inline void SetLinearAttenuation( float linear_attenuation )
      { mLightSource->getLight()->setLinearAttenuation( linear_attenuation ); }
      
      inline float GetLinearAttenuation() const
      { return mLightSource->getLight()->getLinearAttenuation(); }

      inline void SetQuadraticAttenuation( float quadratic_attenuation )
      { mLightSource->getLight()->setQuadraticAttenuation( quadratic_attenuation ); }
      
      inline float GetQuadraticAttenuation() const
      { return mLightSource->getLight()->getQuadraticAttenuation(); }

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
