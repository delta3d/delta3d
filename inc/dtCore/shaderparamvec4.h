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
#ifndef DELTA_SHADERPARAMVEC4
#define DELTA_SHADERPARAMVEC4

#include <dtCore/export.h>
#include <dtCore/shaderparameter.h>
#include <osg/Vec4>

namespace dtCore
{

   class DT_CORE_EXPORT ShaderParamVec4: public dtCore::ShaderParameter
   {
   public:
      /**
       * Constructs the Vec4 paremter.
       * @param name Name given to this parameter.  If it is to be used in a hardware shader,
       *   this name must match the corresponding uniform variable in the shader.
       */
      ShaderParamVec4(const std::string& name);

      /**
       * Gets the type of this parameter.
       */
      virtual const ShaderParameter::ParamType& GetType() const { return ShaderParameter::ParamType::FLOAT_VEC4; }

      /**
       * Assigns the necessary uniform variables for this parameter to the
       * render state.  This allows the parameter to communicate to the underlying
       * shader.
       */
      virtual void AttachToRenderState(osg::StateSet& stateSet);

      /**
       * Method called by the shader owning this parameter if it detects the parameter has changed state.
       * This method when called, sends the current float value in this parameter to the hardware shader.
       */
      virtual void Update();

      /**
       * Sets the value represented by this parameter.
       * @param newValue The new value to set.
       */
      void SetValue(const osg::Vec4& newValue) { mValue = newValue; SetDirty(true); }

      /**
       * Gets the current value in this parameter.
       * @return The current value.
       */
      const osg::Vec4& GetValue() const { return mValue; }

      /**
       * Makes a deep copy of the Shader Parameter. Used when a user assigns
       * a shader to a node because we clone the template shader and its parameters.
       * Note - Like Update(), this is a pure virtual method that must be implemented on each param.
       */
      virtual ShaderParameter* Clone();
   protected:
      virtual ~ShaderParamVec4();
   private:
      osg::Vec4 mValue;
   };

}

#endif /* DELTA_SHADERPARAMVEC4 */
