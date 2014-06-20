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

#include <dtActors/cameradataactor.h>

#include <dtCore/transform.h>
#include <dtCore/camera.h>

#include <dtCore/actorproxyicon.h>
#include <dtCore/booleanactorproperty.h>
#include <dtCore/colorrgbaactorproperty.h>
#include <dtCore/enumactorproperty.h>
#include <dtCore/floatactorproperty.h>
#include <dtCore/vectoractorproperties.h>

#include <osg/Camera>

using namespace dtCore;
using namespace dtCore;
using namespace dtActors;


IMPLEMENT_ENUM(CameraDataActor::ProjectionMode);
CameraDataActor::ProjectionMode CameraDataActor::ProjectionMode::FRUSTUM("Frustum");
CameraDataActor::ProjectionMode CameraDataActor::ProjectionMode::ORTHO("Ortho");
CameraDataActor::ProjectionMode CameraDataActor::ProjectionMode::ORTHO2D("Ortho2D");
CameraDataActor::ProjectionMode CameraDataActor::ProjectionMode::PERSPECTIVE("Perspective");

//////////////////////////////////////////////////////////////////////////
void CameraDataActorProxy::ApplyDataTo(dtCore::Camera& camera)
{
   CameraDataActor *actor = static_cast<CameraDataActor*>(GetDrawable());

   actor->ApplyDataTo(camera);
}

//////////////////////////////////////////////////////////////////////////
void CameraDataActorProxy::CreateDrawable()
{
   SetDrawable(*new CameraDataActor("CameraData"));
}

//////////////////////////////////////////////////////////////////////////
void CameraDataActorProxy::BuildPropertyMap()
{
   TransformableActorProxy::BuildPropertyMap();
   CameraDataActor *actor = static_cast<CameraDataActor*>(GetDrawable());

   // Clear Color 
   AddProperty( new BooleanActorProperty("changeClearColor", "Change Clear Color",
      BooleanActorProperty::SetFuncType(actor, &CameraDataActor::SetChangeClearColor),
       BooleanActorProperty::GetFuncType(actor, &CameraDataActor::GetChangeClearColor),
       "", "Camera ClearColor"));

   AddProperty(new ColorRgbaActorProperty("clearColor", "Clear Color",
      ColorRgbaActorProperty::SetFuncType(actor, &CameraDataActor::SetClearColor),
      ColorRgbaActorProperty::GetFuncType(actor, &CameraDataActor::GetClearColor),
      "", "Camera ClearColor"));

   // Enable
   AddProperty( new BooleanActorProperty("changeEnabled", "Change Enabled",
      BooleanActorProperty::SetFuncType(actor, &CameraDataActor::SetChangeEnabled),
      BooleanActorProperty::GetFuncType(actor, &CameraDataActor::GetChangeEnabled),
      "", "Camera Enable"));

   AddProperty(new BooleanActorProperty("enabled", "Enabled",
      BooleanActorProperty::SetFuncType(actor, &CameraDataActor::SetEnabled),
      BooleanActorProperty::GetFuncType(actor, &CameraDataActor::GetEnabled),
      "", "Camera Enable"));

   // ModelView
   AddProperty( new BooleanActorProperty("changeTranslation", "Change Translation / Position",
      BooleanActorProperty::SetFuncType(actor, &CameraDataActor::SetChangeTranslation),
      BooleanActorProperty::GetFuncType(actor, &CameraDataActor::GetChangeTranslation),
      "", "Camera ModelView"));

   AddProperty( new BooleanActorProperty("changeRotation", "Change Rotation / Orientation",
      BooleanActorProperty::SetFuncType(actor, &CameraDataActor::SetChangeRotation),
      BooleanActorProperty::GetFuncType(actor, &CameraDataActor::GetChangeRotation),
      "", "Camera ModelView"));

   // Projection
   AddProperty( new BooleanActorProperty("changeProjection", "Change Projection",
      BooleanActorProperty::SetFuncType(actor, &CameraDataActor::SetChangeProjection),
      BooleanActorProperty::GetFuncType(actor, &CameraDataActor::GetChangeProjection),
      "", "Camera Projection"));

   AddProperty(new EnumActorProperty<CameraDataActor::ProjectionMode>("projectionMode", "Mode",
      EnumActorProperty<CameraDataActor::ProjectionMode>::SetFuncType(actor, &CameraDataActor::SetProjectionMode),
      EnumActorProperty<CameraDataActor::ProjectionMode>::GetFuncType(actor, &CameraDataActor::GetProjectionMode),
      "", "Camera Projection"));

   AddProperty(new Vec4ActorProperty("projectionRect", "Rectangle (left, right, bottom, top)",
      Vec4ActorProperty::SetFuncType(actor, &CameraDataActor::SetProjectionRect),
      Vec4ActorProperty::GetFuncType(actor, &CameraDataActor::GetProjectionRect),
      "", "Camera Projection"));

   AddProperty(new Vec4ActorProperty("projectionPersp", "Perspective (fovy, aspectRatio, zNear, zFar)",
      Vec4ActorProperty::SetFuncType(actor, &CameraDataActor::SetProjectionPersp),
      Vec4ActorProperty::GetFuncType(actor, &CameraDataActor::GetProjectionPersp),
      "", "Camera Projection"));

   // LODScale
   AddProperty( new BooleanActorProperty("changeLODScale", "Change LODScale",
      BooleanActorProperty::SetFuncType(actor, &CameraDataActor::SetChangeLODScale),
      BooleanActorProperty::GetFuncType(actor, &CameraDataActor::GetChangeLODScale),
      "", "Camera LODScale"));

   AddProperty(new FloatActorProperty("LODScale", "LODScale",
      FloatActorProperty::SetFuncType(actor, &CameraDataActor::SetLODScale),
      FloatActorProperty::GetFuncType(actor, &CameraDataActor::GetLODScale),
      "", "Camera LODScale"));

   // Viewport
   AddProperty( new BooleanActorProperty("changeViewport", "Change Viewport",
      BooleanActorProperty::SetFuncType(actor, &CameraDataActor::SetChangeViewport),
      BooleanActorProperty::GetFuncType(actor, &CameraDataActor::GetChangeViewport),
      "", "Camera Viewport"));

   AddProperty(new Vec4ActorProperty("viewport", "Viewport (x, y, width, height)",
      Vec4ActorProperty::SetFuncType(actor, &CameraDataActor::SetViewport),
      Vec4ActorProperty::GetFuncType(actor, &CameraDataActor::GetViewport),
      "", "Camera Viewport"));
}

