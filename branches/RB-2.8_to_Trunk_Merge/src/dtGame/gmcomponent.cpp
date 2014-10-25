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
#include <prefix/dtgameprefix.h>
#include <dtGame/gmcomponent.h>
#include <dtGame/message.h>
#include <dtCore/propertymacros.h>

namespace dtGame
{
   //////////////////////////////////////////////
   GMComponent::GMComponent(dtCore::SystemComponentType& type)
   : BaseClass()
   , mComponentPriority(&GameManager::ComponentPriority::NORMAL)
   , mType(&type)
   , mParent(NULL)
   , mInitialized(false)
   {
      SetName(type.GetName());
   }

   //////////////////////////////////////////////
   GMComponent::GMComponent(const std::string& name)
   : BaseClass()
   , mComponentPriority(&GameManager::ComponentPriority::NORMAL)
   , mType(new dtCore::SystemComponentType(name, "GMComponents", "An In-code type", BaseGMComponentType))
   , mParent(NULL)
   , mInitialized(false)
   {
      SetName(name);
   }

   //////////////////////////////////////////////
   GMComponent::~GMComponent()
   {
   }

   //////////////////////////////////////////////
   const dtCore::SystemComponentType& GMComponent::GetType() const
   {
      return *mType;
   }

   //////////////////////////////////////////////
   bool GMComponent::IsPlaceable() const
   {
      return false;
   }

   //////////////////////////////////////////////
   void GMComponent::DispatchNetworkMessage(const Message& message)
   {
   }

   //////////////////////////////////////////////
   void GMComponent::ProcessMessage(const Message& message)
   {
   }

   DT_IMPLEMENT_ACCESSOR(GMComponent, dtUtil::EnumerationPointer<GameManager::ComponentPriority>, ComponentPriority)

   //////////////////////////////////////////////
   /*override*/ void GMComponent::BuildPropertyMap()
   {
      BaseClass::BuildPropertyMap();

      const dtUtil::RefString GM_COMP_GROUP("GMComponent");
      typedef dtCore::PropertyRegHelper<GMComponent> RegHelperType;
      RegHelperType propReg(*this, this, GM_COMP_GROUP);

      DT_REGISTER_PROPERTY(ComponentPriority,
            "The priority translates to the order components receive messages.  Higher priorities get messages before lower priority components.",
            RegHelperType, propReg);
   }


   //////////////////////////////////////////////
   void GMComponent::SetGameManager(GameManager* gameManager)
   {
      mParent = gameManager;
   }

   //////////////////////////////////////////////////////////////////////////
   void GMComponent::Init(const dtCore::ActorType& actorType)
   {
      if (!mInitialized)
      {
         BaseClass::Init(actorType);
         mInitialized = true;
      }
   }

   //////////////////////////////////////////////
   void GMComponent::OnAddedToGM()
   {
      Init(*mType);
   }

   //////////////////////////////////////////////
   void GMComponent::OnRemovedFromGM() {}

   //////////////////////////////////////////////
   GMComponent::GMComponent(const GMComponent&)
   : mComponentPriority(&GameManager::ComponentPriority::NORMAL)
   , mType(NULL)
   , mParent(NULL)
   , mInitialized(false)
   {
   }

   //////////////////////////////////////////////
   GMComponent& GMComponent::operator=(const GMComponent&)
   {
      return *this;
   }

}
