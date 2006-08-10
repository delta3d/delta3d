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
#include "dtCore/shaderxml.h"
#include "dtCore/shadermanager.h"
#include "dtCore/shader.h"
#include "dtCore/shadergroup.h"
#include "dtCore/texture2dshaderparameter.h"
#include "dtCore/floatshaderparameter.h"
#include "dtCore/integershaderparameter.h"

#include <dtUtil/xercesutils.h>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMDocumentType.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMNodeIterator.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOMText.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>

#include <osg/Uniform>
#include <sstream>

namespace dtCore
{
   ///////////////////////////////////////////////////////////////////////////////
   const std::string ShaderXML::SHADERLIST_ELEMENT("shaderlist");

   const std::string ShaderXML::SHADERGROUP_ELEMENT("shadergroup");
   const std::string ShaderXML::SHADERGROUP_ATTRIBUTE_NAME("name");

   const std::string ShaderXML::SHADER_ELEMENT("shader");
   const std::string ShaderXML::SHADER_ATTRIBUTE_NAME("name");
   const std::string ShaderXML::SHADER_ATTRIBUTE_DEFAULT("default");
   const std::string ShaderXML::SHADER_SOURCE_ELEMENT("source");
   const std::string ShaderXML::SHADER_SOURCE_ATTRIBUTE_TYPE("type");
   const std::string ShaderXML::SHADER_SOURCE_TYPE_VERTEX("Vertex");
   const std::string ShaderXML:: SHADER_SOURCE_TYPE_FRAGMENT("Fragment");

   const std::string ShaderXML::PARAMETER_ELEMENT("parameter");
   const std::string ShaderXML::PARAMETER_ATTRIBUTE_NAME("name");

   const std::string ShaderXML::TEXTURE2D_ELEMENT("texture2D");
   const std::string ShaderXML::TEXTURE2D_ATTRIBUTE_TEXUNIT("textureUnit");
   const std::string ShaderXML::TEXTURE2D_SOURCE_ELEMENT("source");
   const std::string ShaderXML::TEXTURE2D_SOURCE_ATTRIBUTE_TYPE("type");
   const std::string ShaderXML::TEXTURE2D_WRAP_ELEMENT("wrap");
   const std::string ShaderXML::TEXTURE2D_WRAP_ATTRIBUTE_AXIS("axis");
   const std::string ShaderXML::TEXTURE2D_WRAP_ATTRIBUTE_MODE("mode");

   const std::string ShaderXML::FLOAT_ELEMENT("float");
   const std::string ShaderXML::INT_ELEMENT("integer");
   const std::string ShaderXML::PARAM_ELEMENT_ATTRIBUTE_DEFAULTVALUE("defaultValue");

