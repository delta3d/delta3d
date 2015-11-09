/* -*-c++-*-
 * Delta3D Simulation Training And Game Editor (STAGE)
 * STAGE - This source file (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2005-2008, Alion Science and Technology Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 * Matthew W. Campbell
 */
#ifndef DELTA_EDITOR_EVENTS
#define DELTA_EDITOR_EVENTS

#include <dtEditQt/export.h>
#include <QtCore/QObject>
#include <osg/Referenced>
#include <dtCore/actorproxy.h>
#include <dtQt/typedefs.h>

namespace dtEditQt
{

   /**
    * @class EditorEvents
    * @brief This class is a single repository for any events that are sent around the User
    * Interface.  These are implemented with QT's signal/slot behavior.  Although this class
    * is not technically required, it acts as a single point of implementation for signals.
    * This class is a singleton
    */
   class DT_EDITQT_EXPORT EditorEvents : public QObject, public osg::Referenced
   {
      Q_OBJECT

   public:
      static EditorEvents& GetInstance();

      /**
       * An actor(s) was selected in the viewport.  This event should cause other UI elements to
       * update if they do anything with the current selected object.
       */
      void emitActorsSelected(ActorRefPtrVector& actors);

      /**
       * All the viewports should center on this actor.  Each visible should maintain
       * their current focus.
       */
      void emitGotoActor(dtCore::ActorPtr actor);

      /**
       * All the viewports should center on this location.
       */
      void emitGotoPosition(double x, double y, double z);

      /**
       * Create an actor using the functionality of the ActorBrowser's
       * Create Actor button
       */
      void emitCreateActor();

      /**
       * A new BaseActorObject was created in the UI.  Anybody that needs to know about the new object
       * (like viewports) should listen for this.  Note that the object may or may not be selected
       * after creation, but that event will be sent separately.         *
       * @param proxy The proxy that was created is sent with the signal.
       * @param forceNoAdjustments This is used to indicate that a listener of this event should
       * not perform edits of the data. This was initially used to prevent the  perspective browser
       * from positioning an object that was created by undoing a delete event in front of the camera.
       * @note The forceNoAdjustments flag is no longer really used.  It has value, but isn't currently used.
       */
      void emitActorProxyCreated(dtCore::ActorPtr, bool forceNoAdjustments);

      /**
       * An BaseActorObject is about to be destroyed.  The user has previously selected an object and
       * is now deleting it.  If you need to unlink this object before the memory goes away, then
       * trap this event, otherwise, if you just want to redraw  something, then trap the actual
       * delete message (below).
       * @param proxy The proxy that is about to be destroyed is sent with the signal.
       */
      void emitActorProxyAboutToBeDestroyed(dtCore::ActorPtr proxy);

      /**
       * An BaseActorObject has been destroyed.  The user or some code has deleted it.
       * Do NOT use this message if you need to unlink this object before the memory
       * goes away.  This message is sent AFTER the object is removed from the map.
       * If you want that, then trap the about to be destroyed message above.
       * @param proxy The proxy that is about to be destroyed is sent with the signal.
       */
      void emitActorProxyDestroyed(dtCore::ActorPtr proxy);


      /**
       * This event is sent after the UI is basically initialized.  Elements should listen for
       * this to enable/disable components, or set initial configuration that may be order dependent.
       */
      void emitEditorInitiationEvent();

      /**
       * The user either created a new project or opened an existing project.  Many actions
       * in the editor will need to change, be enabled, etc...
       */
      void emitProjectChanged();

      /**
       * The user closed a map, created a new map, or opened an existing map.  The map changed
       * which means that the viewport and other UI elements need to change.  Some actions
       * will become enabled (if there was previously no current map) or disabled (if the user
       * closed the map).
       */
      void emitCurrentMapChanged();

      /**
       * The user saved the current map.
       */
      void emitCurrentMapSaved();

      /**
       * This is emitted when the viewport contents have been changed and need to be
       * updated.
       */
      void emitViewportsChanged();

      /**
       * The user checks the reset windows action.
       */
      void emitResetWindows();

      /**
       * This is emitted when the user edits the name in the property editor.
       */
      void emitProxyNameChanged(dtCore::BaseActorObject& proxy, std::string oldName);

      /**
       * The user has modified the passed in property on the passed in proxy.  There are
       * no assumptions about the previous value.  You should be able to assume that the value
       * is valid and that it is different than what it was before.  NULL may be valid in some
       * cases.
       */
      void emitActorPropertyChanged(dtCore::ActorPtr proxy,
         ActorPropertyRefPtr property);

