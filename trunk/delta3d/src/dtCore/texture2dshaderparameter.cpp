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

#include "dtCore/texture2dshaderparameter.h"
#include <dtUtil/exception.h>
#include <dtCore/refptr.h>
#include <dtUtil/fileutils.h>

#include <dtCore/globals.h>
#include <osg/StateSet>
#include <osg/Texture2D>
#include <osg/Uniform>
#include <osg/Image>
#include <osgDB/ReadFile>

namespace dtCore
{
   ///////////////////////////////////////////////////////////////////////////////
   Texture2DShaderParameter::Texture2DShaderParameter(const std::string &name) :
      TextureShaderParameter(name)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   Texture2DShaderParameter::~Texture2DShaderParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Texture2DShaderParameter::AttachToRenderState(osg::StateSet &stateSet)
   {
      if (GetTexture().empty() && GetTextureSourceType() != TextureShaderParameter::TextureSourceType::AUTO)
         EXCEPT(ShaderParameterException::INVALID_ATTRIBUTE,"Cannot attach to render state.  Texture "
               "for parameter " + GetName() + " has not been specified.");

      osg::Uniform *uniform = new osg::Uniform(osg::Uniform::SAMPLER_2D,GetName());
      uniform->set((int)GetTextureUnit());
      stateSet.addUniform(uniform);
      SetUniformParam(*uniform);

      if (GetTextureSourceType() == TextureShaderParameter::TextureSourceType::IMAGE)
      {
         dtCore::RefPtr<osg::Texture2D> tex2D = new osg::Texture2D();
         std::string filePath = dtCore::FindFileInPathList(GetTexture());
         osg::Image *image = osgDB::readImageFile(filePath);
         if (image == NULL)
            EXCEPT(ShaderParameterException::INVALID_ATTRIBUTE,"Could not find image for texture at location: "
               + GetTexture());

         tex2D->setImage(image);
         tex2D->setUnRefImageDataAfterApply(true);
         SetTextureObject(*tex2D);

         //These need to be in the XML definition at some point.
         tex2D->setFilter(osg::Texture::MIN_FILTER,osg::Texture::LINEAR_MIPMAP_LINEAR);
         tex2D->setFilter(osg::Texture::MAG_FILTER,osg::Texture::LINEAR);

         //Set the texture addressing...
         const TextureShaderParameter::AddressMode &wrapModeS =
            GetAddressMode(TextureShaderParameter::TextureAxis::S);
         if (wrapModeS == TextureShaderParameter::AddressMode::CLAMP)
            tex2D->setWrap(osg::Texture::WRAP_S,osg::Texture::CLAMP_TO_EDGE);
         else if (wrapModeS == TextureShaderParameter::AddressMode::REPEAT)
            tex2D->setWrap(osg::Texture::WRAP_S,osg::Texture::REPEAT);
         else if (wrapModeS == TextureShaderParameter::AddressMode::MIRROR)
            tex2D->setWrap(osg::Texture::WRAP_S,osg::Texture::MIRROR);

         const TextureShaderParameter::AddressMode &wrapModeT =
            GetAddressMode(TextureShaderParameter::TextureAxis::T);
         if (wrapModeT == TextureShaderParameter::AddressMode::CLAMP)
            tex2D->setWrap(osg::Texture::WRAP_T,osg::Texture::CLAMP_TO_EDGE);
         else if (wrapModeT == TextureShaderParameter::AddressMode::REPEAT)
            tex2D->setWrap(osg::Texture::WRAP_T,osg::Texture::REPEAT);
         else if (wrapModeT == TextureShaderParameter::AddressMode::MIRROR)
            tex2D->setWrap(osg::Texture::WRAP_T,osg::Texture::MIRROR);

         const TextureShaderParameter::AddressMode &wrapModeR =
            GetAddressMode(TextureShaderParameter::TextureAxis::R);
         if (wrapModeR == TextureShaderParameter::AddressMode::CLAMP)
            tex2D->setWrap(osg::Texture::WRAP_R,osg::Texture::CLAMP_TO_EDGE);
         else if (wrapModeR == TextureShaderParameter::AddressMode::REPEAT)
            tex2D->setWrap(osg::Texture::WRAP_R,osg::Texture::REPEAT);
         else if (wrapModeR == TextureShaderParameter::AddressMode::MIRROR)
            tex2D->setWrap(osg::Texture::WRAP_R,osg::Texture::MIRROR);

         //Assign the completed texture attribute to the render state.
         stateSet.setTextureAttributeAndModes(GetTextureUnit(),tex2D.get(),osg::StateAttribute::ON);
      }

      SetDirty(false);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Texture2DShaderParameter::Update()
   {
      osg::Uniform *uniform = GetUniformParam();
      osg::Texture2D *tex2D = static_cast<osg::Texture2D*>(GetTextureObject());

      if (uniform == NULL)
      {
         LOG_WARNING("Could not update shader parameter: " + GetName() + " Uniform "
               "shader parameter was invalid.");
         return;
      }

      if (tex2D == NULL)
      {
         LOG_WARNING("Could not update shader parameter: " + GetName() + " Texture object "
               "was invalid.  Perhaps this is an AUTO texture parameter.");
         return;
      }

      uniform->set((int)GetTextureUnit());
      if (IsImageSourceDirty())
      {
         if (GetTextureSourceType() == TextureShaderParameter::TextureSourceType::IMAGE)
         {
            std::string filePath = dtCore::FindFileInPathList(GetTexture());
            osg::Image *image = osgDB::readImageFile(filePath);
            if (image == NULL)
               EXCEPT(ShaderParameterException::INVALID_ATTRIBUTE,"Could not find image for texture at location: "
                     + GetTexture());

            tex2D->setImage(image);
            tex2D->setUnRefImageDataAfterApply(true);
            SetTextureObject(*tex2D);
         }
      }

      //These need to be in the XML definition at some point.
      tex2D->setFilter(osg::Texture::MIN_FILTER,osg::Texture::LINEAR_MIPMAP_LINEAR);
      tex2D->setFilter(osg::Texture::MAG_FILTER,osg::Texture::LINEAR);

            //Set the texture addressing...
      const TextureShaderParameter::AddressMode &wrapModeS =
            GetAddressMode(TextureShaderParameter::TextureAxis::S);
      if (wrapModeS == TextureShaderParameter::AddressMode::CLAMP)
         tex2D->setWrap(osg::Texture::WRAP_S,osg::Texture::CLAMP_TO_EDGE);
      else if (wrapModeS == TextureShaderParameter::AddressMode::REPEAT)
         tex2D->setWrap(osg::Texture::WRAP_S,osg::Texture::REPEAT);
      else if (wrapModeS == TextureShaderParameter::AddressMode::MIRROR)
         tex2D->setWrap(osg::Texture::WRAP_S,osg::Texture::MIRROR);

      const TextureShaderParameter::AddressMode &wrapModeT =
            GetAddressMode(TextureShaderParameter::TextureAxis::T);
      if (wrapModeT == TextureShaderParameter::AddressMode::CLAMP)
         tex2D->setWrap(osg::Texture::WRAP_T,osg::Texture::CLAMP_TO_EDGE);
      else if (wrapModeT == TextureShaderParameter::AddressMode::REPEAT)
         tex2D->setWrap(osg::Texture::WRAP_T,osg::Texture::REPEAT);
      else if (wrapModeT == TextureShaderParameter::AddressMode::MIRROR)
         tex2D->setWrap(osg::Texture::WRAP_T,osg::Texture::MIRROR);

      const TextureShaderParameter::AddressMode &wrapModeR =
            GetAddressMode(TextureShaderParameter::TextureAxis::R);
      if (wrapModeR == TextureShaderParameter::AddressMode::CLAMP)
         tex2D->setWrap(osg::Texture::WRAP_R,osg::Texture::CLAMP_TO_EDGE);
      else if (wrapModeR == TextureShaderParameter::AddressMode::REPEAT)
         tex2D->setWrap(osg::Texture::WRAP_R,osg::Texture::REPEAT);
      else if (wrapModeR == TextureShaderParameter::AddressMode::MIRROR)
         tex2D->setWrap(osg::Texture::WRAP_R,osg::Texture::MIRROR);

      SetDirty(false);
   }

}
