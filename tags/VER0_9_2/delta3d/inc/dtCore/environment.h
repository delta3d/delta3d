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

#ifndef DELTA_ENVIRONMENT
#define DELTA_ENVIRONMENT

#include "dtCore/deltadrawable.h"
#include "dtCore/enveffect.h"
#include "dtCore/sunlightshader.h"
#include "dtCore/skydomeshader.h"
#include "dtCore/skydome.h"
#include "dtCore/scene.h"
#include "dtUtil/deprecationmgr.h"


#include <osg/Vec2>
#include <osg/Vec3>
#include <osg/Group>
#include <osg/Fog>

namespace dtCore
{
   ///A unique environment which controls lighting and visibility
   class DT_EXPORT Environment : public dtCore::DeltaDrawable
   {
   public:
            DECLARE_MANAGEMENT_LAYER(Environment)

      Environment( const std::string& name = "Environment" );
      virtual ~Environment();

      enum FogMode {
         LINEAR = 0, ///<Linear fog
         EXP,        ///<Exponential fog
         EXP2,       ///<Exponential squared fog
         ADV         ///<Advanced light scattering "fog"
      };

      ///Notifies this object that it has been added to a Scene
      virtual void AddedToScene( Scene *scene );
         
      ///Add a DeltaDrawable to be rendered using this Environment's properties.
      virtual bool AddChild( DeltaDrawable *child );

		///Remove a DeltaDrawable added to the Environment.
		void RemoveChild( DeltaDrawable *child );

      ///Deprecated
      void AddDrawable( DeltaDrawable *drawable );

      ///Deprecated
      void RemoveDrawable( DeltaDrawable *drawable );

      /// Add an Environmental Effect to the Environment
      void AddEffect( EnvEffect *effect );

      /// Remove an EnvEffect from this Environment
      void RemEffect( EnvEffect *effect );

      ///Get an Environmental Effect by its index
      EnvEffect* GetEffect( int idx ) { return mEffectList[idx].get(); }

      ///Get the number of Environmental Effects in this Environment
      int GetNumEffects() { return mEffectList.size(); }

      ///Set the base color of the sky
      void SetSkyColor( const osg::Vec3& color );
      void GetSkyColor( osg::Vec3& color ) const { color = mSkyColor; }
      
      //DEPRECIATED
      void SetSkyColor( sgVec3 color )
      {
         DEPRECATE("void SetSkyColor( sgVec3 color )", "void SetSkyColor( const osg::Vec3& color )")
         SetSkyColor(osg::Vec3(color[0], color[1], color[2]));
      }
      void GetSkyColor( sgVec3 color ) const 
      { 
         DEPRECATE("void GetSkyColor( sgVec3 color ) const", "void GetSkyColor( osg::Vec3& color ) const") 
         osg::Vec3 tmp;
         GetSkyColor(tmp);
         color[0] = tmp[0]; color[1] = tmp[1]; color[2] = tmp[2];
      }
      
      ///Set the base color of the fog
      void SetFogColor( const osg::Vec3& color );
      void GetFogColor( osg::Vec3& color ) const { color = mFogColor; }
      
      //DEPRECATED
      void SetFogColor( sgVec3 color )
      {
         DEPRECATE("void SetFogColor( sgVec3 color )", "void SetFogColor( const osg::Vec3& color )")
         SetFogColor(osg::Vec3(color[0], color[1], color[2]));
      }
      //DEPRECATED
      void GetFogColor( sgVec3 color ) const 
      { 
         DEPRECATE("void GetFogColor( sgVec3 color ) const", "void GetFogColor( osg::Vec3& color ) const")
         osg::Vec3 tmp;
         GetFogColor(tmp);
         color[0] = tmp[0]; color[1] = tmp[1]; color[2] = tmp[2];
      }

	   ///Get the modified color of the fog
      void GetModFogColor( osg::Vec3& color ) const { color = mModFogColor; }
      //DEPRECATED
      void GetModFogColor( sgVec3 color ) const 
      {
         DEPRECATE("void GetModFogColor( sgVec3 color )", "void GetModFogColor( osg::Vec3& color )")
         osg::Vec3 tmp;
         GetModFogColor(tmp);
         color[0] = tmp[0]; color[1] = tmp[1]; color[2] = tmp[2];
      }

      ///Set the fog mode
      void SetFogMode( FogMode mode );
      FogMode GetFogMode() const { return mFogMode; }

      ///Supply the advanced fog control values
      void SetAdvFogCtrl( const osg::Vec3& src ) { mAdvFogCtrl = src; }
      void GetAdvFogCtrl( osg::Vec3& dst ) const{ dst = mAdvFogCtrl; }

      //DEPRECATED
      void SetAdvFogCtrl( sgVec3 src ) 
      {
        DEPRECATE("void SetAdvFogCtrl( sgVec3 src )", "void SetAdvFogCtrl( const osg::Vec3& src )")
        SetAdvFogCtrl(osg::Vec3(src[0], src[1], src[2]) );

      }
      //DEPRECATED
      void GetAdvFogCtrl( sgVec3 dst )
      {
         DEPRECATE("void GetAdvFogCtrl( sgVec3 dst )", "void GetAdvFogCtrl( osg::Vec3 dst ) const")
         osg::Vec3 tmp;
         GetAdvFogCtrl(tmp);
         dst[0] = tmp[0]; dst[1] = tmp[1]; dst[2] = tmp[2];
      }

