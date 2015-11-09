/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2007, Alion Science and Technology
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
 * Bradley Anderegg 03/30/2007
 */

#ifndef __DELTA_ANIMATIONHELPER_H__
#define __DELTA_ANIMATIONHELPER_H__

#include <dtAnim/export.h>
#include <dtAnim/attachmentcontroller.h>
#include <dtAnim/basemodelwrapper.h>
#include <dtAnim/modelloader.h>
#include <dtAnim/sequencemixer.h>
#include <dtAnim/animationcomponent.h>

#include <dtGame/datacentricactorcomponent.h>

#include <dtCore/refptr.h>
#include <dtCore/sigslot.h>
#include <dtCore/resourcedescriptor.h>

#include <dtUtil/command.h>
#include <dtUtil/enumeration.h>

#include <osg/Group>
#include <osg/Referenced>

#include <string>
#include <vector>

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace osg
{
   class Node;
   class Group;
}
/// @endcond

namespace dtCore
{
   class ActorProperty;
   class BaseActorObject;
}


namespace dtAnim
{
   class AnimNodeBuilder;
   class AnimationGameActor;
   class AnimationComponent;
   class AnimationUpdaterInterface;
   class PoseController;
   class PoseSequence;



   /////////////////////////////////////////////////////////////////////////////
   // TYPEDEFS
   /////////////////////////////////////////////////////////////////////////////
   typedef dtUtil::Command<void> AnimCommandCallback;
   typedef std::vector<dtCore::RefPtr<AnimCommandCallback> > AnimCommandArray;
   typedef dtUtil::Functor<void,TYPELIST_1(const std::string&)> AnimEventCallback;

   /**
    * Special command callback that passes a reference to the animatable
    * that triggers it.
    */
   typedef dtUtil::Functor<void, TYPELIST_1(Animatable*)> AnimReferenceCallback;
   typedef dtUtil::Command1<void, Animatable*> AnimReferenceCommandCallback;


   /**
    * The AnimationHelper class is a utility class to simplify adding animation
    * to an articulated entity, it provides support for loading, rendering and
    * animating.
    */
   class DT_ANIM_EXPORT AnimationHelper: public dtGame::DataCentricActorComponent<AnimationComponent, AnimationHelper>
   {
   public:
      typedef dtGame::DataCentricActorComponent<dtAnim::AnimationComponent, AnimationHelper> BaseClass;
      static const dtGame::ActorComponent::ACType TYPE;

      static const std::string PROPERTY_SKELETAL_MESH;

      /**
       * The constructor constructs a default AnimNodeBuilder, the Cal3DModelWrapper,
       * and AnimationController
       * are created on LoadModel()
       */
      AnimationHelper();

      DT_DECLARE_ACCESSOR(bool, AutoRegisterWithGMComponent);

      /// Called when the parent actor enters the "world".
      void OnEnteredWorld() override;
      /// Called when the parent actor leaves the "world".
      void OnRemovedFromWorld() override;

      /**
       * This function is used to create the proper actor properties this actor component
       */
      void BuildPropertyMap() override;

      /**
       * The user should call Update() on a per frame basis
       * this function updates the sequence mixer and the Cal3DAnimator
       */
      void Update(float dt) override;

      DT_DECLARE_ACCESSOR(dtCore::ResourceDescriptor, SkeletalMesh);

      DT_DECLARE_ACCESSOR(bool, LoadModelAsynchronously);

      /**
       * If you want to change the way the skeletal mesh node is assigned
       * to the drawable, set this to false and hook to the ModelLoadedSignal.
       */
      DT_DECLARE_ACCESSOR(bool, EnableAttachingNodeToDrawable);

      /**
       * This function loads a character XML (.dtChar) file.  On loading it
       * creates a Cal3DAnimator with the Cal3DModelWrapper
       * and then calls CreateGeode() on the AnimNodeBuilder
       *
       * @param pFilename the name of the file to load
       * @return whether or not we successfully loaded the file
       */
      bool LoadModel(const dtCore::ResourceDescriptor& resource);

