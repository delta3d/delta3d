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
#ifndef DELTA_ANIMATIONGAMEACTOR2_H
#define DELTA_ANIMATIONGAMEACTOR2_H

#include <dtCore/refptr.h>
#include <dtGame/gameactor.h>
#include <dtDAL/plugin_export.h>
#include <dtDAL/namedparameter.h>  // for function signatures
#include <string>                  // for static constants

namespace dtAnim
{
   class AnimationHelper;
}


namespace dtActors
{
   /**
    * This class is the game actor for an animated model.
    * @see GameActor
    * @see AnimationGameActorProxy  
    */
   class DT_PLUGIN_EXPORT AnimationGameActor2 : public dtGame::GameActor
   {
      public:

         /**
          * Constructs a AnimationGameActor actor.
          * @param proxy The actor proxy owning this task actor.     
          */
         AnimationGameActor2(dtGame::GameActorProxy &proxy);   

         /**
          * Loads a model file.
          * @param modelFile The filename of the model to load.
          */
         virtual void SetModel(const std::string &modelFile);


         dtAnim::AnimationHelper* GetHelper();
         const dtAnim::AnimationHelper* GetHelper() const;


      protected:      

         dtCore::RefPtr<dtAnim::AnimationHelper> mHelper;    

         /// Destroys this actor.     
         virtual ~AnimationGameActor2();    

      private:

   };

   /**
    * This class is the proxy for an animated model game object.
    * @see GameActorProxy
    * @see AnimationGameActor  
    */
   class DT_PLUGIN_EXPORT AnimationGameActorProxy2 : public dtGame::GameActorProxy
   {
      public:

         /**
          * Constructs the proxy.
          */
         AnimationGameActorProxy2();

         /**
          * Builds the property map for the task actor proxy.  These properties
          * wrap the specified properties located in the actor.
          */
         virtual void BuildPropertyMap();

         /**
          * Gets the method by which this static mesh is rendered. This is used by STAGE.
          * @return If there is no geometry currently assigned, this
          *  method will return RenderMode::DRAW_BILLBOARD_ICON.  If
          *  there is geometry assigned to this static mesh, RenderMode::DRAW_ACTOR
          *  is returned.
          */
         virtual const dtDAL::ActorProxy::RenderMode& GetRenderMode();

         /**
          * Gets the billboard used to represent static mesh if this proxy's
          * render mode is RenderMode::DRAW_BILLBOARD_ICON. Used by STAGE.
          * @return
          */
         virtual dtDAL::ActorProxyIcon* GetBillBoardIcon();

      protected:

         /// Destroys the proxy.
         virtual ~AnimationGameActorProxy2();

         /**
          * Called by the game manager during creation of the proxy.  This method
          * creates the real actor and returns it.
          */
         virtual void CreateActor();

      private:
   };

}

#endif //DELTA_ANIMATIONGAMEACTOR2_H
