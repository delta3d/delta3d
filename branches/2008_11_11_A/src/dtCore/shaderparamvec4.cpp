/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2008, Alion Science and Technology
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
 * David Guthrie
 */
#include <dtCore/shaderparamvec4.h>
#include <osg/Uniform>
#include <osg/StateSet>

namespace dtCore
{

   ///////////////////////////////////////////////////////////////////////////////
   ShaderParamVec4::ShaderParamVec4(const std::string& name)
   : ShaderParameter(name)
   , mValue(0.0f, 0.0f, 0.0f, 0.0f)
   {
      SetShared(false);
   }

   ///////////////////////////////////////////////////////////////////////////////
   ShaderParamVec4::~ShaderParamVec4()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ShaderParamVec4::AttachToRenderState(osg::StateSet& stateSet)
   {
      osg::Uniform* vec4Uniform = NULL;

      if (IsShared())
      {
         vec4Uniform = GetUniformParam();
      }

      // Create a new one if unshared or if shared but not set yet
      if (vec4Uniform == NULL)
      {
         vec4Uniform = new osg::Uniform(osg::Uniform::FLOAT_VEC4, GetName());
         SetUniformParam(*vec4Uniform);
         vec4Uniform->set(mValue);
      }

      stateSet.addUniform(vec4Uniform);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ShaderParamVec4::Update()
   {
      if (!IsDirty() || GetUniformParam() == NULL)
         return;

      GetUniformParam()->set(mValue);
      SetDirty(false);
   }

   ///////////////////////////////////////////////////////////////////////////////
   ShaderParameter* ShaderParamVec4::Clone()
   {
      ShaderParamVec4* newParam;

      // Shared params are shared at the pointer level, exactly the same. Non shared are new instances
      if (IsShared())
      {
         newParam = this;
      }
      else
      {
         newParam = new ShaderParamVec4(GetName());

         newParam->SetDirty(false);
         newParam->mValue = mValue;
      }

      return newParam;
   }

}
