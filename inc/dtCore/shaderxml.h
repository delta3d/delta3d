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
#ifndef DELTA_SHADERXML
#define DELTA_SHADERXML

#include "dtCore/export.h"
#include <dtCore/refptr.h>
#include <dtCore/shaderparamtexture.h>
#include <string>
#include <vector>

#include <xercesc/dom/DOMElement.hpp>

namespace dtCore
{
   class ShaderProgram;
   class ShaderGroup;
   class ShaderParameter;

   class DT_CORE_EXPORT ShaderXML
   {
      public:

         /**
          * Constructs the parser and initializes xerces.
          */
         ShaderXML();

         /**
          * Destroys the parser and shuts down xerces.
          */
         ~ShaderXML();

         /**
          * Parses the shader XML file.
          * @param fileName Full path to the xml file to parse.
          */
         void ParseXML(const std::string& fileName);

         typedef std::vector<dtCore::RefPtr<ShaderGroup> >  ShaderContainer;

         /** 
          * Get the container of ShaderGroups defined from the loaded file
          * @return The container of loaded ShaderGroups (could be empty)
          */
         const ShaderContainer& GetLoadedShaders() const;

      private:

         /**
          * Builds a shader group from the specified DOM element.
          * @param shaderGroupElem The DOM element containing the shader group definition.
          */
         void ParseShaderGroupElement(xercesc::DOMElement* shaderGroupElem);

         /**
          * Builds a shader object from the specified DOM element.
          * @param shaderElem The DOM element representing a shader in the shader def. file.
          * @param group The shader group to add the newly parsed shader to.
          * @return The new shader parsed from the XML DOM node.
          */
         void ParseShaderElement(xercesc::DOMElement* shaderElem, ShaderGroup& group);

         /**
          * Parses a shader source element child of a shader.  A shader source element specifies
          * a vertex, fragment or general purpose shader source file to link into the resulting
          * shader.
          * @param sourceElem The DOM element for the shader source.
          * @param shader The shader to append the source to.
          */
         void ParseShaderSourceElement(xercesc::DOMElement* sourceElem, ShaderProgram& shader);

         /**
          * Parses a shader parameter from the definitions file.
          * @param paramElement DOM element containing the shader parameter.
          * @param shader The shader to add the newly parsed parameter to.
          */
         void ParseParameterElement(xercesc::DOMElement* paramElement, ShaderProgram& shader);

         /**
          * Parses a shader parameter which represents a one dimensional texture.
          * @param tex1DElement The DOM element for the texture parameter.
          * @param paramName Name to assign to the parameter.
          * @return The newly constructed parameter.
          */
         dtCore::RefPtr<ShaderParameter> ParseTexture1DParameter(xercesc::DOMElement* tex1DElem,
            const std::string& paramName);

         /**
          * Parses a shader parameter which represents a two dimensional texture.
          * @param tex2DElement The DOM element for the texture parameter.
          * @param paramName Name to assign to the parameter.
          * @return The newly constructed parameter.
          */
         dtCore::RefPtr<ShaderParameter> ParseTexture2DParameter(xercesc::DOMElement* tex2DElem,
               const std::string& paramName);

         /**
          * Parses a shader parameter which represents a thre dimensional texture.
          * @param tex3DElement The DOM element for the texture parameter.
          * @param paramName Name to assign to the parameter.
          * @return The newly constructed parameter.
          */
         dtCore::RefPtr<ShaderParameter> ParseTexture3DParameter(xercesc::DOMElement* tex3DElem,
               const std::string& paramName);

         /**
          * Parses a shader parameter which represents a thre dimensional texture.
          * @param texCubeMapElem The DOM element for the texture parameter.
          * @param paramName Name to assign to the parameter.
          * @return The newly constructed parameter.
          */
         dtCore::RefPtr<ShaderParameter> ParseTextureCubeMapParameter(xercesc::DOMElement* texCubeMapElem,
               const std::string& paramName);

