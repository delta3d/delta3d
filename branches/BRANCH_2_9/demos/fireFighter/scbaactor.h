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
#ifndef DELTA_FIRE_FIGHTER_SCBA_ACTOR
#define DELTA_FIRE_FIGHTER_SCBA_ACTOR

#include <fireFighter/gameitemactor.h>
#include <fireFighter/export.h>

class FIRE_FIGHTER_EXPORT SCBAActor : public GameItemActor
{
   public:

      /// Constructor
      SCBAActor(dtGame::GameActorProxy& parent);

      virtual void Activate(bool enable = true);

   protected:

      /// Destructor
      virtual ~SCBAActor();

      /// Called when the actor is added to the game manager
      virtual void OnEnteredWorld();
};

class FIRE_FIGHTER_EXPORT SCBAActorProxy : public GameItemActorProxy
{
   public:

      /// Constructor
      SCBAActorProxy();

      /// Builds the properties
      virtual void BuildPropertyMap();

      /// Builds the invokables
      virtual void BuildInvokables();

      /// Instantiates the actor
      virtual void CreateDrawable() { SetDrawable(*new SCBAActor(*this)); }

   protected:

      /// Destructor
      virtual ~SCBAActorProxy();
};

#endif
