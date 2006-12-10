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
 * Matthew W. Campbell, Curtiss Murphy
 */

#include <prefix/dtcoreprefix-src.h>
#include <dtCore/texture2dshaderparameter.h>
#include <dtUtil/exception.h>
#include <dtCore/refptr.h>

#include <dtCore/globals.h>
#include <osg/StateSet>
#include <osg/Texture2D>
#include <osg/Uniform>
#include <osg/Image>
#include <osgDB/ReadFile>

//#include <dtCore/timer.h>

namespace dtCore
{
   ///////////////////////////////////////////////////////////////////////////////
   Texture2DShaderParameter::Texture2DShaderParameter(const std::string &name) :
      TextureShaderParameter(name)
   {
      SetTextureObject(*(new osg::Texture2D()));
   }

   ///////////////////////////////////////////////////////////////////////////////
   Texture2DShaderParameter::~Texture2DShaderParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Texture2DShaderParameter::AttachToRenderState(osg::StateSet &stateSet)
   {
      //bool needToCreateNewTex2D = (GetTextureObject() == NULL);
      dtCore::RefPtr<osg::Texture2D> tex2D;

      if (GetTexture().empty() && GetTextureSourceType() != TextureShaderParameter::TextureSourceType::AUTO)
         throw dtUtil::Exception(ShaderParameterException::INVALID_ATTRIBUTE,"Cannot attach to render state.  Texture "
               "for parameter " + GetName() + " has not been specified.", __FILE__, __LINE__);

      osg::Uniform *uniform = new osg::Uniform(osg::Uniform::SAMPLER_2D,GetName());
      uniform->set((int)GetTextureUnit());
      stateSet.addUniform(uniform);
      SetUniformParam(*uniform);

      if (GetTextureSourceType() == TextureShaderParameter::TextureSourceType::IMAGE)
      {
         tex2D = static_cast<osg::Texture2D*>(GetTextureObject());

         // load or reload the image - allows caching from the template
         // Note - ImageSourceDirty may not be relevant anymore cause it now loads the image when you call SetTexture().
         if (tex2D->getImage() == NULL || IsImageSourceDirty()) 
         {
            LoadImage();
            //SetTextureObject(*tex2D);
            ApplyTexture2DValues();
         }

         //Assign the completed texture attribute to the render state.
         stateSet.setTextureAttributeAndModes(GetTextureUnit(),tex2D.get(),osg::StateAttribute::ON);
      }

      SetDirty(false);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Texture2DShaderParameter::DetachFromRenderState(osg::StateSet &stateSet)
   {
      osg::Texture2D *tex2D = static_cast<osg::Texture2D*>(GetTextureObject());
      if (tex2D != NULL)
      {
         osg::Image *image = new osg::Image();
         tex2D->setImage(image);
         stateSet.setTextureAttributeAndModes(GetTextureUnit(),tex2D,osg::StateAttribute::OFF);
      }

      // do normal parameter cleanup
      ShaderParameter::DetachFromRenderState(stateSet);
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
         LoadImage();
         //SetTextureObject(*tex2D);
         ApplyTexture2DValues();
      }

      SetDirty(false);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Texture2DShaderParameter::LoadImage()
   {
      if (GetTextureSourceType() == TextureShaderParameter::TextureSourceType::IMAGE)
      {
         //Timer statsTickClock;
         //Timer_t frameTickStart = statsTickClock.Tick();

         RefPtr<osgDB::ReaderWriter::Options> options = new osgDB::ReaderWriter::Options;
         options->setObjectCacheHint(osgDB::ReaderWriter::Options::CACHE_ALL);

         std::string filePath = dtCore::FindFileInPathList(GetTexture());
         osg::Image *image = osgDB::readImageFile(filePath, options.get());

         if (image == NULL)
         {
            // we don't want to crash just because a shader couldnt find an image, but we need to let
            // the user know.
            image = new osg::Image(); // gotta have some sort of image placeholder
            LOG_ALWAYS("Could not find image for shader parameter [" + GetName() + "] at location [" + 
               GetTexture() + "].");
            //throw dtUtil::Exception(ShaderParameterException::INVALID_ATTRIBUTE,"Could not find image for texture at location: " + GetTexture());
         }

         osg::Texture2D *tex2D = static_cast<osg::Texture2D*>(GetTextureObject());
         tex2D->setImage(image);
         //tex2D->setUnRefImageDataAfterApply(true);

         // we aren't dirty anymore

         //Timer_t frameTickStop = statsTickClock.Tick();
         //double processTime = statsTickClock.DeltaSec(frameTickStart, frameTickStop);
         //printf("Load Image time [%f]", processTime);
      }

      SetImageSourceDirty(false);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Texture2DShaderParameter::ApplyTexture2DValues()
   {
      osg::Texture2D *tex2D = static_cast<osg::Texture2D*>(GetTextureObject());

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
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Texture2DShaderParameter::SetTexture(const std::string &path)
   {
      mTexturePath = path;

      LoadImage();
      ApplyTexture2DValues();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Texture2DShaderParameter::SetAddressMode(const TextureAxis &axis, const AddressMode &mode)
   {
      TextureShaderParameter::SetAddressMode(axis, mode);
      ApplyTexture2DValues();
   }

   ///////////////////////////////////////////////////////////////////////////////
   ShaderParameter *Texture2DShaderParameter::Clone() const
   {
      Texture2DShaderParameter *newParam = new Texture2DShaderParameter(GetName());

      newParam->mTextureAddressMode[0] = mTextureAddressMode[0];
      newParam->mTextureAddressMode[1] = mTextureAddressMode[1];
      newParam->mTextureAddressMode[2] = mTextureAddressMode[2];
      newParam->mTextureAddressMode[3] = mTextureAddressMode[3];
      newParam->SetTextureSourceType(GetTextureSourceType());
      newParam->SetTexture(GetTexture());
      newParam->SetTextureUnit(GetTextureUnit());
      newParam->SetImageSourceDirty(false);
      newParam->SetDirty(false);

      // no need to copy over the image. It will get loaded when you attach to render state.

      return newParam;
   }
}
