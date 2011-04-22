#ifndef DELTA_ENVIRONMENT
#define DELTA_ENVIRONMENT

#include "deltadrawable.h"
#include "enveffect.h"
#include "osg/Group"
#include "osg/Fog"
#include "sunlightshader.h"
#include "skydomeshader.h"
#include "skydome.h"

namespace dtCore
{
   ///A unique environment which controls lighting and visibility
   class DT_EXPORT Environment :  public dtCore::DeltaDrawable, public dtCore::Base
   {
   public:
            DECLARE_MANAGEMENT_LAYER(Environment)

      Environment(const std::string name="Environment");
      virtual ~Environment(void);

      enum FogMode {
         LINEAR = 0, ///<Linear fog
         EXP,        ///<Exponential fog
         EXP2,       ///<Exponential squared fog
         ADV         ///<Advanced light scattering "fog"
      };

      ///Get the scene graph node that contains all things to be rendered
      virtual osg::Node* GetOSGNode() {return mNode.get();}

      ///Notifies this object that it has been added to a Scene
      virtual void AddedToScene(Scene* scene);
         
      ///Add a DeltaDrawable to be rendered using this Environment's properties.
      void AddDrawable( DeltaDrawable *drawable );

      /// Add an Environmental Effect to the Environment
      void AddEffect(EnvEffect *effect);

      /// Remove an EnvEffect from this Environment
      void RemEffect(EnvEffect *effect);

      ///Get an Environmental Effect by its index
      EnvEffect* GetEffect(const int idx) {return mEffectList[idx].get();}

      ///Get the number of Environmental Effects in this Environment
      int GetNumEffects(void) {return mEffectList.size();}

      ///Set the base color of the sky
      void SetSkyColor(sgVec3 color);
      void GetSkyColor(sgVec3 color) const {sgCopyVec3(color, mSkyColor);}
      
      ///Set the base color of the fog
      void SetFogColor(sgVec3 color);
      void GetFogColor(sgVec3 color) const {sgCopyVec3(color, mFogColor);}

      ///Set the fog mode
      void SetFogMode(FogMode mode);
      FogMode GetFogMode() const {return mFogMode;}

      ///Supply the advanced fog control values
      void SetAdvFogCtrl(sgVec3 src) {sgCopyVec3(mAdvFogCtrl, src);}
      void GetAdvFogCtrl(sgVec3 dst) {sgCopyVec3(dst, mAdvFogCtrl);}

      ///Set the fog near value (only used for FogMode::LINEAR
      void SetFogNear(const float val);
      float GetFogNear(void)const {return mFogNear;}

      //Turn the fog on or off
      void SetFogEnable(bool enable);
      bool GetFogEnable(void) const {return mFogEnabled;}

      ///Set the visibility distance in meters
      void SetVisibility(float distance);
      float GetVisibility(void)const {return mVisibility;}

      ///Get the current color of the sun
      void GetSunColor(sgVec3 color) {sgCopyVec3(color, mSunColor);}

      ///Get the sun's azimuth and elevation (degrees)
      void GetSunAzEl( float *az, float *el ) {*az=mSunAzimuth; *el=mSunAltitude;}

      void Repaint(void);

      ///Set the environment's date and time
      void SetDateTime( const int yr, const int mo, const int da,
                        const int hr, const int mi, const int sc);

      ///Get the current date/time of the environment
      void GetDateTime( int *yr, int *mo, int *da, int *hr, int *mi, int *sc );

      ///Set the ephemeris reference lat/long
      void SetRefLatLong(sgVec2 latLong);
      void GetRefLatLong(sgVec2 latLong) { sgCopyVec2(latLong, mRefLatLong);}

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

      typedef std::vector<osg::ref_ptr<EnvEffect> > EnvEffectList;

      EnvEffectList mEffectList; ///<The list of environment effects
      EnvEffectList mToBeRemoved;///<temp list of effects to remove
      osg::ref_ptr<osg::Group> mNode; ///<Contains the actual model
      osg::ref_ptr<osg::Group> mEnvEffectNode; ///<Contains the env effects
      osg::ref_ptr<osg::Group> mDrawableNode; ///<Contains the actual model
      virtual void OnMessage(MessageData *data);
      void Update(const double deltaFrameTime);
      void RemoveEffectCache(void);///<actually remove EnvEffects from the Env

      osg::ref_ptr<osg::Light> mSkyLight; ///< The sky light
      
      sgVec3 mSkyColor; ///< The user-set base sky color
      sgVec3 mModSkyColor; ///<The time-based modified color
      sgVec3 mFogColor; ///<the user-set base fog color
      sgVec3 mAdvFogCtrl; ///<values for the advanced fog (Turbidity, Energy, Visibility)
      sgVec3 mModFogColor; ///<time-based fog modified color
      osg::ref_ptr<osg::Fog> mFog; ///< The fog adjuster
      float mVisibility; ///<The user-set visibility distance (m)
      bool mFogEnabled; ///< Is the fog enabled?
      FogMode mFogMode; ///< Linear, Exp, Exp2, Advanced
      float mFogNear; ///<The near point of LINEAR fog
      
      float mSunAltitude; ///< the current sun altitude (deg, 0=horizon)
      float mSunAzimuth; ///<the current sun azimuth (deg, 0=north)
      sgVec3 mSunColor; ///<the current color of the sun
      sgVec2 mRefLatLong;  ///<the ephemeris reference lat/long (deg)
      time_t mCurrTime;   ///< The current time/day of the sim

      double mLastUpdate;
      SunlightShader *mSunlightShader; ///<pixel shader for light scattering
      SkyDomeShader *mSkyDomeShader; ///<pixel shader for the skydome
      osg::ref_ptr<SkyDome> mSkyDome; ///<the added SkyDome (couuld be NULL)

      void UpdateSkyLight(void);
      void UpdateFogColor(void);
      void UpdateSunColor(void);
      void UpdateEnvColors(void);
      void UpdateShaders(void);
};
}

#endif // DELTA_ENVIRONMENT