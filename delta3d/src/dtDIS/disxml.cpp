#include <dtDIS/disxml.h>
#include <dtDIS/sharedstate.h>
#include <dtGame/gamemanager.h>
#include <dtUtil/log.h>
#include <dtUtil/stringutils.h>

#include <xercesc/util/XercesDefs.hpp>  // for xerces namespace definition
#include <xercesc/util/XMLString.hpp>  // for xerces string support


using namespace dtDIS;
XERCES_CPP_NAMESPACE_USE

const char XMLFiles::XML_CONNECTION_SCHEMA_FILE[] = {"dis_connection.xsd\0"};
const char XMLFiles::XML_MAPPING_SCHEMA_FILE[] = {"dis_mapping.xsd\0"};

const char details::XMLConnectionSchema::NODE_CONNECTION[] = {"Connection\0"};
const char details::XMLConnectionSchema::NODE_PORT[] = {"Port\0"};
const char details::XMLConnectionSchema::NODE_IP[] = {"IP\0"};
const char details::XMLConnectionSchema::NODE_PLUGINS[] = {"Plugins\0"};
const char details::XMLConnectionSchema::NODE_EXERCISE_ID[] = {"ExerciseID\0"};
const char details::XMLConnectionSchema::NODE_MTU[] = {"MTU\0"};


const dtDIS::ConnectionData ConnectionXMLHandler::DEFAULT_CONNECTION_DATA = { 62040,
                                                                              "239.1.2.3",
                                                                              "../../dtDIS_trunk/bin/plugins",
                                                                              1,
                                                                              1500 };


ConnectionXMLHandler::ConnectionXMLHandler()
   : mConnectionData( DEFAULT_CONNECTION_DATA )
   , mNodeStack()
{
}

ConnectionXMLHandler::~ConnectionXMLHandler()
{
}

const dtDIS::ConnectionData& ConnectionXMLHandler::GetConnectionData() const
{
   return mConnectionData;
}

void ConnectionXMLHandler::characters(const XMLCh* const chars, const unsigned int length)
{
   if( mNodeStack.empty() )
   {
      return;
   }

   char* cstr = XMLString::transcode( chars );

   switch( mNodeStack.top() )
   {
   case PORT:
      {
         mConnectionData.port = dtUtil::ToType<unsigned int>( cstr );
      } break;

   case IP:
      {
         mConnectionData.ip = cstr;
      } break;

   case PLUGINS:
      {
         mConnectionData.plug_dir = cstr;
      } break;

   case EXERCISE_ID:
      {
         mConnectionData.exercise_id = dtUtil::ToType<unsigned int>(cstr);
      } break;

   case MTU:
      {
         mConnectionData.MTU = dtUtil::ToType<unsigned int>(cstr);
      } break;

   default:
      {
         LOG_ERROR("Could not determine xml node type.")
      };
   }

   XMLString::release( &cstr );
}

void ConnectionXMLHandler::endDocument()
{
}

void ConnectionXMLHandler::endElement(const XMLCh* const uri,const XMLCh* const localname,const XMLCh* const qname)
{
   if( mNodeStack.empty() )
   {
      return;
   }

   mNodeStack.pop();
}

void ConnectionXMLHandler::ignorableWhitespace(const XMLCh* const chars, const unsigned int length)
{
}

void ConnectionXMLHandler::processingInstruction(const XMLCh* const target, const XMLCh* const data)
{
}

void ConnectionXMLHandler::setDocumentLocator(const XERCES_CPP_NAMESPACE_QUALIFIER Locator* const locator)
{
}

void ConnectionXMLHandler::startDocument()
{
}

