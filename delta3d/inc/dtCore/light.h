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

//#include "dtCore/base.h"
#include "dtCore/export.h"
//#include "dtCore/notify.h"
#include "dtCore/macros.h"
#include "dtCore/deltadrawable.h"


namespace dtCore
{
   const int MAX_LIGHTS = 8;

   class DT_EXPORT Light : virtual public DeltaDrawable
   {

   public:

      enum LightingMode
      {
         GLOBAL = 0,
         LOCAL  = 1
      };

      Light( int number, LightingMode mode, osg::LightSource* lightSource );
      virtual ~Light() = 0;

      virtual osg::Node* GetOSGNode() { return mLightSource.get(); } 

      inline osg::LightSource* GetLightSource()
      { return mLightSource.get(); }

      inline void SetLightingMode( const LightingMode mode )
      {
         mLightingMode = mode;
      }
      
      inline LightingMode GetLightingMode() const
      { return mLightingMode; }

      void SetEnabled( bool enabled );
      
      inline bool GetEnabled()
      { return mEnabled; }
      
      void SetLightModel( osg::LightModel* model, bool enabled );

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

   protected:

      LightingMode mLightingMode;
      osg::ref_ptr<osg::LightSource> mLightSource;
      bool mEnabled;
   };
}



#endif // DELTA_LIGHT
