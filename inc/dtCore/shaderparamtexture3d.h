/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2009
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
#ifndef DELTA_TEXTURE3DSHADERPARAMETER
#define DELTA_TEXTURE3DSHADERPARAMETER

////////////////////////////////////////////////////////////////////////////////

#include <dtCore/shaderparamtexture.h>
#include <dtCore/export.h>

namespace dtCore
{
   /**
    * This class represents a 3D texture parameter.
    */
   class DT_CORE_EXPORT ShaderParamTexture3D : public ShaderParamTexture
   {
      public:

         /**
          * Constructs the texture parameter.
          */
         ShaderParamTexture3D(const std::string& name);

         /**
          * Gets the type of this shader parameter.
          * @return SAMPLER_3D.
          */
         virtual const ShaderParameter::ParamType& GetType() const
         {
            return ShaderParameter::ParamType::SAMPLER_3D;
         }

         /**
          * Assigns the 3D texture parameter to the render state.
          */
         virtual void AttachToRenderState(osg::StateSet& stateSet);

         /**
          * Each shader parameter has this chance to clean itself up from the
          * stateset. It should remoave whatever attributes and properties
          * that it set when it was attached.
          * @param stateSet The render state to cleanup.
          */
         virtual void DetachFromRenderState(osg::StateSet& stateSet);

         /**
          * Sets the Filters and wrap modes on the Texture3D object.
          */
         void ApplyTexture3DValues();

         /**
          * Updates the texture attributes attached to this shader parameter.
          */
         virtual void Update();

         /**
          * Sets the path to the texture to use for this parameter.
          * @param path The path to the texture file.  Must be relative to
          *    the current delta3d data file path.
          */
         virtual void SetTexture(const std::string& path);

         /**
          * Loads the image.  Called when SetTexture is called.
          */
         void LoadImage();

         /**
          * Overriden from TextureShaderParameter
          * Sets the texture addressing mode.  This is applied when texture
          * coordinates wishing to access this texture parameter fall outside
          * of the range 0.0 - 1.0.
          * @param axis The axis enumeration to assign the address mode to.
          * @param mode The address mode enumeration to assign.
          */
         void SetAddressMode(const TextureAxis& axis, const AddressMode& mode);

         /**
          * Makes a deep copy of the Shader Parameter. Used when a user assigns
          * a shader to a node because we clone the template shader and its parameters.
          * Note - Like Update(), this is a pure virtual method that must be implemented on each param.
          */
         virtual ShaderParameter* Clone();

      protected:

         /**
          * Destroys the texture parameter.
          */
         virtual ~ShaderParamTexture3D();

      private:
         // Disallowed to prevent compile errors on VS2003. It apparently
         // creates this functions even if they are not used, and if
         // this class is forward declared, these implicit functions will
         // cause compiler errors for missing calls to "ref".
         ShaderParamTexture3D& operator=(const ShaderParamTexture3D&);
         ShaderParamTexture3D(const ShaderParamTexture3D&);
   };
} // namespace dtCore

////////////////////////////////////////////////////////////////////////////////

#endif // DELTA_TEXTURE3DSHADERPARAMETER
