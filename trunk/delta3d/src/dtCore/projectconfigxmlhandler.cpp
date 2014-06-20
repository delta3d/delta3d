/* -*-c++-*-
 * Delta3D
 * Copyright 2011, Alion Science and Technology
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
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 * David Guthrie
 */

#include <prefix/dtcoreprefix.h>
#include <dtCore/projectconfigxmlhandler.h>
#include <dtCore/exceptionenum.h>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/sax/SAXParseException.hpp>

#include <dtUtil/xercesutils.h>
#include <dtUtil/stringutils.h>

namespace dtCore
{
   class ProjectConfigXMLConstantsInit
   {
   public:
      ProjectConfigXMLConstantsInit() { ProjectConfigXMLConstants::StaticInit(); }
      ~ProjectConfigXMLConstantsInit() { ProjectConfigXMLConstants::StaticShutdown(); }
   };

   const char* const ProjectConfigXMLConstants::SCHEMA_VERSION = "1.0";

   XMLCh* ProjectConfigXMLConstants::PROJECT_CONFIG_ELEMENT = NULL;
   XMLCh* ProjectConfigXMLConstants::PROJECT_CONFIG_NAMESPACE = NULL;
   XMLCh* ProjectConfigXMLConstants::HEADER_ELEMENT = NULL;
   XMLCh* ProjectConfigXMLConstants::NAME_ELEMENT = NULL;
   XMLCh* ProjectConfigXMLConstants::DESCRIPTION_ELEMENT = NULL;
   XMLCh* ProjectConfigXMLConstants::AUTHOR_ELEMENT = NULL;
   XMLCh* ProjectConfigXMLConstants::COMMENT_ELEMENT = NULL;
   XMLCh* ProjectConfigXMLConstants::COPYRIGHT_ELEMENT = NULL;
   XMLCh* ProjectConfigXMLConstants::SCHEMA_VERSION_ELEMENT = NULL;

   XMLCh* ProjectConfigXMLConstants::CONTEXTS_ELEMENT = NULL;
   XMLCh* ProjectConfigXMLConstants::READ_ONLY_ATTRIBUTE = NULL;
   XMLCh* ProjectConfigXMLConstants::CONTEXT_ELEMENT = NULL;
   XMLCh* ProjectConfigXMLConstants::PATH_ATTRIBUTE = NULL;

   static ProjectConfigXMLConstantsInit gInitMe;

   void ProjectConfigXMLConstants::StaticInit()
   {
      PROJECT_CONFIG_ELEMENT = xercesc::XMLString::transcode("projectConfig");
      PROJECT_CONFIG_NAMESPACE = xercesc::XMLString::transcode("xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"project_config.xsd\"");
      HEADER_ELEMENT = xercesc::XMLString::transcode("header");
      NAME_ELEMENT = xercesc::XMLString::transcode("name");
      DESCRIPTION_ELEMENT = xercesc::XMLString::transcode("description");
      AUTHOR_ELEMENT = xercesc::XMLString::transcode("author");
      COMMENT_ELEMENT = xercesc::XMLString::transcode("comment");
      COPYRIGHT_ELEMENT = xercesc::XMLString::transcode("copyright");
      SCHEMA_VERSION_ELEMENT = xercesc::XMLString::transcode("schemaVersion");

      CONTEXTS_ELEMENT = xercesc::XMLString::transcode("contexts");
      READ_ONLY_ATTRIBUTE = xercesc::XMLString::transcode("readOnly");
      CONTEXT_ELEMENT = xercesc::XMLString::transcode("context");
      PATH_ATTRIBUTE = xercesc::XMLString::transcode("path");
   }

   void ProjectConfigXMLConstants::StaticShutdown()
   {
      xercesc::XMLString::release(&PROJECT_CONFIG_ELEMENT);
      xercesc::XMLString::release(&PROJECT_CONFIG_NAMESPACE);
      xercesc::XMLString::release(&HEADER_ELEMENT);
      xercesc::XMLString::release(&NAME_ELEMENT);
      xercesc::XMLString::release(&DESCRIPTION_ELEMENT);
      xercesc::XMLString::release(&AUTHOR_ELEMENT);
      xercesc::XMLString::release(&COMMENT_ELEMENT);
      xercesc::XMLString::release(&COPYRIGHT_ELEMENT);
      xercesc::XMLString::release(&SCHEMA_VERSION_ELEMENT);

      xercesc::XMLString::release(&CONTEXTS_ELEMENT);
      xercesc::XMLString::release(&READ_ONLY_ATTRIBUTE);
      xercesc::XMLString::release(&CONTEXT_ELEMENT);
      xercesc::XMLString::release(&PATH_ATTRIBUTE);
   }

