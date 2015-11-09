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
 * Erik Johnson
 */
#ifndef DELTA_TEXTURESHADERPARAMETER
#define DELTA_TEXTURESHADERPARAMETER

#include <dtCore/shaderparameter.h>
#include <dtCore/export.h>
#include <dtCore/resourcedescriptor.h>

#include <osg/Texture>

namespace dtCore
{
   /**
    * This class contains the logic and attributes necessary to contain a
    * Texture shader parameter.  A Texture parameter is a parameter
    * that contains the information and state necessary to encapsulate
    * a texture map and its associated properties.
    * @note For specialized classes see the Texture1D, Texture2D, and
    *    Texture3D subclasses.
    */
   class DT_CORE_EXPORT ShaderParamTexture : public ShaderParameter
   {
      public:

         /**
          * Specifies the different texture addressing or texture wrap
          * modes that can be assigned to this parameter.
          */
         class DT_CORE_EXPORT AddressMode : public dtUtil::Enumeration
         {
            DECLARE_ENUM(AddressMode);
            public:
               static const AddressMode CLAMP;
               static const AddressMode REPEAT;
               static const AddressMode MIRROR;
            private:
               AddressMode(const std::string &name) : dtUtil::Enumeration(name)
               {
                  AddInstance(this);
               }
         };

         /**
          * An enumeration of the four texture axis.
          */
         class DT_CORE_EXPORT TextureAxis : public dtUtil::Enumeration
         {
            DECLARE_ENUM(TextureAxis);
            public:
               static const TextureAxis S;
               static const TextureAxis T;
               static const TextureAxis R;
               static const TextureAxis Q;
            private:
               TextureAxis(const std::string &name) : dtUtil::Enumeration(name)
               {
                  AddInstance(this);
               }
         };

         /**
          * This enumeration defines the different types of sources that may be
          * used for a texture parameter.
          */
         class DT_CORE_EXPORT TextureSourceType : public dtUtil::Enumeration
         {
            DECLARE_ENUM(TextureSourceType);
            public:

               /**
                * This source type is a normal texture image located in a file
                * that can be loaded.
                */
               static const TextureSourceType IMAGE;


               /**
                * These source types represent the six different faces of a cube map.
                */
               static const TextureSourceType CUBEMAP_IMAGE_POSITIVE_X;
               static const TextureSourceType CUBEMAP_IMAGE_NEGATIVE_X;
               static const TextureSourceType CUBEMAP_IMAGE_POSITIVE_Y;
               static const TextureSourceType CUBEMAP_IMAGE_NEGATIVE_Y;
               static const TextureSourceType CUBEMAP_IMAGE_POSITIVE_Z;
               static const TextureSourceType CUBEMAP_IMAGE_NEGATIVE_Z;

               /**
                * This source type can be used for different circumstances.  For example,
                * if the texture was generated during runtime or is embedded within a particular
                * model being rendered with the shader owning a texture shader parameter.
                */
               static const TextureSourceType AUTO;

            private:
               TextureSourceType(const std::string &name) : dtUtil::Enumeration(name)
               {
                  AddInstance(this);
               }
         };

         /**
          * Constructs the shader parameter.
          * @param name The name of the shader parameter.
          * @note If this is to be used by a GLSL shader, the name
          *    must match the name of the uniform variable.
          */
         ShaderParamTexture(const std::string &name);

         /**
          * Gets the type of this parameter.
          */
         virtual const ShaderParameter::ParamType &GetType() const = 0;

         /**
          * Assigns the necessary texture states for this parameter to the
          * render state.
          */
         virtual void AttachToRenderState(osg::StateSet &stateSet) = 0;

         /**
          * Updates the texture states of the shader parameter.  This includes changes to the
          * texture source or texture addressing parameters.
          */
         virtual void Update() = 0;

         /**
          * Makes a deep copy of the Shader Parameter. Used when a user assigns
          * a shader to a node because we clone the template shader and its parameters.
          * Note - Like Update(), this is a pure virtual method that must be implemented on each param.
          */
         virtual ShaderParameter *Clone() = 0;

