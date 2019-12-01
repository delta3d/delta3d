#include <dtDIS/disxml.h>
#include <dtDIS/sharedstate.h>
#include <dtDIS/propertyname.h>
#include <dtUtil/log.h>
#include <dtUtil/stringutils.h>
#include <dtCore/actorfactory.h>
#include <dtCore/exceptionenum.h>
#include <dtUtil/macros.h>
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
const char details::XMLConnectionSchema::NODE_SITE_ID[] = {"SiteID\0"};
const char details::XMLConnectionSchema::NODE_APPLICATION_ID[] = {"ApplicationID\0"};
const char details::XMLConnectionSchema::NODE_MTU[] = {"MTU\0"};
const char details::XMLConnectionSchema::NODE_USE_BROADCAST[] = {"Broadcast\0"};


const dtDIS::ConnectionData ConnectionXMLHandler::DEFAULT_CONNECTION_DATA;


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

#if XERCES_VERSION_MAJOR < 3
void ConnectionXMLHandler::characters(const XMLCh* const chars, const unsigned int length)
#else
void ConnectionXMLHandler::characters(const XMLCh* const chars, const XMLSize_t length)
#endif
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

   case SITE_ID:
      {
         mConnectionData.site_id = dtUtil::ToType<unsigned short>(cstr);
      } break;

   case APPLICATION_ID:
      {
         mConnectionData.application_id = dtUtil::ToType<unsigned short>(cstr);
      } break;

   case MTU:
      {
         mConnectionData.MTU = dtUtil::ToType<unsigned int>(cstr);
      } break;

   case USE_BROADCAST:
      {
         mConnectionData.use_broadcast = dtUtil::ToType<bool>(cstr);
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

#if XERCES_VERSION_MAJOR < 3
void ConnectionXMLHandler::ignorableWhitespace(const XMLCh* const chars, const unsigned int length)
#else
void ConnectionXMLHandler::ignorableWhitespace(const XMLCh* const chars, const XMLSize_t length)
#endif
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
   char* cstr = XMLString::transcode(localname);

   if (XMLString::equals(cstr, dtDIS::details::XMLConnectionSchema::NODE_IP))
   {
      mNodeStack.push(IP);
   }
   else if (XMLString::equals(cstr, dtDIS::details::XMLConnectionSchema::NODE_PLUGINS))
   {
      mNodeStack.push(PLUGINS);
   }
   else if (XMLString::equals(cstr, dtDIS::details::XMLConnectionSchema::NODE_PORT))
   {
      mNodeStack.push(PORT);
   }
   else if (XMLString::equals(cstr, dtDIS::details::XMLConnectionSchema::NODE_EXERCISE_ID))
   {
      mNodeStack.push(EXERCISE_ID);
   }
   else if (XMLString::equals(cstr, dtDIS::details::XMLConnectionSchema::NODE_SITE_ID))
   {
      mNodeStack.push(SITE_ID);
   }
   else if (XMLString::equals(cstr, dtDIS::details::XMLConnectionSchema::NODE_APPLICATION_ID))
   {
      mNodeStack.push(APPLICATION_ID);
   }
   else if (XMLString::equals(cstr, dtDIS::details::XMLConnectionSchema::NODE_MTU))
   {
      mNodeStack.push(MTU);
   }
   else if (XMLString::equals(cstr, dtDIS::details::XMLConnectionSchema::NODE_USE_BROADCAST))
   {
      mNodeStack.push(USE_BROADCAST);
   }

   XMLString::release(&cstr);
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

const char details::XMLEntityMapSchema::NODE_LIBRARIES[] = {"Libraries\0"};
const char details::XMLEntityMapSchema::NODE_LIBRARY[] = {"Library\0"};
const char details::XMLEntityMapSchema::NODE_LIBRARYDATA_NAME[] = {"LibraryName\0"};
const char details::XMLEntityMapSchema::NODE_LIBRARYDATA_VERSION[] = {"LibraryVersion\0"};

const char details::XMLEntityMapSchema::NODE_MAPPING[] = {"Mapping\0"};

const char details::XMLEntityMapSchema::NODE_ENTITYTYPE[] = {"EntityType\0"};
const char details::XMLEntityMapSchema::NODE_ENTITYTYPE_ENTITY_KIND[] = {"EntityKind\0"};
const char details::XMLEntityMapSchema::NODE_ENTITYTYPE_DOMAIN[] = {"Domain\0"};
const char details::XMLEntityMapSchema::NODE_ENTITYTYPE_COUNTRY[] = {"Country\0"};
const char details::XMLEntityMapSchema::NODE_ENTITYTYPE_SUBCATEGORY[] = {"Subcategory\0"};
const char details::XMLEntityMapSchema::NODE_ENTITYTYPE_CATEGORY[] = {"Category\0"};
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

const char details::XMLEntityMapSchema::NODE_PROPERTY_NAMES[] = {"PropertyNames\0"};
const char details::XMLEntityMapSchema::NODE_ENTITY_LOCATION[] = {"EntityLocation\0"};
const char details::XMLEntityMapSchema::NODE_ENTITY_ORIENTATION[] = {"EntityOrientation\0"};
const char details::XMLEntityMapSchema::NODE_ENTITY_LINEAR_VELOCITY[] = {"EntityLinearVelocity\0"};
const char details::XMLEntityMapSchema::NODE_ENTITY_LAST_KNOWN_LOCATION[] = {"EntityLastKnownLocation\0"};
const char details::XMLEntityMapSchema::NODE_ENTITY_LAST_KNOWN_ORIENTATION[] = {"EntityLastKnownOrientation\0"};
const char details::XMLEntityMapSchema::NODE_ENTITY_DR_ALGO[] = {"EntityDeadReckoningAlgorithm\0"};
const char details::XMLEntityMapSchema::NODE_ENTITY_GROUND_CLAMP[] = {"EntityGroundClamp\0"};
const char details::XMLEntityMapSchema::NODE_ENTITY_NON_DAMAGED[] = {"EntityNonDamaged\0"};
const char details::XMLEntityMapSchema::NODE_ENTITY_DAMAGED[] = {"EntityDamaged\0"};
const char details::XMLEntityMapSchema::NODE_ENTITY_DESTROYED[] = {"EntityDestroyed\0"};
const char details::XMLEntityMapSchema::NODE_ENTITY_TYPE[] = {"EntityTypeProperty\0"};
const char details::XMLEntityMapSchema::NODE_ENTITY_ID[] = {"EntityID\0"};
const char details::XMLEntityMapSchema::NODE_ENTITY_FORCE_ID[] = {"EntityForceID\0"};
const char details::XMLEntityMapSchema::NODE_ENTITY_SMOKE_PLUME[] = {"EntitySmokePlumePresent\0"};
const char details::XMLEntityMapSchema::NODE_ENTITY_FLAMES[] = {"EntityFlamesPresent\0"};

EntityMapXMLHandler::EntityMapXMLHandler(SharedState* config)
   : mSharedState(config)
   , mNodeStack()
{
}

EntityMapXMLHandler::~EntityMapXMLHandler()
{
}

#if XERCES_VERSION_MAJOR < 3
void EntityMapXMLHandler::characters(const XMLCh* const chars, const unsigned int length)
#else
void EntityMapXMLHandler::characters(const XMLCh* const chars, const XMLSize_t length)
#endif
{
   if( mNodeStack.empty() )
   {
      return;
   }

   char* cstr = XMLString::transcode( chars );

   switch( mNodeStack.top() )
   {
   case LIBRARIES:
      {
      } break;

   case LIBRARY:
      {
      } break;

   case LIBRARY_NAME:
      {
         mLibraryName = cstr;
      } break;

   case LIBRARY_VERSION:
      {
         mLibraryVersion = cstr;
      } break;

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


   case PROPERTY_NAMES:
      {
      } break;

   case ENTITY_LOCATION:
      {
         dtDIS::EnginePropertyName::ENTITY_LOCATION = std::string(cstr);
      } break;
   case ENTITY_ORIENTATION:
      {
         dtDIS::EnginePropertyName::ENTITY_ORIENTATION = std::string(cstr);
      } break;
   case ENTITY_LINEAR_VELOCITY:
      {
         dtDIS::EnginePropertyName::ENTITY_LINEARY_VELOCITY = std::string(cstr);
      } break;
   case ENTITY_LAST_KNOWN_LOCATION:
      {
         dtDIS::EnginePropertyName::LAST_KNOWN_LOCATION = std::string(cstr);
      } break;
   case ENTITY_LAST_KNOWN_ORIENTATION:
      {
         dtDIS::EnginePropertyName::LAST_KNOWN_ORIENTATION = std::string(cstr);
      } break;
   case ENTITY_DR_ALGO:
      {
         dtDIS::EnginePropertyName::DEAD_RECKONING_ALGORITHM = std::string(cstr);
      } break;
   case ENTITY_GROUND_CLAMP:
      {
         dtDIS::EnginePropertyName::GROUND_CLAMP = std::string(cstr);
      } break;

   case ENTITY_NON_DAMAGED:
      {
         dtDIS::EnginePropertyName::RESOURCE_DAMAGE_OFF = std::string(cstr);
      } break;
   case ENTITY_DAMAGED:
      {
         dtDIS::EnginePropertyName::RESOURCE_DAMAGE_ON = std::string(cstr);
      } break;
   case ENTITY_DESTROYED:
      {
         dtDIS::EnginePropertyName::RESOURCE_DAMAGE_DESTROYED = std::string(cstr);
      } break;
   case ENTITY_TYPE:
      {
         dtDIS::EntityPropertyName::ENTITY_TYPE = std::string(cstr);
      } break;
   case ENTITY_ID:
      {
         dtDIS::EntityPropertyName::ENTITYID = std::string(cstr);
      } break;
   case ENTITY_FORCE_ID:
      {
         dtDIS::EnginePropertyName::FORCE_ID = std::string(cstr);
      } break;
   case ENTITY_SMOKE_PLUME_PRESENT:
      {
         dtDIS::EnginePropertyName::SMOKE_PLUME_PRESENT = std::string(cstr);
      } break;
   case ENTITY_FLAMES_PRESENT:
      {
         dtDIS::EnginePropertyName::FLAMES_PRESENT = std::string(cstr);
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

   switch (mNodeStack.top())
   {
   case ACTORDATA_RESOURCE:
      {
         // modify the resource mapping
         dtCore::ResourceDescriptor descriptor(mCurrentResourceIdentifier);

         mSharedState->GetEntityMap().SetEntityResource(mCurrentEntityType, descriptor);

         mCurrentResourceIdentifier.clear();
      } 
      break;

   case LIBRARY:
      {
         // Load the library.
         try
         {
            if (dtCore::ActorFactory::GetInstance().GetRegistry(mLibraryName) == NULL)
            {
               dtCore::ActorFactory::GetInstance().LoadActorRegistry(mLibraryName);
            }
         }
         catch (const dtCore::ProjectResourceErrorException &e)
         {
            DTUNREFERENCED_PARAMETER(e);
            mMissingLibraries.push_back(mLibraryName);

            LOG_ERROR("Error loading library " + mLibraryName + " version " + mLibraryVersion + " in the library manager.  Exception message to follow.");
         }
         catch (const dtUtil::Exception& e)
         {
            DTUNREFERENCED_PARAMETER(e);
            mMissingLibraries.push_back(mLibraryName);

            LOG_ERROR("Unknown exception loading library " + mLibraryName + " version " + mLibraryVersion + " in the library manager.  Exception message to follow.");
         }

         // Clear data so nothing remains for the next library.
         mLibraryName.clear();
         mLibraryVersion.clear();
      }
      break;

   case MAPPING:
      {
         // modify the actor mapping

         // find the actortype
         const dtCore::ActorType* actortype = dtCore::ActorFactory::GetInstance().FindActorType(mCurrentActorTypeCategory, mCurrentActorTypeName);

         if (actortype != NULL)
         {
            // make a mapping
            mSharedState->GetEntityMap().SetEntityActorType(mCurrentEntityType, actortype);
         }
         else
         {
            LOG_ERROR("Actor Type with name, " + mCurrentActorTypeName + ", was not found in the GameManager.")
         }

         // clear data so nothing remains for the next mapping
         mCurrentEntityType = DIS::EntityType();
         mCurrentActorTypeCategory.clear();
         mCurrentActorTypeName.clear();
      } 
      break;

   default:
      break;
   }

   mNodeStack.pop();
}

#if XERCES_VERSION_MAJOR < 3
void EntityMapXMLHandler::ignorableWhitespace(const XMLCh* const chars, const unsigned int length)
#else
void EntityMapXMLHandler::ignorableWhitespace(const XMLCh* const chars, const XMLSize_t length)
#endif
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
   else if( XMLString::equals(cstr, dtDIS::details::XMLEntityMapSchema::NODE_ENTITYTYPE_ENTITY_KIND) )
   {
      mNodeStack.push( ENTITYTYPE_ENTITY_KIND );
   }
   else if( XMLString::equals(cstr, dtDIS::details::XMLEntityMapSchema::NODE_ENTITYTYPE_DOMAIN) )
   {
      mNodeStack.push( ENTITYTYPE_DOMAIN );
   }
   else if( XMLString::equals(cstr, dtDIS::details::XMLEntityMapSchema::NODE_ENTITYTYPE_COUNTRY) )
   {
      mNodeStack.push( ENTITYTYPE_COUNTRY );
   }
   else if( XMLString::equals(cstr, dtDIS::details::XMLEntityMapSchema::NODE_ENTITYTYPE_CATEGORY) )
   {
      mNodeStack.push( ENTITYTYPE_CATEGORY );
   }
   else if( XMLString::equals(cstr, dtDIS::details::XMLEntityMapSchema::NODE_ENTITYTYPE_SUBCATEGORY) )
   {
      mNodeStack.push( ENTITYTYPE_SUBCATEGORY );
   }
   else if( XMLString::equals(cstr, dtDIS::details::XMLEntityMapSchema::NODE_ENTITYTYPE_SPECIFIC) )
   {
      mNodeStack.push( ENTITYTYPE_SPECIFIC );
   }
   else if( XMLString::equals(cstr, dtDIS::details::XMLEntityMapSchema::NODE_ENTITYTYPE_EXTRA) )
   {
      mNodeStack.push( ENTITYTYPE_EXTRA );
   }
   else if( XMLString::equals(cstr, dtDIS::details::XMLEntityMapSchema::NODE_LIBRARYDATA_NAME) )
   {
      mNodeStack.push( LIBRARY_NAME );
   }
   else if( XMLString::equals(cstr, dtDIS::details::XMLEntityMapSchema::NODE_LIBRARYDATA_VERSION) )
   {
      mNodeStack.push( LIBRARY_VERSION );
   }
   else if( XMLString::equals(cstr, dtDIS::details::XMLEntityMapSchema::NODE_LIBRARY) )
   {
      mNodeStack.push( LIBRARY );
   }
   else if( XMLString::equals(cstr, dtDIS::details::XMLEntityMapSchema::NODE_LIBRARIES) )
   {
      mNodeStack.push( LIBRARIES );
   }
   else if( XMLString::equals(cstr, dtDIS::details::XMLEntityMapSchema::NODE_MAPPING) )
   {
      mNodeStack.push( MAPPING );
   }
   else if( XMLString::equals(cstr, dtDIS::details::XMLEntityMapSchema::NODE_PROPERTY_NAMES) )
   {
      mNodeStack.push( PROPERTY_NAMES );
   }
   else if( XMLString::equals(cstr, dtDIS::details::XMLEntityMapSchema::NODE_ENTITY_LOCATION) )
   {
      mNodeStack.push( ENTITY_LOCATION );
   }
   else if( XMLString::equals(cstr, dtDIS::details::XMLEntityMapSchema::NODE_ENTITY_ORIENTATION) )
   {
      mNodeStack.push( ENTITY_ORIENTATION );
   }
   else if( XMLString::equals(cstr, dtDIS::details::XMLEntityMapSchema::NODE_ENTITY_LINEAR_VELOCITY) )
   {
      mNodeStack.push( ENTITY_LINEAR_VELOCITY );
   }
   else if( XMLString::equals(cstr, dtDIS::details::XMLEntityMapSchema::NODE_ENTITY_LAST_KNOWN_LOCATION) )
   {
      mNodeStack.push( ENTITY_LAST_KNOWN_LOCATION );
   }
   else if( XMLString::equals(cstr, dtDIS::details::XMLEntityMapSchema::NODE_ENTITY_LAST_KNOWN_ORIENTATION) )
   {
      mNodeStack.push( ENTITY_LAST_KNOWN_ORIENTATION );
   }
   else if( XMLString::equals(cstr, dtDIS::details::XMLEntityMapSchema::NODE_ENTITY_DR_ALGO) )
   {
      mNodeStack.push( ENTITY_DR_ALGO );
   }
   else if( XMLString::equals(cstr, dtDIS::details::XMLEntityMapSchema::NODE_ENTITY_GROUND_CLAMP) )
   {
      mNodeStack.push( ENTITY_GROUND_CLAMP );
   }
   else if( XMLString::equals(cstr, dtDIS::details::XMLEntityMapSchema::NODE_ENTITY_NON_DAMAGED) )
   {
      mNodeStack.push( ENTITY_NON_DAMAGED );
   }
   else if( XMLString::equals(cstr, dtDIS::details::XMLEntityMapSchema::NODE_ENTITY_DAMAGED) )
   {
      mNodeStack.push( ENTITY_DAMAGED );
   }
   else if( XMLString::equals(cstr, dtDIS::details::XMLEntityMapSchema::NODE_ENTITY_DESTROYED) )
   {
      mNodeStack.push( ENTITY_DESTROYED );
   }
   else if( XMLString::equals(cstr, dtDIS::details::XMLEntityMapSchema::NODE_ENTITY_TYPE) )
   {
      mNodeStack.push( ENTITY_TYPE );
   }
   else if( XMLString::equals(cstr, dtDIS::details::XMLEntityMapSchema::NODE_ENTITY_ID) )
   {
      mNodeStack.push( ENTITY_ID );
   }
   else if( XMLString::equals(cstr, dtDIS::details::XMLEntityMapSchema::NODE_ENTITY_FORCE_ID) )
   {
      mNodeStack.push( ENTITY_FORCE_ID );
   }
   else if( XMLString::equals(cstr, dtDIS::details::XMLEntityMapSchema::NODE_ENTITY_SMOKE_PLUME) )
   {
      mNodeStack.push(ENTITY_SMOKE_PLUME_PRESENT);
   }
   else if( XMLString::equals(cstr, dtDIS::details::XMLEntityMapSchema::NODE_ENTITY_FLAMES) )
   {
      mNodeStack.push(ENTITY_FLAMES_PRESENT);
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
