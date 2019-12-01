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

#include <prefix/dtcoreprefix.h>
#include <dtCore/shaderparamtexture2d.h>
#include <dtUtil/datapathutils.h>
#include <dtUtil/exception.h>
#include <dtCore/refptr.h>

#include <osg/StateSet>
#include <osg/Texture2D>
#include <osg/Uniform>
#include <osg/Image>
#include <osgDB/ReadFile>


namespace dtCore
{
   ///////////////////////////////////////////////////////////////////////////////
   ShaderParamTexture2D::ShaderParamTexture2D(const std::string &name) :
      ShaderParamTexture(name)
   {
      SetShared(true); // we want to share Textures by default
      SetTextureObject(*(new osg::Texture2D()));
   }

   ///////////////////////////////////////////////////////////////////////////////
   ShaderParamTexture2D::~ShaderParamTexture2D()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ShaderParamTexture2D::AttachToRenderState(osg::StateSet &stateSet)
   {
      dtCore::RefPtr<osg::Texture2D> tex2D;

      if (GetTexture().empty() && GetTextureSourceType() != ShaderParamTexture::TextureSourceType::AUTO)
         throw dtCore::ShaderParameterInvalidAttributeException("Cannot attach to render state.  Texture "
               "for parameter " + GetName() + " has not been specified.", __FILE__, __LINE__);

      osg::Uniform *uniform = NULL;

      if (IsShared())
         uniform = GetUniformParam();
      // Create a new one if unshared or if shared but not set yet
      if (uniform == NULL)
      {
         uniform = new osg::Uniform(osg::Uniform::SAMPLER_2D,GetName());
         uniform->set((int)GetTextureUnit());
         SetUniformParam(*uniform);
      }

      stateSet.addUniform(uniform);

      // Load (if necessary) and Set the Tex2D on the StateSet 
      if (GetTextureSourceType() == ShaderParamTexture::TextureSourceType::IMAGE)
      {
         tex2D = static_cast<osg::Texture2D*>(GetTextureObject());

         // load or reload the image - allows caching from the template
         // Note - ImageSourceDirty may not be relevant anymore cause it now loads the image when you call SetTexture().
         // note - If shared, load only happens the first time it is assigned. 
         if (tex2D->getImage() == NULL || IsImageSourceDirty()) 
         {
            LoadImage();
            ApplyTexture2DValues();
         }

         //Assign the completed texture attribute to the render state.
         stateSet.setTextureAttributeAndModes(GetTextureUnit(), tex2D.get(), osg::StateAttribute::ON);
      }

      SetDirty(false);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ShaderParamTexture2D::DetachFromRenderState(osg::StateSet &stateSet)
   {
      osg::Texture2D *tex2D = static_cast<osg::Texture2D*>(GetTextureObject());
      if (tex2D != NULL)
      {
         if (!IsShared())
         {
            osg::Image *image = new osg::Image();
            tex2D->setImage(image);
         }
         stateSet.setTextureAttributeAndModes(GetTextureUnit(),tex2D,osg::StateAttribute::OFF);
      }

      // do normal parameter cleanup
      ShaderParameter::DetachFromRenderState(stateSet);
   }


   ///////////////////////////////////////////////////////////////////////////////
   void ShaderParamTexture2D::Update()
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
   void ShaderParamTexture2D::LoadImage()
   {
      if (GetTextureSourceType() == ShaderParamTexture::TextureSourceType::IMAGE)
      {
         //Timer statsTickClock;
         //Timer_t frameTickStart = statsTickClock.Tick();

         RefPtr<osgDB::ReaderWriter::Options> options = new osgDB::ReaderWriter::Options;
         options->setObjectCacheHint(osgDB::ReaderWriter::Options::CACHE_ALL);

         std::string filePath = dtUtil::FindFileInPathList(GetTexture());
         osg::Image *image = osgDB::readImageFile(filePath, options.get());

         if (image == NULL)
         {
            // we don't want to crash just because a shader couldnt find an image, but we need to let
            // the user know.
            image = new osg::Image(); // gotta have some sort of image placeholder
            LOG_ALWAYS("Could not find image for shader parameter [" + GetName() + "] at location [" + 
               GetTexture() + "].");
            //throw dtCore::ShaderParameterInvalidAttributeException("Could not find image for texture at location: " + GetTexture());
         }

         osg::Texture2D *tex2D = static_cast<osg::Texture2D*>(GetTextureObject());
         tex2D->setImage(image);

         // we aren't dirty anymore

         //Timer_t frameTickStop = statsTickClock.Tick();
         //double processTime = statsTickClock.DeltaSec(frameTickStart, frameTickStop);
         //printf("Load Image time [%f]", processTime);
      }

      SetImageSourceDirty(false);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ShaderParamTexture2D::ApplyTexture2DValues()
   {
      osg::Texture2D *tex2D = static_cast<osg::Texture2D*>(GetTextureObject());

      //These need to be in the XML definition at some point.
      tex2D->setFilter(osg::Texture::MIN_FILTER,osg::Texture::LINEAR_MIPMAP_LINEAR);
      tex2D->setFilter(osg::Texture::MAG_FILTER,osg::Texture::LINEAR);

      //Set the texture addressing...
      const ShaderParamTexture::AddressMode &wrapModeS =
         GetAddressMode(ShaderParamTexture::TextureAxis::S);
      if (wrapModeS == ShaderParamTexture::AddressMode::CLAMP)
         tex2D->setWrap(osg::Texture::WRAP_S,osg::Texture::CLAMP_TO_EDGE);
      else if (wrapModeS == ShaderParamTexture::AddressMode::REPEAT)
         tex2D->setWrap(osg::Texture::WRAP_S,osg::Texture::REPEAT);
      else if (wrapModeS == ShaderParamTexture::AddressMode::MIRROR)
         tex2D->setWrap(osg::Texture::WRAP_S,osg::Texture::MIRROR);

      const ShaderParamTexture::AddressMode &wrapModeT =
         GetAddressMode(ShaderParamTexture::TextureAxis::T);
      if (wrapModeT == ShaderParamTexture::AddressMode::CLAMP)
         tex2D->setWrap(osg::Texture::WRAP_T,osg::Texture::CLAMP_TO_EDGE);
      else if (wrapModeT == ShaderParamTexture::AddressMode::REPEAT)
         tex2D->setWrap(osg::Texture::WRAP_T,osg::Texture::REPEAT);
      else if (wrapModeT == ShaderParamTexture::AddressMode::MIRROR)
         tex2D->setWrap(osg::Texture::WRAP_T,osg::Texture::MIRROR);

      const ShaderParamTexture::AddressMode &wrapModeR =
         GetAddressMode(ShaderParamTexture::TextureAxis::R);
      if (wrapModeR == ShaderParamTexture::AddressMode::CLAMP)
         tex2D->setWrap(osg::Texture::WRAP_R,osg::Texture::CLAMP_TO_EDGE);
      else if (wrapModeR == ShaderParamTexture::AddressMode::REPEAT)
         tex2D->setWrap(osg::Texture::WRAP_R,osg::Texture::REPEAT);
      else if (wrapModeR == ShaderParamTexture::AddressMode::MIRROR)
         tex2D->setWrap(osg::Texture::WRAP_R,osg::Texture::MIRROR);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ShaderParamTexture2D::SetTexture(const std::string &path)
   {
      mTexturePath = path;

      LoadImage();
      ApplyTexture2DValues();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ShaderParamTexture2D::SetAddressMode(const TextureAxis &axis, const AddressMode &mode)
   {
      ShaderParamTexture::SetAddressMode(axis, mode);
      ApplyTexture2DValues();
   }

   ///////////////////////////////////////////////////////////////////////////////
   ShaderParameter *ShaderParamTexture2D::Clone()
   {
      ShaderParamTexture2D *newParam;

      // Shared params are shared at the pointer level, exactly the same. Non shared are new instances
      if (IsShared())
         newParam = this;
      else
      {
         newParam = new ShaderParamTexture2D(GetName());

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
