#include <dtDIS/sharedstate.h>
#include <dtCore/actorproxy.h>
#include <dtUtil/xercesparser.h>
#include <dtDIS/disxml.h>

#include <cstddef>                   // for NULL

using namespace dtDIS;



//////////////////////////////////////////////////////////////////////////
bool ActorMapConfig::AddActorMapping(const DIS::EntityType& eid, const dtCore::ActorType* at)
{
   DEPRECATE("bool ActorMapConfig::AddActorMapping(const DIS::EntityType&, const dtCore::ActorType*)",
             "void EntityMap::SetEntityActorType(const DIS::EntityType&, const dtCore::ActorType*)");

   mEntityMap->SetEntityActorType(eid, at);
   return true;
}

//////////////////////////////////////////////////////////////////////////
bool ActorMapConfig::RemoveActorMapping(const DIS::EntityType& eid)
{
   DEPRECATE("bool ActorMapConfig::RemoveActorMapping(const DIS::EntityType&)",
             "void EntityMap::RemoveEntityActorType(const DIS::EntityType&)");

   mEntityMap->RemoveEntityActorType(eid);   
   return true;
}

//////////////////////////////////////////////////////////////////////////
bool ActorMapConfig::GetMappedActor(const DIS::EntityType& eid, const dtCore::ActorType*& toWrite)
{
   DEPRECATE("bool ActorMapConfig::GetMappedActor(const DIS::EntityType&, const dtCore::ActorType*&)",
             "const dtCore::ActorType* EntityMap::GetMappedActorType(const DIS::EntityType&) const");

   toWrite = mEntityMap->GetMappedActorType(eid);

   if (toWrite)
   {
      return true;
   }
   else
   {
      return false;
   }
}

//////////////////////////////////////////////////////////////////////////
bool ResourceMapConfig::AddResourceMapping(const DIS::EntityType& eid, const dtCore::ResourceDescriptor& resource)
{
   DEPRECATE("bool ResourceMapConfig::AddResourceMapping(const DIS::EntityType&, const dtCore::ResourceDescriptor&)",
             "void EntityMap::SetEntityResource(const DIS::EntityType&, const dtCore::ResourceDescriptor&)");
   
   mEntityMap->SetEntityResource(eid, resource);
   return true;
}

//////////////////////////////////////////////////////////////////////////
bool ResourceMapConfig::RemoveResourceMapping(const DIS::EntityType& eid)
{
   DEPRECATE("bool ResourceMapConfig::RemoveResourceMapping(const DIS::EntityType&)",
             "void EntityMap::RemoveEntityResource(const DIS::EntityType&)");

   mEntityMap->RemoveEntityResource(eid);
   return true;;
}

//////////////////////////////////////////////////////////////////////////
bool ResourceMapConfig::GetMappedResource(const DIS::EntityType& eid, dtCore::ResourceDescriptor& toWrite) const
{
   DEPRECATE("bool ResourceMapConfig::GetMappedResource(const DIS::EntityType&, dtCore::ResourceDescriptor&) const",
             "const dtCore::ResourceDescriptor& EntityMap::GetMappedResource(const DIS::EntityType&) const");

   toWrite = mEntityMap->GetMappedResource(eid);
   if (toWrite == dtCore::ResourceDescriptor::NULL_RESOURCE)
   {
      return false;
   }
   else
   {
      return true;
   }   
}


//////////////////////////////////////////////////////////////////////////
SharedState::SharedState(const std::string& connectionXMLFile,
                         const std::string& entityMappingXMLFile)
   : mActorMapConfig(&mEntityTypeMap)
   , mResourceMapConfig(&mEntityTypeMap)
   , mActiveEntityControl()
   , mConnectionData()
   , mSiteID(1)
   , mApplicationID(1)
{
   //TODO Should read and process a DIS xml configuration file

   if (!connectionXMLFile.empty())
   {
      //parse connection file
      ParseConnectionData(connectionXMLFile);
   }

   if (!entityMappingXMLFile.empty())
   {
      ParseEntityMappingData(entityMappingXMLFile);
   }

   //initialize to something valid
   mCoordConverter.SetIncomingCoordinateType(dtUtil::IncomingCoordinateType::GEOCENTRIC);
   mCoordConverter.SetLocalCoordinateType(dtUtil::LocalCoordinateType::CARTESIAN_FLAT_EARTH);
}

