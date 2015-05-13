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
   class UnitCamera;
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

      static const int TEXTURE_UNIT_PREDEPTH;
      static const int TEXTURE_UNIT_REFLECTION;

      static const std::string REFLECTION_TEXTURE_UNIFORM;

      static const std::string UNIFORM_DEPTH_ONLY_PASS;
      static const std::string UNIFORM_PREDEPTH_TEXTURE;

      class DT_RENDER_EXPORT ResizeCallback : public osg::Referenced
      {
      public:
         ResizeCallback() {};

         virtual void OnResize(MultipassScene&, int width, int height) = 0;

      protected:
         virtual ~ResizeCallback() {};
      };

   public:
      MultipassScene();
      MultipassScene(const SceneType& sceneId, const SceneEnum& defaultScene);

      virtual ~MultipassScene();

      virtual void CreateScene(SceneManager&, const GraphicsQuality&);
      
      osgPPU::Processor* GetPPUProcessor();
      const osgPPU::Processor* GetPPUProcessor() const;
      
      bool GetUsingDefaultUnitOut();
      bool DetachDefaultUnitOut();

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

      void SetEnablePreDepthPass(bool);
      bool GetEnablePreDepthPass() const;

      int GetPreDepthBufferImageFormat() const;
      void SetPreDepthBufferImageFormat(int);

      int GetDepthBufferImageFormat() const;
      void SetDepthBufferImageFormat(int);

      osgPPU::UnitDepthbufferBypass* GetPreDepthBufferBypass();
      const osgPPU::UnitDepthbufferBypass* GetPreDepthBufferBypass() const;

      osgPPU::UnitDepthbufferBypass* GetDepthBypass();
      const osgPPU::UnitDepthbufferBypass* GetDepthBypass() const;

      osg::Camera* GetCamera();
      const osg::Camera* GetCamera() const;

      osg::Camera* GetPreDepthCamera();
      const osg::Camera* GetPreDepthCamera() const;

      osgPPU::UnitCamera* GetMultipassPPUCamera();
      const osgPPU::UnitCamera* GetMultipassPPUCamera() const;

      osg::Texture* GetPreDepthTexture();
      const osg::Texture* GetPreDepthTexture() const;


      void SetCamera(osg::Camera&);

      /***
      *  Override AddScene to allow multipass effects combine
      */
      virtual bool AddScene(SceneBase&);

      osgPPU::Unit* GetFirstUnit();
      const osgPPU::Unit* GetFirstUnit() const;

      osgPPU::Unit* GetLastUnit();
      const osgPPU::Unit* GetLastUnit() const;

      virtual void Resize(osg::Camera*);

      /***
      * On resize the multipass and render buffers are recreated
      *     use a callback to update the references to your textures.
      */
      void AddResizeCallback(ResizeCallback& cb);
      void RemoveResizeCallback(ResizeCallback& cb);

      /***
      * Removes buffer attachments and creates FBO attachments for color and / or depth
      */
      void SetupMultipassCamera(osg::Camera& camera, osg::Viewport& vp, bool use_color, bool use_depth, dtCore::RefPtr<osg::Texture2D>& colorTexture, dtCore::RefPtr<osg::Texture2D>& depthTexture);

      /***
      * Used by SetupMultipassCamera(), useful for creating osg texture objects
      */
      osg::Texture2D* CreateRenderTexture(int tex_width, int tex_height, bool depth, bool nearest, int imageFormat);

   protected:
      void SetFirstUnit(osgPPU::Unit&);
      void SetLastUnit(osgPPU::Unit&);

      void DetachRenderer(osg::Camera& camera);

   private:
      MultipassSceneImpl* mImpl;
   };


   class DT_RENDER_EXPORT MultipassSceneActor : public dtCore::BaseActorObject
   {
   public:
      typedef dtCore::BaseActorObject BaseClass;
      MultipassSceneActor();

      virtual void BuildPropertyMap();
      virtual void CreateDrawable();

      virtual bool IsPlaceable() const;

   protected:
      virtual ~MultipassSceneActor();
   };
}

#endif // DELTA_MULTIPASSSCENE_H