         /**
          * Sets the path to the texture to use for this parameter.
          * @param path The path to the texture file.  Must be relative to
          *    the current delta3d data file path.
          */
         virtual void SetTexture(const std::string &path);
         
         /**
          * Gets the current texture path string.
          * @return A string containing the path to this parameter's texture.
          */
         const std::string &GetTexture() const { return mTexturePath; }


         /**
          * Sets the texture using a project resource descriptor that will convert to a file path.
          */
         virtual void SetTextureResource(const dtCore::ResourceDescriptor& value);

         /**
          * Gets the project resource descriptor that was used to load the texture file.
          */
         dtCore::ResourceDescriptor GetTextureResource() const;

         /**
          * Sets the texture addressing mode.  This is applied when texture
          * coordinates wishing to access this texture parameter fall outside
          * of the range 0.0 - 1.0.
          * @param axis The axis enumeration to assign the address mode to.
          * @param mode The address mode enumeration to assign.
          */
         void SetAddressMode(const TextureAxis &axis, const AddressMode &mode);

         /**
          * Gets the address mode assigned to the specified texture axis.
          * @param axis The texture axis to query.
          */
         const AddressMode &GetAddressMode(const TextureAxis &axis);

         /**
          * Sets the texture unit to use for this texture parameter.
          * @param unit The texture unit.
          */
         void SetTextureUnit(unsigned int unit) { mTextureUnit = unit; SetDirty(true); }

         /**
          * Gets the texture unit currently assigned to this shader parameter.
          * @return The texture unit.
          */
         unsigned int GetTextureUnit() const { return mTextureUnit; }

         /**
          * Sets the source texture type of this parameter.
          * @param type The source type.
          * @see TextureSourceType
          * @note This does not dirty the parameter state.  If this method is called after the
          *   parameter has been assigned to a render state, it must be reassigned after calling this
          *   method to ensure the texture gets uploaded to the graphics hardware.
          */
         void SetTextureSourceType(const TextureSourceType &type) { mSourceType = &type; }

         /**
          * Gets the source texture type of this parameter.
          * @return The source type currently in use by this parameter.
          */
         const TextureSourceType &GetTextureSourceType() const { return *mSourceType; }

      protected:

         /**
          * Cleans up the parameter.
          */
         virtual ~ShaderParamTexture();

         /**
          * Sets the texture state attribute used by this shader parameter.
          * @param tex The texture state attribute to set.
          */
         void SetTextureObject(osg::Texture &tex) { mTextureObject = &tex; }

         /**
          * Gets the texture object currently assigned to this shader parameter.
          * @return The current texture object.
          */
         osg::Texture *GetTextureObject() { return mTextureObject.get(); }

         /**
          * Gets whether or not the image used by this texture has been changed.
          * @return True if the source image has been changed, false otherwise.
          */
         bool IsImageSourceDirty() const { return mImageSrcChanged; }

         /**
          * Sets whether or not the image used by this texture has changed since the
          * last update.
          * @param flag True if it has changed, false otherwise.
          */
         void SetImageSourceDirty(bool flag) { mImageSrcChanged = flag; }

      protected:
         bool mImageSrcChanged;
         unsigned int mTextureUnit;
         const AddressMode *mTextureAddressMode[4]; //s,t,r,q
         const TextureSourceType *mSourceType;
         dtCore::RefPtr<osg::Texture> mTextureObject;
         std::string mTexturePath;

         dtCore::ResourceDescriptor mDescriptor;

      private:
         // Disallowed to prevent compile errors on VS2003. It apparently
         // creates this functions even if they are not used, and if
         // this class is forward declared, these implicit functions will
         // cause compiler errors for missing calls to "ref".
         ShaderParamTexture& operator=( const ShaderParamTexture& ); 
         ShaderParamTexture( const ShaderParamTexture& );
   };
}

#endif