SharedState::~SharedState()
{
}

ActorMapConfig& SharedState::GetActorMap()
{
   DEPRECATE("ActorMapConfig& SharedState::GetActorMap()", "N/A");
   return mActorMapConfig;
}

const ActorMapConfig& SharedState::GetActorMap() const
{
   DEPRECATE("const ActorMapConfig& SharedState::GetActorMap() const", "N/A");
   return mActorMapConfig;
}

ResourceMapConfig& SharedState::GetResourceMap()
{
   DEPRECATE("ResourceMapConfig& SharedState::GetResourceMap()", "N/A");
   return mResourceMapConfig;
}

const ResourceMapConfig& SharedState::GetResourceMap() const
{
   DEPRECATE("const ResourceMapConfig& SharedState::GetResourceMap() const", "N/A");
   return mResourceMapConfig;
}

ActiveEntityControl& SharedState::GetActiveEntityControl()
{
   return mActiveEntityControl;
}

const ActiveEntityControl& SharedState::GetActiveEntityControl() const
{
   return mActiveEntityControl;
}

void SharedState::SetConnectionData(const ConnectionData& data)
{
   mConnectionData = data;
   SetApplicationID(mConnectionData.application_id);
   SetSiteID(mConnectionData.site_id);
}

const ConnectionData& SharedState::GetConnectionData() const
{
   return mConnectionData;
}


////////////////////////////////////////////////////////////////////////////////
void dtDIS::SharedState::ParseConnectionData(const std::string& file)
{
   dtUtil::XercesParser parser;
   dtDIS::ConnectionXMLHandler handler;
   bool parsed = parser.Parse(file, handler, "dis_connection.xsd");

   if (parsed)
   {
      this->SetConnectionData(handler.GetConnectionData());
   }
}

////////////////////////////////////////////////////////////////////////////////
void dtDIS::SharedState::ParseEntityMappingData(const std::string& file)
{
   dtUtil::XercesParser parser;
   dtDIS::EntityMapXMLHandler handler(this);
   bool parsed = parser.Parse(file, handler, "dis_mapping.xsd");

   if (parsed)
   {
   }
}

////////////////////////////////////////////////////////////////////////////////
void dtDIS::SharedState::SetSiteID(unsigned short ID)
{
   mSiteID = ID;
}

////////////////////////////////////////////////////////////////////////////////
unsigned short dtDIS::SharedState::GetSiteID() const
{
   return mSiteID;
}

////////////////////////////////////////////////////////////////////////////////
void dtDIS::SharedState::SetApplicationID(unsigned short ID)
{
   mApplicationID = ID;
}

////////////////////////////////////////////////////////////////////////////////
unsigned short dtDIS::SharedState::GetApplicationID() const
{
   return mApplicationID;
}

////////////////////////////////////////////////////////////////////////////////
void dtDIS::SharedState::SetCoordinateConverter(const dtUtil::Coordinates& coordConverter)
{
   mCoordConverter = coordConverter; //copy

   //Ensure that the converter is setup to what dtDIS needs.  Incoming DIS is geocentric and
   //the local is Flat earth?
   //mCoordConverter.SetIncomingCoordinateType(dtUtil::IncomingCoordinateType::GEOCENTRIC);
   //mCoordConverter.SetLocalCoordinateType(dtUtil::LocalCoordinateType::CARTESIAN_FLAT_EARTH);
}

////////////////////////////////////////////////////////////////////////////////
const dtUtil::Coordinates& dtDIS::SharedState::GetCoordinateConverter() const
{
   return mCoordConverter;
}

////////////////////////////////////////////////////////////////////////////////
dtUtil::Coordinates& dtDIS::SharedState::GetCoordinateConverter()
{
   return mCoordConverter;
}

//////////////////////////////////////////////////////////////////////////
EntityMap& dtDIS::SharedState::GetEntityMap()
{
   return mEntityTypeMap;
}

//////////////////////////////////////////////////////////////////////////
const EntityMap& dtDIS::SharedState::GetEntityMap() const
{
   return mEntityTypeMap;
}
