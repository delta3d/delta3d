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
#ifndef DELTA_TEXTURE2DSHADERPARAMETER
#define DELTA_TEXTURE2DSHADERPARAMETER

#include <dtCore/textureshaderparameter.h>
#include <dtCore/export.h>

namespace dtCore
{
   /**
     * This class represents a 2D texture parameter.
     */
   class DT_CORE_EXPORT Texture2DShaderParameter : public TextureShaderParameter
   {
      public:

         /**
          * Constructs the texture parameter.
          */
         Texture2DShaderParameter(const std::string &name);

         /**
          * Gets the type of this shader parameter.
          * @return SAMPLER_2D.
          */
         virtual const ShaderParameter::ParamType &GetType() const
         {
            return ShaderParameter::ParamType::SAMPLER_2D;
         }

         /**
          * Assigns the 2D texture parameter to the render state.
          */
         virtual void AttachToRenderState(osg::StateSet &stateSet);

         /**
          * Updates the texture attributes attached to this shader parameter.
          */
         virtual void Update();

      protected:

         /**
          * Destroys the texture parameter.
          */
         virtual ~Texture2DShaderParameter();

      private:

         // Disallowed to prevent compile errors on VS2003. It apparently
         // creates this functions even if they are not used, and if
         // this class is forward declared, these implicit functions will
         // cause compiler errors for missing calls to "ref".
         Texture2DShaderParameter& operator=( const Texture2DShaderParameter& ); 
         Texture2DShaderParameter( const Texture2DShaderParameter& );
   };
}

#endif
