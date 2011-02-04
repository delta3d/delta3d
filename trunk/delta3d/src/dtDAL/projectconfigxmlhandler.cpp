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

#include <dtDAL/projectconfigxmlhandler.h>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/sax/SAXParseException.hpp>

#include <dtUtil/xercesutils.h>
#include <dtUtil/stringutils.h>

namespace dtDAL
{
   class ProjectConfigXMLConstantsInit
   {
   public:
      ProjectConfigXMLConstantsInit() { ProjectConfigXMLConstants::StaticInit(); }
      ~ProjectConfigXMLConstantsInit() { ProjectConfigXMLConstants::StaticShutdown(); }
   };


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
      HEADER_ELEMENT = xercesc::XMLString::transcode("header");
      NAME_ELEMENT = xercesc::XMLString::transcode("name");
      DESCRIPTION_ELEMENT = xercesc::XMLString::transcode("description");
      AUTHOR_ELEMENT = xercesc::XMLString::transcode("author");
      COMMENT_ELEMENT = xercesc::XMLString::transcode("comment");
      COPYRIGHT_ELEMENT = xercesc::XMLString::transcode("copyright");
      SCHEMA_VERSION_ELEMENT = xercesc::XMLString::transcode("schemaVersion");

      CONTEXTS_ELEMENT = xercesc::XMLString::transcode("contexts");
      READ_ONLY_ATTRIBUTE = xercesc::XMLString::transcode("readOnly");
      CONTEXTS_ELEMENT = xercesc::XMLString::transcode("context");
      PATH_ATTRIBUTE = xercesc::XMLString::transcode("path");
   }

   void ProjectConfigXMLConstants::StaticShutdown()
   {
      xercesc::XMLString::release(&HEADER_ELEMENT);
      xercesc::XMLString::release(&NAME_ELEMENT);
      xercesc::XMLString::release(&DESCRIPTION_ELEMENT);
      xercesc::XMLString::release(&AUTHOR_ELEMENT);
      xercesc::XMLString::release(&COMMENT_ELEMENT);
      xercesc::XMLString::release(&COPYRIGHT_ELEMENT);
      xercesc::XMLString::release(&SCHEMA_VERSION_ELEMENT);

      xercesc::XMLString::release(&CONTEXTS_ELEMENT);
      xercesc::XMLString::release(&READ_ONLY_ATTRIBUTE);
      xercesc::XMLString::release(&CONTEXTS_ELEMENT);
      xercesc::XMLString::release(&PATH_ATTRIBUTE);
   }

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
   void ProjectConfigXMLHandler::characters(const XMLCh* const chars, const XMLSize_t length)
   {
      BaseXMLHandler::characters(chars, length);

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

            mConfig->SetReadOnly(true);

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


}
