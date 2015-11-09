/* -*-c++-*-
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2014, Caper Holdings, LLC
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
* Bradley Anderegg
*/
#ifndef DELTA_EPHEMERIS_SCENE_H
#define DELTA_EPHEMERIS_SCENE_H

#include <dtRender/scenebase.h>

#include <dtUtil/datetime.h>
#include <dtCore/baseactorobject.h>
#include <osg/Vec2>
#include <osg/Vec4>
#include <dtUtil/getsetmacros.h>
#include <string>

namespace osg
{
   class LightSource;
}

namespace dtRender
{
   class EphemerisImpl;

   class DT_RENDER_EXPORT EphemerisScene : public SceneBase
   {
   public:
      typedef SceneBase BaseClass;
      static const dtCore::RefPtr<SceneType> EPHEMERIS_SCENE;

      enum FogMode
      {
         LINEAR = 0, ///<Linear fog
         EXP,        ///<Exponential fog
         EXP2       ///<Exponential squared fog
      };

   public:
      EphemerisScene();
      virtual ~EphemerisScene();
      
      virtual void CreateScene(SceneManager&, const GraphicsQuality&);

      virtual osg::Group* GetSceneNode();
      virtual const osg::Group* GetSceneNode() const;

      osg::Vec3d GetSunPosition() const;
      osg::Vec3d GetMoonPosition() const;

      virtual void SetLatitudeLongitude(float latitude, float longitude);

      void SetDateTimeAsString(const std::string& timeAndDate);
      std::string GetDateTimeAsString() const;
      
      void SetDateTime(const dtUtil::DateTime&);
      dtUtil::DateTime GetDateTime() const;

      void SetTimeFromSystem();
      void SetTimeToLocalTime();

      void SetFogDensity(float density);
      float GetFogDensity() const;

      bool GetFogEnable() const;
      void SetFogEnable(bool enable);

      void SetFogColor(const osg::Vec4& color);
      const osg::Vec4& GetFogColor() const;

      void SetFogMode(FogMode mode);

      void SetFogNear( float val );

      void SetVisibility( float distance );
      float GetVisibility() const;

      osg::LightSource* GetLightSource();
      const osg::LightSource* GetLightSource() const;

      double ApproximateSkyBrightness(double sunAlt);

      DT_DECLARE_ACCESSOR_INLINE(bool, SetToLocalTime)
      DT_DECLARE_ACCESSOR_INLINE(bool, SetTimeFromSystem)
      DT_DECLARE_ACCESSOR_INLINE(bool, AutoComputeFogColor)
      DT_DECLARE_ACCESSOR_INLINE(osg::Vec2, LatLong)

      
   protected:
      virtual void OnTimeChanged();
      virtual void UpdateFogColor();
      
   private:
      bool SetTimeAndDate(std::istringstream& iss);
      void BindShader();

      EphemerisImpl* mImpl;
   };

   class DT_RENDER_EXPORT EphemerisSceneActor : public dtCore::BaseActorObject
   {
   public:
      typedef dtCore::BaseActorObject BaseClass;
      EphemerisSceneActor();

      virtual void BuildPropertyMap();
      virtual void CreateDrawable();

      virtual bool IsPlaceable() const;

   protected:
      virtual ~EphemerisSceneActor();
   };
}

#endif // DELTA_EPHEMERIS_SCENE_H
