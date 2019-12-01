/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
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
 */

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtCore/shaderparameteractorproperty.h>



namespace dtCore
{
   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   ShaderPropertyBuilder::ShaderPropertyBuilder()
   {}

   ////////////////////////////////////////////////////////////////////////////////
   ShaderPropertyBuilder::~ShaderPropertyBuilder()
   {}

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::DataType* ShaderPropertyBuilder::GetDataTypeForParam(dtCore::ShaderParameter& param)
   {
      dtCore::DataType* dataType = NULL;

      typedef dtCore::ShaderParameter::ParamType ParamType;
      const ParamType* paramType = &param.GetType();

      if (paramType == &ParamType::INT)
      {
         dataType = &dtCore::DataType::INT;
      }
      else if (paramType == &ParamType::FLOAT)
      {
         dataType = &dtCore::DataType::FLOAT;
      }
      else if (paramType == &ParamType::FLOAT_VEC4)
      {
         dataType = &dtCore::DataType::VEC4;
      }
      else if (paramType == &ParamType::SAMPLER_1D
         || paramType == &ParamType::SAMPLER_2D
         || paramType == &ParamType::SAMPLER_3D
         || paramType == &ParamType::SAMPLER_CUBE)
      {
         dataType = &dtCore::DataType::TEXTURE;
      }

      // TODO:
      // Add support for the following types.
      /*
      static const ParamType FLOAT_VEC2;
      static const ParamType FLOAT_VEC3;

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

      static const ParamType SAMPLER_SHADOW_1D;
      static const ParamType SAMPLER_SHADOW_2D;

      static const ParamType TIMER_FLOAT;*/

      return dataType;
   }

   template<typename T_ShaderPropType>
   dtCore::RefPtr<T_ShaderPropType> CreateProperty(
      const dtUtil::RefString& name,
      dtCore::ShaderParameter* param = NULL,
      const dtUtil::RefString& label = "",
      const dtUtil::RefString& desc = "",
      const dtUtil::RefString& groupName = "")
   {
      // Ensure the parameter is a valid instance.
      dtCore::RefPtr<typename T_ShaderPropType::ParamType> actualParam
         = dynamic_cast<typename T_ShaderPropType::ParamType*>(param);
      if ( ! actualParam.valid())
      {
         actualParam = new typename T_ShaderPropType::ParamType(name);
         param = actualParam.get();

         if (param != NULL)
         {
            LOG_WARNING("Could not cast shader parameter \""
               + param->GetName() + "\" [" + param->GetType().GetName()
               + "] to appropriate type [" + actualParam->GetType().GetName() + "]");
         }
      }

      // Try to ensure a valid name.
      std::string nameValue(name);
      if (nameValue.empty())
      {
         nameValue = param->GetName();
      }

      // Try to ensure a valid label
      std::string labelValue(label);
      if (label.Get().empty())
      {
         labelValue = name;
      }

      dtCore::RefPtr<T_ShaderPropType> prop
         = new T_ShaderPropType(actualParam.get(), nameValue, labelValue, desc, groupName);

      return prop;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<dtCore::ActorProperty>
      ShaderPropertyBuilder::CreateShaderProperty(dtCore::ShaderParameter& param)
   {
      dtCore::RefPtr<dtCore::ActorProperty> prop;
      dtUtil::RefString propName = param.GetName();

      using namespace dtCore;
      typedef dtCore::ShaderParameter::ParamType ParamType;
      const ParamType* paramType = &param.GetType();

      if (paramType == &ParamType::INT)
      {
         prop = CreateProperty<ShaderParamIntProperty>(propName, &param);
      }
      else if (paramType == &ParamType::FLOAT)
      {
         prop = CreateProperty<ShaderParamFloatProperty>(propName, &param);
      }
      else if (paramType == &ParamType::FLOAT_VEC4)
      {
         prop = CreateProperty<ShaderParamVec4Property>(propName, &param);
      }
      else if (paramType == &ParamType::SAMPLER_1D)
      {
         prop = CreateProperty<ShaderParamTexture1DProperty>(propName, &param);
      }
      else if (paramType == &ParamType::SAMPLER_2D)
      {
         prop = CreateProperty<ShaderParamTexture2DProperty>(propName, &param);
      }
      else if (paramType == &ParamType::SAMPLER_3D)
      {
         prop = CreateProperty<ShaderParamTexture3DProperty>(propName, &param);
      }
      else if (paramType == &ParamType::SAMPLER_CUBE)
      {
         prop = CreateProperty<ShaderParamTextureCubeMapProperty>(propName, &param);
      }

      return prop;
   }

   ////////////////////////////////////////////////////////////////////////////////
   int ShaderPropertyBuilder::CreateShaderPropertiesForContainer(
      dtCore::ShaderProgram& shaderProgram, dtCore::PropertyContainer& outPropertyContainer)
   {
      int propCount = 0;

      typedef std::vector<dtCore::RefPtr<dtCore::ShaderParameter> > ParamList;
      ParamList params;

      shaderProgram.GetParameterList(params);
      if ( ! params.empty())
      {
         dtCore::DataType* curType = NULL;
         dtCore::ShaderParameter* curParam = NULL;
         ParamList::iterator curIter = params.begin();
         ParamList::iterator endIter = params.end();
         for ( ; curIter != endIter; ++curIter)
         {
            curParam = curIter->get();
            curType = GetDataTypeForParam(*curParam);

            if (curType == NULL)
            {
               LOG_WARNING("No matching dtCore::DataType for shader parameter \"" +
                  curParam->GetName() + "\" of type ["
                  + curParam->GetType().GetName() + "]. Cannot map to appropriate UI control.");
            }
            else
            {
               std::string propName = curParam->GetName();
               if (NULL != outPropertyContainer.GetProperty(propName))
               {
                  LOG_ERROR("Cannot add shader actor property \"" + propName
                     + "\" since it conflicts with an existing property of the same name.");
               }
               else
               {
                  dtCore::RefPtr<dtCore::ActorProperty> prop = CreateShaderProperty(*curParam);

                  outPropertyContainer.AddProperty(prop.get());

                  ++propCount;
               }
            }
         }
      }

      return propCount;
   }

}
