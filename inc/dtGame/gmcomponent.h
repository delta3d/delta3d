/*
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
 * William E. Johnson II
 */

#ifndef DELTA_GMCOMPONENT
#define DELTA_GMCOMPONENT

#include <string>
#include <dtGame/gamemanager.h>
#include <dtCore/systemcomponenttype.h>
#include <dtCore/base.h>

namespace dtGame
{

   class Message;

   class DT_GAME_EXPORT GMComponent : public dtCore::BaseActorObject
   {
   public:
      typedef dtCore::BaseActorObject BaseClass;

      /// All derived gm component types must pass this or a descendant as a parent type.
      static const dtCore::RefPtr<dtCore::SystemComponentType> BaseGMComponentType;

      /// Constructor
      /// @param type code must supply a component type for this
      GMComponent(dtCore::SystemComponentType& type);

      /**
       * With this constructor, a custom actor type object will be generated to stub in its place
       * This will allow one to create a component that only exists in code using the older method of creating it.
       * This will not work for components that can be loaded from a map.
       */
      GMComponent(const std::string& name);

      const dtCore::SystemComponentType& GetType() const;

      bool IsPlaceable() const override;

   protected:
      /// Destructor
      virtual ~GMComponent();

   public:

      bool IsSystemComponent() const override { return true; }

      /**
       * handles a sent a message
       * @param The message
       */
      virtual void DispatchNetworkMessage(const Message& message);

      /**
       * handles a processed a message
       * @param The message
       */
      virtual void ProcessMessage(const Message& message);

      /**
       * Gets the game manager that owns this component
       * @return The game manager
       * @see dtGame::GameManager
       */
      GameManager* GetGameManager() const { return mParent.get(); }

      /**
       * Get the priority of this component.  This can be set as a property and the value will be used
       * when the map is loaded into the GM.  The priority translates to the order components receive messages.
       * Higher priorities get messages sooner.
       * @return the value of the priority.
       */
      DT_DECLARE_ACCESSOR(dtUtil::EnumerationPointer<GameManager::ComponentPriority>, ComponentPriority);

      void BuildPropertyMap() override;

      /**
       * Called immediately after a component is added to the GM. Override this
       * to do init type behavior that needs access to the GameManager.
       */
      virtual void OnAddedToGM();

      /**
       * Called immediately after a component is removed from the GM. This is
       * where any previously allocated memory should be deallocated, files unloaded,
       * resources free'd, etc.  This gets called when the GMComponent gets removed
       * from the GameManager and when the GameManager gets shut down.
       * @see GameManager::RemoveComponent()
       * @see GameManager::Shutdown()
       */
      virtual void OnRemovedFromGM();

      void Init(const dtCore::ActorType& actorType) override;

      /**
       * Sets the game manager that owns this component
       * Only the GameManager should set this or things won't work.
       * @see dtGame::GameManager
       */
      void SetGameManager(GameManager* gameManager);
   private:


      dtCore::RefPtr<dtCore::SystemComponentType> mType;

      dtCore::ObserverPtr<GameManager> mParent;
      bool mInitialized;

      // -----------------------------------------------------------------------
      //  Unimplemented constructors and operators
      // -----------------------------------------------------------------------
      GMComponent(const GMComponent&);
      GMComponent& operator=(const GMComponent&);
   };

} // namespace dtGame

#endif // DELTA_GMCOMPONENT
