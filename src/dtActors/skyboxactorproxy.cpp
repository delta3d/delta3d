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
 * William E. Johnson II
 */

#include <dtActors/skyboxactorproxy.h>
#include <dtCore/resourceactorproperty.h>
#include <dtCore/datatype.h>
#include <dtCore/functor.h>
#include <dtCore/enumactorproperty.h>

namespace dtActors
{
   IMPLEMENT_ENUM(SkyBoxActorProxy::RenderProfile);
   SkyBoxActorProxy::RenderProfile SkyBoxActorProxy::RenderProfile::FIXED_FUNCTION("Fixed Function");
   SkyBoxActorProxy::RenderProfile SkyBoxActorProxy::RenderProfile::CUBE_MAP("Cubemap");
   SkyBoxActorProxy::RenderProfile SkyBoxActorProxy::RenderProfile::ANGULAR_MAP("Angular map");
   SkyBoxActorProxy::RenderProfile SkyBoxActorProxy::RenderProfile::DEFAULT("Default");

   /////////////////////////////////////////////////////////////////////////////
   SkyBoxActorProxy::SkyBoxActorProxy()
      : mRenderProfile(&RenderProfile::DEFAULT)
   {
      SetClassName("dtActors::Skybox");
   }

   /////////////////////////////////////////////////////////////////////////////
   void SkyBoxActorProxy::CreateDrawable()
   {
      SetDrawable(*new dtCore::SkyBox("skybox"));
   }

   DT_IMPLEMENT_ACCESSOR_GETTER(SkyBoxActorProxy,dtCore::ResourceDescriptor, TopTexture)
   void SkyBoxActorProxy::SetTopTexture(const dtCore::ResourceDescriptor& rd)
   {
      mTopTexture = rd;
      SetTexture(dtCore::SkyBox::SKYBOX_TOP, dtCore::ResourceActorProperty::GetResourcePath(rd));
   }

   DT_IMPLEMENT_ACCESSOR_GETTER(SkyBoxActorProxy,dtCore::ResourceDescriptor, BottomTexture)
   void SkyBoxActorProxy::SetBottomTexture(const dtCore::ResourceDescriptor& rd)
   {
      mBottomTexture = rd;
      SetTexture(dtCore::SkyBox::SKYBOX_BOTTOM, dtCore::ResourceActorProperty::GetResourcePath(rd));
   }
   DT_IMPLEMENT_ACCESSOR_GETTER(SkyBoxActorProxy,dtCore::ResourceDescriptor, FrontTexture)
   void SkyBoxActorProxy::SetFrontTexture(const dtCore::ResourceDescriptor& rd)
   {
      mFrontTexture = rd;
      SetTexture(dtCore::SkyBox::SKYBOX_FRONT, dtCore::ResourceActorProperty::GetResourcePath(rd));
   }
   DT_IMPLEMENT_ACCESSOR_GETTER(SkyBoxActorProxy,dtCore::ResourceDescriptor, BackTexture)
   void SkyBoxActorProxy::SetBackTexture(const dtCore::ResourceDescriptor& rd)
   {
      mBackTexture = rd;
      SetTexture(dtCore::SkyBox::SKYBOX_BACK, dtCore::ResourceActorProperty::GetResourcePath(rd));
   }
   DT_IMPLEMENT_ACCESSOR_GETTER(SkyBoxActorProxy,dtCore::ResourceDescriptor, LeftTexture)
   void SkyBoxActorProxy::SetLeftTexture(const dtCore::ResourceDescriptor& rd)
   {
      mLeftTexture = rd;
      SetTexture(dtCore::SkyBox::SKYBOX_LEFT, dtCore::ResourceActorProperty::GetResourcePath(rd));
   }
   DT_IMPLEMENT_ACCESSOR_GETTER(SkyBoxActorProxy,dtCore::ResourceDescriptor, RightTexture)
   void SkyBoxActorProxy::SetRightTexture(const dtCore::ResourceDescriptor& rd)
   {
      mRightTexture = rd;
      SetTexture(dtCore::SkyBox::SKYBOX_RIGHT, dtCore::ResourceActorProperty::GetResourcePath(rd));
   }


