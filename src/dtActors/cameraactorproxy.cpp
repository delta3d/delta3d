/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, MOVES Institute & BMH Associates, Inc.
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
 * William E. Johnson II
 * Chris Osborn
 */
#include <dtActors/cameraactorproxy.h>

#include <dtCore/camera.h>

#include <dtCore/actorproxyicon.h>
#include <dtCore/booleanactorproperty.h>
#include <dtCore/colorrgbaactorproperty.h>
#include <dtCore/functor.h>

#include <sstream>

using namespace dtCore;
using namespace dtCore;
using namespace dtActors;

///////////////////////////////////////////////////////////////////////////////
void CameraActorProxy::CreateDrawable()
{
   SetDrawable(*new dtCore::Camera);

   static int actorCount = 0;
   std::ostringstream ss;
   ss << "Camera" << actorCount++;
   SetName(ss.str());

   Camera *cam = static_cast<Camera*>(GetDrawable());

   cam->SetEnabled(false);
}

///////////////////////////////////////////////////////////////////////////////
void CameraActorProxy::BuildPropertyMap()
{
   const std::string& GROUPNAME = "Camera";
   TransformableActorProxy::BuildPropertyMap();

   Camera* cam = static_cast<Camera*>(GetDrawable());

   AddProperty(new BooleanActorProperty("Enable", "Enabled",
      BooleanActorProperty::SetFuncType(cam, &dtCore::Camera::SetEnabled),
      BooleanActorProperty::GetFuncType(cam, &dtCore::Camera::GetEnabled),
      "Enables or disables this camera at runtime", GROUPNAME));

   // This property is used for the manipulation of the clear color
   // of a camera. Uses 4 values in the RGBA format for color
   // representation. All values are clamped between 0 - 1.
   // Default is 0, 0, 1, 0 (blue)
   AddProperty(new ColorRgbaActorProperty("Clear Color", "Clear Color",
      ColorRgbaActorProperty::SetFuncType(this, &dtActors::CameraActorProxy::SetClearColor),
      ColorRgbaActorProperty::GetFuncType(this, &dtActors::CameraActorProxy::GetClearColor),
      "Sets the camera's clear color, which can be thought of as the background color", GROUPNAME));
}

////////////////////////////////////////////////////////////////////////////////
dtCore::ActorProxyIcon* CameraActorProxy::GetBillBoardIcon()
{
   if (!mBillBoardIcon.valid())
   {
      mBillBoardIcon = new dtCore::ActorProxyIcon(dtCore::ActorProxyIcon::IMAGE_BILLBOARD_CAMERA);
   }

   return mBillBoardIcon.get();

}

///////////////////////////////////////////////////////////////////////////////
osg::Vec4f CameraActorProxy::GetClearColor()
{
   Camera *cam = static_cast<Camera*>(GetDrawable());

   osg::Vec4 color;
   cam->GetClearColor(color);
   return color;
}

///////////////////////////////////////////////////////////////////////////////
void CameraActorProxy::SetClearColor(const osg::Vec4 &color)
{
   Camera *cam = static_cast<Camera*>(GetDrawable());

   cam->SetClearColor(color);
}
