/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology
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
 * Bradley G Anderegg
 */
#ifndef DELTA_ANIMATIONGAMEACTOR_H
#define DELTA_ANIMATIONGAMEACTOR_H

#include <dtCore/refptr.h>
#include <dtGame/gameactor.h>
#include <dtAnim/export.h>
#include <dtAnim/animationhelper.h> //for RefPtr member
#include <string>                  // for static constants

namespace dtAnim
{

   /**
    * This class is the proxy for an animated model game object.
    * @see GameActorProxy
    * @see AnimationGameActor
    */
   class DT_ANIM_EXPORT AnimationGameActor : public dtGame::GameActorProxy
   {
   public:
      /**
       * Constructs the proxy.
       */
      AnimationGameActor();

      /**
       * Builds the property map for the task actor proxy.  These properties
       * wrap the specified properties located in the actor.
       */
      virtual void BuildPropertyMap();

      /**
       * Overridden to add the animation helper as an actor component.
       */
      virtual void BuildActorComponents();

      /**
       * Gets the method by which this static mesh is rendered. This is used by STAGE.
       * @return If there is no geometry currently assigned, this
       *  method will return RenderMode::DRAW_BILLBOARD_ICON.  If
       *  there is geometry assigned to this static mesh, RenderMode::DRAW_ACTOR
       *  is returned.
       */
      virtual const dtCore::BaseActorObject::RenderMode& GetRenderMode();

      /**
       * Gets the billboard used to represent static mesh if this proxy's
       * render mode is RenderMode::DRAW_BILLBOARD_ICON. Used by STAGE.
       * @return
       */
      virtual dtCore::ActorProxyIcon* GetBillBoardIcon();

      virtual osg::BoundingBox GetBoundingBox();

   protected:
      /// Destroys the proxy.
      virtual ~AnimationGameActor();

      /**
       * Called by the game manager during creation of the proxy.  This method
       * creates the real actor and returns it.
       */
      virtual void CreateDrawable();
   };

} // namespace dtAnim

#endif // DELTA_ANIMATIONGAMEACTOR_H
