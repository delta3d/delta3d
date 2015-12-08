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
 * Curtiss Murphy
 */
#ifndef DELTA_UNDOMANAGER
#define DELTA_UNDOMANAGER

#include <dtEditQt/export.h>
#include <osg/Referenced>
#include <QtCore/QObject>
#include <vector>
#include <stack>
#include <string>
#include <list>
#include <dtCore/actorproxy.h>
#include <dtCore/actortype.h>
#include <dtCore/actorproperty.h>
#include <dtQt/typedefs.h>

namespace dtCore
{
   class UniqueId;
}

namespace dtEditQt
{

   /**
    * This class is used to manage undo and redo behavior.  It traps events for when objects
    * are created, deleted, or have their properties changed.  Then, it stores an internal
    * version of those changes that can later be undone or redone by the user.  The Undo Manager
    * can be accessed via EditorData.
    *
    * @note This class should NEVER hold onto any real objects.  For instance, it won't hold onto
    * libraries, ActorProxies, Actors, or other map/project specific data.
    */
   class DT_EDITQT_EXPORT UndoManager : QObject
   {
      Q_OBJECT

   public:
      /**
       * Constructor
       */
      UndoManager();

      /**
       * Destructor
       */
      virtual ~UndoManager();

      /**
      * Begins a group of undo events.
      */
      void beginMultipleUndo();

      /**
      * Ends a group of undo events.
      */
      void endMultipleUndo();

      /**
       * Returns true if there are undo items in the undo stack.
       */
      bool hasUndoItems();

      /**
       * Returns true if there are redo items in the redo stack
       */
      bool hasRedoItems();

      /**
       * Called when the user presses the redo button
       * @note It's a silly name, but it's just too much fun not to use it
       */
      void doRedo();

      /**
       * Called when the user presses the undo button
       * @note It's a silly name, but it's just too much fun not to use it
       */
      void doUndo();

      /**
       * Removes all change events from the undo list.
       */
      void clearUndoList();

      /**
       * Removes all change events from the redo list.
       */
      void clearRedoList();

      /**
      * Creates a group actor event.
      */
      void groupActor(dtCore::ActorPtr proxy);

      /**
      * Creates an ungroup actor event.
      */
      void unGroupActor(dtCore::ActorPtr proxy);

   protected:

   public slots:
      /**
       * When an actor property is called, we find the last 'about to be changed property'
       * event and add it to the undo list.
       * @param proxy The proxy that was changed.
       * @param property The property of the proxy that was modified.
       */
      void onActorPropertyChanged(dtCore::ActorPtr actor,
         ActorPropertyRefPtr property);

      /**
       * An actor property is about to change.  We create the change event object, but
       * don't add it to the undo list until we get the actual changed event.
       */
      void actorPropertyAboutToChange(dtCore::ActorPtr actor,
         ActorPropertyRefPtr property, std::string oldValue, std::string newValue);

      /**
       * When an actor is created, we add a create event to the undo list.
       */
      void onActorProxyCreated(dtCore::ActorPtr proxy, bool forceNoAdjustments);

      /**
       * When an actor is destroyed, we add a destroy event to the undo list.
       */
      void onActorProxyDestroyed(dtCore::ActorPtr actor);

      /**
       * Called when a map, project, or libraries change.  Clears all undo/redo events.
       */
      void clearAllHistories();

   private:
      /**
       * This is an internal class used by the ChangeEvent class to track property data
       * about a specific change event. It is a simple data class with public attributes.
       */
      class UndoPropertyData : public osg::Referenced
      {
      public:
         UndoPropertyData() {}
         virtual ~UndoPropertyData() {}

         std::string mPropertyName;
         std::string mOldValue;
         std::string mNewValue;
      };

      /**
       * This is an internal class used by the UndoManager to track events.  It is a simple
       * data class and exposes its attributes publicly.
       */
      class ChangeEvent : public osg::Referenced
      {
      public:
         enum ChangeEventType
         {
            PROPERTY_CHANGED,
            PROXY_CREATED,
            PROXY_DELETED,
            MULTI_UNDO_BEGIN,
            MULTI_UNDO_END,
            GROUP_CREATED,
            GROUP_DELETED,
            TYPE_UKNOWN
         };

         ChangeEvent() : mType(TYPE_UKNOWN), mObjectId(false) {}
         virtual ~ChangeEvent() {}

         ChangeEventType mType;
         std::vector< dtCore::RefPtr<UndoPropertyData> > mUndoPropData;
         dtCore::UniqueId mObjectId;
         std::string mTypeName;
         std::string mTypeCategory;
      };

      // variables
      std::stack< dtCore::RefPtr<ChangeEvent> > mUndoStack;
      std::stack< dtCore::RefPtr<ChangeEvent> > mRedoStack;
      // this variable is used to avoid recursing on events to our selves
      // because undo or redo makjes changes to the data, but we don't want
      // to listen to our own events
      bool mRecursePrevent;

      // the about to change is set when the about to change event is
      // sent.  If the next call to change event is NOT the same object id
      // and property, then it is discarded.  This helps to only track change
      // events that were intended to be tracked in undo. It also prevents us
      // from holding to a change event for a change that didn't actually complete
      dtCore::RefPtr<ChangeEvent> mAboutToChangeEvent;

      /**
       * Enables the redo and undo buttons based on the current state of the stacks
       */
      void enableButtons();

      /**
       * Internal method to handle an undo/redo for a property value change.  Creates
       * the appropriate undo or redo event.
       */
      void handleUndoRedoPropertyValue(ChangeEvent* event, dtCore::BaseActorObject* proxy, bool isUndo);

      /**
       * Internal method to handle an undo/redo of a create proxy, which really
       * means deleting the object and creating an appropriate delete undo or redo event.
       */
      void handleUndoRedoCreateObject(ChangeEvent* event, dtCore::BaseActorObject* proxy, bool isUndo);

      /**
       * Internal method to handle an undo/redo of a delete proxy, which really
       * means re-creating the object and creating an appropriate create undo or redo event.
       */
      void handleUndoRedoDeleteObject(ChangeEvent* event, bool isUndo);

      /**
      * Internal method to handle an undo/redo of group creation.
      */
      void handleUndoRedoCreateGroup(ChangeEvent* event, dtCore::BaseActorObject* proxy, bool createGroup, bool isUndo);

      /**
       * Internal method to create a complete change event for a proxy.  This event is
       * used by both the delete and change event types.  It makes an UndoPropertyData for
       * every property on the proxy.
       * @return Returns a new instance of a ChangeEvent.  You must delete this or make
       * an dtCore::RefPtr for it
       */
      ChangeEvent* createFullUndoEvent(dtCore::BaseActorObject* proxy);

      /**
       * Determines what type of event we are working with and calls the appropriate
       * handleUndoRedoXXX method.
       */
      void handleUndoRedoEvent(ChangeEvent* event, bool isUndo);

      int   mGroupIndex;
   };

} // namespace dtEditQt

#endif // DELTA_UNDOMANAGER