   ///////////////////////////////////////////////////////////////////////////////
   ShaderXML::ShaderXML()
   {
      try
      {
         xercesc::XMLPlatformUtils::Initialize();
      }
      catch(const xercesc::XMLException &e)
      {
         char *message = xercesc::XMLString::transcode(e.getMessage());

         std::ostringstream error;
         error << "Error initializing XML toolkit: " << message;
         xercesc::XMLString::release(&message);
         EXCEPT(ShaderException::XML_PARSER_ERROR,error.str());
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   ShaderXML::~ShaderXML()
   {
      try
      {
         xercesc::XMLPlatformUtils::Terminate();
      }
      catch(const xercesc::XMLException &e)
      {
         char *message = xercesc::XMLString::transcode(e.getMessage());

         std::ostringstream error;
         error << "Error shutting down XML toolkit: " << message;
         xercesc::XMLString::release(&message);
         EXCEPT(ShaderException::XML_PARSER_ERROR,error.str());
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ShaderXML::ParseXML(const std::string &fileName)
   {
      xercesc::XercesDOMParser parser;

      parser.setValidationScheme(xercesc::XercesDOMParser::Val_Never);
      parser.setDoNamespaces(false);
      parser.setDoSchema(false);
      parser.setLoadExternalDTD(false);

      try
      {
         parser.parse(fileName.c_str());

         xercesc::DOMDocument *xmlDoc = parser.getDocument();
         xercesc::DOMElement *shaderList = xmlDoc->getDocumentElement();
         if (shaderList == NULL)
            EXCEPT(ShaderException::XML_PARSER_ERROR,"Shader XML document is empty.");

         dtUtil::XMLStringConverter strConv(shaderList->getTagName());
         if (strConv.ToString() != ShaderXML::SHADERLIST_ELEMENT)
            EXCEPT(ShaderException::XML_PARSER_ERROR,"Malformed shader list element tag name.");

         xercesc::DOMNodeList *children = shaderList->getChildNodes();
         for (XMLSize_t i=0; i<children->getLength(); i++)
         {
            xercesc::DOMNode *node = children->item(i);

            if (node == NULL)
               continue;
            if (node->getNodeType() != xercesc::DOMNode::ELEMENT_NODE)
               continue;

            //If we got here, we have a shader group element.
            xercesc::DOMElement *element = static_cast<xercesc::DOMElement *>(node);
            ParseShaderGroupElement(element);
         }
      }
      catch (const xercesc::XMLException &e)
      {
         char *message = xercesc::XMLString::transcode(e.getMessage());
         std::ostringstream error;

         error << "Error parsing shader file: " << fileName << ".  Reason: " <<
            message;

         xercesc::XMLString::release(&message);
         EXCEPT(ShaderException::XML_PARSER_ERROR,error.str());
      }
      catch (const xercesc::DOMException &e)
      {
         char *message = xercesc::XMLString::transcode(e.getMessage());
         std::ostringstream error;

         error << "Error processing DOM:  Reason: " << message;

         xercesc::XMLString::release(&message);
         EXCEPT(ShaderException::XML_PARSER_ERROR,error.str());
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ShaderXML::ParseShaderGroupElement(xercesc::DOMElement *shaderGroupElem)
   {
      dtUtil::XMLStringConverter strConv(shaderGroupElem->getTagName());
      if (strConv.ToString() != ShaderXML::SHADERGROUP_ELEMENT)
         EXCEPT(ShaderException::XML_PARSER_ERROR,"Malformed shader group element tag name.");

      //Shader elements only have one attribute (the name) so parse it and continue on.
      std::string groupName = GetElementAttribute(*shaderGroupElem,
            ShaderXML::SHADERGROUP_ATTRIBUTE_NAME);
      dtCore::RefPtr<ShaderGroup> newGroup = new ShaderGroup(groupName);

      //Children of a shadergroup element are shaders...
      xercesc::DOMNodeList *children = shaderGroupElem->getChildNodes();
      for (XMLSize_t i=0; i<children->getLength(); i++)
      {
         xercesc::DOMNode *node = children->item(i);

         if (node == NULL)
            continue;
         if (node->getNodeType() != xercesc::DOMNode::ELEMENT_NODE)
            continue;

         //If we got here, we have either a uniform element or a source element.
         xercesc::DOMElement *element = static_cast<xercesc::DOMElement *>(node);

         dtUtil::XMLStringConverter elemName(element->getTagName());
         if (elemName.ToString() == ShaderXML::SHADER_ELEMENT)
            ParseShaderElement(element,*newGroup);
         else
            EXCEPT(ShaderException::XML_PARSER_ERROR,"Foreign element found in shader XML source.");
      }

      ShaderManager::GetInstance().AddShaderGroup(*newGroup);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ShaderXML::ParseShaderElement(xercesc::DOMElement *shaderElem, ShaderGroup &group)
   {
      //Shader elements have a name attribute and possibly a default attribute telling the
      //shader group that this shader is the default.
      std::string shaderName = GetElementAttribute(*shaderElem,ShaderXML::SHADER_ATTRIBUTE_NAME);
      std::string isDefault = GetElementAttribute(*shaderElem,ShaderXML::SHADER_ATTRIBUTE_DEFAULT);
      dtCore::RefPtr<Shader> newShader = new Shader(shaderName);

      //Children of a shader element are either shader sources or parameters...
      xercesc::DOMNodeList *children = shaderElem->getChildNodes();
      for (XMLSize_t i=0; i<children->getLength(); i++)
      {
         xercesc::DOMNode *node = children->item(i);

         if (node == NULL)
            continue;
         if (node->getNodeType() != xercesc::DOMNode::ELEMENT_NODE)
            continue;

         //If we got here, we have either a uniform element or a source element.
         xercesc::DOMElement *element = static_cast<xercesc::DOMElement *>(node);

         dtUtil::XMLStringConverter elemName(element->getTagName());
         if (elemName.ToString() == ShaderXML::SHADER_SOURCE_ELEMENT)
            ParseShaderSourceElement(element,*newShader);
         else if (elemName.ToString() == ShaderXML::PARAMETER_ELEMENT)
            ParseParameterElement(element,*newShader);
         else
            EXCEPT(ShaderException::XML_PARSER_ERROR,"Foreign element found in shader XML source.");
      }

      if (isDefault == "yes")
         group.AddShader(*newShader,true);
      else
         group.AddShader(*newShader);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ShaderXML::ParseShaderSourceElement(xercesc::DOMElement *sourceElem, Shader &shader)
   {
      std::string type = GetElementAttribute(*sourceElem,ShaderXML::SHADER_SOURCE_ATTRIBUTE_TYPE);

      //The source element has one child:  The text specifing the source file.
      xercesc::DOMNodeList *children = sourceElem->getChildNodes();
      if (children->getLength() != 1 || children->item(0)->getNodeType() != xercesc::DOMNode::TEXT_NODE)
         EXCEPT(ShaderException::XML_PARSER_ERROR,"Shader source should only have one child text element.");

      xercesc::DOMText *file = static_cast<xercesc::DOMText *>(children->item(0));

      dtUtil::XMLStringConverter fileConverter(file->getNodeValue());

      if (type == ShaderXML::SHADER_SOURCE_TYPE_VERTEX)
         shader.SetVertexShaderSource(fileConverter.ToString());
      else if (type == ShaderXML::SHADER_SOURCE_TYPE_FRAGMENT)
         shader.SetFragmentShaderSource(fileConverter.ToString());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ShaderXML::ParseParameterElement(xercesc::DOMElement *paramElement, Shader &shader)
   {
      std::string paramName = GetElementAttribute(*paramElement,ShaderXML::PARAMETER_ATTRIBUTE_NAME);
      xercesc::DOMNodeList *children = paramElement->getChildNodes();

      for (XMLSize_t i=0; i<children->getLength(); i++)
      {
         xercesc::DOMNode *node = children->item(i);

         if (node == NULL)
            continue;
         if (node->getNodeType() != xercesc::DOMNode::ELEMENT_NODE)
            continue;

         //If we got here, then we have some sort of shader parameter specialization.
         xercesc::DOMElement *typeElement = static_cast<xercesc::DOMElement *>(node);
         dtCore::RefPtr<ShaderParameter> newParam = NULL;

         dtUtil::XMLStringConverter paramType(typeElement->getTagName());
         std::string toString = paramType.ToString();
         if (toString == ShaderXML::TEXTURE2D_ELEMENT)
            newParam = ParseTexture2DParameter(typeElement,paramName);
         else if (toString == ShaderXML::FLOAT_ELEMENT)
            newParam = ParseFloatParameter(typeElement,paramName);
         else if (toString == ShaderXML::INT_ELEMENT)
            newParam = ParseIntParameter(typeElement,paramName);
         else
            EXCEPT(ShaderException::XML_PARSER_ERROR,"Invalid element found while parsing "
                  "shader parameter.");

         shader.AddParameter(*newParam);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<ShaderParameter> ShaderXML::ParseTexture2DParameter(
         xercesc::DOMElement *tex2DElem, const std::string &paramName)
   {
      //First get the texture unit attribute from the element.
      dtCore::RefPtr<Texture2DShaderParameter> newParam = new Texture2DShaderParameter(paramName);
      xercesc::DOMNodeList *children = tex2DElem->getChildNodes();

      //Get the integer for the texture unit.
      std::string texUnitString = GetElementAttribute(*tex2DElem,ShaderXML::TEXTURE2D_ATTRIBUTE_TEXUNIT);
      if (!texUnitString.empty())
      {
         std::istringstream ss;
         unsigned int texUnit;
         ss.str(texUnitString);
         ss >> texUnit;
         newParam->SetTextureUnit(texUnit);
      }

      //Now parse the individual child elements of the 2D texture element.
      for (XMLSize_t i=0; i<children->getLength(); i++)
      {
         xercesc::DOMNode *node = children->item(i);

         if (node == NULL)
            continue;
         if (node->getNodeType() != xercesc::DOMNode::ELEMENT_NODE)
            continue;

         xercesc::DOMElement *texElement = static_cast<xercesc::DOMElement *>(node);

         dtUtil::XMLStringConverter elemNameConverter(texElement->getTagName());
         std::string elemName = elemNameConverter.ToString();
         if (elemName == ShaderXML::TEXTURE2D_SOURCE_ELEMENT)
         {
            //Source elements have a type attribute that specify whether or not the
            //texture source is to come from an image file or from some source generated
            //elsewhere.
            std::string sourceType = GetElementAttribute(*texElement,
                  ShaderXML::TEXTURE2D_SOURCE_ATTRIBUTE_TYPE);

            if (sourceType == TextureShaderParameter::TextureSourceType::IMAGE.GetName())
            {
               xercesc::DOMNodeList *children = texElement->getChildNodes();
               if (children->getLength() != 1 ||
                  children->item(0)->getNodeType() != xercesc::DOMNode::TEXT_NODE)
               {
                  EXCEPT(ShaderException::XML_PARSER_ERROR,"Shader source should only have one child text element.");
               }

               xercesc::DOMText *file = static_cast<xercesc::DOMText *>(children->item(0));

               dtUtil::XMLStringConverter fileConverter(file->getNodeValue());
               newParam->SetTexture(fileConverter.ToString());
               newParam->SetTextureSourceType(TextureShaderParameter::TextureSourceType::IMAGE);
            }
            else if (sourceType == TextureShaderParameter::TextureSourceType::AUTO.GetName())
            {
               newParam->SetTextureSourceType(TextureShaderParameter::TextureSourceType::AUTO);
            }
            else
            {
               EXCEPT(ShaderException::XML_PARSER_ERROR,"Unknown texture2D source type.");
            }
         }
         else if (elemName == ShaderXML::TEXTURE2D_WRAP_ELEMENT)
         {
            std::string axis = GetElementAttribute(*texElement,ShaderXML::TEXTURE2D_WRAP_ATTRIBUTE_AXIS);
            std::string mode = GetElementAttribute(*texElement,ShaderXML::TEXTURE2D_WRAP_ATTRIBUTE_MODE);

            if (!axis.empty() && !mode.empty())
            {
               const TextureShaderParameter::AddressMode *wrapMode = GetTextureAddressMode(mode);
               const TextureShaderParameter::TextureAxis *texAxis = GetTextureAxis(axis);

               if (wrapMode == NULL)
                  EXCEPT(ShaderException::XML_PARSER_ERROR,"Invalid address mode of: " + mode +
                        " specified for shader parameter: " + paramName);
               if (texAxis == NULL)
                  EXCEPT(ShaderException::XML_PARSER_ERROR,"Invalid texture axis: " + axis +
                        " specified for shader parameter: " + paramName);

               newParam->SetAddressMode(*texAxis,*wrapMode);
            }
         }
         else
         {
            EXCEPT(ShaderException::XML_PARSER_ERROR,"Unknown element in Texture2D parameter.");
         }
      }

      return static_cast<ShaderParameter *>(newParam.get());
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<ShaderParameter> ShaderXML::ParseFloatParameter(xercesc::DOMElement *floatElem,
         const std::string &paramName)
   {
      dtCore::RefPtr<FloatShaderParameter> newParam = new FloatShaderParameter(paramName);

      std::string value = GetElementAttribute(*floatElem,ShaderXML::PARAM_ELEMENT_ATTRIBUTE_DEFAULTVALUE);
      if (!value.empty())
      {
         std::istringstream ss;
         float defaultValue;
         ss.str(value);
         ss >> defaultValue;
         newParam->SetValue(defaultValue);
      }

      return static_cast<ShaderParameter*>(newParam.get());
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<ShaderParameter> ShaderXML::ParseIntParameter(xercesc::DOMElement *intElem,
         const std::string &paramName)
   {
      dtCore::RefPtr<IntegerShaderParameter> newParam = new IntegerShaderParameter(paramName);

      std::string value = GetElementAttribute(*intElem,ShaderXML::PARAM_ELEMENT_ATTRIBUTE_DEFAULTVALUE);
      if (!value.empty())
      {
         std::istringstream ss;
         int defaultValue;
         ss.str(value);
         ss >> defaultValue;
         newParam->SetValue(defaultValue);
      }

      return static_cast<ShaderParameter*>(newParam.get());
   }

   ///////////////////////////////////////////////////////////////////////////////
   std::string ShaderXML::GetElementAttribute(xercesc::DOMElement &element, const std::string &attribName)
   {
      XMLCh *xmlChar = xercesc::XMLString::transcode(attribName.c_str());
      dtUtil::XMLStringConverter converter(element.getAttribute(xmlChar));
      xercesc::XMLString::release(&xmlChar);

      return converter.ToString();
   }

   ///////////////////////////////////////////////////////////////////////////////
   const TextureShaderParameter::AddressMode *ShaderXML::GetTextureAddressMode(const std::string &mode)
   {
      if (TextureShaderParameter::AddressMode::CLAMP == mode)
         return &TextureShaderParameter::AddressMode::CLAMP;
      else if (TextureShaderParameter::AddressMode::REPEAT == mode)
         return &TextureShaderParameter::AddressMode::REPEAT;
      else if (TextureShaderParameter::AddressMode::MIRROR == mode)
         return &TextureShaderParameter::AddressMode::MIRROR;
      else
         return NULL;
   }

   ///////////////////////////////////////////////////////////////////////////////
   const TextureShaderParameter::TextureAxis *ShaderXML::GetTextureAxis(const std::string &axis)
   {
      if (TextureShaderParameter::TextureAxis::S == axis)
         return &TextureShaderParameter::TextureAxis::S;
      else if (TextureShaderParameter::TextureAxis::T == axis)
         return &TextureShaderParameter::TextureAxis::T;
      else if (TextureShaderParameter::TextureAxis::R == axis)
         return &TextureShaderParameter::TextureAxis::R;
      else if (TextureShaderParameter::TextureAxis::Q == axis)
         return &TextureShaderParameter::TextureAxis::Q;
      else
         return NULL;
   }

}
