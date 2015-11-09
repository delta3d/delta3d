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
 * John Grant
 */

#ifndef __DTUTIL_HOT_SPOT_XML_H__
#define __DTUTIL_HOT_SPOT_XML_H__

#include <xercesc/sax2/ContentHandler.hpp>  // for a base class
#include <vector>
#include <stack>
#include <dtUtil/hotspotdefinition.h>
#include <dtUtil/export.h>
#include <dtUtil/warningdisable.h>
#include <osg/Vec3>

DT_DISABLE_WARNING_START_CLANG("-Woverloaded-virtual");
namespace dtUtil
{

   class DT_UTIL_EXPORT HotSpotFileHandler : public XERCES_CPP_NAMESPACE_QUALIFIER ContentHandler
   {
   public:
      typedef std::vector<HotSpotDefinition> HotSpotDefinitionVector;

      static const osg::Vec3 HEADING_VEC;
      static const osg::Vec3 PITCH_VEC;
      static const osg::Vec3 ROLL_VEC;

      static const char HOT_SPOT_NODE_NAME[];
      static const char NAME_ATTRIBUTE_NAME[];
      static const char DEFAULT_VALUE[];
      static const char HOT_SPOT_PARENT_NODE_NAME[];
      static const char LOCAL_TRANSLATION_NODE_NAME[];
      static const char LOCAL_ROTATION_NODE_NAME[];

      void characters(const XMLCh* const chars, const unsigned int length);
      void endDocument() {}
      void endElement(const XMLCh* const uri,const XMLCh* const localname,const XMLCh* const qname);
      void ignorableWhitespace(const XMLCh* const chars, const unsigned int length) {}
      void processingInstruction(const XMLCh* const target, const XMLCh* const data) {}
      void setDocumentLocator(const XERCES_CPP_NAMESPACE_QUALIFIER Locator* const locator) {}
      void startDocument() {}

      void startElement(const XMLCh* const uri,const XMLCh* const localname,
                        const XMLCh* const qname, const XERCES_CPP_NAMESPACE_QUALIFIER Attributes& attrs);

      void startPrefixMapping(const XMLCh* const prefix, const XMLCh* const uri) {};
      void endPrefixMapping(const XMLCh* const prefix) {};
      void skippedEntity(const XMLCh* const name) {};

      HotSpotFileHandler();
      ~HotSpotFileHandler();

      HotSpotDefinitionVector& GetData();

   private:
      HotSpotFileHandler(const HotSpotFileHandler& );

      enum HotSpotNode
      {
         NODE_UNKNOWN,
         NODE_HOT_SPOT,
         NODE_HOT_SPOT_PARENT,
         NODE_HOT_SPOT_LOCAL_TRANSLATION,
         NODE_HOT_SPOT_LOCAL_ROTATION
      };

      typedef std::stack<HotSpotNode> NodeStack;

      HotSpotDefinition mCurrentData;
      HotSpotDefinitionVector mHotSpots;
      NodeStack mNodeStack;
   };
}
DT_DISABLE_WARNING_END
#endif // __DTUTIL_HOT_SPOT_XML_H__