   //////////////////////////////////////////////////////////////////////////
   ProjectConfigXMLHandler::ParsingData::ParsingData()
   : mInHeader(false)
   , mInContexts(false)
   {

   }

   //////////////////////////////////////////////////////////////////////////
   ProjectConfigXMLHandler::ProjectConfigXMLHandler()
   : mConfig(new ProjectConfig)
   {
   }

   //////////////////////////////////////////////////////////////////////////
   ProjectConfigXMLHandler::~ProjectConfigXMLHandler()
   {
   }

   //////////////////////////////////////////////////////////////////////////
   ProjectConfig& ProjectConfigXMLHandler::GetProjectConfig()
   {
      return *mConfig;
   }

   //////////////////////////////////////////////////////////////////////////
   void ProjectConfigXMLHandler::CombinedCharacters(const XMLCh* const chars, size_t length)
   {
      BaseXMLHandler::CombinedCharacters(chars, length);

      xmlCharString& topEl = mElements.top();

      if (mData.mInHeader)
      {
         if (topEl == ProjectConfigXMLConstants::NAME_ELEMENT)
         {
            mConfig->SetName(dtUtil::XMLStringConverter(chars).ToString());
         }
         else if (topEl == ProjectConfigXMLConstants::DESCRIPTION_ELEMENT)
         {
            mConfig->SetDescription(dtUtil::XMLStringConverter(chars).ToString());
         }
         else if (topEl == ProjectConfigXMLConstants::AUTHOR_ELEMENT)
         {
            mConfig->SetAuthor(dtUtil::XMLStringConverter(chars).ToString());
         }
         else if (topEl == ProjectConfigXMLConstants::COMMENT_ELEMENT)
         {
            mConfig->SetComment(dtUtil::XMLStringConverter(chars).ToString());
         }
         else if (topEl == ProjectConfigXMLConstants::COPYRIGHT_ELEMENT)
         {
            mConfig->SetCopyright(dtUtil::XMLStringConverter(chars).ToString());
         }
         else if (topEl == ProjectConfigXMLConstants::SCHEMA_VERSION_ELEMENT)
         {
            // ignored for now.
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void ProjectConfigXMLHandler::ElementStarted(
      const XMLCh* const uri,
      const XMLCh* const localname,
      const XMLCh* const qname,
      const xercesc::Attributes& attrs)
   {
      BaseXMLHandler::ElementStarted(uri, localname, qname, attrs);
      if (!mData.mInHeader && !mData.mInContexts)
      {
         if (xercesc::XMLString::compareString(localname, ProjectConfigXMLConstants::HEADER_ELEMENT) == 0)
         {
            mData.mInHeader = true;
         }
         else if (xercesc::XMLString::compareString(localname, ProjectConfigXMLConstants::CONTEXTS_ELEMENT) == 0)
         {
            mData.mInContexts = true;

            const XMLCh* readOnlyStr = attrs.getValue(ProjectConfigXMLConstants::READ_ONLY_ATTRIBUTE);
            if (readOnlyStr != NULL)
            {
               bool readOnly = dtUtil::ToType<bool>(dtUtil::XMLStringConverter(readOnlyStr).ToString());
               mConfig->SetReadOnly(readOnly);
            }
         }
      }
      else if (mData.mInContexts)
      {
         if (xercesc::XMLString::compareString(localname, ProjectConfigXMLConstants::CONTEXT_ELEMENT) == 0)
         {
            const XMLCh* pathAttrXmlCh = attrs.getValue(ProjectConfigXMLConstants::PATH_ATTRIBUTE);
            if (pathAttrXmlCh != NULL)
            {
               std::string path = dtUtil::XMLStringConverter(pathAttrXmlCh).ToString();
               mConfig->AddContextData(ContextData(path));
            }
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void ProjectConfigXMLHandler::ElementEnded(
      const XMLCh* const uri,
      const XMLCh* const localname,
      const XMLCh* const qname)
   {
      BaseXMLHandler::ElementEnded(uri, localname, qname);
      if (mData.mInHeader && xercesc::XMLString::compareString(localname, ProjectConfigXMLConstants::HEADER_ELEMENT) == 0)
      {
         mData.mInHeader = false;
      }
      else if (mData.mInContexts && xercesc::XMLString::compareString(localname, ProjectConfigXMLConstants::CONTEXTS_ELEMENT) == 0)
      {
         mData.mInContexts = false;
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void ProjectConfigXMLHandler::Reset()
   {
      BaseXMLHandler::Reset();
      mData.mInHeader = false;
      mData.mInContexts = false;

      mConfig = NULL;
      mConfig = new ProjectConfig();
   }

   //////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////
   ProjectConfigXMLWriter::ProjectConfigXMLWriter()
   {
   }

   //////////////////////////////////////////////////////////////////////////
   void ProjectConfigXMLWriter::Save(const ProjectConfig& projConf, std::ostream& outStr)
   {
      mFormatTarget.SetOutputStream(&outStr);

      try {

         WriteHeader();

         BeginElement(ProjectConfigXMLConstants::PROJECT_CONFIG_ELEMENT, ProjectConfigXMLConstants::PROJECT_CONFIG_NAMESPACE);
         BeginElement(ProjectConfigXMLConstants::HEADER_ELEMENT);
         BeginElement(ProjectConfigXMLConstants::NAME_ELEMENT);
         AddCharacters(projConf.GetName());
         EndElement(); // End Project Config Name Element.
         BeginElement(ProjectConfigXMLConstants::DESCRIPTION_ELEMENT);
         AddCharacters(projConf.GetDescription());
         EndElement(); // End Description Element.
         BeginElement(ProjectConfigXMLConstants::AUTHOR_ELEMENT);
         AddCharacters(projConf.GetAuthor());
         EndElement(); // End Author Element.
         BeginElement(ProjectConfigXMLConstants::COMMENT_ELEMENT);
         AddCharacters(projConf.GetComment());
         EndElement(); // End Comment Element.
         BeginElement(ProjectConfigXMLConstants::COPYRIGHT_ELEMENT);
         AddCharacters(projConf.GetCopyright());
         EndElement(); // End Copyright Element.
         BeginElement(ProjectConfigXMLConstants::SCHEMA_VERSION_ELEMENT);
         AddCharacters(std::string(ProjectConfigXMLConstants::SCHEMA_VERSION));
         EndElement(); // End Schema Version Element.
         EndElement(); // End Header Element.

         XMLCh* stringReadOnly = xercesc::XMLString::transcode((std::string("readOnly=\"") + dtUtil::ToString(projConf.GetReadOnly()) + "\"").c_str());
         BeginElement(ProjectConfigXMLConstants::CONTEXTS_ELEMENT, stringReadOnly);
         xercesc::XMLString::release(&stringReadOnly);
         for (unsigned i = 0; i < projConf.GetNumContextData(); ++i)
         {
            XMLCh* stringX = xercesc::XMLString::transcode((std::string("path=\"") + projConf.GetContextData(i).GetPath() + "\"").c_str());
            BeginElement(ProjectConfigXMLConstants::CONTEXT_ELEMENT, stringX, true);
            xercesc::XMLString::release(&stringX);
         }
         EndElement(); // End Contexts Element.
         EndElement(); // End Project Config Element.
      }
      catch (dtUtil::Exception& ex)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                             "Caught Exception \"%s\" while attempting to save project config \"%s\".",
                             ex.What().c_str(), projConf.GetName().c_str());
         mFormatTarget.SetOutputStream(NULL);
         throw;
      }
      catch (...)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                             "Unknown exception while attempting to save map \"%s\".",
                             projConf.GetName().c_str());
         mFormatTarget.SetOutputStream(NULL);
         throw dtCore::MapSaveException( std::string("Unknown exception saving map \"") + projConf.GetName() + ("\"."), __FILE__, __LINE__);
      }

   }


   //////////////////////////////////////////////////////////////////////////
   ProjectConfigXMLWriter::~ProjectConfigXMLWriter()
   {
   }
}
