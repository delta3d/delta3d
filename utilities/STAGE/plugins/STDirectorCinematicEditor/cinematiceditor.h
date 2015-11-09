/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2008 MOVES Institute
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
 * Author: Jeff P. Houde
 */

#ifndef DIRECTOR_CINEMATIC_EDITOR_PLUGIN
#define DIRECTOR_CINEMATIC_EDITOR_PLUGIN

#include "export.h"
#include <ui_cinematiceditor.h>

#include <KeyFrameEvent.h>
#include <AnimationEvent.h>

#include <dtEditQt/mainwindow.h>
#include <dtEditQt/plugininterface.h>

#include <dtDirector/director.h>
#include <dtDirectorQt/customeditortool.h>

#include <dtCore/transform.h>

#include <dtAnim/animationchannel.h>

#include <QtGui/QWidget>

#include <QtCore/QTimer>

#include <cal3d/global.h>

#if defined(CAL3D_VERSION) && CAL3D_VERSION >= 1300
   #define MANUAL_ANIMATIONS
#endif

#define NEAR_EQUAL(a, b) (a - 0.001 < b && a + 0.001 > b)

using namespace dtEditQt;

class QAction;
class QTimeLine;
class QMouseEvent;

namespace dtEditQt
{
   class Viewport;
}


/**
 * The DirectorCinematicEditorPlugin is a plugin that is used as a tool
 * to place a LinkedPointsActor into the world.
 */
