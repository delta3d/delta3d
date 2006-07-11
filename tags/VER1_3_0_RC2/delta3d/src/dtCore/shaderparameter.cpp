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
#include "dtCore/shaderparameter.h"
#include "dtCore/shader.h"

#include <osg/Uniform>

namespace dtCore
{
   ///////////////////////////////////////////////////////////////////////////////
   IMPLEMENT_ENUM(ShaderParameterException);
   ShaderParameterException ShaderParameterException::INVALID_ATTRIBUTE("INVALID_ATTRIBUTE");
   ///////////////////////////////////////////////////////////////////////////////

   ///////////////////////////////////////////////////////////////////////////////
   IMPLEMENT_ENUM(ShaderParameter::ParamType);
   const ShaderParameter::ParamType ShaderParameter::ParamType::FLOAT("FLOAT");
   const ShaderParameter::ParamType ShaderParameter::ParamType::FLOAT_VEC2("FLOAT_VEC2");
   const ShaderParameter::ParamType ShaderParameter::ParamType::FLOAT_VEC3("FLOAT_VEC3");
   const ShaderParameter::ParamType ShaderParameter::ParamType::FLOAT_VEC4("FLOAT_VEC4");

   const ShaderParameter::ParamType ShaderParameter::ParamType::INT("INT");
   const ShaderParameter::ParamType ShaderParameter::ParamType::INT_VEC2("INT_VEC2");
   const ShaderParameter::ParamType ShaderParameter::ParamType::INT_VEC3("INT_VEC3");
   const ShaderParameter::ParamType ShaderParameter::ParamType::INT_VEC4("INT_VEC4");

   const ShaderParameter::ParamType ShaderParameter::ParamType::BOOLEAN("BOOLEAN");
   const ShaderParameter::ParamType ShaderParameter::ParamType::BOOLEAN_VEC2("BOOLEAN_VEC2");
   const ShaderParameter::ParamType ShaderParameter::ParamType::BOOLEAN_VEC3("BOOLEAN_VEC3");
   const ShaderParameter::ParamType ShaderParameter::ParamType::BOOLEAN_VEC4("BOOLEAN_VEC4");

   const ShaderParameter::ParamType ShaderParameter::ParamType::MATRIX2x2("MATRIX2x2");
   const ShaderParameter::ParamType ShaderParameter::ParamType::MATRIX3x3("MATRIX3x3");
   const ShaderParameter::ParamType ShaderParameter::ParamType::MATRIX4x4("MATRIX4x4");

   const ShaderParameter::ParamType ShaderParameter::ParamType::SAMPLER_1D("SAMPLER_1D");
   const ShaderParameter::ParamType ShaderParameter::ParamType::SAMPLER_2D("SAMPLER_2D");
   const ShaderParameter::ParamType ShaderParameter::ParamType::SAMPLER_3D("SAMPLER_3D");
   const ShaderParameter::ParamType ShaderParameter::ParamType::SAMPLER_CUBE("SAMPLER_CUBE");
   const ShaderParameter::ParamType ShaderParameter::ParamType::SAMPLER_SHADOW_1D("SAMPLER_SHADOW_1D");
   const ShaderParameter::ParamType ShaderParameter::ParamType::SAMPLER_SHADOW_2D("SAMPLER_SHADOW_2D");
   ///////////////////////////////////////////////////////////////////////////////

   ///////////////////////////////////////////////////////////////////////////////
   ShaderParameter::ShaderParameter(const std::string &name) : mIsDirty(false), mVarName(name), mParentShader(NULL),
      mUniform(NULL)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   ShaderParameter::~ShaderParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ShaderParameter::SetDirty(bool flag)
   {
      mIsDirty = flag;
      if (mParentShader != NULL)
         mParentShader->SetDirty(flag);
   }
   
   ///////////////////////////////////////////////////////////////////////////////
   void ShaderParameter::SetParentShader(Shader *shader)
   {
      mParentShader = shader;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ShaderParameter::SetUniformParam(osg::Uniform &uniform)
   {
      mUniform = &uniform;
   }
}