   /////////////////////////////////////////////////////////////////////////////
   void SkyBoxActorProxy::BuildPropertyMap()
   {
      dtActors::EnvEffectActorProxy::BuildPropertyMap();

      const std::string GROUPNAME = "SkyBox Textures";

      AddProperty(new dtCore::EnumActorProperty<RenderProfile>("Render Profile", "Render Profile",
         dtCore::EnumActorProperty<RenderProfile>::SetFuncType(this, &SkyBoxActorProxy::SetRenderProfilePreference),
         dtCore::EnumActorProperty<RenderProfile>::GetFuncType(this, &SkyBoxActorProxy::GetRenderProfilePreference),
         "Sets the way the skybox renders.",
         "Skybox Profile"));

      AddProperty(new dtCore::ResourceActorProperty(dtCore::DataType::TEXTURE,
         "Top Texture", "Top Texture",
         dtCore::ResourceActorProperty::SetDescFuncType(this, &SkyBoxActorProxy::SetTopTexture),
         dtCore::ResourceActorProperty::GetDescFuncType(this, &SkyBoxActorProxy::GetTopTexture),
         "Sets the texture on the top of the sky box", GROUPNAME));

      AddProperty(new dtCore::ResourceActorProperty(dtCore::DataType::TEXTURE,
         "Bottom Texture", "Bottom Texture",
         dtCore::ResourceActorProperty::SetDescFuncType(this, &SkyBoxActorProxy::SetBottomTexture),
         dtCore::ResourceActorProperty::GetDescFuncType(this, &SkyBoxActorProxy::GetBottomTexture),
         "Sets the texture on the bottom of the sky box", GROUPNAME));

      AddProperty(new dtCore::ResourceActorProperty(dtCore::DataType::TEXTURE,
         "Left Texture", "Left Texture",
         dtCore::ResourceActorProperty::SetDescFuncType(this, &SkyBoxActorProxy::SetLeftTexture),
         dtCore::ResourceActorProperty::GetDescFuncType(this, &SkyBoxActorProxy::GetLeftTexture),
         "Sets the texture on the left of the sky box", GROUPNAME));

      AddProperty(new dtCore::ResourceActorProperty(dtCore::DataType::TEXTURE,
         "Right Texture", "Right Texture",
         dtCore::ResourceActorProperty::SetDescFuncType(this, &SkyBoxActorProxy::SetRightTexture),
         dtCore::ResourceActorProperty::GetDescFuncType(this, &SkyBoxActorProxy::GetRightTexture),
         "Sets the texture on the right of the sky box", GROUPNAME));

      AddProperty(new dtCore::ResourceActorProperty(dtCore::DataType::TEXTURE,
         "Front Texture", "Front Texture",
         dtCore::ResourceActorProperty::SetDescFuncType(this, &SkyBoxActorProxy::SetFrontTexture),
         dtCore::ResourceActorProperty::GetDescFuncType(this, &SkyBoxActorProxy::GetFrontTexture),
          "Gets the texture on the front of the sky box", GROUPNAME));

      AddProperty(new dtCore::ResourceActorProperty(dtCore::DataType::TEXTURE,
         "Back Texture", "Back Texture",
         dtCore::ResourceActorProperty::SetDescFuncType(this, &SkyBoxActorProxy::SetBackTexture),
         dtCore::ResourceActorProperty::GetDescFuncType(this, &SkyBoxActorProxy::GetBackTexture),
         "Sets the texture on the back of the sky box", GROUPNAME));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SkyBoxActorProxy::SetRenderProfilePreference(RenderProfile& profile)
   {
      mRenderProfile = &profile;

      dtCore::SkyBox::RenderProfileEnum newProfile;

      // Translate the enumeration profile to the c++ enum
      if (mRenderProfile == &RenderProfile::FIXED_FUNCTION)
      {
         newProfile = dtCore::SkyBox::RP_FIXED_FUNCTION;
      }
      else if (mRenderProfile == &RenderProfile::CUBE_MAP)
      {
         newProfile = dtCore::SkyBox::RP_CUBE_MAP;
      }
      else if (mRenderProfile == &RenderProfile::ANGULAR_MAP)
      {
         newProfile = dtCore::SkyBox::RP_ANGULAR_MAP;
      }
      else
      {
         newProfile = dtCore::SkyBox::RP_DEFAULT;
      }

      dtCore::DeltaDrawable* actor = GetDrawable();

      if (actor)
      {
         dtCore::SkyBox* skybox = dynamic_cast<dtCore::SkyBox*>(actor);
         skybox->SetRenderProfilePreference(newProfile);
      }
   }

}
