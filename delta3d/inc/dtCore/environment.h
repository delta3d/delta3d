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
#include <dtUtil/datetime.h>
#include <osg/Vec2>
#include <osg/Vec3>
#include <time.h>

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

      Environment(const std::string& name = "Environment");
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
      virtual void AddedToScene(Scene* scene);

      ///Add a DeltaDrawable to be rendered using this Environment's properties.
      virtual bool AddChild(DeltaDrawable* child);

      ///Remove a DeltaDrawable added to the Environment.
      void RemoveChild(DeltaDrawable* child);

      /// Add an Environmental Effect to the Environment
      void AddEffect(EnvEffect* effect);

      /// Remove an EnvEffect from this Environment
      void RemEffect(EnvEffect* effect);

      ///Get an Environmental Effect by its index
      EnvEffect* GetEffect(int idx) const;

      ///Get the number of Environmental Effects in this Environment
      int GetNumEffects() const;

      ///Set the base color of the sky
      void SetSkyColor(const osg::Vec3& color);
      void GetSkyColor(osg::Vec3& color) const;

      /** Set the base color of the fog.  This color is then adjusted internally
      * using the time of day.  NOTE: This value is not used for the fog
      * when the FogMode is ADV, but it still can be used by the EnvEffects.
      */
      void SetFogColor(const osg::Vec3& color);
      void GetFogColor(osg::Vec3& color) const;

      ///Get the modified color of the fog
      void GetModFogColor(osg::Vec3& color) const;

      /** Set the fog mode.  Any Drawables added to the Environment will be fogged
      *   based on the FogMode.  The ADV FogMode uses a pixel shader to calculate
      *   a somewhat physically correct fog/haze.  LINEAR, EXP, and EXP2 use standard
      *   OpenGL Fog.
      */
      void SetFogMode(FogMode mode);
      FogMode GetFogMode() const;

      ///Supply the advanced fog control values
      void SetAdvFogCtrl(const osg::Vec3& src);
      void GetAdvFogCtrl(osg::Vec3& dst) const;

      ///Set the fog near value (only used for FogMode::LINEAR
      void SetFogNear(float val);
      float GetFogNear() const;

      //Turn the fog on or off
      void SetFogEnable(bool enable);
      bool GetFogEnable() const;

      //Set fog Density
      void SetFogDensity(float density);
      float GetFogDensity() const;

      ///Set the visibility distance in meters
      void SetVisibility(float distance);
      float GetVisibility() const;

      ///Get the current color of the sun
      void GetSunColor(osg::Vec3& color) const;

      void GetSunAzEl(float& az, float& el) const;

      osg::Vec2 GetSunAzEl() const;

      void Repaint();

      //use this only to force an update
      //otherwise it will update every second
      void Update(const double deltaFrameTime);

      /** Set the starting date and time.  Any value of -1 resets the date/time
      *  to be the system time.
      * @param yr Year (1900-xxxx)
      * @param mo Month of the year (1-12)
      * @param da Day of the Month (1-31)
      * @param hr Hour since midnight (0-23)
      * @param mi Minutes after the hour (0-59)
      * @param sc Seconds pass the minute (0-59)
      */
      void SetDateTime(unsigned yr, unsigned mo, unsigned da,
                       unsigned hr, unsigned mi, unsigned sc);

      void SetDateTime(const dtUtil::DateTime& dateTime);

      void GetDateTime(unsigned& yr, unsigned& mo, unsigned& da, unsigned& hr, unsigned& mi, unsigned& sc) const;

      const dtUtil::DateTime& GetDateTime() const;
      dtUtil::DateTime& GetDateTime();

      /**
        * Set whether we use the System's simulation time for the environment
        * or a static value.
        * @param useSimTime If true, the Environment's time will be kept in sync with the
        * dtCore::System Simulation time. False will keep a static time.
        * @see dtCore::System::GetSimulationClockTime()
        * @see SetDateTime()
        */
      void SetUseSimTime(bool useSimTime);

      ///True if we use sim time to update the environment or not
      bool GetUseSimTime() const;

      ///Set the ephemeris reference lat/long
      void SetRefLatLong(const osg::Vec2& latLong);
      void GetRefLatLong(osg::Vec2& latLong) const;

      ///Set the wind's minimum and maximum speed in m/s
      void SetWindSpeedMinMaxValues(float minSpeed, float maxSpeed);

      void SetWindSpeed(float speed);
      float GetWindSpeed() const;

      void SetWindDirection(const osg::Vec3& direction);
      osg::Vec3 GetWindDirection() const;

      ///required by DeltaDrawable
      osg::Node* GetOSGNode();
      const osg::Node* GetOSGNode() const;

      void SetOSGNode(osg::Node* pNode);

      /** Replace the internal Group node that contains all the added Drawable
        * children with the supplied.  This will add all previously added children
        * to the new node.
        * @param pNode The new Group node to use as the parent of all added Drawables.
        * @see AddChid()
        */
      void SetDrawableNode(osg::Group* pNode);

      /** Get a pointer to the internally managed Group node that is the parent
        * of all added Drawables.
        * @return The Group node parent of all added Drawables
        * @see AddChild()
        */
      osg::Group* GetDrawableNode() const;

      /** Get a pointer to the Light the Environment is controlling, based on the
        * ephemeris calculations.  Typically this is the default Light created
        * by the Scene.
        * @return The Light the Environment is controlling.  Could be NULL.
        */
      Light* GetSkyLight() const;

      void SetSkyLight(Light* newSkyLight);

      //Signal fired when the wind changes
      sigslot::signal2<const osg::Vec3&, float> WindChangedSignal;

   private:

      class InterpTable
      {
         struct TableEntry
         {
            TableEntry()
               : ind(0.0)
               , dep(0.0)
            {
            }

            TableEntry(double independent, double dependent)
               : ind(independent)
               , dep(dependent)
            {
            }

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

      InterpTable* mAmbLightTable;
      InterpTable* mDifLightTable;
      InterpTable* mSpecLightTable;
      InterpTable* mSkyLightTable;

      osg::Vec3 mAmbLightColor; ///<The current ambient light color
      osg::Vec3 mDifLightColor; ///<The current diffuse light color
      osg::Vec3 mSpecLightColor; ///<The current specular light color

      typedef std::vector< RefPtr<EnvEffect> > EnvEffectList;

      EnvEffectList mEffectList; ///<The list of environment effects
      EnvEffectList mToBeRemoved;///<temp list of effects to remove
      RefPtr<osg::Group> mEnvEffectNode; ///<Contains the env effects
      RefPtr<osg::Group> mDrawableNode; ///<Contains the actual model
      virtual void OnSystem(const dtUtil::RefString& str, double deltaSim, double deltaReal)
;
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

      double mSunAltitude; ///< the current sun altitude (deg, 0=horizon)
      double mSunAzimuth; ///<the current sun azimuth (deg, 0=north)
      osg::Vec3 mSunColor; ///<the current color of the sun
      osg::Vec2 mRefLatLong;  ///<the ephemeris reference lat/long (deg)
      dtUtil::DateTime mCurrTime;   ///< The current time/day of the sim

      double mLastUpdate;
      SunlightShader* mSunlightShader; ///<pixel shader for light scattering
      SkyDomeShader* mSkyDomeShader; ///<pixel shader for the skydome
      RefPtr<SkyDome> mSkyDome; ///<the added SkyDome (couuld be 0)

      float mWindMinSpeed; ///<the minimum speed of the wind in m/s
      float mWindMaxSpeed; ///<the maximum speed of the wind in m/s
      float mCurrentWindSpeed; ///<the current speed of the wind
      float mDesiredWindSpeed; ///<the wind speed we want the current speed to iterate towards
      osg::Vec3 mDesiredWindDirection; ///<the desired direction of the wind
      osg::Vec3 mCurrentWindDirection; ///<the current direction of the wind

      RefPtr<osg::Node> mNode;

      bool mUseSimTime; ///<Whether we should use sim time as our environment time or not

      /**
      * Recalculates wind speed based on current wind min and max
      */
      void RecalculateWindSpeed();

      /**
      * Updates the current wind speed to iterate towards the desired wind speed
      * and updates the current wind direction to iterate towards the desired wind
      * direction
      */
      void UpdateWind();

      /**
      * Updates the sky light based on the sun angle.
      */
      void UpdateSkyLight();

      /** Update the fog color based on the sun angle and the sun color.
      *
      *  TODO: Adjust based on the Camera's current heading
      */
      void UpdateFogColor();

      /// Update the color of the sun light based on it's angle.
      void UpdateSunColor();

      void UpdateEnvColors();
      void UpdateShaders();
};
}

#endif // DELTA_ENVIRONMENT
