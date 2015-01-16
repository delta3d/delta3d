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
#ifndef DELTA_FIRE_FIGHTER_HATCH_ACTOR
#define DELTA_FIRE_FIGHTER_HATCH_ACTOR

#include <fireFighter/gameitemactor.h>
#include <fireFighter/export.h>

namespace dtAudio
{
   class Sound;
}

namespace osg
{
   class MatrixTransform;
}

class FIRE_FIGHTER_EXPORT HatchActor : public GameItemActor
{
   public:

      /// Constructer
      HatchActor(dtGame::GameActorProxy& parent);

      /**
       * Activates this game item
       */
      virtual void Activate(bool enable = true);

      /// Called when this actor is added to the scene
      virtual void OnEnteredWorld();

      /// Invokable called when the GameMap is loaded
      void OnMapLoaded(const dtGame::Message &msg);

   protected:

      /// Destructor
      virtual ~HatchActor();

   private:

      osg::MatrixTransform *mHatchNode;
      bool mGameMapLoaded;
};

class FIRE_FIGHTER_EXPORT HatchActorProxy : public GameItemActorProxy
{
   public:

      /// Constructor
      HatchActorProxy();

      /// Builds the properties
      virtual void BuildPropertyMap();

      /// Builds invokables
      virtual void BuildInvokables();

      /// Instantiates the actor
      virtual void CreateDrawable() { SetDrawable(*new HatchActor(*this)); }

   protected:

      /// Destructor
      virtual ~HatchActorProxy();
};

#endif
