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

#include <prefix/dtdalprefix.h>
#include <dtCore/mapheaderhandler.h>
#include <dtCore/mapxmlconstants.h>
#include <dtUtil/xercesutils.h>
#include <xercesc/util/XMLString.hpp>

XERCES_CPP_NAMESPACE_USE
using namespace dtCore;

////////////////////////////////////////////////////////////////////////////////
dtCore::MapHeaderHandler::MapHeaderHandler()
: mIsHeaderParsed(false)
{
}

////////////////////////////////////////////////////////////////////////////////
dtCore::MapHeaderHandler::~MapHeaderHandler()
{
}

////////////////////////////////////////////////////////////////////////////////
const MapHeaderData& MapHeaderHandler::GetHeaderData() const
{
   return mHeaderData;
}

////////////////////////////////////////////////////////////////////////////////
void MapHeaderHandler::ElementEnded(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname)
{
   BaseXMLHandler::ElementEnded(uri, localname, qname);

   if (XMLString::compareString(localname, MapXMLConstants::HEADER_ELEMENT) == 0)
   {
      mIsHeaderParsed = true;
   }
}

////////////////////////////////////////////////////////////////////////////////
bool MapHeaderHandler::HeaderParsed() const
{
   return mIsHeaderParsed;
}

//////////////////////////////////////////////////////////////////////////
#if XERCES_VERSION_MAJOR < 3
void MapHeaderHandler::characters(const XMLCh* const chars, const unsigned int length)
#else
void MapHeaderHandler::characters(const XMLCh* const chars, const XMLSize_t length)
#endif
{
   BaseXMLHandler::characters(chars, length);

   xmlCharString& topEl = mElements.top();

   const std::string str = dtUtil::XMLStringConverter(chars).ToString();;

   if (topEl == MapXMLConstants::NAME_ELEMENT)
   {
      mHeaderData.mName = str;
   }            
   else if (topEl == MapXMLConstants::DESCRIPTION_ELEMENT)
   {
      mHeaderData.mDescription = str;
   }
   else if (topEl == MapXMLConstants::AUTHOR_ELEMENT)
   {
      mHeaderData.mAuthor = str;
   }
   else if (topEl == MapXMLConstants::COMMENT_ELEMENT)
   {
      mHeaderData.mComment = str;
   }
   else if (topEl == MapXMLConstants::COPYRIGHT_ELEMENT)
   {
      mHeaderData.mCopyright = str;
   }
   else if (topEl == MapXMLConstants::CREATE_TIMESTAMP_ELEMENT)
   {
      mHeaderData.mCreateTime = str;
   }
   else if (topEl == MapXMLConstants::LAST_UPDATE_TIMESTAMP_ELEMENT)
   {
      mHeaderData.mLastUpdateTime = str;
   }
   else if (topEl == MapXMLConstants::EDITOR_VERSION_ELEMENT)
   {
      mHeaderData.mEditorVersion = str;
   }
   else if (topEl == MapXMLConstants::SCHEMA_VERSION_ELEMENT)
   {
      mHeaderData.mSchemaVersion = str;
   }
}
