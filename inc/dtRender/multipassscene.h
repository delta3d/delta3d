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
#ifndef DELTA_MULTIPASSSCENE_H
#define DELTA_MULTIPASSSCENE_H

#include <dtRender/scenegroup.h>

#include <dtCore/baseactorobject.h>

namespace dtCore
{
   class Camera;
}

namespace osg
{
   class Viewport;
   class Camera;
   class Texture2D;
}

namespace osgPPU
{
   class Processor;
   class Unit;
   class UnitOut;
   class UnitBypass;
   class UnitDepthbufferBypass;
   class UnitInResampleOut;
}

namespace dtRender
{
   class MultipassSceneImpl;

   class DT_RENDER_EXPORT MultipassScene : public SceneGroup
   {
   public:
      typedef SceneGroup BaseClass;
      static const dtCore::RefPtr<SceneType> MULTIPASS_SCENE;

   public:
      MultipassScene();
      MultipassScene(const SceneType& sceneId, const SceneEnum& defaultScene);

      virtual ~MultipassScene();

      virtual void CreateScene(SceneManager&, const GraphicsQuality&);
      
      osgPPU::Processor* GetPPUProcessor();
      const osgPPU::Processor* GetPPUProcessor() const;
      
      osgPPU::UnitOut* GetUnitOut();
      const osgPPU::UnitOut* GetUnitOut() const;
      
      void SetEnableColorBypass(bool);
      bool GetEnableColorBypass() const;

      int GetColorBufferImageFormat() const;
      void SetColorBufferImageFormat(int);

      osgPPU::UnitBypass* GetColorBypass();
      const osgPPU::UnitBypass* GetColorBypass() const;

      void SetEnableResampleColor(bool enable);
      bool GetEnableResampleColor() const;

      void SetColorResampleFactor(float factor/*= 0.5*/ );
      float GetColorResampleFactor() const;

      osgPPU::UnitInResampleOut* GetResampleColor();
      const osgPPU::UnitInResampleOut* GetResampleColor() const;

      void SetEnableDepthBypass(bool);
      bool GetEnableDepthBypass() const;

      int GetDepthBufferImageFormat() const;
      void SetDepthBufferImageFormat(int);

      osgPPU::UnitDepthbufferBypass* GetDepthBufferBypass();
      const osgPPU::UnitDepthbufferBypass* GetDepthBufferBypass() const;

      dtCore::Camera* GetCamera();
      const dtCore::Camera* GetCamera() const;

      void SetCamera(dtCore::Camera&);

      /***
      *  Override AddScene to allow multipass effects combine
      */
      virtual bool AddScene(SceneBase&);

      osgPPU::Unit* GetFirstUnit();
      const osgPPU::Unit* GetFirstUnit() const;

      osgPPU::Unit* GetLastUnit();
      const osgPPU::Unit* GetLastUnit() const;


   protected:
      void SetFirstUnit(osgPPU::Unit&);
      void SetLastUnit(osgPPU::Unit&);

      void SetupMultipassCamera(osg::Camera& camera, osg::Viewport& vp);
      osg::Texture2D* CreateRenderTexture(int tex_width, int tex_height, bool depth, bool nearest, int imageFormat);

   private:
      MultipassSceneImpl* mImpl;
   };


   class DT_RENDER_EXPORT MultipassSceneProxy : public dtCore::BaseActorObject
   {
   public:
      typedef dtCore::BaseActorObject BaseClass;
      MultipassSceneProxy();

      virtual void BuildPropertyMap();
      virtual void CreateDrawable();

      virtual bool IsPlaceable() const;

   protected:
      virtual ~MultipassSceneProxy();
   };
}

#endif // DELTA_MULTIPASSSCENE_H
