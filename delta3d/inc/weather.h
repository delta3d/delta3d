#pragma once
#include "base.h"
#include "Environment.h"
#include "cloudplane.h"

namespace dtABC
{

   ///High level controls for representing weather 

   /** The Weather class is a high-level control for weather management.  It
     * abstracts complicated weather effects into simple controls.  There are
     * two levels of settings: basic weather types and weather themes.  The 
     * basic weather types allow you to quickly setup weather by setting the
     * cloud coverage, the visibility, and the wind strength.  A representable
     * weather pattern will be displayed.
     *
     * The weather themes are pre-built weather effects that have built-in 
     * parameters which encompass a particular weather pattern.  The weather
     * themes override any previously set basic weather types.
     *
     * To use Weather, instantiate the class and supply either the basic types or
     * one of the themes.
     *
     * Then add the Weather's Environment to your Scene using 
     * Weather::GetEnvironment().
     * 
     * Make sure to add your Drawables using Weather::AddDrawable().  This will
     * ensure that the Drawables get affected by the fog and lighting.
     */
   class DT_EXPORT Weather : dtCore::Base
   {
   public:
      DECLARE_MANAGEMENT_LAYER(Weather)

      Weather(void);
      virtual ~Weather(void);

      enum CloudType{
         CLOUD_CLEAR,
         CLOUD_FEW,
         CLOUD_SCATTERED,
         CLOUD_BROKEN,
         CLOUD_OVERCAST
      };

      enum WindType{
         WIND_NONE,
         WIND_BREEZE,
         WIND_LIGHT,
         WIND_MODERATE,
         WIND_HEAVY,
         WIND_SEVERE
      };

      enum VisibilityType{
         VIS_UNLIMITED, ///<no restrictions
         VIS_FAR,       ///<50km
         VIS_MODERATE,  ///<25km
         VIS_LIMITED,   ///<8km
         VIS_CLOSE      ///<1.5km
      };

      enum WeatherTheme{
         THEME_CLEAR,///<no clouds, good visibility, no wind
         THEME_FAIR,///<light clouds, avg. vis, light winds
         THEME_FOGGY,///<low visibility, no wind
         THEME_RAINY///<overcast clouds, limited vis, mod winds
      };

      ///Creates a set of clouds to represent the generic cloud description
      void SetBasicCloudType( const CloudType type );
      CloudType GetBasicCloudType(void) const {return mCloudType;}

      ///Creates wind layers to represent the wind description
      void SetBasicWindType(const WindType windType);
      WindType GetBasicWindType(void)const {return mWindType;}

      ///Convenience function for the Environment
      void SetBasicVisibilityType( const VisibilityType visType);
      VisibilityType GetBasicVisibilityType(void)const {return mVisType;}

      ///Preset weather themes which control clouds, winds, and visibility
      void SetTheme(const WeatherTheme theme);
      WeatherTheme GetTheme(void) const {return mTheme;}

      ///Get a handle to the Weather's Environment instance
      dtCore::Environment* GetEnvironment(void)const {return mEnvironment;}

      ///Set the weather's rate of change (-1.0 to 1.0)
      void SetRateOfChange(const float rate);
      float GetRateOfChange(void) const {return mRateOfChange;}

      ///Add a Drawable to be affected by this weather's lighting and fog
      void AddDrawable( dtCore::Drawable *drawable);

   private:
      typedef std::vector<osg::ref_ptr<dtCore::CloudPlane> > CloudPlaneList;
      CloudPlaneList mClouds;

      dtCore::Environment *mEnvironment;
      CloudType mCloudType;
      VisibilityType mVisType;
      WindType mWindType;
      float mRateOfChange; ///<weather change rate (-1 to 1)
      WeatherTheme mTheme;
   };

}