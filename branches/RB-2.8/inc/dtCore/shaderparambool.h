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
 * Erik Johnson
 */
#ifndef DELTA_BOOLSHADERPARAMETER
#define DELTA_BOOLSHADERPARAMETER

#include <dtCore/export.h>
#include <dtCore/shaderparameter.h>

namespace dtCore
{
   /**
    *  This class is a shader parameter containing a single boolean value.
    */
   class DT_CORE_EXPORT ShaderParamBool : public ShaderParameter
   {
      public:

         /**
          * Constructs the bool paremter.
          * @param name Name given to this parameter.  If it is to be used in a hardware shader,
          *   this name must match the corresponding uniform variable in the shader.
          */
         ShaderParamBool(const std::string &name);

         /**
          * Gets the type of this parameter.
          */
         virtual const ShaderParameter::ParamType &GetType() const { return ShaderParameter::ParamType::BOOLEAN; }

         /**
          * Assigns the necessary uniform variables for this parameter to the
          * render state.  This allows the parameter to communicate to the underlying
          * shader.
          */
         virtual void AttachToRenderState(osg::StateSet &stateSet);

         /**
          * Method called by the shader owning this parameter if it detects the parameter has changed state.
          * This method when called, sends the current float value in this parameter to the hardware shader.
          */
         virtual void Update();

         /**
          * Sets the boolean value represented by this parameter.
          * @param newValue The new value to set.
          */
         void SetValue(bool newValue) { mValue = newValue; SetDirty(true); }

         /**
          * Gets the current value in this parameter.
          * @return The current integer value.
          */
         bool GetValue() const { return mValue; }

         /**
          * Makes a deep copy of the Shader Parameter. Used when a user assigns
          * a shader to a node because we clone the template shader and its parameters.
          * Note - Like Update(), this is a pure virtual method that must be implemented on each param.
          */
         virtual ShaderParameter* Clone();

      protected:
         virtual ~ShaderParamBool();

      private:
         bool mValue;
   };
}

#endif