      /**
       * @note Just because this function returns false doesn't mean you have a loaded
       *       model.  The load could have failed, but due to threading issues, the model doesn't
       *       get assigned and setup until the Update(..) call after the load finishes, so it very well may
       *       be that the model has finished loading, but it's waiting to be assigned on tick.
       *       The ModelLoadedSignal will emit when the model is assigned, so you should listen for that.
       *  @return true if a background load is running but is not yet completed.
       */
      bool IsLoadingAsynchronously();

      /// Unloads the character model and calls related callbacks.  Should be the same as LoadModel("");
      void UnloadModel();

      /**
       * Emits a signal when the model files have been loaded and assigned to
       * this actor component.  You should listen for this whether you have async on or not because
       * the code essentially works the same way.
       */
      sigslot::signal1<AnimationHelper*> ModelLoadedSignal;
      sigslot::signal1<AnimationHelper*> ModelUnloadedSignal;

      /**
       * This function plays the specified animation defined within the character XML
       *
       * @param The name of the animation to play
       */
      void PlayAnimation(const std::string& pAnim);

      /**
       *  This function stops playing an animation by name over the course
       *  of time specified by fade out.
       *
       * @param The name of the animation to clear
       * @param The amount of time to fade out over
       */
      void ClearAnimation(const std::string& pAnim, float fadeOutTime);

      /**
       *  This function stops playing all currently active animations
       *  over the time specified by fade out.
       *
       * @param The amount of time to fade out over
       */
      void ClearAll(float fadeOutTime);

      /**
       * @return the osg::Node created by the builder on LoadModel
       */
      osg::Node* GetNode();

      /**
       * @return the osg::Node created by the builder on LoadModel
       */
      const osg::Node* GetNode() const;

      /**
       * @return The Cal3DAnimator created on LoadModel
       */
      dtAnim::AnimationUpdaterInterface* GetAnimator();

      /**
       * @return The Cal3DAnimator created on LoadModel
       */
      const dtAnim::AnimationUpdaterInterface* GetAnimator() const;

      /**
       * @return The Cal3DModelWrapper held by the animator
       */
      dtAnim::BaseModelWrapper* GetModelWrapper();

      /**
       * @return The Cal3DModelWrapper held by the animator
       */
      const dtAnim::BaseModelWrapper* GetModelWrapper() const;

      /**
       * @return The SequenceMixer used to play, clear, and register new
       * animations
       */
      SequenceMixer& GetSequenceMixer();

      /**
       * @return The SequenceMixer used to play, clear, and register new
       * animations
       */
      const SequenceMixer& GetSequenceMixer() const;

      /**
       * This flag is used by the AnimationComponent to determine
       * if this entity should be ground clamped.d
       */
      bool GetGroundClamp() const;

      /**
       * Set whether or not this entity should be ground clamped
       */
      void SetGroundClamp(bool b);

      /**
       * The animation helper has an attachment controller that moves the
       * Transformable attachments to match up to the bones.
       * @return the currently assigned attachment controller.
       */
      AttachmentController* GetAttachmentController();

      /**
       * Assigns a new AttachmentController.  One is created by default, so
       * this is provided to allow a developer to subclass the controller and
       * assign the new one to the helper.
       */
      void SetAttachmentController(AttachmentController* newController);

      /**
       * Gets a pose controller to manage poses if poses have been loaded.
       */
      PoseController* GetPoseController();

      void SetPosesEnabled(bool enabled);
      bool GetPosesEnabled() const;

      /**
       * Set whether command callbacks should be handled for this helper.
       */
      void SetCommandCallbacksEnabled(bool enable);
      bool GetCommandCallbacksEnabled() const;

      /**
       * Set the callback that is responsible for sending game events.
       * @param callback The callback that is in charge of responding
       *        to encountered events on animations.
       * @return Number of event command callbacks registered under
       *         the specified Send Event Callback.
       */
      unsigned SetSendEventCallback(AnimEventCallback callback);

      /**
       * Access the callback that was registered.
       */
      AnimEventCallback GetSendEventCallback() const;

