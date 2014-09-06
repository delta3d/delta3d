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

#ifndef SHADER_PARAMETER_ACTOR_PROPERTY
#define SHADER_PARAMETER_ACTOR_PROPERTY

#include <dtCore/export.h>
#include <dtCore/floatactorproperty.h>
#include <dtCore/intactorproperty.h>
#include <dtCore/propertycontainer.h>
#include <dtCore/refptr.h>
#include <dtCore/resourceactorproperty.h>
#include <dtCore/shaderparamfloat.h>
#include <dtCore/shaderparamint.h>
#include <dtCore/shaderparamoscillator.h>
#include <dtCore/shaderparamvec4.h>
#include <dtCore/shaderparamtexture1d.h>
#include <dtCore/shaderparamtexture2d.h>
#include <dtCore/shaderparamtexture3d.h>
#include <dtCore/shaderparamtexturecubemap.h>
#include <dtCore/shaderprogram.h>
#include <dtCore/vectoractorproperties.h>
#include <dtUtil/log.h>
#include <dtUtil/refstring.h>

namespace dtCore
{
   ////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////
   template<class T_ShaderParam, typename T_BaseActorProperty>
   class ShaderParameterActorProperty : public T_BaseActorProperty
   {
   public:
      typedef T_BaseActorProperty BaseClass;
      typedef typename BaseClass::SetFuncType SetFuncType;
      typedef typename BaseClass::GetFuncType GetFuncType;
      typedef T_ShaderParam ParamType;
      typedef ShaderParameterActorProperty<T_ShaderParam, T_BaseActorProperty> ClassType;
   
      static dtCore::RefPtr<ClassType> CreateProperty(const dtUtil::RefString& name,
                        typename ClassType::ParamType* param = NULL,
                        const dtUtil::RefString& label = "",
                        const dtUtil::RefString& desc = "",
                        const dtUtil::RefString& groupName = "")
      {
         // Ensure the parameter is a valid instance.
         dtCore::RefPtr<ClassType::ParamType> actualParam = param;
         if ( ! actualParam.valid())
         {
            actualParam = new ClassType::ParamType(name);
            param = actualParam.get();
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

         dtCore::RefPtr<ClassType> prop
            = new ClassType(param, nameValue, labelValue, desc, groupName);

         return prop;
      }


      ShaderParameterActorProperty(T_ShaderParam* param,
                        const dtUtil::RefString& name,
                        const dtUtil::RefString& label,
                        const dtUtil::RefString& desc = "",
                        const dtUtil::RefString& groupName = "");

      T_ShaderParam* GetShaderParam()
      {
         return mParam.get();
      }

      const T_ShaderParam* GetShaderParam() const
      {
         return mParam.get();
      }

   protected:
      virtual ~ShaderParameterActorProperty() {}

      ///Maximum length the string for this string property can be.
      dtCore::RefPtr<T_ShaderParam> mParam;
   };



   // Constructor for primitive types.
   template<class T_ShaderParam, typename T_BaseActorProperty>
   ShaderParameterActorProperty<T_ShaderParam, T_BaseActorProperty>::ShaderParameterActorProperty(
      T_ShaderParam* param,
      const dtUtil::RefString& name,
      const dtUtil::RefString& label,
      const dtUtil::RefString& desc,
      const dtUtil::RefString& groupName)
      : BaseClass(name, label,
         typename T_BaseActorProperty::SetFuncType(param, &T_ShaderParam::SetValue),
         typename T_BaseActorProperty::GetFuncType(param, &T_ShaderParam::GetValue),
         desc, groupName)
      , mParam(param)
   {}

   // Constructor for resource types
   template<>
   ShaderParameterActorProperty<dtCore::ShaderParamTexture1D, dtCore::ResourceActorProperty>::ShaderParameterActorProperty(
      dtCore::ShaderParamTexture1D* param,
      const dtUtil::RefString& name,
      const dtUtil::RefString& label,
      const dtUtil::RefString& desc,
      const dtUtil::RefString& groupName)
      : BaseClass(dtCore::DataType::TEXTURE, name, label,
         dtCore::ResourceActorProperty::SetDescFuncType(param, &dtCore::ShaderParamTexture1D::SetTextureResource),
         dtCore::ResourceActorProperty::GetDescFuncType(param, &dtCore::ShaderParamTexture1D::GetTextureResource),
         desc, groupName)
      , mParam(param)
   {}
   
