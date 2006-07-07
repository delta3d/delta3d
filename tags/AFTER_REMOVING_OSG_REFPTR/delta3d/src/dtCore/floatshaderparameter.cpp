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
#include "dtCore/floatshaderparameter.h"
#include <osg/Uniform>
#include <osg/StateSet>

namespace dtCore
{
   ///////////////////////////////////////////////////////////////////////////////
   FloatShaderParameter::FloatShaderParameter(const std::string &name) : ShaderParameter(name)
   {
      mValue = 0.0f;
   }

   ///////////////////////////////////////////////////////////////////////////////
   FloatShaderParameter::~FloatShaderParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void FloatShaderParameter::AttachToRenderState(osg::StateSet &stateSet)
   {
      osg::Uniform *floatUniform = new osg::Uniform(osg::Uniform::FLOAT,GetName());
      SetUniformParam(*floatUniform);
      stateSet.addUniform(floatUniform);
      floatUniform->set(mValue);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void FloatShaderParameter::Update()
   {
      if (!IsDirty() || GetUniformParam() == NULL)
         return;

      GetUniformParam()->set(mValue);
      SetDirty(false);
   }

}
