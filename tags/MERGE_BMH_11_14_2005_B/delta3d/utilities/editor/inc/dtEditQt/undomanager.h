/* 
* Delta3D Open Source Game and Simulation Engine 
* Simulation, Training, and Game Editor (STAGE)
* Copyright (C) 2005, BMH Associates, Inc. 
*
* This program is free software; you can redistribute it and/or modify it under
* the terms of the GNU General Public License as published by the Free 
* Software Foundation; either version 2 of the License, or (at your option) 
* any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
* FOR A PARTICULAR PURPOSE. See the GNU General Public License for more 
* details.
*
* You should have received a copy of the GNU General Public License 
* along with this library; if not, write to the Free Software Foundation, Inc., 
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 
*
* @author Curtiss Murphy
*/
#ifndef DELTA_UNDOMANAGER
#define DELTA_UNDOMANAGER

#include <osg/Referenced>
#include <osg/ref_ptr>
#include <QObject>
#include <vector>
#include <stack>
#include <string>
#include <list>
#include "dtDAL/actorproxy.h"
#include "dtDAL/actortype.h"
#include "dtDAL/actorproperty.h"
#include "dtEditQt/typedefs.h"

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
    class UndoManager : QObject
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

    protected:

    public slots:
        /**
         * When an actor property is called, we find the last 'about to be changed property' 
         * event and add it to the undo list. 
         * @param proxy The proxy that was changed.
         * @param property The property of the proxy that was modified.
         */
        void onActorPropertyChanged(proxyRefPtr proxy,
            propertyRefPtr property);

        /**
         * An actor property is about to change.  We create the change event object, but 
         * don't add it to the undo list until we get the actual changed event.
         */
        void actorPropertyAboutToChange(proxyRefPtr proxy,
            propertyRefPtr property, std::string oldValue, std::string newValue);

        /**
          * When an actor is created, we add a create event to the undo list.
          */
        void onActorProxyCreated(proxyRefPtr proxy, bool forceNoAdjustments);

        /**
         * When an actor is destroyed, we add a destroy event to the undo list.
         */
        void onActorProxyDestroyed(proxyRefPtr proxy);

        /**
         * When the name changes, trap this event.
         */        
        void onProxyNameChanged(proxyRefPtr proxy, std::string oldName);

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
            UndoPropertyData() {};
            virtual ~UndoPropertyData() {};

            std::string propertyName;
            std::string oldValue;
            std::string newValue;
        };

        /**
        * This is an internal class used by the UndoManager to track events.  It is a simple
        * data class and exposes its attributes publicly.
        */
        class ChangeEvent : public osg::Referenced 
        {
        public:
            enum ChangeEventType {PROPERTY_CHANGED, PROXY_NAME_CHANGED, 
                PROXY_CREATED, PROXY_DELETED};

            ChangeEvent() {};
            virtual ~ChangeEvent() {};

            ChangeEventType type;
            std::vector<osg::ref_ptr < UndoPropertyData > > undoPropData;
            std::string objectId;
            std::string actorTypeName;
            std::string actorTypeCategory;
            std::string oldName; // for proxy name changed events

        };

        // variables
        std::stack<osg::ref_ptr < ChangeEvent > > undoStack;
        std::stack<osg::ref_ptr < ChangeEvent > > redoStack;
        // this variable is used to avoid recursing on events to our selves
        // because undo or redo makjes changes to the data, but we don't want 
        // to listen to our own events
        bool recursePrevent;

        // the about to change is set when the about to change event is 
        // sent.  If the next call to change event is NOT the same object id 
        // and property, then it is discarded.  This helps to only track change 
        // events that were intended to be tracked in undo. It also prevents us
        // from holding to a change event for a change that didn't actually complete
        osg::ref_ptr<ChangeEvent> aboutToChangeEvent;

        /**
         * Enables the redo and undo buttons based on the current state of the stacks
         */
        void enableButtons();

        /**
         * Internal method to handle an undo/redo for a property value change.  Creates 
         * the appropriate undo or redo event.
         */
        void handleUndoRedoPropertyValue(ChangeEvent *event, dtDAL::ActorProxy *proxy, bool isUndo);

        /**
         * Internal method to handle an undo/redo for a name change event.  Creates
         * the appropriate undo or redo event.
         */
        void handleUndoRedoNameChange(ChangeEvent *event, dtDAL::ActorProxy *proxy, bool isUndo);

        /**
         * Internal method to handle an undo/redo of a create proxy, which really 
         * means deleting the object and creating an appropriate delete undo or redo event.
         */
        void handleUndoRedoCreateObject(ChangeEvent *event, dtDAL::ActorProxy *proxy, bool isUndo);

        /**
         * Internal method to handle an undo/redo of a delete proxy, which really 
         * means re-creating the object and creating an appropriate create undo or redo event.
         */
        void handleUndoRedoDeleteObject(ChangeEvent *event, bool isUndo);

        /**
         * Internal method to create a complete change event for a proxy.  This event is 
         * used by both the delete and change event types.  It makes an UndoPropertyData for 
         * every property on the proxy.
         * @return Returns a new instance of a ChangeEvent.  You must delete this or make 
         * an osg::ref_ptr for it
         */
        ChangeEvent *createFullUndoEvent(dtDAL::ActorProxy *proxy);

        /**
         * Determines what type of event we are working with and calls the appropriate 
         * handleUndoRedoXXX method.
         */
        void handleUndoRedoEvent(ChangeEvent *event, bool isUndo);
    };
}

#endif
