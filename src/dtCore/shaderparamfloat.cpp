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
#include <dtCore/shaderparamfloat.h>
#include <osg/Uniform>
#include <osg/StateSet>

namespace dtCore
{
   ///////////////////////////////////////////////////////////////////////////////
   ShaderParamFloat::ShaderParamFloat(const std::string& name)
   : ShaderParameter(name)
   , mValue(0.0f)
   {
      SetShared(false); // floats are probably not intended to be shared by default.
   }

   ///////////////////////////////////////////////////////////////////////////////
   ShaderParamFloat::~ShaderParamFloat()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ShaderParamFloat::AttachToRenderState(osg::StateSet &stateSet)
   {
      osg::Uniform* floatUniform = NULL;

      if (IsShared())
      {
         floatUniform = GetUniformParam();
      }

      // Create a new one if unshared or if shared but not set yet
      if (floatUniform == NULL)
      {
         floatUniform = new osg::Uniform(osg::Uniform::FLOAT,GetName());
         SetUniformParam(*floatUniform);
         floatUniform->set(mValue);
      }

      stateSet.addUniform(floatUniform);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ShaderParamFloat::Update()
   {
      if (!IsDirty() || GetUniformParam() == NULL)
         return;

      GetUniformParam()->set(mValue);
      SetDirty(false);
   }

   ///////////////////////////////////////////////////////////////////////////////
   ShaderParameter* ShaderParamFloat::Clone()
   {
      ShaderParamFloat* newParam;

      // Shared params are shared at the pointer level, exactly the same. Non shared are new instances
      if (IsShared())
         newParam = this;
      else
      {
         newParam = new ShaderParamFloat(GetName());

         newParam->SetDirty(false);
         newParam->mValue = mValue;
      }

      return newParam;
   }
}
