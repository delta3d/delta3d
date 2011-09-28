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
#include <dtCore/prefabiconhandler.h>
#include <dtCore/mapxmlconstants.h>
#include <dtUtil/xercesutils.h>
#include <xercesc/util/XMLString.hpp>

XERCES_CPP_NAMESPACE_USE
using namespace dtCore;

////////////////////////////////////////////////////////////////////////////////
dtCore::PrefabIconHandler::PrefabIconHandler()
: mIsIconParsed(false)
{
}

////////////////////////////////////////////////////////////////////////////////
dtCore::PrefabIconHandler::~PrefabIconHandler()
{
}

////////////////////////////////////////////////////////////////////////////////
std::string PrefabIconHandler::GetIconName() const
{
   return mIconName;
}

////////////////////////////////////////////////////////////////////////////////
bool PrefabIconHandler::HandledDesiredData() const
{
   return mIsIconParsed;
}

////////////////////////////////////////////////////////////////////////////////
void PrefabIconHandler::ElementEnded(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname)
{
   BaseXMLHandler::ElementEnded(uri, localname, qname);

   if (XMLString::compareString(localname, MapXMLConstants::ICON_ELEMENT) == 0)
   {
      mIsIconParsed = true;
   }
}

//////////////////////////////////////////////////////////////////////////
#if XERCES_VERSION_MAJOR < 3
void PrefabIconHandler::characters(const XMLCh* const chars, const unsigned int length)
#else
void PrefabIconHandler::characters(const XMLCh* const chars, const XMLSize_t length)
#endif
{
   BaseXMLHandler::characters(chars, length);

   xmlCharString& topEl = mElements.top();

   const std::string str = dtUtil::XMLStringConverter(chars).ToString();

   if (topEl == MapXMLConstants::ICON_ELEMENT)
   {
      mIconName = str;
   }
}

//////////////////////////////////////////////////////////////////////////
