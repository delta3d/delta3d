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
* @author Matthew W. Campbell
*/
#ifndef DELTA_EDITOR_EVENTS
#define DELTA_EDITOR_EVENTS

#include <QtCore/QObject>
#include <osg/Referenced>
#include <vector>
#include "dtDAL/actorproxy.h"
#include "dtDAL/actorproperty.h"
#include "typedefs.h"

// Forward Declarations
namespace dtCore
{
    class DeltaDrawable;
}


namespace dtEditQt
{

    /**
     * @class EditorEvents
     * @brief This class is a single repository for any events that are sent around the User
     * Interface.  These are implemented with QT's signal/slot behavior.  Although this class
     * is not technically required, it acts as a single point of implementation for signals.
     * This class is a singleton
     */
    class EditorEvents : public QObject, public osg::Referenced
    {
        Q_OBJECT

    public:

        static EditorEvents &getInstance();

        /**
         * An actor(s) was selected in the viewport.  This event should cause other UI elements to
         * update if they do anything with the current selected object.
         */
        void emitActorsSelected(ActorProxyRefPtrVector &actors);

        /**
         * All the viewports should center on this actor.  Each visible should maintain
         * their current focus.
         */
        void emitGotoActor(ActorProxyRefPtr actor);

        /**
         * A new ActorProxy was created in the UI.  Anybody that needs to know about the new object
         * (like viewports) should listen for this.  Note that the object may or may not be selected
         * after creation, but that event will be sent separately.         *
         * @param proxy The proxy that was created is sent with the signal.
         * @param forceNoAdjustments This is used to indicate that a listener of this event should 
         * not perform edits of the data. This was initially used to prevent the  perspective browser 
         * from positioning an object that was created by undoing a delete event in front of the camera.
         * @note The forceNoAdjustments flag is no longer really used.  It has value, but isn't currently used.
         */
        void emitActorProxyCreated(ActorProxyRefPtr, bool forceNoAdjustments);

        /**
         * An ActorProxy is about to be destroyed.  The user has previously selected an object and
         * is now deleting it.  If you need to unlink this object before the memory goes away, then
         * trap this event, otherwise, if you just want to redraw  something, then trap the actual
         * delete message (below).
         * @param proxy The proxy that is about to be destroyed is sent with the signal.
         */
        void emitActorProxyAboutToBeDestroyed(ActorProxyRefPtr proxy);

        /**
         * An ActorProxy has been destroyed.  The user or some code has deleted it.
         * Do NOT use this message if you need to unlink this object before the memory
         * goes away.  This message is sent AFTER the object is removed from the map.
         * If you want that, then trap the about to be destroyed message above.
         * @param proxy The proxy that is about to be destroyed is sent with the signal.
         */
        void emitActorProxyDestroyed(ActorProxyRefPtr proxy);

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
         * This is emitted when the viewport contents have been changed and need to be
         * updated.
         */
        void emitViewportsChanged();

        /**
         * This is emitted when the user edits the name in the property editor.
         */
        void emitProxyNameChanged(ActorProxyRefPtr proxy, std::string oldName);

        /**
         * The user has modified the passed in property on the passed in proxy.  There are
         * no assumptions about the previous value.  You should be able to assume that the value
         * is valid and that it is different than what it was before.  NULL may be valid in some
         * cases.
         */
        void emitActorPropertyChanged(ActorProxyRefPtr proxy,
            ActorPropertyRefPtr property);

        /**
         * A property is about to be changed.  This message holds the old and new toString() 
         * values of the property.  It is primarily used by the undo process.
         */
        void emitActorPropertyAboutToChange(ActorProxyRefPtr proxy,
            ActorPropertyRefPtr property, std::string oldValue, std::string newValue);


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

    signals:
        void selectedActors(ActorProxyRefPtrVector &actors);
        void gotoActor(ActorProxyRefPtr actor);
        void actorProxyCreated(ActorProxyRefPtr proxy, bool forceNoAdjustments);
        void actorProxyAboutToBeDestroyed(ActorProxyRefPtr proxy);
        void actorProxyDestroyed(ActorProxyRefPtr proxy);
        void editorInitiationEvent();
        void projectChanged();
        void currentMapChanged();
        void viewportsChanged();
        void actorPropertyChanged(ActorProxyRefPtr proxy, ActorPropertyRefPtr property);
        void actorPropertyAboutToChange(ActorProxyRefPtr proxy,
            ActorPropertyRefPtr property, std::string oldValue, std::string newValue);
        void proxyNameChanged(ActorProxyRefPtr proxy, std::string oldName);
        void beginChangeTransaction();
        void endChangeTransaction();
        void mapLibraryImported();
        void mapLibraryRemoved();
        void mapLibraryAboutToBeRemoved();
        void editorCloseEvent();
        void mapPropertyChanged();
        void editorPreferencesChanged();

    private:
        EditorEvents();
        EditorEvents &operator=(const EditorEvents &rhs) { return *this; }
        EditorEvents(const EditorEvents &rhs) { }
        virtual ~EditorEvents();

        static dtCore::RefPtr<EditorEvents> instance;
    };

}

#endif
