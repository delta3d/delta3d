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

#include "dtCore/export.h"
#include "dtCore/macros.h"
#include "dtCore/deltadrawable.h"


namespace dtCore
{
   const int MAX_LIGHTS = 8;

   /**
   * An abstract class declaring the base functionality for a vertex light.
   * It's properties are thin wrappers of OpenGL light calls.
   */
   class DT_EXPORT Light : virtual public DeltaDrawable
   {

      DECLARE_MANAGEMENT_LAYER(Light)

   public:

      ///GLOBAL lights everything in scene, LOCAL only lights children
      enum LightingMode
      {
         GLOBAL = 0,
         LOCAL  = 1
      };

      Light( int number, const std::string& name = "defaultLight", LightingMode mode = GLOBAL );

      ///Copy constructor from an osg::LightSource
      Light( const osg::LightSource& lightSource, const std::string& name = "defaultLight", LightingMode mode = GLOBAL );
      
      virtual ~Light() = 0;

      virtual osg::Node* GetOSGNode() { return mLightSource.get(); } 

      ///Get the const internal osg::LightSource
      const osg::LightSource* GetLightSource() const
      { return mLightSource.get(); }

      ///Get the non-const internal osg::LightSource
      osg::LightSource* GetLightSource()
      { return mLightSource.get(); }

      ///Change lighting mode to GLOBAL or LOCAL mode
      void SetLightingMode( LightingMode mode );
      
      LightingMode GetLightingMode() const
      { return mLightingMode; }

      ///Turn light on or off
      void SetEnabled( bool enabled );
      
      bool GetEnabled()
      { return mEnabled; }

      /*!
      * Sets the OpenGL light number associated with this light. Lights numbers
      * must be in the range 0-7. There is a default scene light setup at light
      * number 0, so that will be overwritten if you set your own light to 0.
      *
      * @param number : The OpenGL light number
      */
      void SetNumber( int number )
      { mLightSource->getLight()->setLightNum( number ); }

      int GetNumber() const
      { return mLightSource->getLight()->getLightNum(); }

      void SetAmbient( float r, float g, float b, float a )
      { mLightSource->getLight()->setAmbient( osg::Vec4( r, g, b, a) ); }
      
      void GetAmbient( float& r, float& g, float& b, float& a ) const;
      
      void SetDiffuse( float r, float g, float b, float a )
      { mLightSource->getLight()->setDiffuse( osg::Vec4( r, g, b, a) ); }
      
      void GetDiffuse( float& r, float& g, float& b, float& a ) const;
      
      void SetSpecular( float r, float g, float b, float a )
      { mLightSource->getLight()->setSpecular( osg::Vec4( r, g, b, a) ); }
      
      void GetSpecular( float& r, float& g, float& b, float& a ) const;

      virtual void AddedToScene( Scene *scene );

   protected:

      LightingMode mLightingMode;
      RefPtr<osg::LightSource> mLightSource;
      bool mEnabled;
   };
}



#endif // DELTA_LIGHT
