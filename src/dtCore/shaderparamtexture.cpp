/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology
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
 * Matthew W. Campbell
 */
#include <prefix/dtcoreprefix.h>
#include <dtCore/project.h>
#include <dtCore/shaderparamtexture.h>
#include <dtUtil/log.h>

#include <osg/StateSet>
#include <osg/Uniform>
#include <osg/Texture>

namespace dtCore
{
   ///////////////////////////////////////////////////////////////////////////////
   IMPLEMENT_ENUM(ShaderParamTexture::AddressMode)
   const ShaderParamTexture::AddressMode ShaderParamTexture::AddressMode::CLAMP("Clamp");
   const ShaderParamTexture::AddressMode ShaderParamTexture::AddressMode::REPEAT("Repeat");
   const ShaderParamTexture::AddressMode ShaderParamTexture::AddressMode::MIRROR("Mirror");

   IMPLEMENT_ENUM(ShaderParamTexture::TextureAxis)
   const ShaderParamTexture::TextureAxis ShaderParamTexture::TextureAxis::S("S");
   const ShaderParamTexture::TextureAxis ShaderParamTexture::TextureAxis::T("T");
   const ShaderParamTexture::TextureAxis ShaderParamTexture::TextureAxis::R("R");
   const ShaderParamTexture::TextureAxis ShaderParamTexture::TextureAxis::Q("Q");

   IMPLEMENT_ENUM(ShaderParamTexture::TextureSourceType)
   const ShaderParamTexture::TextureSourceType ShaderParamTexture::TextureSourceType::IMAGE("Image");
   const ShaderParamTexture::TextureSourceType ShaderParamTexture::TextureSourceType::AUTO("Auto");
   const ShaderParamTexture::TextureSourceType ShaderParamTexture::TextureSourceType::CUBEMAP_IMAGE_POSITIVE_X("CubeMapPositiveX");
   const ShaderParamTexture::TextureSourceType ShaderParamTexture::TextureSourceType::CUBEMAP_IMAGE_NEGATIVE_X("CubeMapNegativeX");
   const ShaderParamTexture::TextureSourceType ShaderParamTexture::TextureSourceType::CUBEMAP_IMAGE_POSITIVE_Y("CubeMapPositiveY");
   const ShaderParamTexture::TextureSourceType ShaderParamTexture::TextureSourceType::CUBEMAP_IMAGE_NEGATIVE_Y("CubeMapNegativeY");
   const ShaderParamTexture::TextureSourceType ShaderParamTexture::TextureSourceType::CUBEMAP_IMAGE_POSITIVE_Z("CubeMapPositiveZ");
   const ShaderParamTexture::TextureSourceType ShaderParamTexture::TextureSourceType::CUBEMAP_IMAGE_NEGATIVE_Z("CubeMapNegativeZ");

   ///////////////////////////////////////////////////////////////////////////////
   ShaderParamTexture::ShaderParamTexture(const std::string &name) :
         ShaderParameter(name), mImageSrcChanged(false), mTextureUnit(0), mTextureObject(NULL), mTexturePath("")
   {
      SetShared(true); // we want to share Texture parameters by default
      mTextureAddressMode[0] = &AddressMode::CLAMP;
      mTextureAddressMode[1] = &AddressMode::CLAMP;
      mTextureAddressMode[2] = &AddressMode::CLAMP;
      mTextureAddressMode[3] = &AddressMode::CLAMP;
      mSourceType = &TextureSourceType::IMAGE;
   }

   ///////////////////////////////////////////////////////////////////////////////
   ShaderParamTexture::~ShaderParamTexture()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ShaderParamTexture::SetAddressMode(const TextureAxis &axis, const AddressMode &mode)
   {
      if (axis == TextureAxis::S)
         mTextureAddressMode[0] = &mode;
      else if (axis == TextureAxis::T)
         mTextureAddressMode[1] = &mode;
      else if (axis == TextureAxis::R)
         mTextureAddressMode[2] = &mode;
      else
         mTextureAddressMode[3] = &mode;

      SetDirty(true);
   }

   ///////////////////////////////////////////////////////////////////////////////
   const ShaderParamTexture::AddressMode& ShaderParamTexture::GetAddressMode(
      const ShaderParamTexture::TextureAxis &axis)
   {
      if (axis == TextureAxis::S)
         return *mTextureAddressMode[0];
      else if (axis == TextureAxis::T)
         return *mTextureAddressMode[1];
      else if (axis == TextureAxis::R)
         return *mTextureAddressMode[2];
      else
         return *mTextureAddressMode[3];
   }

   /////////////////////////////////////////////////////////////////////////////
   void ShaderParamTexture::SetTexture(const std::string &path)
   {
      mTexturePath = path;
      
      SetDirty(true);
      SetImageSourceDirty(true);
   }

   /////////////////////////////////////////////////////////////////////////////
   void ShaderParamTexture::SetTextureResource(const dtCore::ResourceDescriptor& value)
   {
      mDescriptor = value;

      if (dtCore::Project::GetInstance().IsContextValid())
      {
         if ( ! mDescriptor.IsEmpty())
         {
            std::string filePath = dtCore::Project::GetInstance().GetResourcePath(value);
            SetTexture(filePath);
         }
         else
         {
            SetTexture("");
         }
      }
      else
      {
         LOG_WARNING("Cannot access the project context for assigning texture file: " + value.GetResourceIdentifier());
      }
   }

   dtCore::ResourceDescriptor ShaderParamTexture::GetTextureResource() const
   {
      return mDescriptor;
   }

}
