#include <dtDIS/sharedstate.h>
#include <dtDAL/actorproxy.h>
#include <dtUtil/xercesparser.h>
#include <dtDIS/disxml.h>

#include <cstddef>                   // for NULL

using namespace dtDIS;



bool ActorMapConfig::AddActorMapping(const DIS::EntityType& eid, const dtDAL::ActorType* at)
{
   return( mMap.insert( ActorMap::value_type(eid,at) ).second );
}

bool ActorMapConfig::RemoveActorMapping(const DIS::EntityType& eid)
{
   return( mMap.erase( eid )>0 );
}

bool ActorMapConfig::GetMappedActor(const DIS::EntityType& eid, const dtDAL::ActorType*& toWrite)
{
   ActorMap::iterator iter = mMap.find( eid );
   if( iter != mMap.end() )
   {
      toWrite = iter->second.get();
      return true;
   }

   //toWrite = NULL;
   return false;
}


bool ResourceMapConfig::AddResourceMapping(const DIS::EntityType& eid, const dtDAL::ResourceDescriptor& resource)
{
   return( mMap.insert( ResourceMap::value_type(eid,resource) ).second );
}

bool ResourceMapConfig::RemoveResourceMapping(const DIS::EntityType& eid)
{
   return( mMap.erase( eid )>0 );
}

bool ResourceMapConfig::GetMappedResource(const DIS::EntityType& eid, const dtDAL::ResourceDescriptor*& toWrite) const
{
   ResourceMap::const_iterator iter = mMap.find( eid );
   if( iter != mMap.end() )
   {
      toWrite = &(iter->second);
      return true;
   }

   //toWrite = NULL;
   return false;
}



SharedState::SharedState(const std::string& connectionXMLFile,
                         const std::string& entityMappingXMLFile)
   : mActorMapConfig()
   , mResourceMapConfig()
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
}

SharedState::~SharedState()
{
}

ActorMapConfig& SharedState::GetActorMap()
{
   return mActorMapConfig;
}

const ActorMapConfig& SharedState::GetActorMap() const
{
   return mActorMapConfig;
}

ResourceMapConfig& SharedState::GetResourceMap()
{
   return mResourceMapConfig;
}

const ResourceMapConfig& SharedState::GetResourceMap() const
{
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