      /**
       * Method to register an arbitrary command to be fired at a specific
       * offset time for a specific animation.
       * @param animName Name of the animation that should trigger the command.
       * @param timeOffset Time relative to the animation at which the specified
       *        command should be triggered.
       * @param command Command object that holds a callback and parameters to
       *        be called when the specified time offset is reached.
       * @return TRUE if the command was successfully registered for the animation;
       *         FALSE if it has already been registered.
       */
      bool RegisterCommandCallback(const std::string& animName,
         float timeOffset, AnimCommandCallback& command);

      /**
       * Unregister commands for a specific animation at a specific time offset.
       * @param animName Name of the animation to unregister command(s) for the
       *        specified time.
       * @param timeOffset Time relative to the animation at which a command should
       *        be removed.
       * @param command Optional parameter that flags a specific command to be removed.
       *        By default NULL is used to cause all commands to be removed at the
       *        the specified time for the specified animation.
       * @return Number of commands that were removed.
       */
      unsigned UnregisterCommandCallback(const std::string& animName,
         float timeOffset, AnimCommandCallback* command = NULL);

      /**
       * Unregister all commands associated with the specified animation.
       * @param animName Name of the animation to have all its associated commands removed.
       * @return Number of commands that were found and removed.
       */
      unsigned UnregisterCommandCallbacks(const std::string& animName);

      /**
       * Method to access all commands assigned to an animation for a specified time.
       * @param animName Name of the animation that may have commands associated with it.
       * @param timeOffset Time relative to the animation at which command(s)
       *        may have been added.
       * @param outArray Collection to capture the commands associated with the animation.
       * @return Number of items added to outArray.
       */
      unsigned GetCommandCallbacks(const std::string& animName,
         float timeOffset, AnimCommandArray& outArray);

      /**
       * Method to access all commands assigned to an animation for a specified time range.
       * @param animName Name of the animation that may have commands associated with it.
       * @param startTime Start time for the time range, relative to the animation..
       * @param endTime End time for the time range, relative to the animation..
       * @param outArray Collection to capture the commands associated with the animation.
       * @return Number of items added to outArray.
       */
      unsigned GetCommandCallbacks(const std::string& animName,
         float startTime, float endTime, AnimCommandArray& outArray);

      /**
       * Collects commands to be executed within the specified time range,
       * in the time space of the root animation sequence.
       * @param startTime Range absolute start time in the time space of the root sequence.
       * @param endTime Range absolute end time in the time space of the root sequence.
       * @return Number of commands that were collected.
       */
      unsigned CollectCommands(double startTime,double endTime);

      /**
       * Method to execute all commands that were gathered for the
       * current frame, but that should be executed after all animation
       * have had a chance to update; for the sake of thread safety.
       * @return Number of commands that were executed.
       */
      unsigned ExecuteCommands();

      /**
       * Get the count of commands that may have been collected recently.
       * @return Number of commands waiting to be executed.
       */
      unsigned GetCollectedCommandCount() const;

      /**
       * Method remove event commands registered to the currently set
       * Send Event Callback.
       * @return Number of event command callbacks that were removed.
       */
      unsigned ClearAnimationEventCallbacks();

      /**
       * Convenience method for determining the duration of a registered animation.
       * NOTE: This subsequently calls CalculateDuration on the accessed animation
       * which is not cheap, relatively speaking. Avoid calling this every frame.
       * @param animName Name of the animation to be measured.
       * @return Duration time of the animation.
       *         0 will normally indicate that nothing was found.
       */
      float GetAnimationDuration(const std::string& animName) const;

      /**
       * Attach skeletal mesh node to a parent drawable. You may pass in a node, or it will just attach it to the drawable
       * of the actor that owns this actor component.
       */
      virtual void AttachNodeToDrawable(osg::Group* parent = NULL);

      /// Detaches the skeletal mesh node from all osg node parents.
      virtual void DetachNodeFromDrawable();

      // This is public for test reasons.  It may be removed with no deprecation.  It just checks for completion of a background load.
      virtual void CheckLoadingState();

   protected:
      virtual ~AnimationHelper();

      virtual void OnLoadCompleted(AnimationHelper*);
      virtual void OnUnloadCompleted(AnimationHelper*);

