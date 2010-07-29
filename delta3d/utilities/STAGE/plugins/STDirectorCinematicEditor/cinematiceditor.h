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

#include <dtEditQt/mainwindow.h>
#include <dtEditQt/plugininterface.h>

#include <dtDirector/director.h>
#include <dtDirectorQt/directoreditor.h>
#include <dtDirectorQt/customeditortool.h>

#include <dtCore/transform.h>

#include <QtGui/QWidget>


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
    * @Note:  This method should be overloaded to perform any initial
    * operations when this tool is activated.
    *
    * @param[in]  graph  The graph to open the tool for.
    */
   virtual void Open(dtDirector::DirectorGraph* graph);

   /**
    * Event handler to close the tool.
    * @Note:  This method should be overloaded to perform any shut down
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
   * Handles when actors are selected.
   *
   * @param[in]  actors  The list of actors being selected.
   */
   void onActorsSelected(ActorProxyRefPtrVector& actors);

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
      TransformData(int time, const dtCore::Transform& transform, const osg::Vec3& scale, KeyFrameEvent* event)
      {
         mTime = time;
         mTransform = transform;
         mScale = scale;
         mEvent = event;
      }

      int                  mTime;
      dtCore::Transform    mTransform;
      osg::Vec3            mScale;

      KeyFrameEvent*       mEvent;
   };

   // Event Data
   struct OutputData
   {
      OutputData(int time, const std::string& name, KeyFrameEvent* event)
      {
         mTime = time;
         mName = name;
         mEvent = event;
      }

      int            mTime;
      std::string    mName;

      KeyFrameEvent* mEvent;
   };

   // Actor Data
   struct ActorData
   {
      ActorData()
         : mActor(NULL)
      {
      }

      dtCore::ObserverPtr<dtDAL::ActorProxy> mActor;

      std::vector<TransformData> mTransformData;
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
    * @param[in]  movable    True if this event can be moved to a different time.
    * @param[in]  event      If provided, this will use the given event instead of creating a new one.
    *
    * @return     The new event.
    */
   KeyFrameEvent* InsertTransform(int time, const dtCore::Transform& transform, const osg::Vec3& scale, bool movable = true, KeyFrameEvent* event = NULL);

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
    * @param[in]  dataList  The transform data list.
    * @param[in]  time      The time.
    *
    * @return     A pointer to the data found, NULL if not found.
    */
   TransformData* GetOrCreateTransformData(int time);

   /**
    * Inserts an output event into the list, sorted by time.
    *
    * @param[in]  time       The time.
    * @param[in]  name       The output name.
    * @param[in]  event      If provided, this will use the given event instead of creating a new one.
    *
    * @return     The new event.
    */
   KeyFrameEvent* InsertOutput(int time, const std::string& name, KeyFrameEvent* event = NULL);

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
    * Updates actors in STAGE based on time.
    *
    * @param[in]  time  The time.
    */
   void LerpActors(int time);

   Ui_CinematicEditor mUI;
   MainWindow*    mMainWindow;

   // Actor list.
   std::vector<ActorData>  mActorData;
   std::vector<OutputData> mOutputData;
   int                     mSelectedActor;

   // Events
   KeyFrameEvent* mTransformEvent;
   KeyFrameEvent* mAnimationEvent;
   KeyFrameEvent* mOutputEvent;

   // Timeline
   QTimeLine*     mTimeLine;
   bool           mPlaying;
   float          mTotalTime;
};

#endif // DIRECTOR_CINEMATIC_EDITOR_PLUGIN
