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

#include <dtCore/plugin_export.h>
#include <dtABC/export.h>
#include <dtCore/actorproperty.h>
#include <dtCore/map.h>

#include <dtGame/gameactor.h>
#include <dtGame/gameactorproxy.h>

#include <dtDirector/director.h>

namespace dtActors
{
   /////////////////////////////////////////////////////////////////////////////
   // ACTOR CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_PLUGIN_EXPORT DirectorActor : public dtGame::GameActorProxy
   {
      DECLARE_MANAGEMENT_LAYER(DirectorActor)

   public:
      typedef dtGame::GameActorProxy BaseClass;

      static const dtUtil::RefString CLASS_NAME;

      /**
      * Constructor.
      *
      * @param[in]  parent  The Game Actor Proxy.
      */
      DirectorActor();
      /**
      * Creates the Actor for this Proxy.
      */
      virtual void CreateDrawable();

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
      virtual dtCore::ActorProxyIcon* GetBillBoardIcon();

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
      * Sets the Director Resource value at the current array index.
      *
      * @param[in]  value  The resource value.
      */
      void SetDirectorResource(const dtCore::ResourceDescriptor& value);

      /**
      * Retrieves the Director Resource at the current array index.
      *
      * @return  A resource descriptor string.
      */
      dtCore::ResourceDescriptor GetDirectorResource();

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
      dtCore::ResourceDescriptor GetDefaultDirector(void);

      /**
      * Retrieves the array of Director Resources.
      *
      * @return  Resource Director Array.
      */
      std::vector<dtCore::ResourceDescriptor> GetDirectorArray() const;

      /**
      * Sets the Array of Director Resources.
      *
      * @param[in]  value  The new array.
      */
      void SetDirectorArray(const std::vector<dtCore::ResourceDescriptor>& value);

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

      std::vector<dtCore::RefPtr<dtDirector::Director> > mDirectorList;

   private:

      bool mNodeLogging;

      std::vector<dtCore::ResourceDescriptor> mResourceList;
      int mResourceIndex;
   };

}
#endif // directoractorproxy_h__