      virtual bool SetupPoses(const dtAnim::BaseModelData& modelData);

      void OnTickLocal(const dtGame::TickMessage& /*tickMessage*/) override;
      void OnTickRemote(const dtGame::TickMessage& /*tickMessage*/) override;


   private:

      /**
       * Class for maintaining command callback references
       * associated with an animation and time offset.
       */
      class TimeOffsetCommand : public osg::Referenced
      {
      public:
         TimeOffsetCommand(float offset, AnimCommandCallback& command)
            : mIsEventCallback(false)
            , mOffset(offset)
            , mCommand(&command)
         {
         };

         bool mIsEventCallback;
         float mOffset;
         dtCore::RefPtr<AnimCommandCallback> mCommand;
         dtCore::RefPtr<Animatable> mAnim;
      };

      /**
       * Internal version of the method of a similar name.
       * This variant method returns the actual animation name/command/time
       * information struct that is created on registration. Other methods
       * in this class call this version of the method for special case
       * purposes, such as distinguishing which commands are automatically
       * generated versus custom commands registered external calling code.
       * @param animName Name of the animation that should trigger the command.
       * @param timeOffset Time relative to the animation at which the specified
       *        command should be triggered.
       * @param command Command object that holds a callback and parameters to
       *        be called when the specified time offset is reached.
       * @return Reference to a valid information struct that was created,
       *         if successfully registered.
       */
      TimeOffsetCommand* RegisterCommandCallback_Internal(
         const std::string& animName, float timeOffset, AnimCommandCallback& command);

      /**
       * Method to go through all registered animations and create
       * event command callbacks for each event name that is found
       * on the animations.
       * @return Number of commands that were created.
       */
      unsigned CreateAnimationEventCallbacks();

      /**
       * Method to create event command callbacks for each event
       * name that is found on the specified animatable.
       * @param anim A globally registered animation that may have events
       *             to be fired during the course of an animation.
       * @return Number of commands that were created.
       */
      unsigned CreateAnimationEventCallbacks(const Animatable& anim);

      /**
       * Gather the commands associated with the specified animatable
       * within the specified range of time.
       * @param anim An active animatable with information for finding the commands.
       * @param startTime Range absolute start time in the time space of the root sequence.
       * @param endTime Range absolute end time in the time space of the root sequence.
       * @param outArray Collection to capture the commands associated with the animatable.
       * @return Number of items added to outArray.
       */
      unsigned CollectCommandCallbacks(Animatable& anim,
         double startTime, double endTime, AnimCommandArray& outArray);

      /**
       * Method to remove a specific command from the command queue.
       * @param commandToRemove The command that should be removed from the queue.
       * @return Number of items removed that were pointing to the specified command.
       */
      unsigned RemoveCommandFromQueue(AnimCommandCallback& commandToRemove);

      bool mGroundClamp;
      bool mEnableCommands;
      double mLastUpdateTime;
      std::string mAsyncFile;
      dtCore::RefPtr<osg::Group> mParent;
      dtCore::RefPtr<SequenceMixer> mSequenceMixer;
      dtCore::RefPtr<AttachmentController> mAttachmentController;
      dtCore::RefPtr<dtAnim::BaseModelWrapper> mModelWrapper;
      dtCore::RefPtr<PoseSequence> mPoseSequence;

      typedef std::multimap<std::string, dtCore::RefPtr<TimeOffsetCommand> > CommandMap;
      CommandMap mCommandMap;

      AnimEventCallback mSendEventCallback;
      AnimCommandArray mCommands;

      void RegisterAnimations(const dtAnim::BaseModelData& sourceData);

      // this gets the resource path for the skeletal mesh and calls the configured load functionality.
      void LoadSkeletalMesh();

      void OnModelLoadCompleted(dtAnim::BaseModelWrapper* newModel, dtAnim::ModelLoader::LoadingState loadState);

      dtCore::RefPtr<dtAnim::ModelLoader> mModelLoader;
   };

} // namespace dtAnim

#endif // __DELTA_ANIMATIONHELPER_H__