void ConnectionXMLHandler::startElement(const XMLCh* const uri,const XMLCh* const localname,const XMLCh* const qname, const XERCES_CPP_NAMESPACE_QUALIFIER Attributes& attrs)
{
   char* cstr = XMLString::transcode( localname );

   if( XMLString::equals(cstr, dtDIS::details::XMLConnectionSchema::NODE_IP) )
   {
      mNodeStack.push( IP );
   }
   else if( XMLString::equals(cstr, dtDIS::details::XMLConnectionSchema::NODE_PLUGINS) )
   {
      mNodeStack.push( PLUGINS );
   }
   else if( XMLString::equals(cstr, dtDIS::details::XMLConnectionSchema::NODE_PORT) )
   {
      mNodeStack.push( PORT );
   }
   else if( XMLString::equals(cstr, dtDIS::details::XMLConnectionSchema::NODE_EXERCISE_ID) )
   {
      mNodeStack.push( EXERCISE_ID );
   }
   else if( XMLString::equals(cstr, dtDIS::details::XMLConnectionSchema::NODE_MTU) )
   {
      mNodeStack.push( MTU );
   }

   XMLString::release( &cstr );
}

void ConnectionXMLHandler::startPrefixMapping(const XMLCh* const prefix,const XMLCh* const uri)
{
}

void ConnectionXMLHandler::endPrefixMapping(const XMLCh* const prefix)
{
}

void ConnectionXMLHandler::skippedEntity(const XMLCh* const name)
{
}



// ---- EntityMap stuff  ---- //
const char details::XMLEntityMapSchema::NODE_COMMON_CATEGORY[] = {"Category\0"};

const char details::XMLEntityMapSchema::NODE_MAPPING[] = {"Mapping\0"};

const char details::XMLEntityMapSchema::NODE_ENTITYTYPE[] = {"EntityType\0"};
const char details::XMLEntityMapSchema::NODE_ENTITYTYPE_ENTITY_KIND[] = {"EntityKind\0"};
const char details::XMLEntityMapSchema::NODE_ENTITYTYPE_DOMAIN[] = {"Domain\0"};
const char details::XMLEntityMapSchema::NODE_ENTITYTYPE_COUNTRY[] = {"Country\0"};
const char details::XMLEntityMapSchema::NODE_ENTITYTYPE_SUBCATEGORY[] = {"Subcategory\0"};
const char details::XMLEntityMapSchema::NODE_ENTITYTYPE_SPECIFIC[] = {"Specific\0"};
const char details::XMLEntityMapSchema::NODE_ENTITYTYPE_EXTRA[] = {"Extra\0"};

const char details::XMLEntityMapSchema::NODE_ACTORDATA[] = {"ActorData\0"};

const char details::XMLEntityMapSchema::NODE_ACTORDATA_ACTORTYPE[] = {"ActorType\0"};
const char details::XMLEntityMapSchema::NODE_ACTORDATA_ACTORTYPE_NAME[] = {"Name\0"};

const char details::XMLEntityMapSchema::NODE_ACTORDATA_RESOURCE[] = {"Resource\0"};
const char details::XMLEntityMapSchema::NODE_ACTORDATA_RESOURCE_DESCRIPTOR[] = {"Descriptor\0"};
const char details::XMLEntityMapSchema::NODE_ACTORDATA_RESOURCE_ACTORPROPERTY[] = {"ActorProperty\0"};

const char details::XMLEntityMapSchema::ATTRIBUTE_RESOURCE_GROUP[] = {"Group\0"};
const char details::XMLEntityMapSchema::ATTRIBUTE_RESOURCE_RENDERSUITE[] = {"RenderSuite\0"};

EntityMapXMLHandler::EntityMapXMLHandler(SharedState* config, dtGame::GameManager* gm )
   : mSharedState(config)
   , mGM( gm )
   , mNodeStack()
{
}

EntityMapXMLHandler::~EntityMapXMLHandler()
{
}

