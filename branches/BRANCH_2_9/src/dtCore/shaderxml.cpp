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
#include <dtCore/shaderxml.h>
#include <dtCore/shadermanager.h> //needed for exception defintions
#include <dtCore/shaderprogram.h>
#include <dtCore/shadergroup.h>
#include <dtCore/shaderparamtexture1d.h>
#include <dtCore/shaderparamtexture2d.h>
#include <dtCore/shaderparamtexture3d.h>
#include <dtCore/shaderparamtexturecubemap.h>
#include <dtCore/shaderparamfloat.h>
#include <dtCore/shaderparamvec4.h>
#include <dtCore/shaderparamint.h>
#include <dtCore/shaderparamoscillator.h>

#include <dtUtil/exception.h>
#include <dtUtil/stringutils.h>
#include <dtUtil/xercesutils.h>
#include <dtUtil/xerceserrorhandler.h>

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
   const std::string ShaderXML::SHADER_ATTRIBUTE_VERTICES_OUT("verticesout");
   const std::string ShaderXML::SHADER_ATTRIBUTE_DEFAULT("default");
   const std::string ShaderXML::SHADER_ATTRIBUTE_EDITOR("editor");
   const std::string ShaderXML::SHADER_SOURCE_ELEMENT("source");
   const std::string ShaderXML::SHADER_SOURCE_ATTRIBUTE_TYPE("type");
   const std::string ShaderXML::SHADER_SOURCE_TYPE_VERTEX("Vertex");
   const std::string ShaderXML::SHADER_SOURCE_TYPE_FRAGMENT("Fragment");
   const std::string ShaderXML::SHADER_SOURCE_TYPE_GEOMETRY("Geometry");

   const std::string ShaderXML::PARAMETER_ELEMENT("parameter");
   const std::string ShaderXML::PARAMETER_ATTRIBUTE_NAME("name");
   const std::string ShaderXML::PARAMETER_ATTRIBUTE_SHARED("shared");

   const std::string ShaderXML::TEXTURE1D_ELEMENT("texture1D");
   const std::string ShaderXML::TEXTURE1D_ATTRIBUTE_TEXUNIT("textureUnit");
   const std::string ShaderXML::TEXTURE1D_SOURCE_ELEMENT("source");
   const std::string ShaderXML::TEXTURE1D_SOURCE_ATTRIBUTE_TYPE("type");
   const std::string ShaderXML::TEXTURE1D_WRAP_ELEMENT("wrap");
   const std::string ShaderXML::TEXTURE1D_WRAP_ATTRIBUTE_AXIS("axis");
   const std::string ShaderXML::TEXTURE1D_WRAP_ATTRIBUTE_MODE("mode");

   const std::string ShaderXML::TEXTURE2D_ELEMENT("texture2D");
   const std::string ShaderXML::TEXTURE2D_ATTRIBUTE_TEXUNIT("textureUnit");
   const std::string ShaderXML::TEXTURE2D_SOURCE_ELEMENT("source");
   const std::string ShaderXML::TEXTURE2D_SOURCE_ATTRIBUTE_TYPE("type");
   const std::string ShaderXML::TEXTURE2D_WRAP_ELEMENT("wrap");
   const std::string ShaderXML::TEXTURE2D_WRAP_ATTRIBUTE_AXIS("axis");
   const std::string ShaderXML::TEXTURE2D_WRAP_ATTRIBUTE_MODE("mode");

   const std::string ShaderXML::TEXTURE3D_ELEMENT("texture3D");
   const std::string ShaderXML::TEXTURE3D_ATTRIBUTE_TEXUNIT("textureUnit");
   const std::string ShaderXML::TEXTURE3D_SOURCE_ELEMENT("source");
   const std::string ShaderXML::TEXTURE3D_SOURCE_ATTRIBUTE_TYPE("type");
   const std::string ShaderXML::TEXTURE3D_WRAP_ELEMENT("wrap");
   const std::string ShaderXML::TEXTURE3D_WRAP_ATTRIBUTE_AXIS("axis");
   const std::string ShaderXML::TEXTURE3D_WRAP_ATTRIBUTE_MODE("mode");

   const std::string ShaderXML::TEXTURECUBEMAP_ELEMENT("textureCubeMap");
   const std::string ShaderXML::TEXTURECUBEMAP_ATTRIBUTE_TEXUNIT("textureUnit");
   const std::string ShaderXML::TEXTURECUBEMAP_SOURCE_ELEMENT("source");
   const std::string ShaderXML::TEXTURECUBEMAP_SOURCE_ATTRIBUTE_TYPE("type");
   const std::string ShaderXML::TEXTURECUBEMAP_WRAP_ELEMENT("wrap");
   const std::string ShaderXML::TEXTURECUBEMAP_WRAP_ATTRIBUTE_AXIS("axis");
   const std::string ShaderXML::TEXTURECUBEMAP_WRAP_ATTRIBUTE_MODE("mode");
   const std::string ShaderXML::TEXTURECUBEMAP_IMAGE_POSITIVE_X("ImagePositiveX");
   const std::string ShaderXML::TEXTURECUBEMAP_IMAGE_NEGATIVE_X("ImageNegativeX");
   const std::string ShaderXML::TEXTURECUBEMAP_IMAGE_POSITIVE_Y("ImagePositiveY");
   const std::string ShaderXML::TEXTURECUBEMAP_IMAGE_NEGATIVE_Y("ImageNegativeY");
   const std::string ShaderXML::TEXTURECUBEMAP_IMAGE_POSITIVE_Z("ImagePositiveZ");
   const std::string ShaderXML::TEXTURECUBEMAP_IMAGE_NEGATIVE_Z("ImageNegativeZ");

   const std::string ShaderXML::FLOAT_ELEMENT("float");
   const std::string ShaderXML::INT_ELEMENT("integer");
   const std::string ShaderXML::VEC4_ELEMENT("vec4");
   const std::string ShaderXML::PARAM_ELEMENT_ATTRIBUTE_DEFAULTVALUE("defaultValue");

   const std::string ShaderXML::OSCILLATOR_ELEMENT("oscillator");
   const std::string ShaderXML::OSCILLATOR_ATTRIB_OFFSET("offset");
   const std::string ShaderXML::OSCILLATOR_ATTRIB_RANGE_MIN("rangemin");
   const std::string ShaderXML::OSCILLATOR_ATTRIB_RANGE_MAX("rangemax");
   const std::string ShaderXML::OSCILLATOR_ATTRIB_CYCLETIME_MIN("cycletimemin");
   const std::string ShaderXML::OSCILLATOR_ATTRIB_CYCLETIME_MAX("cycletimemax");
   const std::string ShaderXML::OSCILLATOR_ATTRIB_CYCLE_COUNT("cyclecount");
   const std::string ShaderXML::OSCILLATOR_ATTRIB_USEREALTIME("userealtime");
   const std::string ShaderXML::OSCILLATOR_ATTRIB_OSCILLATION_TYPE("oscillation");
   const std::string ShaderXML::OSCILLATOR_ATTRIB_TRIGGER("trigger");

   /////////////////////////////////////////////////////////////////////////////
   ShaderXML::ShaderXML()
   {
      try
      {
         xercesc::XMLPlatformUtils::Initialize();
      }
      catch(const xercesc::XMLException& e)
      {
         char* message = xercesc::XMLString::transcode(e.getMessage());

         std::ostringstream error;
         error << "Error initializing XML toolkit: " << message;
         xercesc::XMLString::release(&message);
         throw ShaderXmlParserException(error.str(), __FILE__, __LINE__);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   ShaderXML::~ShaderXML()
   {
      try
      {
         xercesc::XMLPlatformUtils::Terminate();
      }
      catch(const xercesc::XMLException& e)
      {
         char *message = xercesc::XMLString::transcode(e.getMessage());

         std::ostringstream error;
         error << "Error shutting down XML toolkit: " << message;
         xercesc::XMLString::release(&message);
         throw ShaderXmlParserException(error.str(), __FILE__, __LINE__);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ShaderXML::ParseXML(const std::string& fileName)
   {
      mShaderGroupContainer.clear();

      xercesc::XercesDOMParser parser;
      dtUtil::XercesErrorHandler errorHandler;
      parser.setValidationScheme(xercesc::XercesDOMParser::Val_Never);
      parser.setDoNamespaces(false);
      parser.setDoSchema(false);
      parser.setLoadExternalDTD(false);
      parser.setErrorHandler(&errorHandler);

      try
      {
         parser.parse(fileName.c_str());

         xercesc::DOMDocument* xmlDoc = parser.getDocument();
         xercesc::DOMElement* shaderList = xmlDoc->getDocumentElement();
         
         if (shaderList == NULL)
         {
            throw ShaderXmlParserException("Shader XML document is empty.", __FILE__, __LINE__);
         }

         dtUtil::XMLStringConverter strConv(shaderList->getTagName());

         if (strConv.ToString() != ShaderXML::SHADERLIST_ELEMENT)
         {
            throw ShaderXmlParserException("Malformed shader list element tag name.", __FILE__, __LINE__);
         }

         xercesc::DOMNodeList *children = shaderList->getChildNodes();
         for (XMLSize_t i=0; i<children->getLength(); i++)
         {
            xercesc::DOMNode *node = children->item(i);

            if (node == NULL)
            {
               continue;
            }

            if (node->getNodeType() != xercesc::DOMNode::ELEMENT_NODE)
            {
               continue;
            }

            //If we got here, we have a shader group element.
            xercesc::DOMElement* element = static_cast<xercesc::DOMElement*>(node);
            ParseShaderGroupElement(element);
         }
      }
      catch (const xercesc::XMLException& e)
      {
         char* message = xercesc::XMLString::transcode(e.getMessage());
         std::ostringstream error;

         error << "Error parsing shader file: " << fileName << ".  Reason: " <<
            message;

         xercesc::XMLString::release(&message);
         throw ShaderXmlParserException( error.str(), __FILE__, __LINE__);
      }
      catch (const xercesc::DOMException& e)
      {
         char* message = xercesc::XMLString::transcode(e.getMessage());
         std::ostringstream error;

         error << "Error processing DOM:  Reason: " << message;

         xercesc::XMLString::release(&message);
         throw ShaderXmlParserException(error.str(), __FILE__, __LINE__);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void ShaderXML::ParseShaderGroupElement(xercesc::DOMElement* shaderGroupElem)
   {
      dtUtil::XMLStringConverter strConv(shaderGroupElem->getTagName());
      if (strConv.ToString() != ShaderXML::SHADERGROUP_ELEMENT)
         throw ShaderXmlParserException("Malformed shader group element tag name.", __FILE__, __LINE__);

      //Shader elements only have one attribute (the name) so parse it and continue on.
      std::string groupName = GetElementAttribute(*shaderGroupElem,
            ShaderXML::SHADERGROUP_ATTRIBUTE_NAME);
      dtCore::RefPtr<ShaderGroup> newGroup = new ShaderGroup(groupName);

      //Children of a shadergroup element are shaders...
      xercesc::DOMNodeList* children = shaderGroupElem->getChildNodes();
      for (XMLSize_t i = 0; i < children->getLength(); i++)
      {
         xercesc::DOMNode* node = children->item(i);

         if (node == NULL)
         {
            continue;
         }

         if (node->getNodeType() != xercesc::DOMNode::ELEMENT_NODE)
         {
            continue;
         }

         //If we got here, we have either a uniform element or a source element.
         xercesc::DOMElement *element = static_cast<xercesc::DOMElement *>(node);

         dtUtil::XMLStringConverter elemName(element->getTagName());

         if (elemName.ToString() == ShaderXML::SHADER_ELEMENT)
         {
            try 
            {
               ParseShaderElement(element,*newGroup);
            } 
            catch (ShaderSourceException& e) 
            {
               dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,
                  "Error loading shader: %s", e.What().c_str());
            }
         }
         else
         {
            throw ShaderXmlParserException("Foreign element found in shader XML source.", __FILE__, __LINE__);
         }
      }

      //store the ShaderGroup in our container
      mShaderGroupContainer.push_back(newGroup);
   }

   /////////////////////////////////////////////////////////////////////////////
   void ShaderXML::ParseShaderElement(xercesc::DOMElement* shaderElem, ShaderGroup& group)
   {
      //Shader elements have a name attribute and possibly a default attribute telling the
      //shader group that this shader is the default.
      std::string shaderName = GetElementAttribute(*shaderElem, ShaderXML::SHADER_ATTRIBUTE_NAME);
      std::string isDefault = GetElementAttribute(*shaderElem, ShaderXML::SHADER_ATTRIBUTE_DEFAULT);
      std::string isEditor = GetElementAttribute(*shaderElem, ShaderXML::SHADER_ATTRIBUTE_EDITOR);
      std::string verticesout = GetElementAttribute(*shaderElem, ShaderXML::SHADER_ATTRIBUTE_VERTICES_OUT);

      dtCore::RefPtr<ShaderProgram> newShader = new ShaderProgram(shaderName);

      // If the shader uses a geometry shader, this should be specified
      if (!verticesout.empty())
      {
         newShader->SetGeometryShaderVerticesOut(dtUtil::ToType<unsigned int>(verticesout));
      }

      //Children of a shader element are either shader sources or parameters...
      xercesc::DOMNodeList* children = shaderElem->getChildNodes();
      for (XMLSize_t i = 0; i < children->getLength(); i++)
      {
         xercesc::DOMNode* node = children->item(i);

         if (node == NULL)
         {
            continue;
         }

         if (node->getNodeType() != xercesc::DOMNode::ELEMENT_NODE)
         {
            continue;
         }

         //If we got here, we have either a uniform element or a source element.
         xercesc::DOMElement* element = static_cast<xercesc::DOMElement*>(node);

         dtUtil::XMLStringConverter elemName(element->getTagName());

         if (elemName.ToString() == ShaderXML::SHADER_SOURCE_ELEMENT)
         {
            ParseShaderSourceElement(element,*newShader);
         }
         else if (elemName.ToString() == ShaderXML::PARAMETER_ELEMENT)
         {
            ParseParameterElement(element,*newShader);
         }
         else
         {
            throw ShaderXmlParserException("Foreign element found in shader XML source.", __FILE__, __LINE__);
         }
      }

      if (isDefault == "yes")
      {
         group.AddShader(*newShader,true);
      }
      else if (isEditor == "yes")
      {
         group.AddShader(*newShader,false, true);
      }
      else
      {
         group.AddShader(*newShader);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void ShaderXML::ParseShaderSourceElement(xercesc::DOMElement* sourceElem, ShaderProgram& shader)
   {
      std::string type = GetElementAttribute(*sourceElem,ShaderXML::SHADER_SOURCE_ATTRIBUTE_TYPE);

      //The source element has one child:  The text specifing the source file.
      xercesc::DOMNodeList* children = sourceElem->getChildNodes();

      if (children->getLength() != 1 || children->item(0)->getNodeType() != xercesc::DOMNode::TEXT_NODE)
      {
         throw ShaderXmlParserException(
            "Shader source should only have one child text element.", __FILE__, __LINE__);
      }

      xercesc::DOMText* file = static_cast<xercesc::DOMText*>(children->item(0));

      dtUtil::XMLStringConverter fileConverter(file->getNodeValue());

     if (type == ShaderXML::SHADER_SOURCE_TYPE_GEOMETRY)
     {
        std::string verticesOut = GetElementAttribute(*sourceElem, ShaderXML::SHADER_ATTRIBUTE_VERTICES_OUT);

        shader.AddGeometryShader(fileConverter.ToString());
     }
     else if (type == ShaderXML::SHADER_SOURCE_TYPE_VERTEX)
     {
        shader.AddVertexShader(fileConverter.ToString());
     }
     else if (type == ShaderXML::SHADER_SOURCE_TYPE_FRAGMENT)
     {
        shader.AddFragmentShader(fileConverter.ToString());
     }
   }

   /////////////////////////////////////////////////////////////////////////////
   void ShaderXML::ParseParameterElement(xercesc::DOMElement* paramElement, ShaderProgram& shader)
   {
      std::string paramName = GetElementAttribute(*paramElement,ShaderXML::PARAMETER_ATTRIBUTE_NAME);
      std::string isShared = GetElementAttribute(*paramElement,ShaderXML::PARAMETER_ATTRIBUTE_SHARED);
      xercesc::DOMNodeList* children = paramElement->getChildNodes();

      for (XMLSize_t i = 0; i < children->getLength(); i++)
      {
         xercesc::DOMNode* node = children->item(i);

         if (node == NULL)
         {
            continue;
         }

         if (node->getNodeType() != xercesc::DOMNode::ELEMENT_NODE)
         {
            continue;
         }

         //If we got here, then we have some sort of shader parameter specialization.
         xercesc::DOMElement *typeElement = static_cast<xercesc::DOMElement*>(node);
         dtCore::RefPtr<ShaderParameter> newParam = NULL;

         dtUtil::XMLStringConverter paramType(typeElement->getTagName());
         std::string toString = paramType.ToString();
         if (toString == ShaderXML::TEXTURE2D_ELEMENT)
         {
            newParam = ParseTexture2DParameter(typeElement, paramName);
         }
         else if (toString == ShaderXML::TEXTURE3D_ELEMENT)
         {
            newParam = ParseTexture3DParameter(typeElement, paramName);
         }
         else if (toString == ShaderXML::TEXTURECUBEMAP_ELEMENT)
         {
            newParam = ParseTextureCubeMapParameter(typeElement, paramName);
         }
         else if (toString == ShaderXML::FLOAT_ELEMENT)
         {
            newParam = ParseFloatParameter(typeElement, paramName);
         }
         else if (toString == ShaderXML::VEC4_ELEMENT)
         {
            newParam = ParseVec4Parameter(typeElement, paramName);
         }
         else if (toString == ShaderXML::INT_ELEMENT)
         {
            newParam = ParseIntParameter(typeElement, paramName);
         }
         else if (toString == ShaderXML::OSCILLATOR_ELEMENT)
         {
            newParam = ParseFloatTimerParameter(typeElement, paramName);
         }
         else if (toString == ShaderXML::TEXTURE1D_ELEMENT)
         {
            newParam = ParseTexture1DParameter(typeElement, paramName);
         }
         else
         {
            throw ShaderXmlParserException(
            "Unknown parameter type element found: '"+toString+"'",
            __FILE__, __LINE__);
         }

         // Set shared
         if (!isShared.empty())
         {
            if (isShared == "yes")
            {
               newParam->SetShared(true);
            }
            else if (isShared == "no")
            {
               newParam->SetShared(false);
            }
            else
            {
               throw ShaderXmlParserException("Invalid option for 'shared' on parameter [" +
                  newParam->GetName() + "]. Shared is optional, to override the default for this parameter type, use 'yes' or 'no'.",
                  __FILE__, __LINE__);
            }
         }

         shader.AddParameter(*newParam);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<ShaderParameter> ShaderXML::ParseTexture1DParameter(
      xercesc::DOMElement* tex1DElem, const std::string& paramName)
   {
      // First get the texture unit attribute from the element.
      dtCore::RefPtr<ShaderParamTexture1D> newParam = new ShaderParamTexture1D(paramName);
      xercesc::DOMNodeList* children = tex1DElem->getChildNodes();

      // Get the integer for the texture unit.
      std::string texUnitString = GetElementAttribute(*tex1DElem, ShaderXML::TEXTURE1D_ATTRIBUTE_TEXUNIT);
      if (!texUnitString.empty())
      {
         std::istringstream ss;
         unsigned int texUnit;
         ss.str(texUnitString);
         ss >> texUnit;
         newParam->SetTextureUnit(texUnit);
      }

      //Now parse the individual child elements of the 2D texture element.
      for (XMLSize_t i = 0; i < children->getLength(); i++)
      {
         xercesc::DOMNode* node = children->item(i);

         if (node == NULL)
         {
            continue;
         }
         if (node->getNodeType() != xercesc::DOMNode::ELEMENT_NODE)
         {
            continue;
         }

         xercesc::DOMElement* texElement = static_cast<xercesc::DOMElement*>(node);

         dtUtil::XMLStringConverter elemNameConverter(texElement->getTagName());
         std::string elemName = elemNameConverter.ToString();
         if (elemName == ShaderXML::TEXTURE1D_SOURCE_ELEMENT)
         {
            // Source elements have a type attribute that specify whether or not the
            // texture source is to come from an image file or from some source generated
            // elsewhere.
            std::string sourceType = GetElementAttribute(*texElement,
               ShaderXML::TEXTURE1D_SOURCE_ATTRIBUTE_TYPE);

            if (sourceType == ShaderParamTexture::TextureSourceType::IMAGE.GetName())
            {
               xercesc::DOMNodeList* children = texElement->getChildNodes();
               if (children->getLength() != 1 ||
                  children->item(0)->getNodeType() != xercesc::DOMNode::TEXT_NODE)
               {
                  throw ShaderXmlParserException("Shader source should only have one child text element.", __FILE__, __LINE__);
               }

               xercesc::DOMText* file = static_cast<xercesc::DOMText*>(children->item(0));

               dtUtil::XMLStringConverter fileConverter(file->getNodeValue());
               newParam->SetTexture(fileConverter.ToString());
               newParam->SetTextureSourceType(ShaderParamTexture::TextureSourceType::IMAGE);
            }
            else if (sourceType == ShaderParamTexture::TextureSourceType::AUTO.GetName())
            {
               newParam->SetTextureSourceType(ShaderParamTexture::TextureSourceType::AUTO);
            }
            else
            {
               throw ShaderXmlParserException("Unknown texture1D source type.", __FILE__, __LINE__);
            }
         }
         else if (elemName == ShaderXML::TEXTURE1D_WRAP_ELEMENT)
         {
            std::string axis = GetElementAttribute(*texElement,ShaderXML::TEXTURE1D_WRAP_ATTRIBUTE_AXIS);
            std::string mode = GetElementAttribute(*texElement,ShaderXML::TEXTURE1D_WRAP_ATTRIBUTE_MODE);

            if (!axis.empty() && !mode.empty())
            {
               const ShaderParamTexture::AddressMode* wrapMode = GetTextureAddressMode(mode);
               const ShaderParamTexture::TextureAxis* texAxis  = GetTextureAxis(axis);

               if (wrapMode == NULL)
               {
                  throw ShaderXmlParserException( "Invalid address mode of: " + mode +
                     " specified for shader parameter: " + paramName, __FILE__, __LINE__);
               }
               if (texAxis == NULL)
               {
                  throw ShaderXmlParserException( "Invalid texture axis: " + axis +
                     " specified for shader parameter: " + paramName, __FILE__, __LINE__);
               }

               newParam->SetAddressMode(*texAxis, *wrapMode);
            }
         }
         else
         {
            throw ShaderXmlParserException(
               "Unknown element in Texture2D parameter.", __FILE__, __LINE__);
         }
      }

      return static_cast<ShaderParameter *>(newParam.get());
   }

   /////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<ShaderParameter> ShaderXML::ParseTexture2DParameter(
         xercesc::DOMElement* tex2DElem, const std::string& paramName)
   {
      //First get the texture unit attribute from the element.
      dtCore::RefPtr<ShaderParamTexture2D> newParam = new ShaderParamTexture2D(paramName);
      xercesc::DOMNodeList* children = tex2DElem->getChildNodes();

      //Get the integer for the texture unit.
      std::string texUnitString = GetElementAttribute(*tex2DElem, ShaderXML::TEXTURE2D_ATTRIBUTE_TEXUNIT);
      if (!texUnitString.empty())
      {
         std::istringstream ss;
         unsigned int texUnit;
         ss.str(texUnitString);
         ss >> texUnit;
         newParam->SetTextureUnit(texUnit);
      }

      //Now parse the individual child elements of the 2D texture element.
      for (XMLSize_t i = 0; i < children->getLength(); i++)
      {
         xercesc::DOMNode* node = children->item(i);

         if (node == NULL)
         {
            continue;
         }
         if (node->getNodeType() != xercesc::DOMNode::ELEMENT_NODE)
         {
            continue;
         }

         xercesc::DOMElement* texElement = static_cast<xercesc::DOMElement* >(node);

         dtUtil::XMLStringConverter elemNameConverter(texElement->getTagName());
         std::string elemName = elemNameConverter.ToString();
         if (elemName == ShaderXML::TEXTURE2D_SOURCE_ELEMENT)
         {
            //Source elements have a type attribute that specify whether or not the
            //texture source is to come from an image file or from some source generated
            //elsewhere.
            std::string sourceType = GetElementAttribute(*texElement,
                  ShaderXML::TEXTURE2D_SOURCE_ATTRIBUTE_TYPE);

            if (sourceType == ShaderParamTexture::TextureSourceType::IMAGE.GetName())
            {
               xercesc::DOMNodeList* children = texElement->getChildNodes();
               if (children->getLength() != 1 ||
                  children->item(0)->getNodeType() != xercesc::DOMNode::TEXT_NODE)
               {
                  throw ShaderXmlParserException("Shader source should only have one child text element.", __FILE__, __LINE__);
               }

               xercesc::DOMText* file = static_cast<xercesc::DOMText*>(children->item(0));

               dtUtil::XMLStringConverter fileConverter(file->getNodeValue());
               newParam->SetTexture(fileConverter.ToString());
               newParam->SetTextureSourceType(ShaderParamTexture::TextureSourceType::IMAGE);
            }
            else if (sourceType == ShaderParamTexture::TextureSourceType::AUTO.GetName())
            {
               newParam->SetTextureSourceType(ShaderParamTexture::TextureSourceType::AUTO);
            }
            else
            {
               throw ShaderXmlParserException("Unknown texture2D source type.", __FILE__, __LINE__);
            }
         }
         else if (elemName == ShaderXML::TEXTURE2D_WRAP_ELEMENT)
         {
            std::string axis = GetElementAttribute(*texElement,ShaderXML::TEXTURE2D_WRAP_ATTRIBUTE_AXIS);
            std::string mode = GetElementAttribute(*texElement,ShaderXML::TEXTURE2D_WRAP_ATTRIBUTE_MODE);

            if (!axis.empty() && !mode.empty())
            {
               const ShaderParamTexture::AddressMode* wrapMode = GetTextureAddressMode(mode);
               const ShaderParamTexture::TextureAxis* texAxis = GetTextureAxis(axis);

               if (wrapMode == NULL)
               {
                  throw ShaderXmlParserException("Invalid address mode of: " + mode +
                        " specified for shader parameter: " + paramName, __FILE__, __LINE__);
               }
               if (texAxis == NULL)
               {
                  throw ShaderXmlParserException("Invalid texture axis: " + axis +
                        " specified for shader parameter: " + paramName, __FILE__, __LINE__);
               }

               newParam->SetAddressMode(*texAxis, *wrapMode);
            }
         }
         else
         {
            throw ShaderXmlParserException(
               "Unknown element in Texture2D parameter.", __FILE__, __LINE__);
         }
      }

      return static_cast<ShaderParameter *>(newParam.get());
   }


   /////////////////////////////////////////////////////////////////////////////

   dtCore::RefPtr<ShaderParameter> ShaderXML::ParseTexture3DParameter(
         xercesc::DOMElement* tex3DElem, const std::string& paramName)
   {
      //First get the texture unit attribute from the element.
      dtCore::RefPtr<ShaderParamTexture3D> newParam = new ShaderParamTexture3D(paramName);
      xercesc::DOMNodeList* children = tex3DElem->getChildNodes();

      //Get the integer for the texture unit.
      std::string texUnitString = GetElementAttribute(*tex3DElem, ShaderXML::TEXTURE3D_ATTRIBUTE_TEXUNIT);
      if (!texUnitString.empty())
      {
         std::istringstream ss;
         unsigned int texUnit;
         ss.str(texUnitString);
         ss >> texUnit;
         newParam->SetTextureUnit(texUnit);
      }

      //Now parse the individual child elements of the 2D texture element.
      for (XMLSize_t i = 0; i < children->getLength(); i++)
      {
         xercesc::DOMNode* node = children->item(i);

         if (node == NULL)
         {
            continue;
         }
         if (node->getNodeType() != xercesc::DOMNode::ELEMENT_NODE)
         {
            continue;
         }

         xercesc::DOMElement* texElement = static_cast<xercesc::DOMElement*>(node);

         dtUtil::XMLStringConverter elemNameConverter(texElement->getTagName());
         std::string elemName = elemNameConverter.ToString();
         if (elemName == ShaderXML::TEXTURE2D_SOURCE_ELEMENT)
         {
            //Source elements have a type attribute that specify whether or not the
            //texture source is to come from an image file or from some source generated
            //elsewhere.
            std::string sourceType = GetElementAttribute(*texElement,
                  ShaderXML::TEXTURE2D_SOURCE_ATTRIBUTE_TYPE);

            if (sourceType == ShaderParamTexture::TextureSourceType::IMAGE.GetName())
            {
               xercesc::DOMNodeList* children = texElement->getChildNodes();
               if (children->getLength() != 1 ||
                  children->item(0)->getNodeType() != xercesc::DOMNode::TEXT_NODE)
               {
                  throw ShaderXmlParserException("Shader source should only have one child text element.", __FILE__, __LINE__);
               }

               xercesc::DOMText* file = static_cast<xercesc::DOMText*>(children->item(0));

               dtUtil::XMLStringConverter fileConverter(file->getNodeValue());
               newParam->SetTexture(fileConverter.ToString());
               newParam->SetTextureSourceType(ShaderParamTexture::TextureSourceType::IMAGE);
            }
            else if (sourceType == ShaderParamTexture::TextureSourceType::AUTO.GetName())
            {
               newParam->SetTextureSourceType(ShaderParamTexture::TextureSourceType::AUTO);
            }
            else
            {
               throw ShaderXmlParserException("Unknown texture3D source type.", __FILE__, __LINE__);
            }
         }
         else if (elemName == ShaderXML::TEXTURE3D_WRAP_ELEMENT)
         {
            std::string axis = GetElementAttribute(*texElement,ShaderXML::TEXTURE3D_WRAP_ATTRIBUTE_AXIS);
            std::string mode = GetElementAttribute(*texElement,ShaderXML::TEXTURE3D_WRAP_ATTRIBUTE_MODE);

            if (!axis.empty() && !mode.empty())
            {
               const ShaderParamTexture::AddressMode* wrapMode = GetTextureAddressMode(mode);
               const ShaderParamTexture::TextureAxis* texAxis = GetTextureAxis(axis);

               if (wrapMode == NULL)
               {
                  throw ShaderXmlParserException("Invalid address mode of: " + mode +
                        " specified for shader parameter: " + paramName, __FILE__, __LINE__);
               }
               if (texAxis == NULL)
               {
                  throw ShaderXmlParserException("Invalid texture axis: " + axis +
                        " specified for shader parameter: " + paramName, __FILE__, __LINE__);
               }

               newParam->SetAddressMode(*texAxis, *wrapMode);
            }
         }
         else
         {
            throw ShaderXmlParserException(
               "Unknown element in Texture2D parameter.", __FILE__, __LINE__);
         }
      }

      return static_cast<ShaderParameter *>(newParam.get());
   }


   /////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<ShaderParameter> ShaderXML::ParseTextureCubeMapParameter(
         xercesc::DOMElement* texCubeMapElem, const std::string& paramName)
   {
      //First get the texture unit attribute from the element.
      dtCore::RefPtr<ShaderParamTextureCubeMap> newParam = new ShaderParamTextureCubeMap(paramName);
      xercesc::DOMNodeList* children = texCubeMapElem->getChildNodes();

      //Get the integer for the texture unit.
      std::string texUnitString = GetElementAttribute(*texCubeMapElem, ShaderXML::TEXTURECUBEMAP_ATTRIBUTE_TEXUNIT);
      if (!texUnitString.empty())
      {
         std::istringstream ss;
         unsigned int texUnit;
         ss.str(texUnitString);
         ss >> texUnit;
         newParam->SetTextureUnit(texUnit);
      }

      //Now parse the individual child elements of the 2D texture element.
      for (XMLSize_t i = 0; i < children->getLength(); i++)
      {
         xercesc::DOMNode* node = children->item(i);

         if (node == NULL)
         {
            continue;
         }
         if (node->getNodeType() != xercesc::DOMNode::ELEMENT_NODE)
         {
            continue;
         }

         xercesc::DOMElement* texElement = static_cast<xercesc::DOMElement* >(node);

         dtUtil::XMLStringConverter elemNameConverter(texElement->getTagName());
         std::string elemName = elemNameConverter.ToString();
         if (elemName == ShaderXML::TEXTURECUBEMAP_SOURCE_ELEMENT)
         {
            //Source elements have a type attribute that specify whether or not the
            //texture source is to come from an image file or from some source generated
            //elsewhere.
            std::string sourceType = GetElementAttribute(*texElement,
                  ShaderXML::TEXTURECUBEMAP_SOURCE_ATTRIBUTE_TYPE);

            if (sourceType == ShaderXML::TEXTURECUBEMAP_IMAGE_POSITIVE_X ||
                sourceType == ShaderXML::TEXTURECUBEMAP_IMAGE_NEGATIVE_X ||
                sourceType == ShaderXML::TEXTURECUBEMAP_IMAGE_POSITIVE_Y ||
                sourceType == ShaderXML::TEXTURECUBEMAP_IMAGE_NEGATIVE_Y ||
                sourceType == ShaderXML::TEXTURECUBEMAP_IMAGE_POSITIVE_Z ||
                sourceType == ShaderXML::TEXTURECUBEMAP_IMAGE_NEGATIVE_Z
               )
            {
               xercesc::DOMNodeList* children = texElement->getChildNodes();
               if (children->getLength() != 1 ||
                  children->item(0)->getNodeType() != xercesc::DOMNode::TEXT_NODE)
               {
                  throw ShaderXmlParserException("Shader source should only have one child text element.", __FILE__, __LINE__);
               }

               xercesc::DOMText* file = static_cast<xercesc::DOMText*>(children->item(0));

               dtUtil::XMLStringConverter fileConverter(file->getNodeValue());
               
               if(sourceType == ShaderXML::TEXTURECUBEMAP_IMAGE_POSITIVE_X)
               {
                  newParam->SetTextureSourceType(ShaderParamTexture::TextureSourceType::CUBEMAP_IMAGE_POSITIVE_X);
               }
               else if(sourceType == ShaderXML::TEXTURECUBEMAP_IMAGE_NEGATIVE_X)
               {
                  newParam->SetTextureSourceType(ShaderParamTexture::TextureSourceType::CUBEMAP_IMAGE_NEGATIVE_X);
               }
               else if(sourceType == ShaderXML::TEXTURECUBEMAP_IMAGE_POSITIVE_Y)
               {
                  newParam->SetTextureSourceType(ShaderParamTexture::TextureSourceType::CUBEMAP_IMAGE_POSITIVE_Y);
               }
               else if(sourceType == ShaderXML::TEXTURECUBEMAP_IMAGE_NEGATIVE_Y)
               {
                  newParam->SetTextureSourceType(ShaderParamTexture::TextureSourceType::CUBEMAP_IMAGE_NEGATIVE_Y);
               }
               else if(sourceType == ShaderXML::TEXTURECUBEMAP_IMAGE_POSITIVE_Z)
               {
                  newParam->SetTextureSourceType(ShaderParamTexture::TextureSourceType::CUBEMAP_IMAGE_POSITIVE_Z);
               }
               else if(sourceType == ShaderXML::TEXTURECUBEMAP_IMAGE_NEGATIVE_Z)
               {
                  newParam->SetTextureSourceType(ShaderParamTexture::TextureSourceType::CUBEMAP_IMAGE_NEGATIVE_Z);
               }
               newParam->SetTexture(fileConverter.ToString());
            }
            else if (sourceType == ShaderParamTexture::TextureSourceType::AUTO.GetName())
            {
               newParam->SetTextureSourceType(ShaderParamTexture::TextureSourceType::AUTO);
            }
            else
            {
               throw ShaderXmlParserException("Unknown texture3D source type.", __FILE__, __LINE__);
            }
         }
         else if (elemName == ShaderXML::TEXTURECUBEMAP_WRAP_ELEMENT)
         {
            std::string axis = GetElementAttribute(*texElement,ShaderXML::TEXTURECUBEMAP_WRAP_ATTRIBUTE_AXIS);
            std::string mode = GetElementAttribute(*texElement,ShaderXML::TEXTURECUBEMAP_WRAP_ATTRIBUTE_MODE);

            if (!axis.empty() && !mode.empty())
            {
               const ShaderParamTexture::AddressMode* wrapMode = GetTextureAddressMode(mode);
               const ShaderParamTexture::TextureAxis* texAxis = GetTextureAxis(axis);

               if (wrapMode == NULL)
               {
                  throw ShaderXmlParserException("Invalid address mode of: " + mode +
                        " specified for shader parameter: " + paramName, __FILE__, __LINE__);
               }
               if (texAxis == NULL)
               {
                  throw ShaderXmlParserException("Invalid texture axis: " + axis +
                        " specified for shader parameter: " + paramName, __FILE__, __LINE__);
               }

               newParam->SetAddressMode(*texAxis, *wrapMode);
            }
         }
         else
         {
            throw ShaderXmlParserException(
               "Unknown element in TextureCubeMap parameter.", __FILE__, __LINE__);
         }
      }

      return static_cast<ShaderParameter *>(newParam.get());
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<ShaderParameter> ShaderXML::ParseFloatParameter(xercesc::DOMElement* floatElem,
         const std::string& paramName)
   {
      dtCore::RefPtr<ShaderParamFloat> newParam = new ShaderParamFloat(paramName);

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
   dtCore::RefPtr<ShaderParameter> ShaderXML::ParseVec4Parameter(xercesc::DOMElement* vec4Elem,
         const std::string& paramName)
   {
      dtCore::RefPtr<ShaderParamVec4> newParam = new ShaderParamVec4(paramName);

      std::string value = GetElementAttribute(*vec4Elem, ShaderXML::PARAM_ELEMENT_ATTRIBUTE_DEFAULTVALUE);
      if (!value.empty())
      {
         std::istringstream ss;
         osg::Vec4 defaultValue;
         ss.str(value);
         ss >> defaultValue.x();
         ss >> defaultValue.y();
         ss >> defaultValue.z();
         ss >> defaultValue.w();
         newParam->SetValue(defaultValue);
      }

      return static_cast<ShaderParameter*>(newParam.get());

   }

   ///////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<ShaderParameter> ShaderXML::ParseFloatTimerParameter(xercesc::DOMElement* timerElem,
      const std::string &paramName)
   {
      std::string valueString;
      float tempValue;
      dtCore::RefPtr<ShaderParamOscillator> newParam = new ShaderParamOscillator(paramName);

      // OFFSET
      valueString = GetElementAttribute(*timerElem, ShaderXML::OSCILLATOR_ATTRIB_OFFSET);
      if (!valueString.empty())
      {
         std::istringstream ss;
         ss.str(valueString);
         ss >> tempValue;
         newParam->SetOffset(tempValue);
      }

      // RANGEMIN
      valueString = GetElementAttribute(*timerElem, ShaderXML::OSCILLATOR_ATTRIB_RANGE_MIN);
      if (!valueString.empty())
      {
         std::istringstream ss;
         ss.str(valueString);
         ss >> tempValue;
         newParam->SetRangeMin(tempValue);
      }

      // RANGEMAX
      valueString = GetElementAttribute(*timerElem, ShaderXML::OSCILLATOR_ATTRIB_RANGE_MAX);
      if (!valueString.empty())
      {
         std::istringstream ss;
         ss.str(valueString);
         ss >> tempValue;
         newParam->SetRangeMax(tempValue);
      }

      // CYCLE TIME MIN
      valueString = GetElementAttribute(*timerElem, ShaderXML::OSCILLATOR_ATTRIB_CYCLETIME_MIN);
      if (!valueString.empty())
      {
         std::istringstream ss;
         ss.str(valueString);
         ss >> tempValue;
         newParam->SetCycleTimeMin(tempValue);
      }

      // CYCLE TIME MAX
      valueString = GetElementAttribute(*timerElem, ShaderXML::OSCILLATOR_ATTRIB_CYCLETIME_MAX);
      if (!valueString.empty())
      {
         std::istringstream ss;
         ss.str(valueString);
         ss >> tempValue;
         newParam->SetCycleTimeMax(tempValue);
      }

      // CYCLE COUNT
      valueString = GetElementAttribute(*timerElem, ShaderXML::OSCILLATOR_ATTRIB_CYCLE_COUNT);
      if (!valueString.empty())
      {
         int tempIntValue;
         std::istringstream ss;
         ss.str(valueString);
         ss >> tempIntValue;
         newParam->SetCycleCountTotal(tempIntValue);
      }

      // TRIGGER
      valueString = GetElementAttribute(*timerElem, ShaderXML::OSCILLATOR_ATTRIB_TRIGGER);
      if (!valueString.empty())
      {
         if (valueString == ShaderParamOscillator::OscillationTrigger::AUTO.GetName())
         {
            newParam->SetOscillationTrigger(ShaderParamOscillator::OscillationTrigger::AUTO);
         }
         else if (valueString == ShaderParamOscillator::OscillationTrigger::MANUAL.GetName())
         {
            newParam->SetOscillationTrigger(ShaderParamOscillator::OscillationTrigger::MANUAL);
         }
         else
         {
            std::ostringstream error;
            error << "Error parseing floattimer on parameter [" << paramName <<
               "] for trigger attribute [" << valueString << "]. Should be 'auto' or 'manual'.";
            throw ShaderXmlParserException( error.str(), __FILE__, __LINE__);
         }
      }

      // USE REAL TIME
      valueString = GetElementAttribute(*timerElem,ShaderXML::OSCILLATOR_ATTRIB_USEREALTIME);
      if (!valueString.empty())
      {
         if (valueString == "true")
         {
            newParam->SetUseRealTime(true);
         }
         else if (valueString == "false")
         {
            newParam->SetUseRealTime(false);
         }
         else
         {
            std::ostringstream error;
            error << "Error parsing floattimer on parameter [" << paramName <<
               "] for userealtime attribute [" << valueString << "].  Should be 'true' or 'false'.";
            throw ShaderXmlParserException(error.str(), __FILE__, __LINE__);
         }
      }

      // OSCILLATION
      valueString = GetElementAttribute(*timerElem,ShaderXML::OSCILLATOR_ATTRIB_OSCILLATION_TYPE);
      if (!valueString.empty())
      {
         if (valueString == ShaderParamOscillator::OscillationType::UP.GetName())
         {
            newParam->SetOscillationType(ShaderParamOscillator::OscillationType::UP);
         }
         else if (valueString == ShaderParamOscillator::OscillationType::DOWN.GetName())
         {
            newParam->SetOscillationType(ShaderParamOscillator::OscillationType::DOWN);
         }
         else if (valueString == ShaderParamOscillator::OscillationType::UPANDDOWN.GetName())
         {
            newParam->SetOscillationType(ShaderParamOscillator::OscillationType::UPANDDOWN);
         }
         else if (valueString == ShaderParamOscillator::OscillationType::DOWNANDUP.GetName())
         {
            newParam->SetOscillationType(ShaderParamOscillator::OscillationType::DOWNANDUP);
         }
         else
         {
            std::ostringstream error;
            error << "Error parsing floattimer on parameter [" << paramName <<
               "] for oscillation attribute [" << valueString << "].  Should be 'Up', 'Down', 'UpAndDown', or 'DownAndUp'.";
            throw ShaderXmlParserException(error.str(), __FILE__, __LINE__);
         }
      }

      return static_cast<ShaderParameter*>(newParam.get());
   }


   /////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<ShaderParameter> ShaderXML::ParseIntParameter(xercesc::DOMElement* intElem,
         const std::string& paramName)
   {
      dtCore::RefPtr<ShaderParamInt> newParam = new ShaderParamInt(paramName);

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

   /////////////////////////////////////////////////////////////////////////////
   std::string ShaderXML::GetElementAttribute(xercesc::DOMElement& element, const std::string& attribName)
   {
      XMLCh* xmlChar = xercesc::XMLString::transcode(attribName.c_str());
      dtUtil::XMLStringConverter converter(element.getAttribute(xmlChar));
      xercesc::XMLString::release(&xmlChar);

      return converter.ToString();
   }

   /////////////////////////////////////////////////////////////////////////////
   const ShaderParamTexture::AddressMode* ShaderXML::GetTextureAddressMode(const std::string& mode)
   {
      if (ShaderParamTexture::AddressMode::CLAMP == mode)
      {
         return &ShaderParamTexture::AddressMode::CLAMP;
      }
      else if (ShaderParamTexture::AddressMode::REPEAT == mode)
      {
         return &ShaderParamTexture::AddressMode::REPEAT;
      }
      else if (ShaderParamTexture::AddressMode::MIRROR == mode)
      {
         return &ShaderParamTexture::AddressMode::MIRROR;
      }
      else
      {
         return NULL;
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   const ShaderParamTexture::TextureAxis* ShaderXML::GetTextureAxis(const std::string& axis)
   {
      if (ShaderParamTexture::TextureAxis::S == axis)
      {
         return &ShaderParamTexture::TextureAxis::S;
      }
      else if (ShaderParamTexture::TextureAxis::T == axis)
      {
         return &ShaderParamTexture::TextureAxis::T;
      }
      else if (ShaderParamTexture::TextureAxis::R == axis)
      {
         return &ShaderParamTexture::TextureAxis::R;
      }
      else if (ShaderParamTexture::TextureAxis::Q == axis)
      {
         return &ShaderParamTexture::TextureAxis::Q;
      }
      else
      {
         return NULL;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   const ShaderXML::ShaderContainer& ShaderXML::GetLoadedShaders() const
   {
      return mShaderGroupContainer;
   }
}