         /**
          * Parses a shader parameter which represents a floating point value.
          * @param floatElem The DOM element for the float parameter.
          * @param paramName Name to assign to the parameter.
          * @return The newly constructed parameter.
          */
         dtCore::RefPtr<ShaderParameter> ParseFloatParameter(xercesc::DOMElement* floatElem,
               const std::string& paramName);

         /**
          * Parses a shader parameter which represents a float vec4 value.
          * @param vec4Elem The DOM element for the vec4 parameter.
          * @param paramName Name to assign to the parameter.
          * @return The newly constructed parameter.
          */
         dtCore::RefPtr<ShaderParameter> ParseVec4Parameter(xercesc::DOMElement* vec4Elem,
               const std::string& paramName);

         /**
          * Parses a shader parameter which represents an oscillating floating
          * point timer value.
          * @param floatElem The DOM element for the float parameter.
          * @param paramName Name to assign to the parameter.
          * @return The newly constructed parameter.
          */
         dtCore::RefPtr<ShaderParameter> ParseFloatTimerParameter(xercesc::DOMElement* timerElem,
            const std::string& paramName);

         /**
          * Parses a shader parameter which represents an integer value.
          * @param intElem The DOM element for the integer parameter.
          * @param paramName Name to assign to the parameter.
          * @return The newly constructed parameter.
          */
         dtCore::RefPtr<ShaderParameter> ParseIntParameter(xercesc::DOMElement* intElem,
               const std::string& paramName);

         /**
          * Helper method to get the specified attribute from the XML DOM element.
          * @param element The DOM element containing the attribute in question.
          * @param attribName Name of the attribute.
          * @return The contents of the attribute.  An empty string is returned if
          *   the attribute could not be found.
          */
         std::string GetElementAttribute(xercesc::DOMElement& element, const std::string& attribName);

         /**
          * Simple helper method to get the texture address enumeration matching the specified string.
          * @param mode String to match to the appropriate address mode enumueration.
          * @return The matching enumeration or NULL if there was no match.
          */
         const ShaderParamTexture::AddressMode* GetTextureAddressMode(const std::string& mode);

         /**
          * Simple helper method to get the texture axis enumeration matching the specified string.
          * @param axis String to match to the appropriate axis enumueration.
          * @return The matching enumeration or NULL if there was no match.
          */
         const ShaderParamTexture::TextureAxis* GetTextureAxis(const std::string& axis);

         //Elements and attributes found in the shader definitions XML file.
         static const std::string SHADERLIST_ELEMENT;

         static const std::string SHADERGROUP_ELEMENT;
         static const std::string SHADERGROUP_ATTRIBUTE_NAME;

         static const std::string SHADER_ELEMENT;
         static const std::string SHADER_ATTRIBUTE_NAME;
         static const std::string SHADER_ATTRIBUTE_DEFAULT;
         static const std::string SHADER_ATTRIBUTE_EDITOR;
         static const std::string SHADER_SOURCE_ELEMENT;
         static const std::string SHADER_SOURCE_ATTRIBUTE_TYPE;
         static const std::string SHADER_ATTRIBUTE_VERTICES_OUT;
         static const std::string SHADER_SOURCE_TYPE_VERTEX;
         static const std::string SHADER_SOURCE_TYPE_FRAGMENT;
         static const std::string SHADER_SOURCE_TYPE_GEOMETRY;

         static const std::string PARAMETER_ELEMENT;
         static const std::string PARAMETER_ATTRIBUTE_NAME;
         static const std::string PARAMETER_ATTRIBUTE_SHARED;

         static const std::string TEXTURE1D_ELEMENT;
         static const std::string TEXTURE1D_ATTRIBUTE_TEXUNIT;
         static const std::string TEXTURE1D_SOURCE_ELEMENT;
         static const std::string TEXTURE1D_SOURCE_ATTRIBUTE_TYPE;
         static const std::string TEXTURE1D_WRAP_ELEMENT;
         static const std::string TEXTURE1D_WRAP_ATTRIBUTE_AXIS;
         static const std::string TEXTURE1D_WRAP_ATTRIBUTE_MODE;

