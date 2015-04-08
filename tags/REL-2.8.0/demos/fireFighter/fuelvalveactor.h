/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation
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
#ifndef DELTA_FIRE_FIGHTER_FUEL_VALVE_ACTOR
#define DELTA_FIRE_FIGHTER_FUEL_VALVE_ACTOR

#include <fireFighter/gameitemactor.h>
#include <fireFighter/export.h>

class FIRE_FIGHTER_EXPORT FuelValveActor : public GameItemActor
{
   public:

      /// Constructor
      FuelValveActor(dtGame::GameActorProxy& parent);

      /**
       * Activates this game item
       */
      virtual void Activate(bool enable = true);

   protected:

      /// Destructor
      virtual ~FuelValveActor();
};

class FIRE_FIGHTER_EXPORT FuelValveActorProxy : public GameItemActorProxy
{
   public:

      /// Constructor
      FuelValveActorProxy();

      /// Builds the properties
      virtual void BuildPropertyMap();

      /// Builds the invokables
      virtual void BuildInvokables();

      /// Creates the actor
      virtual void CreateDrawable() { SetDrawable(*new FuelValveActor(*this)); }

   protected:

      /// Destructor
      virtual ~FuelValveActorProxy();
};

#endif
