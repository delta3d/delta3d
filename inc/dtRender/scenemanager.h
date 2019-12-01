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
#ifndef DELTA_SCENEMANAGER_H
#define DELTA_SCENEMANAGER_H

#include <dtRender/dtrenderexport.h>
#include <dtRender/scenebase.h>
#include <dtRender/graphicsquality.h>
#include <dtRender/scenetype.h>
#include <dtRender/sceneenum.h>

#include <dtUtil/enumeration.h>

#include <dtCore/observerptr.h>
#include <dtCore/refptr.h>

#include <dtGame/environmentactor.h>

#include <osg/Group>

namespace dtGame
{
   class GameManager;
}

namespace dtCore 
{
   class Camera;
}

namespace dtRender
{
   class SceneManagerImpl;
   class SceneGroup;

   class DT_RENDER_EXPORT SceneManager : public dtGame::IEnvGameActor
   {
   public:
      typedef dtGame::IEnvGameActor BaseClass;

      static const dtUtil::RefString UNIFORM_MAIN_CAMERA_POS;
      static const dtUtil::RefString UNIFORM_MAIN_CAMERA_HPR;
      static const dtUtil::RefString UNIFORM_MAIN_CAMERA_INVERSE_VIEW;
      static const dtUtil::RefString UNIFORM_MAIN_CAMERA_INVERSE_MODELVIEWPROJECTION;

      static const dtUtil::RefString UNIFORM_SCREEN_WIDTH;
      static const dtUtil::RefString UNIFORM_SCREEN_HEIGHT;

      static const dtUtil::RefString UNIFORM_NEAR_PLANE;
      static const dtUtil::RefString UNIFORM_FAR_PLANE;

      static const dtUtil::RefString UNIFORM_FRAME_TIME;
      static const dtUtil::RefString UNIFORM_ELAPSED_TIME;

      //static const dtUtil::RefString UNIFORM_GAMMA;
      //static const dtUtil::RefString UNIFORM_BRIGHTNESS;
      static const dtUtil::RefString UNIFORM_EXPOSURE;

      static const dtUtil::RefString UNIFORM_SCENE_LUMINANCE;
      static const dtUtil::RefString UNIFORM_SCENE_AMBIENCE;


   public:
      SceneManager(dtGame::GameActorProxy& parent);
      virtual ~SceneManager();

      SceneGroup* GetSceneGroup(SceneEnum&);
      const SceneGroup* GetSceneGroup(SceneEnum&) const;

      /**
       * Searches the scene for the first found scene of said type
       * @return NULL if the scene type cannot be found.
       */
      SceneBase* FindSceneByType(SceneType&);
      const SceneBase* FindSceneByType(SceneType&) const;

      /***
       * Fills a vector with all scenes of the specified type.
       */
      void GetAllScenesByType(SceneType&, std::vector<SceneBase*>&);


      /**
       * Searches for the scene containing the specified actor
       * @return NULL if the actor cannot be found.
       */
      SceneBase* FindSceneForDrawable(DeltaDrawable&);
      const SceneBase* FindSceneForDrawable(DeltaDrawable&) const;

      /**
       *  If the scene stack is not empty, new drawables will be added
       *     to the top of that being the result of GetCurrentScene().
       *     The scene should already be added before pushing it on the stack.
       **/
      void PushScene(SceneBase&);
      void PopScene();

      /***
       *  @return the top of the scene stack
       */
      SceneBase* GetCurrentScene();

      const GraphicsQuality& GetGraphicsQuality() const;
      void SetGraphicsQuality(GraphicsQuality&);

      /***
       * Use this setting to disable multipass effects
       */
      void SetEnableMultipass(bool);
      bool GetEnableMultipass() const;

      /***
       *  Setting this disables color clamping on the entire scene
       *     as well as scales the lighting values.
       */
      void SetEnableHDR(bool);
      bool GetEnableHDR() const;

      /***
       *  Exposes the main scene camera.
       *
       */
      void SetSceneCamera(dtCore::Camera*);
      dtCore::Camera* GetSceneCamera();
      const dtCore::Camera* GetSceneCamera() const;

      /***
       *  These are uniforms which effect the shading
       */

      float GetExposure() const;
      void SetExposure(float);

      float GetLuminance() const;
      void SetLuminance(float);

      float GetAmbience() const;
      void SetAmbience(float);

      //dtCore IEnvironment Interface start
      virtual void AddActor(dtCore::DeltaDrawable& dd);

      virtual void RemoveActor(dtCore::DeltaDrawable& dd);

      virtual void RemoveAllActors();

      virtual bool ContainsActor(dtCore::DeltaDrawable& dd) const;

      virtual void GetAllActors(std::vector<dtCore::DeltaDrawable*>& vec);

      virtual unsigned int GetNumEnvironmentChildren() const;
      //dtCore IEnvironment Interface end

      void CreateScene();

      dtGame::GameManager* GetGameManager();
      const dtGame::GameManager* GetGameManager() const;

      void CreateDefaultScene();
      void CreateDefaultMultipassScene();

      virtual void PostComponentInit();

      /*virtual*/ bool IsPlaceable() const;

      /***
      *  Checks to see if a resize is needed, calls the OnResize()
      *     once the resizing has appeared to stop.
      */
      void Resize(int width, int height);
      virtual void OnResize(int width, int height);

   private:
      virtual void AddScene(SceneBase&);
      void InitUniforms();
      void UpdateUniforms(dtCore::Camera& pCamera);

      void SetNodeMask(const SceneEnum& se, osg::Node& n);

      SceneManagerImpl* mImpl;

   };


   class DT_RENDER_EXPORT SceneManagerActor : public dtGame::IEnvGameActorProxy
   {
   public:
      typedef dtGame::IEnvGameActorProxy BaseClass;
      SceneManagerActor();

      virtual void BuildPropertyMap();
      virtual void BuildActorComponents();
      virtual void CreateDrawable();

   protected:
      virtual ~SceneManagerActor();
   };
}

#endif // DELTA_SCENEMANAGER_H