//////////////////////////////////////////////////////////////////////////
dtCore::ActorProxyIcon* CameraDataActorProxy::GetBillBoardIcon()
{
   if(!mBillBoardIcon.valid())
   {
      mBillBoardIcon = new dtCore::ActorProxyIcon(dtCore::ActorProxyIcon::IMAGE_BILLBOARD_CAMERA);
   }

   return mBillBoardIcon.get();
}

//////////////////////////////////////////////////////////////////////////
CameraDataActor::CameraDataActor( const std::string& name ) : Transformable( name )
{

   mChangeClearColor = false;
   mChangeProjection = false;
   mChangeEnabled    = false;
   mChangeLODScale   = false;
   mChangeViewport   = false;
   mChangeTranslation   = true;
   mChangeRotation      = true;

   mProjectionMode   = &ProjectionMode::PERSPECTIVE;
   mLODScale         = 1.0f;

   mEnabled          = false;
}

//////////////////////////////////////////////////////////////////////////
const osg::Matrix& dtActors::CameraDataActor::GetProjectionMatrix()
{
   dtCore::RefPtr<osg::Camera> cam = new osg::Camera();

   if( *mProjectionMode == ProjectionMode::FRUSTUM )
   {
      cam->setProjectionMatrixAsFrustum( mProjectionRect.x(), mProjectionRect.y(), mProjectionRect.z(), mProjectionRect.w(), mProjectionPersp.z(), mProjectionPersp.w() );
   }
   else if( *mProjectionMode == ProjectionMode::ORTHO )
   {
      cam->setProjectionMatrixAsOrtho( mProjectionRect.x(), mProjectionRect.y(), mProjectionRect.z(), mProjectionRect.w(), mProjectionPersp.z(), mProjectionPersp.w() );
   }
   else if( *mProjectionMode == ProjectionMode::ORTHO2D )
   {
      cam->setProjectionMatrixAsOrtho2D( mProjectionRect.x(), mProjectionRect.y(), mProjectionRect.z(), mProjectionRect.w() );
   }
   else if( *mProjectionMode == ProjectionMode::PERSPECTIVE )
   {
      cam->setProjectionMatrixAsPerspective( mProjectionPersp.x(), mProjectionPersp.y(), mProjectionPersp.z(), mProjectionPersp.w() );
   }

   return cam->getProjectionMatrix();
}


////////////////////////////////////////////////////////////////////////////////
dtActors::CameraDataActor::ProjectionMode& dtActors::CameraDataActor::GetProjectionMode()
{
   return *mProjectionMode;
}

////////////////////////////////////////////////////////////////////////////////
void dtActors::CameraDataActor::SetProjectionMode(ProjectionMode& value)
{
   mProjectionMode = &value;
}

////////////////////////////////////////////////////////////////////////////////
void dtActors::CameraDataActor::ApplyDataTo(dtCore::Camera& camera)
{
   if (GetChangeClearColor())
   {
      camera.SetClearColor(GetClearColor());
   }
   if (GetChangeEnabled())
   {
      camera.SetEnabled(GetEnabled());
   }
   if (GetChangeLODScale())
   {
      camera.SetLODScale(GetLODScale());
   }
   if (GetChangeTranslation())
   {
      dtCore::Transform transformSource;
      dtCore::Transform transformTarget;
      GetTransform(transformSource);
      camera.GetTransform(transformTarget);

      transformTarget.SetTranslation(transformSource.GetTranslation());
      camera.SetTransform(transformTarget);
   }
   if (GetChangeRotation())
   {
      dtCore::Transform transformSource;
      dtCore::Transform transformTarget;
      GetTransform(transformSource);
      camera.GetTransform(transformTarget);

      transformTarget.SetRotation(transformSource.GetRotation());
      camera.SetTransform(transformTarget);
   }
   if (GetChangeProjection())
   {
      camera.GetOSGCamera()->setProjectionMatrix(GetProjectionMatrix());
   }
   if (GetChangeViewport())
   {
      const osg::Vec4& viewport = GetViewport();
      camera.GetOSGCamera()->setViewport(viewport.x(), viewport.y(), viewport.z(), viewport.w());
   }
}

////////////////////////////////////////////////////////////////////////////////
dtActors::CameraDataActorProxy::CameraDataActorProxy()
{
   SetClassName("dtActors::CameraDataActor");
}

////////////////////////////////////////////////////////////////////////////////
const dtCore::BaseActorObject::RenderMode& dtActors::CameraDataActorProxy::GetRenderMode()
{
   return dtCore::BaseActorObject::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON;
}