      ///Set the fog near value (only used for FogMode::LINEAR
      void SetFogNear( float val );
      float GetFogNear() const {return mFogNear;}

      //Turn the fog on or off
      void SetFogEnable( bool enable );
      bool GetFogEnable() const {return mFogEnabled;}

      ///Set the visibility distance in meters
      void SetVisibility( float distance );
      float GetVisibility() const {return mVisibility;}

      ///Get the current color of the sun
      void GetSunColor( osg::Vec3& color ) {color = mSunColor;}
      //DEPRECATED
      void GetSunColor( sgVec3 color )
      {
         DEPRECATE("void GetSunColor( sgVec3 color )", "void GetSunColor( osg::Vec3& color )")
         osg::Vec3 tmp;
         GetSunColor(tmp);
         color[0] = tmp[0]; color[1] = tmp[1]; color[2] = tmp[2];
      }

      ///Get the sun's azimuth and elevation (degrees)
      void GetSunAzEl( float *az, float *el ) {*az=mSunAzimuth; *el=mSunAltitude;}

      void Repaint();

      ///Set the environment's date and time
      void SetDateTime( int yr, int mo, int da,
                        int hr, int mi, int sc);

      ///Get the current date/time of the environment
      void GetDateTime( int *yr, int *mo, int *da, int *hr, int *mi, int *sc );

      ///Set the ephemeris reference lat/long
      void SetRefLatLong( const osg::Vec2& latLong );
      void GetRefLatLong( osg::Vec2& latLong )const{latLong = mRefLatLong;}
      
      //DEPRECATED
      void SetRefLatLong( sgVec2 latLong )
      {
         DEPRECATE("void SetRefLatLong( sgVec2 latLong )", "void SetRefLatLong( const osg::Vec2& latLong )")
         SetRefLatLong(osg::Vec2(latLong[0], latLong[1]));
      }
      //DEPRECATED
      void GetRefLatLong( sgVec2 latLong ) 
      {
         DEPRECATE("void GetRefLatLong( sgVec2 latLong ) ", "void SetRefLatLong( sgVec2 latLong )")
         osg::Vec2 tmp;
         GetRefLatLong(tmp);
         latLong[0] = tmp[0]; latLong[1] = tmp[1];
      }

   private:
      class InterpTable
      {
         struct TableEntry
         {
            TableEntry():
         ind(0.0), dep(0.0){}

         TableEntry(double independent, double dependent):
         ind(independent), dep(dependent) {}

         double ind;
         double dep;
         };

         int mSize;
         std::vector<TableEntry> mTable;

      public:
         InterpTable();
         ~InterpTable();
         void AddEntry(double ind, double dep);
         double Interpolate(double x) const;
      };

      InterpTable *mAmbLightTable;
      InterpTable *mDifLightTable;
      InterpTable *mSpecLightTable;
      InterpTable *mSkyLightTable;

      sgVec3 mAmbLightColor; ///<The current ambient light color
      sgVec3 mDifLightColor; ///<The current diffuse light color
      sgVec3 mSpecLightColor; ///<The current specular light color

      typedef std::vector< RefPtr<EnvEffect> > EnvEffectList;
      
      EnvEffectList mEffectList; ///<The list of environment effects
      EnvEffectList mToBeRemoved;///<temp list of effects to remove
      RefPtr<osg::Group> mEnvEffectNode; ///<Contains the env effects
      RefPtr<osg::Group> mDrawableNode; ///<Contains the actual model
      virtual void OnMessage(MessageData *data);
      void Update(const double deltaFrameTime);
      void RemoveEffectCache(void);///<actually remove EnvEffects from the Env

      RefPtr<Light> mSkyLight; ///< The sky light
      
      osg::Vec3 mSkyColor; ///< The user-set base sky color
      osg::Vec3 mModSkyColor; ///<The time-based modified color
      osg::Vec3 mFogColor; ///<the user-set base fog color
      osg::Vec3 mAdvFogCtrl; ///<values for the advanced fog (Turbidity, Energy, Visibility)
      osg::Vec3 mModFogColor; ///<time-based fog modified color
      RefPtr<osg::Fog> mFog; ///< The fog adjuster
      float mVisibility; ///<The user-set visibility distance (m)
      bool mFogEnabled; ///< Is the fog enabled?
      FogMode mFogMode; ///< Linear, Exp, Exp2, Advanced
      float mFogNear; ///<The near point of LINEAR fog
      
      float mSunAltitude; ///< the current sun altitude (deg, 0=horizon)
      float mSunAzimuth; ///<the current sun azimuth (deg, 0=north)
      osg::Vec3 mSunColor; ///<the current color of the sun
      osg::Vec2 mRefLatLong;  ///<the ephemeris reference lat/long (deg)
      time_t mCurrTime;   ///< The current time/day of the sim

      double mLastUpdate;
      SunlightShader *mSunlightShader; ///<pixel shader for light scattering
      SkyDomeShader *mSkyDomeShader; ///<pixel shader for the skydome
      RefPtr<SkyDome> mSkyDome; ///<the added SkyDome (couuld be NULL)

      void UpdateSkyLight(void);
      void UpdateFogColor(void);
      void UpdateSunColor(void);
      void UpdateEnvColors(void);
      void UpdateShaders(void);
};
}

#endif // DELTA_ENVIRONMENT
