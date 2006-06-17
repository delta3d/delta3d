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

#ifndef DELTA_ENVIRONMENT
#define DELTA_ENVIRONMENT

#include <dtCore/deltadrawable.h>
#include <dtUtil/deprecationmgr.h>

#include <osg/Vec2>
#include <osg/Vec3>

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace osg
{
   class Fog;
   class Group;
}
/// @endcond

namespace dtCore
{
   class EnvEffect;
   class Light;
   class Scene;
   class SkyDome;
   class SkyDomeShader;
   class SunlightShader;

   ///A unique environment which controls lighting and visibility
   class DT_CORE_EXPORT Environment : public dtCore::DeltaDrawable
   {
     DECLARE_MANAGEMENT_LAYER(Environment)

   public:
      Environment( const std::string& name = "Environment" );
   protected:
      virtual ~Environment();

   public:

      enum FogMode
      {
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
           
      ///Set the base color of the fog
      void SetFogColor( const osg::Vec3& color );
      void GetFogColor( osg::Vec3& color ) const { color = mFogColor; }      

	   ///Get the modified color of the fog
      void GetModFogColor( osg::Vec3& color ) const { color = mModFogColor; }
     
      ///Set the fog mode
      void SetFogMode( FogMode mode );
      FogMode GetFogMode() const { return mFogMode; }

      ///Supply the advanced fog control values
      void SetAdvFogCtrl( const osg::Vec3& src ) { mAdvFogCtrl = src; }
      void GetAdvFogCtrl( osg::Vec3& dst ) const{ dst = mAdvFogCtrl; }

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
      
      ///Get the sun's azimuth and elevation (degrees)
      void GetSunAzEl( float *az, float *el ) const
      { 
         DEPRECATE(  "void GetSunAzEl( float *az, float *el )", 
                     "void GetSunAzEl( float& az, float& el )" );
         GetSunAzEl( *az, *el );
      }

      void GetSunAzEl( float& az, float& el ) const
      { 
         az = mSunAzimuth; 
         el = mSunAltitude;
      }

      void Repaint();

      ///Set the environment's date and time
      void SetDateTime( int yr, int mo, int da,
                        int hr, int mi, int sc);

      ///Get the current date/time of the environment
      void GetDateTime( int *yr, int *mo, int *da, int *hr, int *mi, int *sc ) const
      {
         DEPRECATE(  "void GetDateTime( int *yr, int *mo, int *da, int *hr, int *mi, int *sc )",
                     "void GetDateTime( int& yr, int& mo, int& da, int& hr, int& mi, int& sc )" );
         GetDateTime( *yr, *mo, *da, *hr, *mi, *sc );
      }

      void GetDateTime( int& yr, int& mo, int& da, int& hr, int& mi, int& sc ) const;

      ///Set the ephemeris reference lat/long
      void SetRefLatLong( const osg::Vec2& latLong );
      void GetRefLatLong( osg::Vec2& latLong ) const { latLong = mRefLatLong; }
      
      ///required by DeltaDrawable
      osg::Node* GetOSGNode(){return mNode.get();}
      const osg::Node* GetOSGNode() const{return mNode.get();}

      void SetOSGNode(osg::Node* pNode){mNode = pNode;}

   private:

      class InterpTable
      {
         struct TableEntry
         {
            TableEntry() :
               ind(0.0), 
               dep(0.0)
            {
            }

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

      osg::Vec3 mAmbLightColor; ///<The current ambient light color
      osg::Vec3 mDifLightColor; ///<The current diffuse light color
      osg::Vec3 mSpecLightColor; ///<The current specular light color

      typedef std::vector< RefPtr<EnvEffect> > EnvEffectList;
      
      EnvEffectList mEffectList; ///<The list of environment effects
      EnvEffectList mToBeRemoved;///<temp list of effects to remove
      RefPtr<osg::Group> mEnvEffectNode; ///<Contains the env effects
      RefPtr<osg::Group> mDrawableNode; ///<Contains the actual model
      virtual void OnMessage(MessageData *data);
      void Update(const double deltaFrameTime);
      void RemoveEffectCache();///<actually remove EnvEffects from the Env

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
      RefPtr<SkyDome> mSkyDome; ///<the added SkyDome (couuld be 0)

      RefPtr<osg::Node> mNode;

      void UpdateSkyLight();
      void UpdateFogColor();
      void UpdateSunColor();
      void UpdateEnvColors();
      void UpdateShaders();
};
}

#endif // DELTA_ENVIRONMENT
