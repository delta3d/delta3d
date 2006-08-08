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
#ifndef DELTA_SHADERPARAMETER
#define DELTA_SHADERPARAMETER

#include <dtCore/export.h>
#include <dtUtil/enumeration.h>
#include <dtCore/refptr.h>
#include <osg/Referenced>
#include <string>

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace osg
{
   class StateSet;
   class Uniform;
}
/// @endcond

namespace dtCore
{
   class Shader;

   /**
    * Exceptions a shader parameter may throw.
    */
   class DT_CORE_EXPORT ShaderParameterException : public dtUtil::Enumeration
   {
      DECLARE_ENUM(ShaderParameterException);
      public:

         ///Throw if a parameter is applied to the rendering state when one or
         ///more of its attributes are invalid.
         static ShaderParameterException INVALID_ATTRIBUTE;

      private:
         ShaderParameterException(const std::string &name) : dtUtil::Enumeration(name)
         {
            AddInstance(this);
         }
   };

   /**
    * This class wraps parameters that may be passed on to Delta3D
    * shaders.  These are similar in concept to shader uniform's
    * however, they can potential contain must more information about
    * the actual render state.
    */
   class DT_CORE_EXPORT ShaderParameter : public osg::Referenced
   {
      public:

         /**
          * This enumeration contains the different types of shader parameters
          * that may exist on a Delta3D shader.
          */
         class DT_CORE_EXPORT ParamType : public dtUtil::Enumeration
         {
            DECLARE_ENUM(ParamType);
            public:

               static const ParamType FLOAT;
               static const ParamType FLOAT_VEC2;
               static const ParamType FLOAT_VEC3;
               static const ParamType FLOAT_VEC4;

               static const ParamType INT;
               static const ParamType INT_VEC2;
               static const ParamType INT_VEC3;
               static const ParamType INT_VEC4;

               static const ParamType BOOLEAN;
               static const ParamType BOOLEAN_VEC2;
               static const ParamType BOOLEAN_VEC3;
               static const ParamType BOOLEAN_VEC4;

               static const ParamType MATRIX2x2;
               static const ParamType MATRIX3x3;
               static const ParamType MATRIX4x4;

               static const ParamType SAMPLER_1D;
               static const ParamType SAMPLER_2D;
               static const ParamType SAMPLER_3D;
               static const ParamType SAMPLER_CUBE;
               static const ParamType SAMPLER_SHADOW_1D;
               static const ParamType SAMPLER_SHADOW_2D;

            protected:
               ParamType(const std::string &name) : dtUtil::Enumeration(name)
               {
                  AddInstance(this);
               }
         };

         /**
          * Constructs the shader parameter.
          * @param name The name assigned to this shader parameter.
          * @note If this parameter is used by a GLSL shader as a uniform variable,
          *    the name of the parameter must be the same as the desired uniform
          *    it is mapped to in order for the mapping to occur correctly.
          */
         ShaderParameter(const std::string &name);

         /**
          * Gets the type of this shader parameter.
          * @return The enumeration defining the type of shader parameter a subclass
          *    of this one represents.
          */
         virtual const ParamType &GetType() const = 0;

         /**
          * Gets the name assigned to this shader parameter.
          * @return The name of this parameter.
          */
         const std::string &GetName() const { return mVarName; }

         /**
          * Each shader parameter should add whatever attributes and properties
          * specific to that parameter to the specified render state.
          * @param stateSet The render state to attach the parameter to.
          */
         virtual void AttachToRenderState(osg::StateSet &stateSet) = 0;

         /**
          * Instructs the parameter to push and dirty state to the shader it is
          * connected to.
          * @note This method is called automatically by this parameter's parent
          *  shader if the parameter has been marked dirty.
          */
         virtual void Update() = 0;

         /**
          * This method signals whether or not the shader parameter has changed state
          * since the last update.
          * @param flag True if state has changed, false to clear the dirty bit.
          */
         void SetDirty(bool flag);

         /**
          * Gets whether or not the parameter has changed state since the last update.
          * @return True if state has changed, false otherwise.
          */
         bool IsDirty() const { return mIsDirty; }

      protected:

         /**
          * Destroys the shader parameter.
          */
         virtual ~ShaderParameter();

         /**
          * Sets the shader owning this shader parameter.
          * @param shader The shader owning this parameter.
          */
        void SetParentShader(Shader *shader);

         /**
          * Gets the shader currently managing this shader parameter.
          * @return The parent shader.
          */
         Shader *GetParentShader() const { return mParentShader; }

         /**
          * Sets the shader uniform parameter for this shader parameter.  Uniform parameters
          * are used to communicate the actual data stored within this logical shader
          * parameter to the actual vertex or fragment shader.
          * @param uniform The uniform to set.
          * @note Shader parameters are not required to bind a uniform if there is no need
          *   for it.  Other parameter implementations may wish to add additional uniform
          *   values depending on the complexity of the underlying data of a particular parameter
          *   implementation.
          */
         void SetUniformParam(osg::Uniform &uniform);

         /**
          * Gets the uniform parameter currently bound to this shader parameter.
          * @return A pointer to the uniform or NULL if it does not exist.
          */
         osg::Uniform *GetUniformParam() { return mUniform.get(); }

      private:
         bool mIsDirty;
         std::string mVarName;
         Shader *mParentShader;
         dtCore::RefPtr<osg::Uniform> mUniform;

         friend class Shader;

         // Disallowed to prevent compile errors on VS2003. It apparently
         // creates this functions even if they are not used, and if
         // this class is forward declared, these implicit functions will
         // cause compiler errors for missing calls to "ref".
         ShaderParameter& operator=( const ShaderParameter& ); 
         ShaderParameter( const ShaderParameter& );
   };
}

#endif
