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
 * @author Matthew W. Campbell
 */
#include "dtCore/textureshaderparameter.h"
#include <dtCore/refptr.h>

#include <osg/StateSet>
#include <osg/Uniform>
#include <osg/Texture>

namespace dtCore
{
   ///////////////////////////////////////////////////////////////////////////////
   IMPLEMENT_ENUM(TextureShaderParameter::AddressMode);
   const TextureShaderParameter::AddressMode TextureShaderParameter::AddressMode::CLAMP("Clamp");
   const TextureShaderParameter::AddressMode TextureShaderParameter::AddressMode::REPEAT("Repeat");
   const TextureShaderParameter::AddressMode TextureShaderParameter::AddressMode::MIRROR("Mirror");

   IMPLEMENT_ENUM(TextureShaderParameter::TextureAxis);
   const TextureShaderParameter::TextureAxis TextureShaderParameter::TextureAxis::S("S");
   const TextureShaderParameter::TextureAxis TextureShaderParameter::TextureAxis::T("T");
   const TextureShaderParameter::TextureAxis TextureShaderParameter::TextureAxis::R("R");
   const TextureShaderParameter::TextureAxis TextureShaderParameter::TextureAxis::Q("Q");

   IMPLEMENT_ENUM(TextureShaderParameter::TextureSourceType);
   const TextureShaderParameter::TextureSourceType TextureShaderParameter::TextureSourceType::IMAGE("Image");
   const TextureShaderParameter::TextureSourceType TextureShaderParameter::TextureSourceType::AUTO("Auto");

   ///////////////////////////////////////////////////////////////////////////////
   TextureShaderParameter::TextureShaderParameter(const std::string &name) :
         ShaderParameter(name), mImageSrcChanged(false), mTextureUnit(0), mTextureObject(NULL), mTexturePath("")
   {
      mTextureAddressMode[0] = &AddressMode::CLAMP;
      mTextureAddressMode[1] = &AddressMode::CLAMP;
      mTextureAddressMode[2] = &AddressMode::CLAMP;
      mTextureAddressMode[3] = &AddressMode::CLAMP;
      mSourceType = &TextureSourceType::IMAGE;
   }

   ///////////////////////////////////////////////////////////////////////////////
   TextureShaderParameter::~TextureShaderParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void TextureShaderParameter::SetAddressMode(const TextureAxis &axis, const AddressMode &mode)
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
   const TextureShaderParameter::AddressMode &TextureShaderParameter::GetAddressMode(
      const TextureShaderParameter::TextureAxis &axis)
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
}
