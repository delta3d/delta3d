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
 * Danny J. McCue
 */

#include <prefix/dtcoreprefix-src.h>
#include <dtCore/shaderparamtexture1d.h>
#include <dtUtil/exception.h>
#include <dtCore/refptr.h>

#include <dtCore/globals.h>
#include <osg/StateSet>
#include <osg/Texture1D>
#include <osg/Uniform>
#include <osg/Image>
#include <osgDB/ReadFile>


namespace dtCore
{
   ///////////////////////////////////////////////////////////////////////////////
   ShaderParamTexture1D::ShaderParamTexture1D(const std::string& name)
      : ShaderParamTexture(name)
   {
      SetShared(true); // we want to share Textures by default
      SetTextureObject(*(new osg::Texture1D()));
   }

   ///////////////////////////////////////////////////////////////////////////////
   ShaderParamTexture1D::~ShaderParamTexture1D()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ShaderParamTexture1D::AttachToRenderState(osg::StateSet& stateSet)
   {
      dtCore::RefPtr<osg::Texture1D> tex1D;

      if (GetTexture().empty() && GetTextureSourceType() != ShaderParamTexture::TextureSourceType::AUTO)
      {
         throw dtUtil::Exception(ShaderParameterException::INVALID_ATTRIBUTE,"Cannot attach to render state.  Texture "
               "for parameter " + GetName() + " has not been specified.", __FILE__, __LINE__);
      }

      osg::Uniform* uniform = NULL;

      if (IsShared())
      {
         uniform = GetUniformParam();
      }

      // Create a new one if unshared or if shared but not set yet
      if (uniform == NULL)
      {
         uniform = new osg::Uniform(osg::Uniform::SAMPLER_1D, GetName());
         uniform->set((int)GetTextureUnit());
         SetUniformParam(*uniform);
      }

      stateSet.addUniform(uniform);

      // Load (if necessary) and Set the Tex2D on the StateSet 
      if (GetTextureSourceType() == ShaderParamTexture::TextureSourceType::IMAGE)
      {
         tex1D = static_cast<osg::Texture1D*>(GetTextureObject());

         // load or reload the image - allows caching from the template
         // Note - ImageSourceDirty may not be relevant anymore cause it now loads the image when you call SetTexture().
         // note - If shared, load only happens the first time it is assigned. 
         if (tex1D->getImage() == NULL || IsImageSourceDirty()) 
         {
            LoadImage();
            ApplyTexture1DValues();
         }

         // Assign the completed texture attribute to the render state.
         stateSet.setTextureAttributeAndModes(GetTextureUnit(), tex1D.get(), osg::StateAttribute::ON);
      }

      SetDirty(false);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ShaderParamTexture1D::DetachFromRenderState(osg::StateSet& stateSet)
   {
      osg::Texture1D* tex1D = static_cast<osg::Texture1D*>(GetTextureObject());
      if (tex1D != NULL)
      {
         if (!IsShared())
         {
            osg::Image* image = new osg::Image();
            tex1D->setImage(image);
         }
         stateSet.setTextureAttributeAndModes(GetTextureUnit(), tex1D, osg::StateAttribute::OFF);
      }

      // do normal parameter cleanup
      ShaderParameter::DetachFromRenderState(stateSet);
   }


   ///////////////////////////////////////////////////////////////////////////////
   void ShaderParamTexture1D::Update()
   {
      osg::Uniform*   uniform = GetUniformParam();
      osg::Texture1D* tex1D   = static_cast<osg::Texture1D*>(GetTextureObject());

      if (uniform == NULL)
      {
         LOG_WARNING("Could not update shader parameter: " + GetName() + " Uniform "
               "shader parameter was invalid.");
         return;
      }

      if (tex1D == NULL)
      {
         LOG_WARNING("Could not update shader parameter: " + GetName() + " Texture object "
               "was invalid.  Perhaps this is an AUTO texture parameter.");
         return;
      }

      uniform->set((int)GetTextureUnit());
      if (IsImageSourceDirty())
      {
         LoadImage();
         //SetTextureObject(*tex1D);
         ApplyTexture1DValues();
      }

      SetDirty(false);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ShaderParamTexture1D::LoadImage()
   {
      if (GetTextureSourceType() == ShaderParamTexture::TextureSourceType::IMAGE)
      {
         // Timer statsTickClock;
         // Timer_t frameTickStart = statsTickClock.Tick();

         RefPtr<osgDB::ReaderWriter::Options> options = new osgDB::ReaderWriter::Options;
         options->setObjectCacheHint(osgDB::ReaderWriter::Options::CACHE_ALL);

         std::string filePath = dtCore::FindFileInPathList(GetTexture());
         osg::Image* image = osgDB::readImageFile(filePath, options.get());

         if (image == NULL)
         {
            // we don't want to crash just because a shader couldnt find an image, but we need to let
            // the user know.
            image = new osg::Image(); // gotta have some sort of image placeholder
            LOG_ALWAYS("Could not find image for shader parameter [" + GetName() + "] at location [" + 
               GetTexture() + "].");
            //throw dtUtil::Exception(ShaderParameterException::INVALID_ATTRIBUTE,"Could not find image for texture at location: " + GetTexture());
         }

         osg::Texture1D* tex1D = static_cast<osg::Texture1D*>(GetTextureObject());
         tex1D->setImage(image);

         // we aren't dirty anymore

         //Timer_t frameTickStop = statsTickClock.Tick();
         //double processTime = statsTickClock.DeltaSec(frameTickStart, frameTickStop);
         //printf("Load Image time [%f]", processTime);
      }

      SetImageSourceDirty(false);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ShaderParamTexture1D::ApplyTexture1DValues()
   {
      osg::Texture1D* tex1D = static_cast<osg::Texture1D*>(GetTextureObject());

      // These need to be in the XML definition at some point.
      tex1D->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
      tex1D->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);

      // Set the texture addressing...
      const ShaderParamTexture::AddressMode& wrapModeS =
         GetAddressMode(ShaderParamTexture::TextureAxis::S);
      if (wrapModeS == ShaderParamTexture::AddressMode::CLAMP)
         tex1D->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
      else if (wrapModeS == ShaderParamTexture::AddressMode::REPEAT)
         tex1D->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
      else if (wrapModeS == ShaderParamTexture::AddressMode::MIRROR)
         tex1D->setWrap(osg::Texture::WRAP_S, osg::Texture::MIRROR);

      const ShaderParamTexture::AddressMode& wrapModeT =
         GetAddressMode(ShaderParamTexture::TextureAxis::T);
      if (wrapModeT == ShaderParamTexture::AddressMode::CLAMP)
         tex1D->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
      else if (wrapModeT == ShaderParamTexture::AddressMode::REPEAT)
         tex1D->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
      else if (wrapModeT == ShaderParamTexture::AddressMode::MIRROR)
         tex1D->setWrap(osg::Texture::WRAP_T, osg::Texture::MIRROR);

      const ShaderParamTexture::AddressMode& wrapModeR =
         GetAddressMode(ShaderParamTexture::TextureAxis::R);
      if (wrapModeR == ShaderParamTexture::AddressMode::CLAMP)
         tex1D->setWrap(osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_EDGE);
      else if (wrapModeR == ShaderParamTexture::AddressMode::REPEAT)
         tex1D->setWrap(osg::Texture::WRAP_R, osg::Texture::REPEAT);
      else if (wrapModeR == ShaderParamTexture::AddressMode::MIRROR)
         tex1D->setWrap(osg::Texture::WRAP_R, osg::Texture::MIRROR);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ShaderParamTexture1D::SetTexture(const std::string& path)
   {
      mTexturePath = path;

      LoadImage();
      ApplyTexture1DValues();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ShaderParamTexture1D::SetAddressMode(const TextureAxis& axis, const AddressMode& mode)
   {
      ShaderParamTexture::SetAddressMode(axis, mode);
      ApplyTexture1DValues();
   }

   ///////////////////////////////////////////////////////////////////////////////
   ShaderParameter *ShaderParamTexture1D::Clone()
   {
      ShaderParamTexture1D *newParam;

      // Shared params are shared at the pointer level, exactly the same. Non shared are new instances
      if (IsShared())
      {
         newParam = this;
      }
      else
      {
         newParam = new ShaderParamTexture1D(GetName());

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
      }

      return newParam;
   }
}
