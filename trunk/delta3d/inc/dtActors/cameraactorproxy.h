/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2005, BMH Associates, Inc.
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
* @author William E. Johnson II
*/
#ifndef DELTA_CAMERA_ACTOR_PROXY
#define DELTA_CAMERA_ACTOR_PROXY

#include <osg/Vec4>

#include <dtDAL/plugin_export.h>
#include <dtDAL/transformableactorproxy.h>
#include <dtDAL/exceptionenum.h>

namespace dtActors 
{
   /**
   * @class CameraActorProxy
   * @brief This proxy wraps the Camera Delta3D object.
   */
   class DT_PLUGIN_EXPORT CameraActorProxy : public dtDAL::TransformableActorProxy
   {
   public:

      /**
      * Constructor
      */
      CameraActorProxy() { SetClassName("dtCore::Camera"); }

      /**
      * Adds the properties that are common to all Delta3D camera objects.
      */
      virtual void BuildPropertyMap();

      /**
      * Cameras can be placed in a scene
      */
      virtual bool IsPlaceable() const { return true; }

      virtual const dtDAL::ActorProxy::RenderMode& GetRenderMode()
      {
         return dtDAL::ActorProxy::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON;
      }

      /**
      * Gets the billboard used to represent static meshes if this proxy's
      * render mode is RenderMode::DRAW_BILLBOARD_ICON.
      * @return
      */
      dtDAL::ActorProxyIcon* GetBillBoardIcon();

      /**
      * Gets the camera's clear color
      * @return The current clear color
      */
      osg::Vec4 GetClearColor();

      /**
      * Sets the clear color
      * @param color The color to set
      */
      void SetClearColor(const osg::Vec4 &color);

   protected:

      /**
      * Initializes the actor
      */
      virtual void CreateActor();

      /**
      * Destructor
      */
      virtual ~CameraActorProxy() {}
   };
}
#endif
