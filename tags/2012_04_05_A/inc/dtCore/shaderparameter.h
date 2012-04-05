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
#ifndef DELTA_SHADERPARAMETER
#define DELTA_SHADERPARAMETER

#include <dtCore/export.h>
#include <dtUtil/enumeration.h>
#include <dtUtil/exception.h>
#include <dtCore/refptr.h>
#include <osg/Referenced>
#include <string>
#include <dtCore/base.h>
 
/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace osg
{
   class StateSet;
   class Uniform;
}
/// @endcond

namespace dtCore
{
   class ShaderProgram;

   class ShaderParameterInvalidAttributeException : public dtUtil::Exception
   {
   public:
   	ShaderParameterInvalidAttributeException(const std::string& message, const std::string& filename, unsigned int linenum);
   	virtual ~ShaderParameterInvalidAttributeException() {};
   };
   

   /**
    * This class wraps parameters that may be passed on to Delta3D
    * shaders.  These are similar in concept to shader uniform's
    * however, they can potentially contain much more information about
    * the actual render state.
    */
   class DT_CORE_EXPORT ShaderParameter : public Base //osg::Referenced
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

               static const ParamType TIMER_FLOAT;

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
         ShaderParameter(const std::string& name);

         /**
          * Gets the type of this shader parameter.
          * @return The enumeration defining the type of shader parameter a subclass
          *    of this one represents.
          */
         virtual const ParamType& GetType() const = 0;

         /**
          * Each shader parameter should add whatever attributes and properties
          * specific to that parameter to the specified render state.
          * @param stateSet The render state to attach the parameter to.
          */
         virtual void AttachToRenderState(osg::StateSet& stateSet) = 0;

         /**
          * Each shader parameter has this chance to clean itself up from the
          * stateset. It should remoave whatever attributes and properties
          * that it set when it was attached.
          * @param stateSet The render state to cleanup.
          */
         virtual void DetachFromRenderState(osg::StateSet& stateSet);

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

         /**
          * Sets whether this parameter is shared or not. When a shader is assigned to node 
          * with ShaderManager::AssignShaderFromTemplate, the shared flag indicates if the 
          * templates parameter should be cloned or 'shared'.  A Shared parameter means that multiple
          * nodes will a reference to exactly the same parameter.  It makes the parameter global and 
          * prevents multiple memory allocations and facilitates State Sorting.  
          * @param shared Changes the template shared flag value - shared params are not cloned when assigning.
          * @note Each param type has it's own default for this. Most default to true (base class default). 
          */
         void SetShared(bool shared) { mIsShared = shared; }

         /**
          * Indicates whether this parameter is shared or not. When a shader is assigned to node 
          * with ShaderManager::AssignShaderFromTemplate, the shared flag indicates if the 
          * templates parameter should be cloned or 'shared'.  A Shared parameter means that multiple
          * nodes will a reference to exactly the same parameter.  It makes the parameter global and 
          * prevents multiple memory allocations and facilitates State Sorting.  
          * @return Whether the param is 'shared' or not - shared params are not cloned when assigning.
          * @note Each param type has it's own default for this. Most default to true (base class default). 
          */
         bool IsShared() const { return mIsShared; }

         /**
          * Makes a deep copy of the Shader Parameter. Used when a user assigns
          * a shader to a node because we clone the template shader and its parameters.
          * Note - Like Update(), this is a pure virtual method that must be implemented on each param.
          */
         virtual ShaderParameter* Clone() = 0;

      protected:

         /**
          * Destroys the shader parameter.
          */
         virtual ~ShaderParameter();

         /**
          * Sets the shader owning this shader parameter.
          * @param shader The shader owning this parameter.
          */
        void SetParentShader(ShaderProgram* shader);

         /**
          * Gets the shader currently managing this shader parameter.
          * @return The parent shader.
          */
         ShaderProgram* GetParentShader() const { return mParentShader; }

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
         void SetUniformParam(osg::Uniform& uniform);

         /**
          * Gets the uniform parameter currently bound to this shader parameter.
          * @return A pointer to the uniform or NULL if it does not exist.
          */
         osg::Uniform* GetUniformParam() { return mUniform.get(); }

      private:
         bool mIsDirty;
         bool mIsShared; // Default is true. Indicates that when Cloning, it should simply return a copy of this param, not a new instance
         ShaderProgram* mParentShader;
         dtCore::RefPtr<osg::Uniform> mUniform;

         friend class ShaderProgram;

         // Disallowed to prevent compile errors on VS2003. It apparently
         // creates this functions even if they are not used, and if
         // this class is forward declared, these implicit functions will
         // cause compiler errors for missing calls to "ref".
         ShaderParameter& operator=(const ShaderParameter&);
         ShaderParameter(const ShaderParameter&);
   };
}

#endif
