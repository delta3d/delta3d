/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
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
 * Jeff P. Houde
 */

#ifndef directoractorproxy_h__
#define directoractorproxy_h__

#include <dtDAL/plugin_export.h>
#include <dtABC/export.h>
#include <dtDAL/actorproperty.h>
#include <dtDAL/map.h>

#include <dtGame/gameactor.h>
#include <dtGame/gameactorproxy.h>

#include <dtDirector/director.h>

namespace dtActors
{
   /////////////////////////////////////////////////////////////////////////////
   // ACTOR CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_PLUGIN_EXPORT DirectorActor : public dtGame::GameActor
   {
      DECLARE_MANAGEMENT_LAYER(DirectorActor)

   public:
      typedef dtGame::GameActor BaseClass;

      /**
      * Constructor.
      *
      * @param[in]  parent  The Game Actor Proxy.
      */
      DirectorActor(dtGame::GameActorProxy& parent);

      /**
      * Local tick function.
      *
      * @param[in]  tickMessage  The message.
      */
      virtual void OnTickLocal(const dtGame::TickMessage& tickMessage);

      /**
      * Initializes all of the Directors.
      * @note:  Only use this outside of STAGE.
      */
      virtual void OnLoadDirectors(const dtGame::Message&);
      void LoadDirectors();

      /**
      * Sets the Player Actor.
      *
      * @param[in]  value  The player actor.
      */
      void SetPlayerActor(const dtCore::UniqueId& value);

      /**
      * Retrieves the Player Actor.
      *
      * @return  The Player Actor.
      */
      const dtCore::UniqueId& GetPlayerActor();

      /**
      * Sets the Camera Actor.
      *
      * @param[in]  value  The camera actor.
      */
      void SetCameraActor(const dtCore::UniqueId& value);

      /**
      * Retrieves the Camera Actor.
      *
      * @return  The Camera Actor.
      */
      const dtCore::UniqueId& GetCameraActor();

      /**
      * Sets node logging.
      *
      * @param[in]  value  True to log nodes.
      */
      void SetNodeLogging(bool value) {mNodeLogging = value;}

      /**
      * Retrieves whether scripts are node logging.
      *
      * @return  True if node logging is enabled.
      */
      bool GetNodeLogging() {return mNodeLogging;}

      /**
      * Sets recording on scripts.
      *
      * @param[in]  value  True to record.
      */
      void SetRecording(bool value) {mRecording = value;}

      /**
      * Retrieves whether the scripts are recording.
      */
      bool GetRecording() {return mRecording;}

      /**
      * Sets the Director Resource value at the current array index.
      *
      * @param[in]  value  The resource value.
      */
      void SetDirectorResource(const dtDAL::ResourceDescriptor& value);

      /**
      * Retrieves the Director Resource at the current array index.
      *
      * @return  A resource descriptor string.
      */
      dtDAL::ResourceDescriptor GetDirectorResource();

      /**
      * Sets the current Director array index.
      *
      * @param[in]  index  The index.
      */
      void SetDirectorIndex(int index);

      /**
      * Retrieves a default Director Resource to use in a new array index.
      *
      * @return  A default resource descriptor string.
      */
      dtDAL::ResourceDescriptor GetDefaultDirector(void);

      /**
      * Retrieves the array of Director Resources.
      *
      * @return  Resource Director Array.
      */
      std::vector<dtDAL::ResourceDescriptor> GetDirectorArray() const;

      /**
      * Sets the Array of Director Resources.
      *
      * @param[in]  value  The new array.
      */
      void SetDirectorArray(const std::vector<dtDAL::ResourceDescriptor>& value);

      /**
       * Retrieves the list of instanced directors that are loaded.
       *
       * @return  Instanced Directors.
       */
      std::vector<dtCore::RefPtr<dtDirector::Director> >& GetInstancedDirectors();

   protected:

      /**
      * Destructor.
      */
      virtual ~DirectorActor();

   private:

      dtCore::UniqueId mPlayerActor;
      dtCore::UniqueId mCameraActor;

      bool mNodeLogging;
      bool mRecording;

      std::vector<dtDAL::ResourceDescriptor> mResourceList;
      int mResourceIndex;

      std::vector<dtCore::RefPtr<dtDirector::Director> > mDirectorList;
   };

   /////////////////////////////////////////////////////////////////////////////
   // PROXY CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_PLUGIN_EXPORT DirectorActorProxy : public dtGame::GameActorProxy
   {
   public:
      typedef dtGame::GameActorProxy BaseClass;

      static const dtUtil::RefString CLASS_NAME;

      /**
      * Constructor.
      */
      DirectorActorProxy();

      /**
      * Creates the Actor for this Proxy.
      */
      virtual void CreateActor();

      /**
      * Builds all properties for this Proxy.
      */
      virtual void BuildPropertyMap();

      /**
      * Event handler when this actor has entered the world.
      */
      void OnEnteredWorld();

      /**
      * Build all invokables.
      */
      void BuildInvokables();

      /**
       * Retrieves the render mode for this proxy.
       *
       * @return  The render mode.
       */
      virtual const RenderMode& GetRenderMode();

      /**
      * Retrieves the billboard icon for this proxy.
      *
      * @return  The billboard icon.
      */
      virtual dtDAL::ActorProxyIcon* GetBillBoardIcon();

   protected:

      /**
      * Destructor.
      */
      virtual ~DirectorActorProxy();

   private:
   };
}
#endif // directoractorproxy_h__
