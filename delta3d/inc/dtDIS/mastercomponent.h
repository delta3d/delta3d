#ifndef _dt_dis_master_component_h_
#define _dt_dis_master_component_h_

#include <dtGame/gmcomponent.h>      // for base class
#include <dtDIS/pluginmanager.h>     // for member
#include <dtDIS/connection.h>        // for member
#include <dtDIS/outgoingmessage.h>   // for member
#include <DIS/IncomingMessage.h>     // for member
#include <string>                    // for parameter, member
#include <dtDIS/dtdisexport.h>       // for export symbols

namespace dtDIS
{
   class SharedState;

   ///\brief Supports a framework to translate DIS PDUs into dtGame::Message instances.
   ///
   /// The component will connect to a DIS network
   /// and load plugins to handle different DIS packet types.
   /// All you need to do is add this component to the dtGame::GameManager instance,
   /// and it will handle everything dealing with DIS for you, which includes:
   /// - connecting to the DIS network, as specified in the connection file
   /// - loading DIS packet plugins, also found in the connection file
   /// - mapping DIS::EntityType instances to the ActorType instances specified in the mapping file.
   class DT_DIS_EXPORT MasterComponent : public dtGame::GMComponent
   {
   public:
      /// supply the configuration files needed to support DIS.
      /// @param config the result of reading data files needed for this component to work.  This class does not assume ownership of the memory.
      /// @param connection_file The XML file that shows the ConnectionData.
      /// @param mapping_file The XML file that shows the mappings from DIS::EntityIDs to dtDAL::ActorTypes.
      MasterComponent(SharedState* config);

      /// connects to the socket.
      /// executed by the GameManager after being "added" to the GameManager.
      /// \note This can be ignored by client developers.
      void OnAddedToGM();

      /// disconnects from the socket.
      /// executed by the GameManager after being "removed" from the GameManager.
      /// \note This can be ignored by client developers.
      void OnRemovedFromGM();

      /// executed by the GameManager for observing any Message passing.
      /// \note This can be ignored by client developers.
      void ProcessMessage(const dtGame::Message& msg);

      ///// a convenience accessor, not needed usually.
      //const PluginManager& GetPluginManager() const;


   protected:
      ~MasterComponent();

      /// a slot to be executed when loading each plugin.
      /// \note This can be ignored by client developers.
      void OnPluginLoaded(PluginManager::LibraryRegistry::value_type& entry);
      //void OnPluginLoaded(const std::string& path, PluginManager::RegistryEntry& entry);

      /// a slot to be executed when unloading each plugin.
      /// \note This can be ignored by client developers.
      void OnPluginUnloaded(PluginManager::LibraryRegistry::value_type& plugin);
      //void OnPluginUnloaded(const std::string& path, PluginManager::RegistryEntry& entry);

      //void ConnectToNetwork();
      //void DisconnectFromNetwork();

      void LoadPlugins(const std::string& directory);
      void UnloadPlugins();

   private:
      PluginManager mPluginManager;
      Connection mConnection;
      DIS::IncomingMessage mIncomingMessage;
      OutgoingMessage mOutgoingMessage;
      SharedState* mConfig;
   };
}

#endif  // _dt_dis_master_component_h_