   template<>
   ShaderParameterActorProperty<dtCore::ShaderParamTexture2D, dtCore::ResourceActorProperty>::ShaderParameterActorProperty(
      dtCore::ShaderParamTexture2D* param,
      const dtUtil::RefString& name,
      const dtUtil::RefString& label,
      const dtUtil::RefString& desc,
      const dtUtil::RefString& groupName)
      : BaseClass(dtCore::DataType::TEXTURE, name, label,
         dtCore::ResourceActorProperty::SetDescFuncType(param, &dtCore::ShaderParamTexture2D::SetTextureResource),
         dtCore::ResourceActorProperty::GetDescFuncType(param, &dtCore::ShaderParamTexture2D::GetTextureResource),
         desc, groupName)
      , mParam(param)
   {}

   template<>
   ShaderParameterActorProperty<dtCore::ShaderParamTexture3D, dtCore::ResourceActorProperty>::ShaderParameterActorProperty(
      dtCore::ShaderParamTexture3D* param,
      const dtUtil::RefString& name,
      const dtUtil::RefString& label,
      const dtUtil::RefString& desc,
      const dtUtil::RefString& groupName)
      : BaseClass(dtCore::DataType::TEXTURE, name, label,
         dtCore::ResourceActorProperty::SetDescFuncType(param, &dtCore::ShaderParamTexture3D::SetTextureResource),
         dtCore::ResourceActorProperty::GetDescFuncType(param, &dtCore::ShaderParamTexture3D::GetTextureResource),
         desc, groupName)
      , mParam(param)
   {}

   template<>
   ShaderParameterActorProperty<dtCore::ShaderParamTextureCubeMap, dtCore::ResourceActorProperty>::ShaderParameterActorProperty(
      dtCore::ShaderParamTextureCubeMap* param,
      const dtUtil::RefString& name,
      const dtUtil::RefString& label,
      const dtUtil::RefString& desc,
      const dtUtil::RefString& groupName)
      : BaseClass(dtCore::DataType::TEXTURE, name, label,
         dtCore::ResourceActorProperty::SetDescFuncType(param, &dtCore::ShaderParamTextureCubeMap::SetTextureResource),
         dtCore::ResourceActorProperty::GetDescFuncType(param, &dtCore::ShaderParamTextureCubeMap::GetTextureResource),
         desc, groupName)
      , mParam(param)
   {}



   ////////////////////////////////////////////////////////////////////////////
   // TYPEDEFFINITIONS
   ////////////////////////////////////////////////////////////////////////////
   typedef ShaderParameterActorProperty<dtCore::ShaderParamFloat, dtCore::FloatActorProperty> ShaderParamFloatProperty;
   typedef ShaderParameterActorProperty<dtCore::ShaderParamInt, dtCore::IntActorProperty> ShaderParamIntProperty;
   typedef ShaderParameterActorProperty<dtCore::ShaderParamVec4, dtCore::Vec4ActorProperty> ShaderParamVec4Property;
   typedef ShaderParameterActorProperty<dtCore::ShaderParamOscillator, dtCore::FloatActorProperty> ShaderParamOscillatorProperty;
   typedef ShaderParameterActorProperty<dtCore::ShaderParamTexture1D, dtCore::ResourceActorProperty> ShaderParamTexture1DProperty;
   typedef ShaderParameterActorProperty<dtCore::ShaderParamTexture2D, dtCore::ResourceActorProperty> ShaderParamTexture2DProperty;
   typedef ShaderParameterActorProperty<dtCore::ShaderParamTexture3D, dtCore::ResourceActorProperty> ShaderParamTexture3DProperty;
   typedef ShaderParameterActorProperty<dtCore::ShaderParamTextureCubeMap, dtCore::ResourceActorProperty> ShaderParamTextureCubeMapProperty;

   
   
   ////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////
   /**
    * Utility class for creating shader parameter actor properties.
    */
   class DT_CORE_EXPORT ShaderPropertyBuilder : public osg::Referenced
   {
   public:
      ShaderPropertyBuilder();

      dtCore::DataType* GetDataTypeForParam(dtCore::ShaderParameter& param);

      dtCore::RefPtr<dtCore::ActorProperty> CreateShaderProperty(dtCore::ShaderParameter& param);

      int CreateShaderPropertiesForContainer(dtCore::ShaderProgram& shaderProgram, dtCore::PropertyContainer& outPropertyContainer);
   
   protected:
      virtual ~ShaderPropertyBuilder();
   };
}

#endif //STRING_ACTOR_PROPERTY
