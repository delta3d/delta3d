/* -*-c++-*-
 * Delta3D
 * Copyright 2013, David Guthrie
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
 * David Guthrie
 */

#ifndef AUTOREGISTERGMCOMPONENT_H_
#define AUTOREGISTERGMCOMPONENT_H_

#include <dtGame/gmcomponent.h>
#include <dtGame/message.h>
#include <dtGame/messagetype.h>
#include <dtGame/basemessages.h>
#include <dtUtil/log.h>
#include <dtUtil/threadpool.h>


namespace dtGame
{
   template <typename ActorComponentType> class DataCentricGMComponent: public dtGame::GMComponent
   {
   public:

      typedef dtCore::RefPtr<ActorComponentType> ActorComponentPtr;
      typedef std::vector<ActorComponentPtr> ActorComponentPtrVec;

      class ActorCompUpdateTask: public dtUtil::ThreadPoolTask
      {
      public:
         ActorCompUpdateTask()
         : mUpdateDT(0.0)
         {
         }

         /////////////////////////////////////////////////////////////
         virtual void operator()()
         {
            for (unsigned i = 0; i < mActorComps.size(); ++i)
            {
               mActorComps[i]->Update(mUpdateDT);
            }
         }
         float mUpdateDT;
         ActorComponentPtrVec mActorComps;
      };

      typedef std::vector<dtCore::RefPtr<ActorCompUpdateTask> > ThreadTasksVec;

      struct ActorComponentData
      {
          ActorComponentPtr mActorComp;
      };

      typedef dtGame::GMComponent BaseClass;
      typedef std::map<dtCore::UniqueId, ActorComponentData > ActorCompMap;
      typedef typename  ActorCompMap::allocator_type AllocType_;
      typedef typename AllocType_::value_type ActorCompMapping;
      typedef typename ActorCompMap::iterator ActorCompIter;

      DataCentricGMComponent(dtCore::SystemComponentType& type)
      : BaseClass(type)
      {
      }

      DataCentricGMComponent(const std::string& name)
      : BaseClass(name)
      {
      }

      virtual ~DataCentricGMComponent()
      {
      }

      /**
       * Registers an actor with this component.  To simplify coding in the actor, specifically when it comes
       * to setting properties on the helper, the actor should create it's own helper and pass it in when registering.
       * @param toRegister the actor to register.
       * @param ac the preconfigured actor component object to use.
       * @throws dtUtil::Exception if this actor is already registered with the component.
       */
      virtual bool RegisterActor(dtGame::GameActorProxy& toRegister, ActorComponentType& ac)
      {
         ActorComponentData data;
         data.mActorComp = &ac;
         //if the insert fails, log a message.
         if (!mRegisteredActors.insert(ActorCompMapping(toRegister.GetId(), data)).second)
         {
            LOG_ERROR(std::string("Actor already registered with Component: ") + GetName());
            return false;
         }
         else
         {
            mThreadTasks.clear();
         }
         return true;
      }

      /// alternate unregister that takes the actor.
      void UnregisterActor(dtGame::GameActorProxy& toUnregister)
      {
         UnregisterActor(toUnregister.GetId());
      }

      /**
       * Unregisters an actor with this component.  To simplify coding in the actor, specifically when it comes
       * to setting properties on the helper, the actor should create it's own helper and pass it in when registering.
       * @param toUnregister the actor to register.
       * @param helper the preconfigured helper object to use.
       */
      virtual bool UnregisterActor(const dtCore::UniqueId& actorId)
      {
         bool result = false;
         ActorCompIter iter = mRegisteredActors.find(actorId);
         if (iter != mRegisteredActors.end())
         {
            mRegisteredActors.erase(iter);
            mThreadTasks.clear();
            result = true;
         }
         return result;
      }

      /**
       * @return true if the given actor is registered with this component.
       */
      bool IsRegisteredActor(dtGame::GameActorProxy& actor)
      {
         ActorCompIter iter = mRegisteredActors.find(actor.GetId());
         return iter != mRegisteredActors.end();
      }

      void ClearRegisteredActors()
      {
         mRegisteredActors.clear();
      }

      /**
       * Build up the thread worker tasks.  You must call
       * dtUtil::ThreadPool::ExecuteTasks();
       * yourself
       * @param dt the time delta.
       */
      void BuildThreadWorkerTasks(float dt)
      {
         if (mThreadTasks.empty())
         {
            unsigned threads = dtUtil::ThreadPool::GetNumImmediateWorkerThreads();


            for (unsigned i = 0; i < threads; ++i)
            {
               mThreadTasks.push_back(new ActorCompUpdateTask);
               mThreadTasks.back()->mActorComps.reserve(mRegisteredActors.size() + 1);
            }

            ActorCompIter iter = mRegisteredActors.begin();
            ActorCompIter end = mRegisteredActors.end();

            while (iter != end)
            {
               for (unsigned i = 0; i < threads && iter != end; ++i)
               {
                  ActorCompMapping& current = *iter;
                  mThreadTasks[i]->mActorComps.push_back(current.second.mActorComp);
                  ++iter;
               }
            }
         }

         for (unsigned i = 0; i < mThreadTasks.size(); ++i)
         {
            if (!mThreadTasks[i]->mActorComps.empty())
            {
               mThreadTasks[i]->mUpdateDT = dt;
               dtUtil::ThreadPool::AddTask(*mThreadTasks[i]);
            }
         }
      }


      /////////////////////////////////////////////////////////////////////////////////
      virtual void ProcessMessage(const dtGame::Message& message)
      {
         if (message.GetMessageType() == dtGame::MessageType::TICK_LOCAL)
         {
            const dtGame::TickMessage& mess = static_cast<const dtGame::TickMessage&>(message);
            TickLocal(mess.GetDeltaSimTime());
         }
         else if (message.GetMessageType() == dtGame::MessageType::INFO_ACTOR_DELETED)
         {
            // TODO Write unit tests for the delete message.
            UnregisterActor(message.GetAboutActorId());
         }
         else if (message.GetMessageType() == dtGame::MessageType::INFO_MAP_UNLOADED)
         {
            ClearRegisteredActors();
         }
      }

      /**
       * Basic tick local implementation.  Override to do something else.
       * @param dt simulation time elapsed in fractional seconds.
       */
      virtual void TickLocal(float dt)
      {
         BuildThreadWorkerTasks(dt);
         dtUtil::ThreadPool::ExecuteTasks();
      }

      /**
       * Gets the helper registered for an actor
       * @param proxy The proxy to get the helper for
       * @return A pointer to the helper, or NULL if the proxy is not registered
       */
      ActorComponentType* GetComponentForActor(const dtCore::UniqueId& id)
      {
         const typename ActorCompMap::const_iterator iter = mRegisteredActors.find(id);
         if (iter == mRegisteredActors.end())
         {
            return NULL;
         }

         return (*iter).second.mActorComp.get();
      }

      template <typename Func>
      void ForEachActorComponent(Func func)
      {
         std::for_each(mRegisteredActors.begin(), mRegisteredActors.end(), func);
      }

   private:
      ActorCompMap mRegisteredActors;
      ThreadTasksVec mThreadTasks;
  };
}
#endif /* AUTOREGISTERGMCOMPONENT_H_ */
