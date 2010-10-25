/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2010, Alion Science and Technology
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
 */

#ifndef gmimpl_h__
#define gmimpl_h__

#include <queue>
#include <set>
#include <list>

#include <dtCore/uniqueid.h>
#include <dtCore/timer.h>
#include <dtGame/gmstatistics.h>
#include <dtGame/gmsettings.h>
#include <dtGame/machineinfo.h>
#include <dtGame/messagefactory.h>
#include <dtDAL/librarymanager.h>
#include <dtGame/gamemanager.h>
#include <dtGame/mapchangestatedata.h>
#include <dtGame/gmcomponent.h>
#include <dtGame/environmentactor.h>
#include <dtCore/scene.h>



namespace dtDAL
{
   class BaseActorObject;
}

namespace dtGame
{
   class GameActorProxy;

   /// A wrapper for data like stats to prevent includes wherever gamemanager.h is used - uses the pimple pattern (like system)
   class GMImpl
   {
   public:

      GMImpl(dtCore::Scene& scene);
      
      ~GMImpl()
      {
      }

      struct TimerInfo
      {
         std::string name;
         dtCore::UniqueId aboutActor;
         dtCore::Timer_t time;
         bool repeat;
         dtCore::Timer_t interval;

         bool operator < (const TimerInfo& rhs) const
         {
            if (time == rhs.time)
            {
               return this <& rhs;
            }
            return time < rhs.time;
         }
      };

      /// Does the work of ClearTimer for each of the timer info sets.
      void ClearTimerSingleSet(std::set<TimerInfo>& timerSet,
                               const std::string& name, const GameActorProxy* proxy);

      void ClearTimersForActor(std::set<TimerInfo>& timerSet, const GameActorProxy& proxy);

      /**
       * Helper method to process the timers. This is called from PreFrame
       * @param listToProcess The timer list to process
       * @param clockTime The time to use
       * @note The clock time should correspond to the list to be processed
       */
      void ProcessTimers(GameManager& gm, std::set<TimerInfo>& listToProcess, dtCore::Timer_t clockTime);

      /**
       * Removes the proxy from the scene
       * @param proxy the proxy to remove from the scene.
       */
      void RemoveActorFromScene(GameManager& gm, dtDAL::BaseActorObject& proxy);

      /**
       * Private helper method to send an environment changed message
       * @param envActor The about actor of the message
       */
      void SendEnvironmentChangedMessage(GameManager& gm, IEnvGameActorProxy* envActor);

      typedef std::map< dtCore::UniqueId, dtCore::RefPtr<GameActorProxy> > GameActorMap;
      typedef std::map< dtCore::UniqueId, dtCore::RefPtr<dtDAL::BaseActorObject> > ActorMap;

      /// stats for the work of the GM - in a class so its less obtrusive to the gm
      GMStatistics mGMStatistics;
      dtCore::RefPtr<MachineInfo> mMachineInfo;
      dtCore::RefPtr<IEnvGameActorProxy>  mEnvironment;
      GameActorMap mGameActorProxyMap;
      GameActorMap mPrototypeActors;
      ActorMap  mBaseActorObjectMap;
      std::vector<dtCore::RefPtr<GameActorProxy> > mDeleteList;

      // These are used during changing the map so that
      // the map code can modify game manager with some control.
      //bool mSendCreatesAndDeletes;
      //bool mAddActorsToScene;
      std::set<TimerInfo> mSimulationTimers, mRealTimeTimers;
      MessageFactory mFactory;

      typedef std::pair<dtCore::RefPtr<GameActorProxy>, std::string> ProxyInvokablePair;
      typedef std::multimap<const MessageType*, ProxyInvokablePair > GlobalMessageListenerMap;
      GlobalMessageListenerMap mGlobalMessageListeners;

      typedef std::multimap<dtCore::UniqueId, ProxyInvokablePair > ProxyInvokableMap;
      typedef std::map<const MessageType*,  ProxyInvokableMap> ActorMessageListenerMap;
      ActorMessageListenerMap mActorMessageListeners;

      typedef std::list<dtCore::RefPtr<dtGame::GMComponent> > GMComponentContainer;
      GMComponentContainer mComponentList;

      std::queue<dtCore::RefPtr<const Message> > mSendNetworkMessageQueue;
      std::queue<dtCore::RefPtr<const Message> > mSendMessageQueue;

      dtCore::RefPtr<dtCore::Scene> mScene;
      dtCore::RefPtr<dtDAL::LibraryManager> mLibMgr;
      GameManager::NameVector mLoadedMaps;
      dtCore::RefPtr<MapChangeStateData> mMapChangeStateData;

      /// application the gm has. the one and only.
      dtABC::Application* mApplication;
      bool mRemoveGameEventsOnMapChange;

      dtUtil::Log* mLogger;

      dtCore::RefPtr<GMSettings> mGMSettings;
   };
}
#endif // gmimpl_h__
