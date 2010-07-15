/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2004-2005 MOVES Institute
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
 * John K. Grant
 */

#ifndef DELTA_APP_XML_CONTENT_HANDLER
#define DELTA_APP_XML_CONTENT_HANDLER

#include <dtABC/export.h>                   // for export symbols
#include <xercesc/sax2/ContentHandler.hpp>  // for a base class
#include <xercesc/sax2/Attributes.hpp>      // for a parameter
#include <string>                           // for data members
#include <dtABC/applicationconfigdata.h>    // for member

namespace dtABC
{
   /// A class to perform the necessary features while a Xerces SAX parser is operating.
   /// Use this with the Xerces SAX2XMLReader.
   class DT_ABC_EXPORT ApplicationConfigHandler : public XERCES_CPP_NAMESPACE_QUALIFIER ContentHandler
   {
      public:
         ApplicationConfigHandler();
         ~ApplicationConfigHandler();

         // inherited pure virtual functions
#if XERCES_VERSION_MAJOR < 3
         virtual void characters(const XMLCh* const chars, const unsigned int length);
         virtual void ignorableWhitespace(const XMLCh* const chars, const unsigned int length);
#else
         virtual void characters(const XMLCh* const chars, const XMLSize_t length);
         virtual void ignorableWhitespace(const XMLCh* const chars, const XMLSize_t length);
#endif
         virtual void endDocument();
         virtual void endElement(const XMLCh* const uri,const XMLCh* const localname,const XMLCh* const qname);
         virtual void processingInstruction(const XMLCh* const target, const XMLCh* const data);
         virtual void setDocumentLocator(const XERCES_CPP_NAMESPACE_QUALIFIER Locator* const locator);
         virtual void startDocument();
         virtual void startElement(const XMLCh* const uri,const XMLCh* const localname,const XMLCh* const qname, const XERCES_CPP_NAMESPACE_QUALIFIER Attributes& attrs);
         virtual void startPrefixMapping(const XMLCh* const prefix, const XMLCh* const uri);
         virtual void endPrefixMapping(const XMLCh* const prefix);
         virtual void skippedEntity(const XMLCh* const name);

         ApplicationConfigData mConfigData;
      private:
         std::string mCurrentElement;
         std::string mPropertyName;
    };
}

#endif  // DELTA_APP_XML_CONTENT_HANDLER
