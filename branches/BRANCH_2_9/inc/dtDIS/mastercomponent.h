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

#ifndef __DELTA_DTDIS_MASTER_COMPONENT_H__
#define __DELTA_DTDIS_MASTER_COMPONENT_H__

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
   class DefaultPlugin;

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
      static const dtCore::RefPtr<dtCore::SystemComponentType> TYPE;
      static const std::string DEFAULT_NAME;

      /// supply the configuration files needed to support DIS.
      /// @param config the result of reading data files needed for this component to work.  This class does not assume ownership of the memory.
      /// @param connection_file The XML file that shows the ConnectionData.
      /// @param mapping_file The XML file that shows the mappings from DIS::EntityIDs to dtCore::ActorTypes.
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

      /// Time to convert the Message into DIS PDU's and send out
      virtual void DispatchNetworkMessage(const dtGame::Message& message);

      ///// a convenience accessor, not needed usually.
      //const PluginManager& GetPluginManager() const;

      /// obtain the IncomingMessage for attaching and removing processors.
      /// @return the IncomingMessage instance.
      DIS::IncomingMessage& GetIncomingMessage();

      /// obtain the IncomingMessage for attaching and removing processors.
      /// @return the IncomingMessage instance.
      const DIS::IncomingMessage& GetIncomingMessage() const;

      /// obtain the OutgoingMessage for attaching and removing adapters.
      /// @return the OutgoingMessage instance.
      OutgoingMessage& GetOutgoingMessage();

      /// obtain the OutgoingMessage for attaching and removing adapters.
      /// @return the OutgoingMessage instance.
      const OutgoingMessage& GetOutgoingMessage() const;

      /// obtain the SharedState to know the current state of entity management and configurations.
      /// @return the SharedState instance.
      SharedState* GetSharedState();

      /// obtain the SharedState to know the current state of entity management and configurations.
      /// @return the SharedState instance.
      const SharedState* GetSharedState() const;

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
      DefaultPlugin* mDefaultPlugin;
   };
}

#endif  // __DELTA_DTDIS_MASTER_COMPONENT_H__
