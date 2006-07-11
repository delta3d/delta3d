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
#ifndef DELTA_SHADERXML
#define DELTA_SHADERXML

#include "dtCore/export.h"
#include "dtCore/textureshaderparameter.h"
#include <dtCore/refptr.h>
#include <string>

#include <xercesc/dom/DOMElement.hpp>

namespace dtCore
{
   class Shader;
   class ShaderGroup;

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
         void ParseXML(const std::string &fileName);

      private:

         /**
          * Builds a shader group from the specified DOM element.
          * @param shaderGroupElem The DOM element containing the shader group definition.
          */
         void ParseShaderGroupElement(xercesc::DOMElement *shaderGroupElem);

         /**
          * Builds a shader object from the specified DOM element.
          * @param shaderElem The DOM element representing a shader in the shader def. file.
          * @param group The shader group to add the newly parsed shader to.
          * @return The new shader parsed from the XML DOM node.
          */
         void ParseShaderElement(xercesc::DOMElement *shaderElem, ShaderGroup &group);

         /**
          * Parses a shader source element child of a shader.  A shader source element specifies
          * a vertex, fragment or general purpose shader source file to link into the resulting
          * shader.
          * @param sourceElem The DOM element for the shader source.
          * @param shader The shader to append the source to.
          */
         void ParseShaderSourceElement(xercesc::DOMElement *sourceElem, Shader &shader);

         /**
          * Parses a shader parameter from the definitions file.
          * @param paramElement DOM element containing the shader parameter.
          * @param shader The shader to add the newly parsed parameter to.
          */
         void ParseParameterElement(xercesc::DOMElement *paramElement, Shader &shader);

         /**
          * Parses a shader parameter which represents a two dimensional texture.
          * @param tex2DElement The DOM element for the texture parameter.
          * @param paramName Name to assign to the parameter.
          * @return The newly constructed parameter.
          */
         dtCore::RefPtr<ShaderParameter> ParseTexture2DParameter(xercesc::DOMElement *tex2DElem,
               const std::string &paramName);

         /**
          * Parses a shader parameter which represents a floating point value.
          * @param floatElem The DOM element for the float parameter.
          * @param paramName Name to assign to the parameter.
          * @return The newly constructed parameter.
          */
         dtCore::RefPtr<ShaderParameter> ParseFloatParameter(xercesc::DOMElement *floatElem,
               const std::string &paramName);

         /**
          * Parses a shader parameter which represents an integer value.
          * @param intElem The DOM element for the integer parameter.
          * @param paramName Name to assign to the parameter.
          * @return The newly constructed parameter.
          */
         dtCore::RefPtr<ShaderParameter> ParseIntParameter(xercesc::DOMElement *intElem,
               const std::string &paramName);

         /**
          * Helper method to get the specified attribute from the XML DOM element.
          * @param element The DOM element containing the attribute in question.
          * @param attribName Name of the attribute.
          * @return The contents of the attribute.  An empty string is returned if
          *   the attribute could not be found.
          */
         std::string GetElementAttribute(xercesc::DOMElement &element, const std::string &attribName);

         /**
          * Simple helper method to get the texture address enumeration matching the specified string.
          * @param mode String to match to the appropriate address mode enumueration.
          * @return The matching enumeration or NULL if there was no match.
          */
         const TextureShaderParameter::AddressMode *GetTextureAddressMode(const std::string &mode);

         /**
          * Simple helper method to get the texture axis enumeration matching the specified string.
          * @param axis String to match to the appropriate axis enumueration.
          * @return The matching enumeration or NULL if there was no match.
          */
         const TextureShaderParameter::TextureAxis *GetTextureAxis(const std::string &axis);

         //Elements and attributes found in the shader definitions XML file.
         static const std::string SHADERLIST_ELEMENT;

         static const std::string SHADERGROUP_ELEMENT;
         static const std::string SHADERGROUP_ATTRIBUTE_NAME;

         static const std::string SHADER_ELEMENT;
         static const std::string SHADER_ATTRIBUTE_NAME;
         static const std::string SHADER_ATTRIBUTE_DEFAULT;
         static const std::string SHADER_SOURCE_ELEMENT;
         static const std::string SHADER_SOURCE_ATTRIBUTE_TYPE;
         static const std::string SHADER_SOURCE_TYPE_VERTEX;
         static const std::string SHADER_SOURCE_TYPE_FRAGMENT;

         static const std::string PARAMETER_ELEMENT;
         static const std::string PARAMETER_ATTRIBUTE_NAME;

         static const std::string TEXTURE2D_ELEMENT;
         static const std::string TEXTURE2D_ATTRIBUTE_TEXUNIT;
         static const std::string TEXTURE2D_SOURCE_ELEMENT;
         static const std::string TEXTURE2D_SOURCE_ATTRIBUTE_TYPE;
         static const std::string TEXTURE2D_WRAP_ELEMENT;
         static const std::string TEXTURE2D_WRAP_ATTRIBUTE_AXIS;
         static const std::string TEXTURE2D_WRAP_ATTRIBUTE_MODE;

         static const std::string FLOAT_ELEMENT;
         static const std::string INT_ELEMENT;
         static const std::string PARAM_ELEMENT_ATTRIBUTE_DEFAULTVALUE;

         //Disable the copy constructor and assignment operator.
         ShaderXML(const ShaderXML &rhs) { }
         ShaderXML &operator=(const ShaderXML &rhs) { return *this; }
   };
}

#endif
