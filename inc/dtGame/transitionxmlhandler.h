/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2009, Alion Science and Technology, BMH Operation
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
 * Bradley Anderegg
 */

#ifndef DELTA_TRANSITION_XML_HANDLER_H
#define DELTA_TRANSITION_XML_HANDLER_H

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtGame/export.h>
#include <dtGame/gamestatecomponent.h>
#include <dtUtil/xercesparser.h>
#include <dtUtil/xercesutils.h>
#include <xercesc/sax2/ContentHandler.hpp>  // for a base class
#include <xercesc/util/XMLString.hpp>

#if XERCES_VERSION_MAJOR < 3
#ifndef XMLSize_t
#define XMLSize_t unsigned
#endif
#endif



namespace dtGame
{
   //////////////////////////////////////////////////////////////////////////
   // TRANSITION XML HANDLER CODE
   //////////////////////////////////////////////////////////////////////////
   class TransitionXMLHandler : public xercesc::ContentHandler
   {
      public:
         TransitionXMLHandler(GameStateComponent* mgr): mManager(mgr) {}
         virtual ~TransitionXMLHandler() {}

         // inherited pure virtual functions
         virtual void characters(const XMLCh* const chars, const XMLSize_t length) {}
         virtual void endDocument() {}
         virtual void ignorableWhitespace(const XMLCh* const chars, const XMLSize_t length) {}
         virtual void processingInstruction(const XMLCh* const target, const XMLCh* const data) {}
         virtual void setDocumentLocator(const XERCES_CPP_NAMESPACE_QUALIFIER Locator* const locator) {}
         virtual void startDocument() {}
         virtual void startPrefixMapping(const	XMLCh* const prefix,const XMLCh* const uri) {}
         virtual void endPrefixMapping(const XMLCh* const prefix) {}
         virtual void skippedEntity(const XMLCh* const name) {}

         virtual void startElement(const XMLCh* const uri,
            const XMLCh* const localname,
            const XMLCh* const qname,
            const XERCES_CPP_NAMESPACE_QUALIFIER Attributes& attrs);

         virtual void endElement(const XMLCh* const uri,
            const XMLCh* const localname,
            const XMLCh* const qname);

      private:
         GameStateComponent* mManager;
         const StateType* mFromState;
         const StateType* mToState;
         const EventType* mEventType;
   };

}

#endif