         static const std::string TEXTURE2D_ELEMENT;
         static const std::string TEXTURE2D_ATTRIBUTE_TEXUNIT;
         static const std::string TEXTURE2D_SOURCE_ELEMENT;
         static const std::string TEXTURE2D_SOURCE_ATTRIBUTE_TYPE;
         static const std::string TEXTURE2D_WRAP_ELEMENT;
         static const std::string TEXTURE2D_WRAP_ATTRIBUTE_AXIS;
         static const std::string TEXTURE2D_WRAP_ATTRIBUTE_MODE;

         static const std::string TEXTURE3D_ELEMENT;
         static const std::string TEXTURE3D_ATTRIBUTE_TEXUNIT;
         static const std::string TEXTURE3D_SOURCE_ELEMENT;
         static const std::string TEXTURE3D_SOURCE_ATTRIBUTE_TYPE;
         static const std::string TEXTURE3D_WRAP_ELEMENT;
         static const std::string TEXTURE3D_WRAP_ATTRIBUTE_AXIS;
         static const std::string TEXTURE3D_WRAP_ATTRIBUTE_MODE;

         static const std::string TEXTURECUBEMAP_ELEMENT;
         static const std::string TEXTURECUBEMAP_ATTRIBUTE_TEXUNIT;
         static const std::string TEXTURECUBEMAP_SOURCE_ELEMENT;
         static const std::string TEXTURECUBEMAP_SOURCE_ATTRIBUTE_TYPE;
         static const std::string TEXTURECUBEMAP_WRAP_ELEMENT;
         static const std::string TEXTURECUBEMAP_WRAP_ATTRIBUTE_AXIS;
         static const std::string TEXTURECUBEMAP_WRAP_ATTRIBUTE_MODE;
         static const std::string TEXTURECUBEMAP_IMAGE_POSITIVE_X;
         static const std::string TEXTURECUBEMAP_IMAGE_NEGATIVE_X;
         static const std::string TEXTURECUBEMAP_IMAGE_POSITIVE_Y;
         static const std::string TEXTURECUBEMAP_IMAGE_NEGATIVE_Y;
         static const std::string TEXTURECUBEMAP_IMAGE_POSITIVE_Z;
         static const std::string TEXTURECUBEMAP_IMAGE_NEGATIVE_Z;

         static const std::string FLOAT_ELEMENT;
         static const std::string VEC4_ELEMENT;
         static const std::string INT_ELEMENT;
         static const std::string PARAM_ELEMENT_ATTRIBUTE_DEFAULTVALUE;

         static const std::string OSCILLATOR_ELEMENT;
         static const std::string OSCILLATOR_ATTRIB_OFFSET;
         static const std::string OSCILLATOR_ATTRIB_RANGE_MIN;
         static const std::string OSCILLATOR_ATTRIB_RANGE_MAX;
         static const std::string OSCILLATOR_ATTRIB_CYCLETIME_MIN;
         static const std::string OSCILLATOR_ATTRIB_CYCLETIME_MAX;
         static const std::string OSCILLATOR_ATTRIB_CYCLE_COUNT;
         static const std::string OSCILLATOR_ATTRIB_USEREALTIME;
         static const std::string OSCILLATOR_ATTRIB_OSCILLATION_TYPE;
         static const std::string OSCILLATOR_ATTRIB_TRIGGER;

         //Disable the copy constructor and assignment operator.
         ShaderXML(const ShaderXML&) { }
         ShaderXML& operator=(const ShaderXML&) { return *this; }

         ShaderContainer mShaderGroupContainer; ///<container of loaded ShaderGroups
   };
}

#endif