void EntityMapXMLHandler::characters(const XMLCh* const chars, const unsigned int length)
{
   if( mNodeStack.empty() )
   {
      return;
   }

   char* cstr = XMLString::transcode( chars );

   switch( mNodeStack.top() )
   {
   case MAPPING:
      {
      } break;

   case ENTITYTYPE:
      {
      } break;

   case ENTITYTYPE_ENTITY_KIND:
      {
         mCurrentEntityType.setEntityKind( dtUtil::ToType<unsigned int>( cstr ) );
      } break;

   case ENTITYTYPE_DOMAIN:
      {
         mCurrentEntityType.setDomain( dtUtil::ToType<unsigned int>( cstr ) );
      } break;

   case ENTITYTYPE_COUNTRY:
      {
         mCurrentEntityType.setCountry( dtUtil::ToType<unsigned int>( cstr ) );
      } break;

   case ENTITYTYPE_CATEGORY:
      {
         mCurrentEntityType.setCategory( dtUtil::ToType<unsigned int>( cstr ) );
      } break;

   case ENTITYTYPE_SUBCATEGORY:
      {
         mCurrentEntityType.setSubcategory( dtUtil::ToType<unsigned int>( cstr ) );
      } break;

   case ENTITYTYPE_SPECIFIC:
      {
         mCurrentEntityType.setSpecific( dtUtil::ToType<unsigned int>( cstr ) );
      } break;

   case ENTITYTYPE_EXTRA:
      {
         mCurrentEntityType.setExtra( dtUtil::ToType<unsigned int>( cstr ) );
      } break;

   case ACTORDATA:
      {
      } break;

   case ACTORDATA_ACTORTYPE:
      {
      } break;

   case ACTORDATA_ACTORTYPE_NAME:
      {
         mCurrentActorTypeName = cstr;
      } break;

   case ACTORDATA_ACTORTYPE_CATEGORY:
      {
         mCurrentActorTypeCategory = cstr;
      } break;

   case ACTORDATA_RESOURCE:
      {
      } break;

   case ACTORDATA_RESOURCE_DESCRIPTOR:
      {
         mCurrentResourceIdentifier = cstr;
      } break;

   case ACTORDATA_RESOURCE_ACTORPROPERTY:
      {
      } break;

   default:
      {
         LOG_ERROR("Unsupported XML Element type, of value: " + dtUtil::ToString(mNodeStack.top()) )
      } break;
   }

   XMLString::release( &cstr );
}

void EntityMapXMLHandler::endDocument()
{
}

void EntityMapXMLHandler::endElement(const XMLCh* const uri,const XMLCh* const localname,const XMLCh* const qname)
{
   if( mNodeStack.empty() )
   {
      return;
   }

   switch( mNodeStack.top() )
   {
   case ACTORDATA_RESOURCE:
      {
         // modify the resource mapping
         dtDAL::ResourceDescriptor descriptor( mCurrentResourceIdentifier );

         ResourceMapConfig& rmapper = mSharedState->GetResourceMap();
         if( !rmapper.AddResourceMapping( mCurrentEntityType, descriptor ) )
         {
            LOG_ERROR("DIS Entity was not mapped for resource with identifier: " + mCurrentResourceIdentifier )
         }

         mCurrentResourceIdentifier.clear();
      } break;

   case MAPPING:
      {
         // modify the actor mapping

         // find the actortype
         dtDAL::ActorType* actortype = mGM->FindActorType( mCurrentActorTypeCategory, mCurrentActorTypeName );

         if( actortype != NULL )
         {
            // make a mapping
            if( !mSharedState->GetActorMap().AddActorMapping( mCurrentEntityType, actortype ) )
            {
               LOG_ERROR("DIS Entity was not mapped for Actor Type with name: " + mCurrentActorTypeName )
            }
         }
         else
         {
            LOG_ERROR("Actor Type with name, " + mCurrentActorTypeName + ", was not found in the GameManager.")
         }

         // clear data so nothing remains for the next mapping
         mCurrentEntityType = DIS::EntityType();
         mCurrentActorTypeCategory.clear();
         mCurrentActorTypeName.clear();
      } break;
   }

   mNodeStack.pop();
}

void EntityMapXMLHandler::ignorableWhitespace(const XMLCh* const chars, const unsigned int length)
{
}

void EntityMapXMLHandler::processingInstruction(const XMLCh* const target, const XMLCh* const data)
{
}

void EntityMapXMLHandler::setDocumentLocator(const XERCES_CPP_NAMESPACE_QUALIFIER Locator* const locator)
{
}

void EntityMapXMLHandler::startDocument()
{
}