class DT_DIRECTOR_CINEMATIC_EDITOR_EXPORT DirectorCinematicEditorPlugin
   : public QWidget
   , public dtDirector::CustomEditorTool
   , public Plugin
{
   Q_OBJECT

public:

   const static std::string PLUGIN_NAME;

   /**
    * Constructor
    *
    * @param[in]  mw  The main window.
    */
   DirectorCinematicEditorPlugin(MainWindow* mw);

   /**
    * Deconstructor.
    */
   ~DirectorCinematicEditorPlugin();

   /**
    * Initializes the window.
    */
   void Initialize();

   /**
    * Resets the UI to reflect all current settings.
    */
   void ResetUI();

   /**
    * Event handler when this tool should be opened for the given graph.
    * @note:  This method should be overloaded to perform any initial
    * operations when this tool is activated.
    *
    * @param[in]  editor  The editor that is using this tool.
    * @param[in]  graph   The graph to open the tool for.
    */
   virtual void Open(dtDirector::DirectorEditor* editor, dtDirector::DirectorGraph* graph);

   /**
    * Event handler to close the tool.
    * @note:  This method should be overloaded to perform any shut down
    * operations when this tool is deactivated.
    */
   virtual void Close();

   /**
    * Destroys the window.
    */
   virtual void Destroy();

   /**
    * override close event to get notified when user closes the dock
    *
    * @param[in]  event  The close event.
    */
   virtual void closeEvent(QCloseEvent* event);

public slots:

   /**
    * Update.
    */
   void OnUpdate();

   /**
   * Handles when actors are selected.
   *
   * @param[in]  actors  The list of actors being selected.
   */
   void onActorsSelected(ActorRefPtrVector& actors);

   /**
   * Handles when actor mode is ended.
   *
   * @param[in]   vp               The viewport triggering this event.
   * @param[in]   e                The mouse event.
   * @param[out]  overrideDefault  Should be set true if you don't want the default behavior to handle this.
   */
   void onEndActorMode(Viewport* vp, QMouseEvent* e, bool* overrideDefault);

   /**
    * Event handler when the current actor combo is changed.
    *
    * @param[in]  index  The current selection.
    */
   void OnActorComboChanged(int index);

   /**
    * Event handler when the add actor button is pressed.
    */
   void OnAddActor();

   /**
    * Event handler when the remove actor button is pressed.
    */
   void OnRemoveActor();

   /**
    * Event handler when the transform event track is enabled/disabled.
    *
    * @param[in]  state  1 = enabled, 0 = disabled.
    */
   void OnTransformEnabled(int state);

   /**
    * Event handler when a transform event is selected on the transform track.
    */
   void OnTransformEventSelected(BaseEvent* event);

   /**
    * Event handler when a transform event is moved.
    */
   void OnTransformEventTimesChanged(int start, int end);

   /**
    * Event handler when a transform event is removed.
    */
   void OnTransformEventRemoved(BaseEvent* event);

   /**
    * Event handler when the add transform button is pressed.
    */
   void OnAddTransform();

   /**
    * Event handler when the remove transform button is pressed.
    */
   void OnRemoveTransform();


   /**
    * Event handler when one of the edit boxes for the transform info is changed.
    *
    * @param[in]  value  The new value of the edit box.
    */
   void OnPosXChanged(QString value);
   void OnPosYChanged(QString value);
   void OnPosZChanged(QString value);
   void OnRotXChanged(QString value);
   void OnRotYChanged(QString value);
   void OnRotZChanged(QString value);
   void OnScaleXChanged(QString value);
   void OnScaleYChanged(QString value);
   void OnScaleZChanged(QString value);

   /**
    * Event handler when an animation event is selected on the animation track.
    */
   void OnAnimationEventSelected(BaseEvent* event);

   /**
    * Event handler when an animation event is moved.
    */
   void OnAnimationEventTimesChanged(int start, int end);

   /**
    * Event handler when an animation event is removed.
    */
   void OnAnimationEventRemoved(BaseEvent* event);

   /**
    * Event handler when the add animation button is pressed.
    */
   void OnAddAnimation();

   /**
    * Event handler when the remove animation button is pressed.
    */
   void OnRemoveAnimation();

   /**
    * Event handler when the current animation combo is changed.
    *
    * @param[in]  index  The current selection.
    */
   void OnAnimationComboChanged(int index);

   /**
    * Event handler when one of the edit fields of the animation event has been changed.
    *
    * @param[in]  value  The new value of the edit box.
    */
   void OnAnimationSpeedChanged(double value);
   void OnAnimationStartTimeChanged(double value);
   void OnAnimationEndTimeChanged(double value);
   void OnAnimationBlendInTimeChanged(double value);
   void OnAnimationBlendOutTimeChanged(double value);
   void OnAnimationWeightChanged(double value);

   /**
    * Event handler when an output event is selected on the event track.
    */
   void OnOutputEventSelected(BaseEvent* event);

   /**
    * Event handler when an output event is moved.
    */
   void OnOutputEventTimesChanged(int start, int end);

   /**
    * Event handler when an output event is removed.
    */
   void OnOutputEventRemoved(BaseEvent* event);

   /**
    * Event handler when the add output button is pressed.
    */
   void OnAddOutput();

   /**
    * Event handler when the remove output button is pressed.
    */
   void OnRemoveOutput();

   /**
    * Event handler when the Link Name edit box for the output event info is changed.
    *
    * @param[in]  value  The new value of the edit box.
    */
   void OnOutputNameChanged(QString value);

   /**
    * Event handler when the Trigger on Play check box for the output event info is changed.
    *
    * @param[in]  value  The new value of the edit box.
    */
   void OnOutputTriggerPlay(int value);

   /**
   * Event handler when the Trigger on Reverse check box for the output event info is changed.
    *
    * @param[in]  value  The new value of the edit box.
    */
   void OnOutputTriggerReverse(int value);

   /**
    * Event handler when the viewed start time edit has been changed.
    *
    * @param[in]  time  The new current time.
    */
   void OnStartTimeChanged(double time);

   /**
    * Event handler when the viewed end time edit has been changed.
    *
    * @param[in]  time  The new current time.
    */
   void OnEndTimeChanged(double time);

   /**
    * Event handler when the current time edit has been changed.
    *
    * @param[in]  time  The new current time.
    */
   void OnCurrentTimeChanged(double time);

   /**
    * Event handler when the total time has been changed.
    *
    * @param[in]  time  The new total time.
    */
   void OnTotalTimeChanged(double time);

   /**
    * Event handler when the Time Line has finished playing.
    */
   void OnTimeLineFinished();

   /**
    * Event handler when the Time Slider has changed position.
    *
    * @param[in]  value  The new position of the slider.
    */
   void OnTimeSliderValueChanged(int value);

   /**
    * Event handler when the Selection Track is finished being edited.
    */
   void OnSelectionEdited();

   /**
    * Event handler for the Play button.
    */
   void OnPlay();

   /**
    * Event handler to load the current graph.
    */
   void OnLoad();

   /**
    * Event handler when the save button is pressed.
    */
   void OnSave();

   /**
    * Event handler when the save and close button is pressed.
    */
   void OnSaveAndClose();

   /**
    * Event handler when the cancel button is pressed.
    */
   void OnCancel();

protected:

private:

   // Transform Actor Data.
   struct TransformData
   {
      TransformData(int time, const dtCore::Transform& transform, const osg::Vec3& scale, bool canScale, KeyFrameEvent* event)
      {
         mTime = time;
         mTransform = transform;
         mScale = scale;
         mCanScale = canScale;
         mEvent = event;
      }

      int                  mTime;
      dtCore::Transform    mTransform;
      osg::Vec3            mScale;
      bool                 mCanScale;

      KeyFrameEvent*       mEvent;
   };
   /**
    * Updates the transform and scale and refreshes the viewports
    */
   void UpdateTransform(TransformData* data);

   // Animation Data
   struct AnimationData
   {
      AnimationData(int time, const std::string& name, float duration, float weight, float speed, AnimationEvent* event)
      {
         mTime = time;
         mName = name;
         mStartTime = 0.0f;
         mEndTime = duration;
         mBlendInTime = 0.25f;
         mBlendOutTime = 0.25f;
         mWeight = weight;
         mSpeed = speed;
         mDuration = duration;
#ifdef MANUAL_ANIMATIONS
         mAnimation = -1;
#endif
         mEvent = event;
      }

      int         mTime;
      std::string mName;
      float       mStartTime;
      float       mEndTime;
      float       mBlendInTime;
      float       mBlendOutTime;
      float       mWeight;
      float       mSpeed;
      float       mDuration;

#ifdef MANUAL_ANIMATIONS
      int         mAnimation;
#endif

      AnimationEvent* mEvent;
   };

   // Event Data
   struct OutputData
   {
      OutputData(int time, const std::string& name, bool triggerPlay, bool triggerReverse, KeyFrameEvent* event)
      {
         mTime = time;
         mName = name;
         mTriggerPlay = triggerPlay;
         mTriggerReverse = triggerReverse;
         mEvent = event;
      }

      int            mTime;
      std::string    mName;
      bool           mTriggerPlay;
      bool           mTriggerReverse;

      KeyFrameEvent* mEvent;
   };

   // Actor Data
   struct ActorData
   {
      ActorData()
         : mActor(NULL)
         , mTransformEnabled(true)
      {
      }

      dtCore::ObserverPtr<dtCore::BaseActorObject> mActor;

      bool                       mTransformEnabled;
      std::vector<TransformData> mTransformData;
      std::vector<AnimationData> mAnimationData;
   };

   /**
    * Selects the actor in STAGE and moves the camera to that actor.
    */
   void GotoSelectedActor();

   /**
    * Inserts a transform event into the list, sorted by time.
    *
    * @param[in]  time       The time.
    * @param[in]  transform  The transform;
    * @param[in]  scale      The scale;
    * @param[in]  canScale   True if this transform actor is scalable.
    * @param[in]  movable    True if this event can be moved to a different time.
    * @param[in]  event      If provided, this will use the given event instead of creating a new one.
    *
    * @return     The new event.
    */
   KeyFrameEvent* InsertTransform(int time, const dtCore::Transform& transform, const osg::Vec3& scale, bool canScale, bool movable = true, KeyFrameEvent* event = NULL);

   /**
    * Retrieves the transform data for a given event.
    *
    * @param[in]  event     The event.
    * @param[in]  outIndex  The index where this data was found.
    *
    * @return     The transform data.
    */
   TransformData* GetTransformData(KeyFrameEvent* event, int* outIndex = NULL);

   /**
    * Finds or creates a transform data at the given time.
    *
    * @param[in]  dataList   The transform data list.
    * @param[in]  time       The time.
    * @param[in]  transform  The transform;
    * @param[in]  scale      The scale;
    * @param[in]  canScale   True if this transform actor is scalable.
    *
    * @return     A pointer to the data found, NULL if not found.
    */
   TransformData* GetOrCreateTransformData(int time, const dtCore::Transform& transform, const osg::Vec3& scale, bool canScale);

   /**
    * Inserts an output event into the list, sorted by time.
    *
    * @param[in]  time            The time.
    * @param[in]  name            The output name.
    * @param[in]  triggerPlay     True to trigger output on normal play.
    * @param[in]  triggerReverse  True to trigger output on reverse.
    * @param[in]  event           If provided, this will use the given event instead of creating a new one.
    *
    * @return     The new event.
    */
   KeyFrameEvent* InsertOutput(int time, const std::string& name, bool triggerPlay, bool triggerReverse, KeyFrameEvent* event = NULL);

   /**
    * Retrieves the transform data for a given event.
    *
    * @param[in]  event     The event.
    * @param[in]  outIndex  The index where this data was found.
    *
    * @return     The transform data.
    */
   OutputData* GetOutputData(KeyFrameEvent* event, int* outIndex = NULL);

   /**
    * Inserts an animation event into the list, sorted by time.
    *
    * @param[in]  time       The time.
    * @param[in]  animName   The animation name;
    * @param[in]  event      If provided, this will use the given event instead of creating a new one.
    *
    * @return     The new event.
    */
   AnimationEvent* InsertAnimation(int time, const std::string& animName, AnimationEvent* event = NULL);

   /**
    * Inserts an animation event into the list, sorted by time.
    *
    * @param[in]  animData   The animation data to insert.
    * @param[in]  event      If provided, this will use the given event instead of creating a new one.
    *
    * @return     The new event.
    */
   AnimationEvent* InsertAnimation(AnimationData& animData, AnimationEvent* event);

   /**
    * Inserts an animation event into the list, sorted by time.
    *
    * @param[in]  animData   The animation data to insert.
    *
    * @return     The new event.
    */
   void InsertAnimation(AnimationData& animData);

   /**
    * Retrieves the animation data for a given event.
    *
    * @param[in]  event     The event.
    * @param[in]  outIndex  The index where this data was found.
    *
    * @return     The transform data.
    */
   AnimationData* GetAnimationData(AnimationEvent* event, int* outIndex = NULL);

   /**
    * Updates actors in STAGE based on time.
    *
    * @param[in]  time  The time.
    */
   void LerpActors(int time);

   QTimer*        mTimer;

   Ui_CinematicEditor mUI;
   MainWindow*    mMainWindow;

   // Actor list.
   std::vector<ActorData>  mActorData;
   std::vector<OutputData> mOutputData;
   int                     mSelectedActor;

   // Events
   KeyFrameEvent*    mTransformEvent;
   AnimationEvent*   mAnimationEvent;
   KeyFrameEvent*    mOutputEvent;

   // Timeline
   QTimeLine*     mTimeLine;
   bool           mPlaying;
   float          mTotalTime;
};

#endif // DIRECTOR_CINEMATIC_EDITOR_PLUGIN
