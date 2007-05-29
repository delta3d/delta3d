/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2007 John K. Grant
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
 * John K. Grant, April 2007.
 */

#ifndef __DELTA_DTDIS_DISXML_H__
#define __DELTA_DTDIS_DISXML_H__

#include <dtDIS/dtdisexport.h>       // for export symbols

#include <string>

#include <xercesc/sax2/ContentHandler.hpp>  // for a base class
#include <xercesc/sax2/Attributes.hpp>      // for a parameter

#include <stack>                            // for std::stack
#include <DIS/EntityType.h>                 // for stored data model
#include <dtCore/refptr.h>                  // for member
#include <dtDIS/sharedstate.h>              // for member

namespace dtGame
{
   class GameManager;
}

namespace dtDIS
{
   class SharedState;

   /// constant definitions for the official schema files.
   struct DT_DIS_EXPORT XMLFiles
   {
      static const char XML_CONNECTION_SCHEMA_FILE[];
      static const char XML_MAPPING_SCHEMA_FILE[];
   };

   ///@cond DOXYGEN_SHOULD_SKIP_THIS
   namespace details
   {

   /// a model of the schema for Connection files gauranteed when parsing
   struct DT_DIS_EXPORT XMLConnectionSchema
   {
      static const char NODE_CONNECTION[];
      static const char NODE_PORT[];
      static const char NODE_IP[];
      static const char NODE_PLUGINS[];
      static const char NODE_EXERCISE_ID[];
      static const char NODE_MTU[];
   };

   } // end namespace details
   ///@endcond

   ///\brief Reads the connection info from a config file.
   class DT_DIS_EXPORT ConnectionXMLHandler : public XERCES_CPP_NAMESPACE_QUALIFIER ContentHandler
   {
   public:
      ConnectionXMLHandler();
      ~ConnectionXMLHandler();

      // inherited pure virtual functions
      void characters(const XMLCh* const chars, const unsigned int length);
      void endDocument();
      void endElement(const XMLCh* const uri,const XMLCh* const localname,const XMLCh* const qname);
      void ignorableWhitespace(const XMLCh* const chars, const unsigned int length);
      void processingInstruction(const XMLCh* const target, const XMLCh* const data);
      void setDocumentLocator(const XERCES_CPP_NAMESPACE_QUALIFIER Locator* const locator);
      void startDocument();
      void startElement(const XMLCh* const uri,const XMLCh* const localname,const XMLCh* const qname, const XERCES_CPP_NAMESPACE_QUALIFIER Attributes& attrs);
      void startPrefixMapping(const	XMLCh* const prefix,const XMLCh* const uri);
      void endPrefixMapping(const XMLCh* const prefix);
      void skippedEntity(const XMLCh* const name);

      // defaults, if parsing doesn't find the values
      static const dtDIS::ConnectionData DEFAULT_CONNECTION_DATA;

      const dtDIS::ConnectionData& GetConnectionData() const;

   private:
      dtDIS::ConnectionData mConnectionData;

      enum Nodes
      {
         PORT,
         IP,
         PLUGINS,
         EXERCISE_ID,
         MTU
      };

      typedef std::stack<Nodes> NodeStack;
      NodeStack mNodeStack;
   };

   // ----- EntityMap stuff ------ //
   ///@cond DOXYGEN_SHOULD_SKIP_THIS
   namespace details
   {

   /// a model of the schema for mapping files gauranteed when parsing
   struct DT_DIS_EXPORT XMLEntityMapSchema
   {
      static const char NODE_COMMON_CATEGORY[];

      static const char NODE_MAPPING[];

      static const char NODE_ENTITYTYPE[];

      static const char NODE_ENTITYTYPE_ENTITY_KIND[];
      static const char NODE_ENTITYTYPE_DOMAIN[];
      static const char NODE_ENTITYTYPE_COUNTRY[];
      static const char NODE_ENTITYTYPE_SUBCATEGORY[];
      static const char NODE_ENTITYTYPE_SPECIFIC[];
      static const char NODE_ENTITYTYPE_EXTRA[];

      static const char NODE_ACTORDATA[];

      static const char NODE_ACTORDATA_ACTORTYPE[];
      static const char NODE_ACTORDATA_ACTORTYPE_NAME[];

      static const char NODE_ACTORDATA_RESOURCE[];
      static const char NODE_ACTORDATA_RESOURCE_ACTORPROPERTY[];
      static const char NODE_ACTORDATA_RESOURCE_DESCRIPTOR[];

      static const char ATTRIBUTE_RESOURCE_RENDERSUITE[];
      static const char ATTRIBUTE_RESOURCE_GROUP[];
   };

   } // end namespace details
   ///@endcond

   ///\brief Stores configuration data loaded from a file for the purposes of mapping DIS to Delta3D.
   ///
   /// Populates the content within the EntityMapControl.
   /// Assumes the dtGame::GameManager has all ActorTypes registered already.
   class DT_DIS_EXPORT EntityMapXMLHandler : public XERCES_CPP_NAMESPACE_QUALIFIER ContentHandler
   {
   public:
      EntityMapXMLHandler(SharedState* config, dtGame::GameManager* gm);
      ~EntityMapXMLHandler();

      // inherited pure virtual functions
      void characters(const XMLCh* const chars, const unsigned int length);
      void endDocument();
      void endElement(const XMLCh* const uri,const XMLCh* const localname,const XMLCh* const qname);
      void ignorableWhitespace(const XMLCh* const chars, const unsigned int length);
      void processingInstruction(const XMLCh* const target, const XMLCh* const data);
      void setDocumentLocator(const XERCES_CPP_NAMESPACE_QUALIFIER Locator* const locator);
      void startDocument();
      void startElement(const XMLCh* const uri,const XMLCh* const localname,const XMLCh* const qname, const XERCES_CPP_NAMESPACE_QUALIFIER Attributes& attrs);
      void startPrefixMapping(const	XMLCh* const prefix,const XMLCh* const uri);
      void endPrefixMapping(const XMLCh* const prefix);
      void skippedEntity(const XMLCh* const name);

   private:
      SharedState* mSharedState;
      dtCore::RefPtr<dtGame::GameManager> mGM;

      enum Nodes
      {
         MAPPING,
         ENTITYTYPE,
         ENTITYTYPE_ENTITY_KIND,
         ENTITYTYPE_DOMAIN,
         ENTITYTYPE_COUNTRY,
         ENTITYTYPE_CATEGORY,
         ENTITYTYPE_SUBCATEGORY,
         ENTITYTYPE_SPECIFIC,
         ENTITYTYPE_EXTRA,
         ACTORDATA,
         ACTORDATA_ACTORTYPE,
         ACTORDATA_ACTORTYPE_NAME,
         ACTORDATA_ACTORTYPE_CATEGORY,
         ACTORDATA_RESOURCE,
         ACTORDATA_RESOURCE_DESCRIPTOR,
         ACTORDATA_RESOURCE_ACTORPROPERTY,
      };

      typedef std::stack<Nodes> NodeStack;
      NodeStack mNodeStack;

      DIS::EntityType mCurrentEntityType;

      std::string mCurrentActorTypeName;
      std::string mCurrentActorTypeCategory;
      std::string mCurrentResourceIdentifier;
   };
}

#endif // __DELTA_DTDIS_DISXML_H__
