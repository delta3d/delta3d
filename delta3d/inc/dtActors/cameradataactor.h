/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2010, MOVES Institute & BMH Associates, Inc.
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

#ifndef __CAMERA_DATA_ACTOR__
#define __CAMERA_DATA_ACTOR__

#include <dtDAL/plugin_export.h>
#include <osg/Vec4>
#include <dtDAL/transformableactorproxy.h>
#include <dtUtil/getsetmacros.h>

namespace dtCore
{
   class Camera;
}

namespace dtActors 
{
   /**
    * Used to store Camera-related information.  To automatically apply data
    * to a Camera, call ApplyDataToCamera().
    */
   class DT_PLUGIN_EXPORT CameraDataActor : public dtCore::Transformable  
   {
      public:

         class DT_PLUGIN_EXPORT ProjectionMode : public dtUtil::Enumeration 
         {
            DECLARE_ENUM(ProjectionMode);
            public:
               static ProjectionMode ORTHO;
               static ProjectionMode ORTHO2D;
               static ProjectionMode FRUSTUM;
               static ProjectionMode PERSPECTIVE;

            private:
               ProjectionMode(const std::string &name) : dtUtil::Enumeration(name){ AddInstance(this);}
         };

         CameraDataActor(const std::string& name = "CameraData Actor");

         /** 
           * Apply the data to the supplied Camera, if the data application has
           * been enabled (e.g., SetChangeClearColor(true)).
           * @param camera The Camera to apply the data to
           */
         void ApplyDataTo(dtCore::Camera& camera);

         // Projection
         const osg::Matrix& GetProjectionMatrix();
         ProjectionMode& GetProjectionMode();
         void SetProjectionMode(ProjectionMode& value);

         // TODO NearFarCulling - enum?
         //bool GetChangeNearFarCulling(){ return mChangeNearFarCulling; };
         //void SetChangeNearFarCulling( bool value ){ mChangeNearFarCulling = value; };

         // TODO: ProjectionResizePolicy - enum?
         //bool GetChangeProjectionRP(){ return mChangeProjectionRP; };
         //void SetChangeProjectionRP( bool value ){ mChangeProjectionRP = value; };

      private:
         ProjectionMode* mProjectionMode;

      DECLARE_PROPERTY_INLINE(bool, Enabled)
      DECLARE_PROPERTY_INLINE(osg::Vec4, ClearColor)
      DECLARE_PROPERTY_INLINE(float, LODScale)
      DECLARE_PROPERTY_INLINE(osg::Vec4, Viewport)
      DECLARE_PROPERTY_INLINE(osg::Vec4, ProjectionRect)
      DECLARE_PROPERTY_INLINE(osg::Vec4, ProjectionPersp)


      DECLARE_PROPERTY_INLINE(bool, ChangeEnabled)
      DECLARE_PROPERTY_INLINE(bool, ChangeClearColor)
      DECLARE_PROPERTY_INLINE(bool, ChangeLODScale)
      DECLARE_PROPERTY_INLINE(bool, ChangeProjection)
      DECLARE_PROPERTY_INLINE(bool, ChangeTranslation)
      DECLARE_PROPERTY_INLINE(bool, ChangeRotation)
      DECLARE_PROPERTY_INLINE(bool, ChangeViewport)
   };

   class DT_PLUGIN_EXPORT CameraDataActorProxy : public dtDAL::TransformableActorProxy
   {
      public:
        CameraDataActorProxy();

        virtual void BuildPropertyMap();

        virtual const dtDAL::ActorProxy::RenderMode& GetRenderMode();
         
        dtDAL::ActorProxyIcon* GetBillBoardIcon();

        void ApplyDataTo(dtCore::Camera& camera);

      protected:
        virtual void CreateActor();
   };
}
#endif