      /**
       * A property is about to be changed.  This message holds the old and new toString()
       * values of the property.  It is primarily used by the undo process.
       */
      void emitActorPropertyAboutToChange(dtCore::ActorPtr proxy,
         ActorPropertyRefPtr property, std::string oldValue, std::string newValue);

      /**
       * The parent child relationships have been modified.
       * @param actor the actor who's parent was modified.
       * @param oldParent the old parent of the actor, or NULL.
       */
      void emitActorHierarchyChanged(dtCore::ActorPtr actor, dtCore::ActorPtr oldParent);

      /**
       * Several emits are about to be sent and the sender wants you to treat them as one big batch.
       * This is useful to prevent excessive refreshes of the viewports as well as providing a way for
       * Undo/Redo to track many transactions that occur together as one event.  If you send this, you
       * MUST send an endChangeTransaction or the system will be in an unstable state.
       *
       * @note Nested transactions are NOT supported
       */
      void emitBeginChangeTransaction();

      /**
       * Several emits have just been sent as one big batch. This event marks the end of that batch.
       * This is useful to prevent excessive refreshes of the viewports as well as providing a way for
       * Undo/Redo to track many transactions that occur together as one event.  ONLY send this if
       * you previously sent a beginChangeTransaction.
       *
       * @note Nested transactions are NOT supported
       */
      void emitEndChangeTransaction();

      /**
       * The user has imported their own library and now the actor browser needs to refresh itself
       */
      void emitMapLibraryImported();

      /**
       * The user has removed a library from the current map.
       */
      void emitMapLibraryRemoved();

      /**
       * This is emitted when the editor is about to exit.
       */
      void emitEditorCloseEvent();

      /**
       * This is emitted when a library is about to be removed from a map
       */
      void emitLibraryAboutToBeRemoved();

      /// Emitted when a maps properties change
      void emitMapPropertyChanged();

      /// Emitted when an editor preference changes.
      void emitEditorPreferencesChanged();

      /// Emitted when something wants to display a status bar message.
      void emitShowStatusBarMessage(const QString message, int timeout);

      ///Emitted right after a game event has been added.
      void emitGameEventAdded();

      ///Emitted right after a game event has been edited.
      void emitGameEventEdited();

      ///Emitted right after a game event has been removed from the map.
      void emitGameEventRemoved();

      ///Emitted right before a game event is removed from the map.
      void emitGameEventAboutToBeRemoved();

   signals:
      void selectedActors(ActorRefPtrVector& actors);
      void gotoActor(dtCore::ActorPtr actor);
      void gotoPosition(double x, double y, double z);
      void createActor();
      void actorProxyCreated(dtCore::ActorPtr proxy, bool forceNoAdjustments);
      void actorProxyAboutToBeDestroyed(dtCore::ActorPtr proxy);
      void actorProxyDestroyed(dtCore::ActorPtr proxy);
      void editorInitiationEvent();
      void projectChanged();
      void currentMapChanged();
      void currentMapSaved();      
      void viewportsChanged();
      void actorPropertyChanged(dtCore::ActorPtr proxy, ActorPropertyRefPtr property);
      void actorPropertyAboutToChange(dtCore::ActorPtr proxy,
         ActorPropertyRefPtr property, std::string oldValue, std::string newValue);
      void actorHierarchyUpdated(dtCore::ActorPtr actor, dtCore::ActorPtr oldParent);
      void ProxyNameChanged(dtCore::BaseActorObject& proxy, std::string oldName);
      void beginChangeTransaction();
      void endChangeTransaction();
      void mapLibraryImported();
      void mapLibraryRemoved();
      void mapLibraryAboutToBeRemoved();


      void mapGameEventAdded();
      void mapGameEventEdited();
      void mapGameEventRemoved();
      void mapGameEventAboutToBeRemoved();

      void editorCloseEvent();
      void mapPropertyChanged();
      void editorPreferencesChanged();
      void resetWindows();            

      void showStatusBarMessage(const QString message, int timeout);

   private:
      EditorEvents();
      EditorEvents& operator=(const EditorEvents& /*rhs*/) { return *this; }
      EditorEvents(const EditorEvents& /*rhs*/) {}
      virtual ~EditorEvents();

      static dtCore::RefPtr<EditorEvents> sInstance;
   };

} // namespace dtEditQt

#endif // DELTA_EDITOR_EVENTS
