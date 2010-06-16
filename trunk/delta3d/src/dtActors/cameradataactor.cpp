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
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/actorproxyicon.h>
#include <dtCore/transform.h>
#include <dtCore/camera.h>

#include <osg/Camera>

using namespace dtCore;
using namespace dtDAL;
using namespace dtActors;


IMPLEMENT_ENUM(CameraDataActor::ProjectionMode);
CameraDataActor::ProjectionMode CameraDataActor::ProjectionMode::FRUSTUM("Frustum");
CameraDataActor::ProjectionMode CameraDataActor::ProjectionMode::ORTHO("Ortho");
CameraDataActor::ProjectionMode CameraDataActor::ProjectionMode::ORTHO2D("Ortho2D");
CameraDataActor::ProjectionMode CameraDataActor::ProjectionMode::PERSPECTIVE("Perspective");

//////////////////////////////////////////////////////////////////////////
void CameraDataActorProxy::ApplyDataToCamera(dtCore::Camera& camera)
{
   CameraDataActor *actor = static_cast<CameraDataActor*>(GetActor());

   actor->ApplyDataToCamera(camera);
}

//////////////////////////////////////////////////////////////////////////
void CameraDataActorProxy::CreateActor()
{
   SetActor(*new CameraDataActor("CameraData"));
}

//////////////////////////////////////////////////////////////////////////
void CameraDataActorProxy::BuildPropertyMap()
{
   TransformableActorProxy::BuildPropertyMap();
   CameraDataActor *actor = static_cast<CameraDataActor*>(GetActor());

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
   AddProperty( new BooleanActorProperty("changeView", "Change ModelView",
      BooleanActorProperty::SetFuncType(actor, &CameraDataActor::SetChangeModelView),
      BooleanActorProperty::GetFuncType(actor, &CameraDataActor::GetChangeModelView),
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
dtDAL::ActorProxyIcon* CameraDataActorProxy::GetBillBoardIcon()
{
   if(!mBillBoardIcon.valid())
   {
      mBillBoardIcon = new dtDAL::ActorProxyIcon(dtDAL::ActorProxyIcon::IMAGE_BILLBOARD_CAMERA);
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
   mChangeModelView  = true;

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
bool dtActors::CameraDataActor::GetChangeClearColor()
{
   return mChangeClearColor;
}

////////////////////////////////////////////////////////////////////////////////
void dtActors::CameraDataActor::SetChangeClearColor(bool value)
{
   mChangeClearColor = value;
}

////////////////////////////////////////////////////////////////////////////////
const osg::Vec4& dtActors::CameraDataActor::GetClearColor()
{
   return mClearColor;
}

////////////////////////////////////////////////////////////////////////////////
void dtActors::CameraDataActor::SetClearColor(const osg::Vec4& color)
{
   mClearColor = color;
}

////////////////////////////////////////////////////////////////////////////////
bool dtActors::CameraDataActor::GetChangeModelView()
{
   return mChangeModelView;
}

////////////////////////////////////////////////////////////////////////////////
void dtActors::CameraDataActor::SetChangeModelView(bool value)
{
   mChangeModelView = value;
}

////////////////////////////////////////////////////////////////////////////////
const dtCore::Transform dtActors::CameraDataActor::GetModelViewTransform()
{
   dtCore::Transform transform; GetTransform(transform); return transform;
}

////////////////////////////////////////////////////////////////////////////////
bool dtActors::CameraDataActor::GetChangeProjection()
{
   return mChangeProjection;
}

////////////////////////////////////////////////////////////////////////////////
void dtActors::CameraDataActor::SetChangeProjection(bool value)
{
   mChangeProjection = value;
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
void dtActors::CameraDataActor::SetProjectionRect(const osg::Vec4& value)
{
   mProjectionRect = value;
}

////////////////////////////////////////////////////////////////////////////////
const osg::Vec4& dtActors::CameraDataActor::GetProjectionRect()
{
   return mProjectionRect;
}

////////////////////////////////////////////////////////////////////////////////
void dtActors::CameraDataActor::SetProjectionPersp(const osg::Vec4& value)
{
   mProjectionPersp = value;
}

////////////////////////////////////////////////////////////////////////////////
const osg::Vec4& dtActors::CameraDataActor::GetProjectionPersp()
{
   return mProjectionPersp;
}

////////////////////////////////////////////////////////////////////////////////
bool dtActors::CameraDataActor::GetChangeLODScale()
{
   return mChangeLODScale;
}

////////////////////////////////////////////////////////////////////////////////
void dtActors::CameraDataActor::SetChangeLODScale(bool value)
{
   mChangeLODScale = value;
}

////////////////////////////////////////////////////////////////////////////////
void dtActors::CameraDataActor::SetLODScale(float value)
{
   mLODScale = value;
}

////////////////////////////////////////////////////////////////////////////////
float dtActors::CameraDataActor::GetLODScale()
{
   return mLODScale;
}

////////////////////////////////////////////////////////////////////////////////
bool dtActors::CameraDataActor::GetChangeEnabled()
{
   return mChangeEnabled;
}

////////////////////////////////////////////////////////////////////////////////
void dtActors::CameraDataActor::SetChangeEnabled(bool value)
{
   mChangeEnabled= value;
}

////////////////////////////////////////////////////////////////////////////////
void dtActors::CameraDataActor::SetEnabled(bool value)
{
   mEnabled= value;
}

////////////////////////////////////////////////////////////////////////////////
bool dtActors::CameraDataActor::GetEnabled()
{
   return mEnabled;
}

////////////////////////////////////////////////////////////////////////////////
bool dtActors::CameraDataActor::GetChangeViewport()
{
   return mChangeViewport;
}

////////////////////////////////////////////////////////////////////////////////
void dtActors::CameraDataActor::SetChangeViewport(bool value)
{
   mChangeViewport = value;
}

////////////////////////////////////////////////////////////////////////////////
void dtActors::CameraDataActor::SetViewport(const osg::Vec4& value)
{
   mViewport = value;
}

////////////////////////////////////////////////////////////////////////////////
const osg::Vec4& dtActors::CameraDataActor::GetViewport()
{
   return mViewport;
}

////////////////////////////////////////////////////////////////////////////////
void dtActors::CameraDataActor::ApplyDataToCamera(dtCore::Camera& camera)
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
   if (GetChangeModelView())
   {
      camera.SetTransform(GetModelViewTransform());
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
bool dtActors::CameraDataActorProxy::IsPlaceable() const
{
   return true;
}

////////////////////////////////////////////////////////////////////////////////
const dtDAL::ActorProxy::RenderMode& dtActors::CameraDataActorProxy::GetRenderMode()
{
   return dtDAL::ActorProxy::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON;
}
