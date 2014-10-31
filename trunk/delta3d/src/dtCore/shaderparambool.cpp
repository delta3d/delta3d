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
#include <dtCore/shaderparambool.h>
#include <osg/Uniform>
#include <osg/StateSet>

namespace dtCore
{
   ///////////////////////////////////////////////////////////////////////////////
   ShaderParamBool::ShaderParamBool(const std::string &name) : ShaderParameter(name)
   {
      SetShared(false); // bools are probably not intended to be shared by default.
      mValue = 0;
   }

   ///////////////////////////////////////////////////////////////////////////////
   ShaderParamBool::~ShaderParamBool()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ShaderParamBool::AttachToRenderState(osg::StateSet &stateSet)
   {
      osg::Uniform* boolUniform = NULL;

      if (IsShared())
      {
         boolUniform = GetUniformParam();
      }

      // Create a new one if unshared or if shared but not set yet
      if (boolUniform == NULL)
      {
         boolUniform = new osg::Uniform(osg::Uniform::BOOL,GetName());
         SetUniformParam(*boolUniform);
         boolUniform->set(mValue);
      }

      stateSet.addUniform(boolUniform);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ShaderParamBool::Update()
   {
      if (!IsDirty() || GetUniformParam() == NULL)
         return;

      GetUniformParam()->set(mValue);
      SetDirty(false);
   }

   ///////////////////////////////////////////////////////////////////////////////
   ShaderParameter* ShaderParamBool::Clone()
   {
      ShaderParamBool* newParam;

      // Shared params are shared at the pointer level, exactly the same. Non shared are new instances
      if (IsShared())
         newParam = this;
      else
      {
         newParam = new ShaderParamBool(GetName());

         newParam->SetDirty(false);
         newParam->mValue = mValue;
      }

      return newParam;
   }
}