void EntityMapXMLHandler::startElement(const XMLCh* const uri,const XMLCh* const localname,const XMLCh* const qname, const XERCES_CPP_NAMESPACE_QUALIFIER Attributes& attrs)
{
   char* cstr = XMLString::transcode( localname );

   if( XMLString::equals(cstr,dtDIS::details::XMLEntityMapSchema::NODE_ACTORDATA) )
   {
      mNodeStack.push( ACTORDATA );
   }
   else if( XMLString::equals(cstr, dtDIS::details::XMLEntityMapSchema::NODE_ACTORDATA_RESOURCE) )
   {
      mNodeStack.push( ACTORDATA_RESOURCE );
   }
   else if( XMLString::equals(cstr, dtDIS::details::XMLEntityMapSchema::NODE_ACTORDATA_RESOURCE_DESCRIPTOR) )
   {
      mNodeStack.push( ACTORDATA_RESOURCE_DESCRIPTOR );
   }
   else if( XMLString::equals(cstr, dtDIS::details::XMLEntityMapSchema::NODE_ACTORDATA_RESOURCE_ACTORPROPERTY) )
   {
      mNodeStack.push( ACTORDATA_RESOURCE_ACTORPROPERTY );
   }
   else if( XMLString::equals(cstr, dtDIS::details::XMLEntityMapSchema::NODE_ACTORDATA_ACTORTYPE) )
   {
      mNodeStack.push( ACTORDATA_ACTORTYPE );
   }
   else if( XMLString::equals(cstr, dtDIS::details::XMLEntityMapSchema::NODE_ACTORDATA_ACTORTYPE_NAME) )
   {
      mNodeStack.push( ACTORDATA_ACTORTYPE_NAME );
   }
   else if( XMLString::equals(cstr, dtDIS::details::XMLEntityMapSchema::NODE_COMMON_CATEGORY) )
   {
      if( mNodeStack.top() == ACTORDATA_ACTORTYPE )
      {
         mNodeStack.push( ACTORDATA_ACTORTYPE_CATEGORY );
      }
      else if( mNodeStack.top() == ENTITYTYPE )
      {
         mNodeStack.push( ENTITYTYPE_CATEGORY );
      }
   }
   else if( XMLString::equals(cstr, dtDIS::details::XMLEntityMapSchema::NODE_ENTITYTYPE) )
   {
      mNodeStack.push( ENTITYTYPE );
   }
   else if( XMLString::equals(cstr, dtDIS::details::XMLEntityMapSchema::NODE_ENTITYTYPE_COUNTRY) )
   {
      mNodeStack.push( ENTITYTYPE_COUNTRY );
   }
   else if( XMLString::equals(cstr, dtDIS::details::XMLEntityMapSchema::NODE_ENTITYTYPE_DOMAIN) )
   {
      mNodeStack.push( ENTITYTYPE_DOMAIN );
   }
   else if( XMLString::equals(cstr, dtDIS::details::XMLEntityMapSchema::NODE_ENTITYTYPE_ENTITY_KIND) )
   {
      mNodeStack.push( ENTITYTYPE_ENTITY_KIND );
   }
   else if( XMLString::equals(cstr, dtDIS::details::XMLEntityMapSchema::NODE_ENTITYTYPE_EXTRA) )
   {
      mNodeStack.push( ENTITYTYPE_EXTRA );
   }
   else if( XMLString::equals(cstr, dtDIS::details::XMLEntityMapSchema::NODE_ENTITYTYPE_EXTRA) )
   {
      mNodeStack.push( ENTITYTYPE_EXTRA );
   }
   else if( XMLString::equals(cstr, dtDIS::details::XMLEntityMapSchema::NODE_ENTITYTYPE_SPECIFIC) )
   {
      mNodeStack.push( ENTITYTYPE_SPECIFIC );
   }
   else if( XMLString::equals(cstr, dtDIS::details::XMLEntityMapSchema::NODE_ENTITYTYPE_SUBCATEGORY) )
   {
      mNodeStack.push( ENTITYTYPE_SUBCATEGORY );
   }
   else if( XMLString::equals(cstr, dtDIS::details::XMLEntityMapSchema::NODE_MAPPING) )
   {
      mNodeStack.push( MAPPING );
   }

   XMLString::release( &cstr );
}

void EntityMapXMLHandler::startPrefixMapping(const XMLCh* const prefix,const XMLCh* const uri)
{
}

void EntityMapXMLHandler::endPrefixMapping(const XMLCh* const prefix)
{
}

void EntityMapXMLHandler::skippedEntity(const XMLCh* const name)
{
}
