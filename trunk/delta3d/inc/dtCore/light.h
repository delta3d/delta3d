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

#include "dtCore/export.h"
#include "dtCore/transformable.h"
#include "dtCore/deltadrawable.h"

namespace dtCore
{

   class DT_EXPORT Light : public Transformable, public DeltaDrawable
   {

   public:

      enum LightingMode
      {
         GLOBAL = 0,
         LOCAL  = 1
      };

      Light( int number, const std::string name, const LightingMode mode );

      //make copy constructor
      Light( osg::LightSource* const source, const std::string name, const LightingMode mode, osg::LightModel* const model = NULL );
      virtual ~Light();

      inline void SetName( const std::string name ) { mName = name; };
      inline const std::string GetName() const { return mName; }

      inline osg::Light* GetOSGLight() { return mLightSource->getLight(); }
      inline const osg::Light* GetOSGLight() const { return mLightSource->getLight(); }

      inline osg::LightSource* GetOSGLightSource() { return mLightSource.get(); }
      inline const osg::LightSource* GetOSGLightSource() const { return mLightSource.get(); }

      inline osg::Node* GetOSGNode() { return mLightSource.get(); }

      inline void SetLightingMode( const LightingMode mode ) { mLightingMode = mode; }
      inline const LightingMode GetLightingMode() const { return mLightingMode; }

      inline void SetNumber( const int number ) { mLightSource->getLight()->setLightNum( number ); }
      inline const int GetNumber() const { return mLightSource->getLight()->getLightNum(); }

      inline void SetAmbient( const osg::Vec4& ambient ) { mLightSource->getLight()->setAmbient( ambient ); }
      inline const osg::Vec4& GetAmbient() const { return  mLightSource->getLight()->getAmbient(); }

      inline void SetDiffuse( const osg::Vec4& diffuse ) { mLightSource->getLight()->setDiffuse( diffuse ); }
      inline const osg::Vec4& GetDiffuse() const { return mLightSource->getLight()->getDiffuse(); }

      inline void SetSpecular( const osg::Vec4& specular ) { mLightSource->getLight()->setSpecular( specular ); }
      inline const osg::Vec4& GetSpecular() const { return mLightSource->getLight()->getSpecular(); }

      inline void SetPosition( const osg::Vec4& position ) { mLightSource->getLight()->setPosition( position ); }
      inline const osg::Vec4& GetPosition() const { return mLightSource->getLight()->getPosition(); }

      inline void SetDirection( const osg::Vec3& direction ) { mLightSource->getLight()->setDirection( direction ); }
      inline const osg::Vec3& GetDirection() const { return mLightSource->getLight()->getDirection(); }

      inline void SetConstantAttenuation( float constant_attenuation ) { mLightSource->getLight()->setConstantAttenuation( constant_attenuation ); }
      inline float GetConstantAttenuation() const { return mLightSource->getLight()->getConstantAttenuation(); }

      inline void SetLinearAttenuation( float linear_attenuation ) { mLightSource->getLight()->setLinearAttenuation( linear_attenuation ); }
      inline float GetLinearAttenuation() const { return mLightSource->getLight()->getLinearAttenuation(); }

      inline void SetQuadraticAttenuation( float quadratic_attenuation ) { mLightSource->getLight()->setQuadraticAttenuation( quadratic_attenuation ); }
      inline float GetQuadraticAttenuation() const { return mLightSource->getLight()->getQuadraticAttenuation(); }

      inline void SetSpotExponent( float spot_exponent ) { mLightSource->getLight()->setSpotExponent( spot_exponent ); }
      inline float GetSpotExponent() const { return mLightSource->getLight()->getSpotExponent(); }

      inline void SetSpotCutoff( float spot_cutoff ) { mLightSource->getLight()->setSpotCutoff( spot_cutoff ); }
      inline float GetSpotCutoff() const { return mLightSource->getLight()->getSpotCutoff(); }

   private:
      std::string mName;
      LightingMode mLightingMode;
      osg::ref_ptr<osg::LightSource> mLightSource;
      osg::ref_ptr<osg::LightModel> mLightModel;

   };

}

#endif // DELTA_LIGHT
