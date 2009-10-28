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
* Michael Guerrero
*/
#ifndef DELTA_CAL3DGAMEACTOR_H
#define DELTA_CAL3DGAMEACTOR_H

#include <dtGame/gameactor.h>
#include <dtAnim/export.h>
#include <dtAnim/cal3danimator.h>
#include <dtAnim/cal3ddatabase.h>
#include <dtDAL/namedparameter.h>  // for function signatures
#include <string>                  // for static constants

// namespace specific forward declarations
namespace dtCore{ class Scene;         }


/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace osg   { class Geode;         }
/// @endcond


namespace dtAnim
{
   /**
   * This class is the game actor for an animated model.
   * @see GameActor
   * @see Cal3DGameActorProxy
   */
   class DT_ANIM_EXPORT Cal3DGameActor : public dtGame::GameActor
   {
   public:
      /// string constants for this actor
      struct DT_ANIM_EXPORT PropertyNames
      {
         static const dtUtil::RefString ANIMATION_GROUP;
         static const dtUtil::RefString ANIMATION_GROUP_LABEL;
         static const dtUtil::RefString ANIMATION_BLEND_GROUP;
         static const dtUtil::RefString ANIMATION_BLEND_WEIGHT;
         static const dtUtil::RefString ANIMATION_BLEND_ID;
         static const dtUtil::RefString ANIMATION_BLEND_DELAY;
         static const dtUtil::RefString RENDER_MODE;
         static const dtUtil::RefString RENDER_MODE_LABEL;
      };

      /// used to describe what to render.
      /// \todo should this be a dtUtil::Enumeration?
      enum RenderModeBits
      {
         RENDER_MODE_NONE  =0,
         RENDER_MODE_SKIN  =1<<0,
         RENDER_MODE_BONES =1<<1
      };

      /**
      * Constructs a Cal3DGameActor actor.
      * @param proxy The actor proxy owning this task actor.
      */
      Cal3DGameActor(dtGame::GameActorProxy &proxy);

      /**
      * Loads a model file.
      * @param modelFile The filename of the model to load.
      */
      virtual void SetModel(const std::string &modelFile);

      /**
      * Called when the actor has been added to the game manager.
      */
      virtual void AddedToScene(dtCore::Scene* scene);

      virtual void OnEnteredWorld();

      void ApplyAnimationGroup(const dtDAL::NamedGroupParameter& prop);
      dtCore::RefPtr<dtDAL::NamedGroupParameter> MakeAnimationGroup();

      dtAnim::Cal3DAnimator* GetAnimator();
      const dtAnim::Cal3DAnimator* GetAnimator() const;

      virtual void OnTickLocal(const dtGame::TickMessage& tickMessage);

      void SetRenderMode(int bits);
      int GetRenderMode() const;

   protected:

      dtCore::RefPtr<osg::Geode>            mModelGeode;
      dtCore::RefPtr<osg::Geode>            mSkeletalGeode;   ///< support to visualize the skeletal.
      dtCore::RefPtr<dtAnim::Cal3DDatabase> mModelLoader;
      dtCore::RefPtr<dtAnim::Cal3DAnimator> mAnimator;

      typedef unsigned char RenderModeBitContainer;
      RenderModeBitContainer mRenderModeBits;

      /// Destroys this actor.
      virtual ~Cal3DGameActor();

   private:

   };

   /**
   * This class is the proxy for an animated model game object.
   * @see GameActorProxy
   * @see Cal3DGameActor
   */
   class DT_ANIM_EXPORT Cal3DGameActorProxy : public dtGame::GameActorProxy
   {
   public:

      /**
      * Constructs the proxy.
      */
      Cal3DGameActorProxy();

      /**
      * Builds the property map for the task actor proxy.  These properties
      * wrap the specified properties located in the actor.
      */
      virtual void BuildPropertyMap();

      /**
      * Registers any invokables used by the proxy.  The invokables
      * allow the actor to hook into the game manager messages system.
      */
      virtual void BuildInvokables();

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
      virtual ~Cal3DGameActorProxy();

      /**
      * Called by the game manager during creation of the proxy.  This method
      * creates the real actor and returns it.
      */
      virtual void CreateActor();

   private:
   };

}

#endif //DELTA_CAL3DGAMEACTOR_H
