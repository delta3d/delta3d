/* -*-c++-*-
 * Delta3D
 * Copyright 2014, David Guthrie
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
#include <dtNetGM/servernetworkcomponent.h>
#include <dtNetGM/clientnetworkcomponent.h>

namespace dtNetGM
{
   const dtCore::RefPtr<dtCore::SystemComponentType> NetworkComponent::TYPE(new dtCore::SystemComponentType("NetworkComponent","GMComponents",
         "Base Client-Server networking component",
         dtGame::GMComponent::BaseGMComponentType));

   const dtCore::RefPtr<dtCore::SystemComponentType> ServerNetworkComponent::TYPE(new dtCore::SystemComponentType("ServerNetworkComponent","GMComponents",
         "Server component for client-server networking.",
         dtNetGM::NetworkComponent::TYPE));
   const dtUtil::RefString ServerNetworkComponent::DEFAULT_NAME(TYPE->GetName());

   const dtCore::RefPtr<dtCore::SystemComponentType> ClientNetworkComponent::TYPE(new dtCore::SystemComponentType("ClientNetworkComponent","GMComponents",
         "Handles a the client side of a Client-server network component.", dtNetGM::NetworkComponent::TYPE));
   const std::string ClientNetworkComponent::DEFAULT_NAME(TYPE->GetName());

}
