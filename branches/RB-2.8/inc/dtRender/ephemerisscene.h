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
#include <osg/Vec4>

#include <string>

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

      virtual void SetLatitudeLongitude(float latitude, float longitude);

      bool SetDateTimeAsString(const std::string& timeAndDate);
      
      void SetDateTime(dtUtil::DateTime&);
      dtUtil::DateTime GetDateTime() const;

      void SetTimeFromSystem();

      void SetFogDensity(float density);
      float GetFogDensity();

      bool GetFogEnable() const;
      void SetFogEnable(bool enable);

      void SetFogColor(const osg::Vec4& color);
      const osg::Vec4& GetFogColor() const;

      void SetFogMode(FogMode mode);

      void SetFogNear( float val );

      void SetVisibility( float distance );
      float GetVisibility ();

   protected:
      virtual void OnTimeChanged();

   private:
      bool SetTimeAndDate(std::istringstream& iss);

      EphemerisImpl* mImpl;
   };

   class DT_RENDER_EXPORT EphemerisSceneProxy : public dtCore::BaseActorObject
   {
   public:
      typedef dtCore::BaseActorObject BaseClass;
      EphemerisSceneProxy();

      virtual void BuildPropertyMap();
      virtual void CreateDrawable();

      virtual bool IsPlaceable() const;

   protected:
      virtual ~EphemerisSceneProxy();
   };
}

#endif // DELTA_EPHEMERIS_SCENE_H
