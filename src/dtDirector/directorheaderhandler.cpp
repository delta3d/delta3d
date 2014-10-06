/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2011, MOVES Institute
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
 * Erik Johnson
 */

#include <prefix/dtcoreprefix.h>
#include <dtDirector/directorheaderhandler.h>
#include <dtCore/mapxmlconstants.h>
#include <dtUtil/xercesutils.h>
#include <xercesc/util/XMLString.hpp>

XERCES_CPP_NAMESPACE_USE
using namespace dtDirector;

////////////////////////////////////////////////////////////////////////////////
dtDirector::DirectorHeaderHandler::DirectorHeaderHandler()
: mIsHeaderParsed(false)
{
}

////////////////////////////////////////////////////////////////////////////////
dtDirector::DirectorHeaderHandler::~DirectorHeaderHandler()
{
}

////////////////////////////////////////////////////////////////////////////////
const DirectorHeaderData& DirectorHeaderHandler::GetHeaderData() const
{
   return mHeaderData;
}

////////////////////////////////////////////////////////////////////////////////
bool DirectorHeaderHandler::HandledDesiredData() const
{
   return mIsHeaderParsed;
}

////////////////////////////////////////////////////////////////////////////////
void DirectorHeaderHandler::ElementEnded(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname)
{
   BaseXMLHandler::ElementEnded(uri, localname, qname);

   if (XMLString::compareString(localname, dtCore::MapXMLConstants::HEADER_ELEMENT) == 0)
   {
      mIsHeaderParsed = true;
   }
}

//////////////////////////////////////////////////////////////////////////
void DirectorHeaderHandler::CombinedCharacters(const XMLCh* const chars, size_t length)
{
   BaseXMLHandler::CombinedCharacters(chars, length);

   xmlCharString& topEl = mElements.top();

   const std::string str = dtUtil::XMLStringConverter(chars).ToString();

   if (topEl == dtCore::MapXMLConstants::NAME_ELEMENT)
   {
      mPropertyName = str;
   }
   else if (topEl == dtCore::MapXMLConstants::ACTOR_PROPERTY_STRING_ELEMENT)
   {
      if (mPropertyName == "Name")
      {
         mHeaderData.mName = str;
      }
      if (mPropertyName == "Description")
      {
         mHeaderData.mDescription = str;
      }
      if (mPropertyName == "Author")
      {
         mHeaderData.mAuthor = str;
      }
      if (mPropertyName == "Comment")
      {
         mHeaderData.mComment = str;
      }
      if (mPropertyName == "Copyright")
      {
         mHeaderData.mCopyright = str;
      }
   }
   else if (topEl == dtCore::MapXMLConstants::CREATE_TIMESTAMP_ELEMENT)
   {
      mHeaderData.mCreateTime = str;
   }
   else if (topEl == dtCore::MapXMLConstants::LAST_UPDATE_TIMESTAMP_ELEMENT)
   {
      mHeaderData.mLastUpdateTime = str;
   }
   else if (topEl == dtCore::MapXMLConstants::EDITOR_VERSION_ELEMENT)
   {
      mHeaderData.mEditorVersion = str;
   }
   else if (topEl == dtCore::MapXMLConstants::SCHEMA_VERSION_ELEMENT)
   {
      mHeaderData.mSchemaVersion = str;
   }
   else if (topEl == dtCore::MapXMLConstants::DIRECTOR_SCRIPT_TYPE)
   {
      mHeaderData.mScriptType = str;
   }
   else if (topEl == dtCore::MapXMLConstants::DIRECTOR_IMPORTED_SCRIPT)
   {
      mHeaderData.mImportedScriptList.push_back(str);
   }
}

//////////////////////////////////////////////////////////////////////////
