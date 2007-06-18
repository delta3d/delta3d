#include <dtDIS/mastercomponent.h>
#include <dtDIS/sharedstate.h>

#include <DIS/PDUType.h>

#include <dtUtil/functor.h>
#include <list>
#include <dtDIS/dllfinder.h>

#include <dtDIS/plugins/default/defaultplugin.h>


using namespace dtDIS;

///\todo what should set the network stream's endian type?  the SharedState's connection data?
MasterComponent::MasterComponent(SharedState* config)
   : dtGame::GMComponent("dtDIS_MasterComponent")
   , mPluginManager()
   , mConnection()
   , mIncomingMessage()
   , mOutgoingMessage(DIS::BIG, config->GetConnectionData().exercise_id )
   , mConfig( config )
   , mDefaultPlugin(new dtDIS::DefaultPlugin())
{
   // add support for the network packets
   LoadPlugins( mConfig->GetConnectionData().plug_dir );
}

MasterComponent::~MasterComponent()
{
   delete mDefaultPlugin;

   // release the memory for the packet support plugins
   mPluginManager.UnloadAllPlugins();

   //mPluginManager.GetLoadedSignal().disconnect( this );
   //mPluginManager.GetUnloadedSignal().disconnect( this );
}

//const PluginManager& MasterComponent::GetPluginManager() const
//{
//   return mPluginManager;
//}

void MasterComponent::OnAddedToGM()
{
   const ConnectionData& connect_data = mConfig->GetConnectionData();

   // add the default "plugin"
   mDefaultPlugin->Start( mIncomingMessage, mOutgoingMessage, GetGameManager(), mConfig );

   // initialize all the plugins
   typedef dtUtil::Functor<void,TYPELIST_1(PluginManager::LibraryRegistry::value_type&)> ForEachPluginFunctor;
   ForEachPluginFunctor func(this,&MasterComponent::OnPluginLoaded);
   PluginManager::LibraryRegistry& plugins = mPluginManager.GetRegistry();
   PluginManager::LibraryRegistry::iterator enditer = plugins.end();
   std::for_each( plugins.begin(), plugins.end(), func );

   // make a connection to the DIS multicast network
   mConnection.Connect( connect_data.port , connect_data.ip.c_str() );
}

void MasterComponent::OnRemovedFromGM()
{
   // add the default "plugin"
   mDefaultPlugin->Finish( mIncomingMessage, mOutgoingMessage );

   // shutdown all the plugins
   typedef dtUtil::Functor<void,TYPELIST_1(PluginManager::LibraryRegistry::value_type&)> ForEachPluginFunctor;
   ForEachPluginFunctor func(this,&MasterComponent::OnPluginUnloaded);
   PluginManager::LibraryRegistry& plugins = mPluginManager.GetRegistry();
   PluginManager::LibraryRegistry::iterator enditer = plugins.end();
   std::for_each( plugins.begin(), plugins.end(), func );

   ///\todo remove the controlled entities.
   // clear the state data
   mConfig->GetActiveEntityControl().ClearAll();

   // stop reading the port
   mConnection.Disconnect();
}

void MasterComponent::LoadPlugins(const std::string& dir)
{
   typedef std::list<std::string> StringList;
   StringList dyn_libs;

   dtDIS::details::DLLFinder filefinder;
   filefinder( dir , dyn_libs );

#ifdef WIN32
      char separator('\\');
#else
      char separator('/');
#endif

   // for each file, load the plugins
   StringList::iterator iter = dyn_libs.begin();
   StringList::iterator enditer = dyn_libs.end();
   for(; iter!=enditer; ++iter)
   {
      mPluginManager.LoadPlugin( dir + separator + *iter );
   }
}

void MasterComponent::OnPluginLoaded(PluginManager::LibraryRegistry::value_type& entry)
{
   dtGame::GameManager* gm = GetGameManager();

   // intialize the plugin
   entry.second.mCreated->Start( mIncomingMessage, mOutgoingMessage, gm, mConfig );
}

void MasterComponent::OnPluginUnloaded(PluginManager::LibraryRegistry::value_type& entry)
{
   entry.second.mCreated->Finish( mIncomingMessage, mOutgoingMessage );
}

///\todo should it handle a pause message, by not updating the incoming or outgoing network classes?
void MasterComponent::ProcessMessage(const dtGame::Message& msg)
{
   const dtGame::MessageType& mt = msg.GetMessageType();
   if( mt == dtGame::MessageType::TICK_LOCAL )
   {
      // read the incoming packets
      const unsigned int MTU = 1500;
      char buffer[MTU];
      size_t recvd( 0 );
      if( recvd = mConnection.Receive( buffer , MTU ) )
      {
         mIncomingMessage.Process( buffer , recvd , DIS::BIG );
      }

      // write the outgoing packets
      {
         const DIS::DataStream& ds = mOutgoingMessage.GetData();
         if( ds.size() > MTU )
         {
            LOG_WARNING("Network buffer is bigger than LAN supports.")
         }
         mConnection.Send( &(ds[0]), ds.size() );
         mOutgoingMessage.ClearData();
      }
   }

   ///\todo move this to DispatchNetworkMessage,
   /// when we are assuming the messageprocessor component is connected, which seems lame,
   /// because right now it doesn't need to rely on the messageprocessor component to exist.
   // build the network buffer
   const dtCore::UniqueId& uid = msg.GetAboutActorId();
   if(dtGame::GameActorProxy* gap = this->GetGameManager()->FindGameActorById( uid ) )
   {
      if( !gap->IsRemote() )
      {
         ///\todo determine if the message will exceed the MTU,
         /// if yes, write to the socket before handling more Messages.
         mOutgoingMessage.Handle( msg );
      }
   }
}

DIS::IncomingMessage& MasterComponent::GetIncomingMessage()
{
   return mIncomingMessage;
}

const DIS::IncomingMessage& MasterComponent::GetIncomingMessage() const
{
   return mIncomingMessage;
}

OutgoingMessage& MasterComponent::GetOutgoingMessage()
{
   return mOutgoingMessage;
}

const OutgoingMessage& MasterComponent::GetOutgoingMessage() const
{
   return mOutgoingMessage;
}

SharedState* MasterComponent::GetSharedState()
{
   return mConfig;
}

const SharedState* MasterComponent::GetSharedState() const
{
   return mConfig;
}
