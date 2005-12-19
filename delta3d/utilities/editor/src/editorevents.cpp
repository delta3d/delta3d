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
#include <QtGui/QMainWindow>
#include <QtCore/QTimer>
#include <QtGui/QMessageBox>

#include "dtEditQt/editorevents.h"
#include "dtEditQt/editordata.h"
#include "dtEditQt/editoractions.h"
#include "dtDAL/actorproxy.h"
#include <dtUtil/log.h>
#include "dtDAL/map.h"

namespace dtEditQt
{

    //Singleton instance of the event manager.
    osg::ref_ptr<EditorEvents> EditorEvents::instance(NULL);

    ///////////////////////////////////////////////////////////////////////////////
    EditorEvents::EditorEvents()
    {

    }

    ///////////////////////////////////////////////////////////////////////////////
    EditorEvents::~EditorEvents()
    {
    }

    ///////////////////////////////////////////////////////////////////////////////
    void EditorEvents::emitActorsSelected(std::vector<osg::ref_ptr<dtDAL::ActorProxy> > &actors)
    {
        LOG_INFO("Emitting UI event - [actorsSelected]");
        emit selectedActors(actors);
    }

    ///////////////////////////////////////////////////////////////////////////////
    void EditorEvents::emitGotoActor(osg::ref_ptr<dtDAL::ActorProxy> actor)
    {
        LOG_INFO("Emitting UI event - [gotoActor]");
        emit gotoActor(actor);
    }

    ///////////////////////////////////////////////////////////////////////////////
    void EditorEvents::emitActorProxyCreated(osg::ref_ptr<dtDAL::ActorProxy> proxy, bool forceNoAdjustments)
    {
        LOG_INFO("Emitting UI event - [actorProxyCreated]");
        emit actorProxyCreated(proxy, forceNoAdjustments);
    }

    ///////////////////////////////////////////////////////////////////////////////
    void EditorEvents::emitActorProxyAboutToBeDestroyed(osg::ref_ptr<dtDAL::ActorProxy> proxy)
    {
        LOG_INFO("Emitting UI event - [actorProxyAboutToBeDestroyed]");
        emit actorProxyAboutToBeDestroyed(proxy);
    }

    ///////////////////////////////////////////////////////////////////////////////
    void EditorEvents::emitActorProxyDestroyed(osg::ref_ptr<dtDAL::ActorProxy> proxy)
    {
        LOG_INFO("Emitting UI event - [actorProxyDestroyed]");
        emit actorProxyDestroyed(proxy);
    }

    ///////////////////////////////////////////////////////////////////////////////
    void EditorEvents::emitEditorInitiationEvent()
    {
        LOG_INFO("Emitting UI event - [editorInitiationEvent]");
        emit editorInitiationEvent();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void EditorEvents::emitProjectChanged()
    {
        LOG_INFO("Emitting UI event - [projectChanged]");
        emit projectChanged();
        //emit
    }

    ///////////////////////////////////////////////////////////////////////////////
    void EditorEvents::emitCurrentMapChanged()
    {
        LOG_INFO("Emitting UI event - [currentMapChanged]");
        emit currentMapChanged();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void EditorEvents::emitViewportsChanged()
    {
        LOG_INFO("Emitting UI event - [viewportsChanged]");
        emit viewportsChanged();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void EditorEvents::emitMapPropertyChanged()
    {
        LOG_INFO("Emitting UI event - [mapPropertyChanged]");
        emit mapPropertyChanged();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void EditorEvents::emitProxyNameChanged(osg::ref_ptr<dtDAL::ActorProxy> proxy, std::string oldName)
    {
        LOG_INFO("Emitting UI event - [proxyNameChanged]");
        emit proxyNameChanged(proxy, oldName);
    }

    ///////////////////////////////////////////////////////////////////////////////
    void EditorEvents::emitActorPropertyChanged(osg::ref_ptr<dtDAL::ActorProxy> proxy,
            osg::ref_ptr<dtDAL::ActorProperty> property)
    {
        //I removed logging from this event.  This is called when the user
        //manipulates actors in the viewports which means the log file is going to have
        //a ton of messages for this event, thus in my opinion, polluting the
        //log file and causing unneeded disk IO. -Matt
        //LOG_INFO("Emitting UI event - [actorPropertyChanged]");
        emit actorPropertyChanged(proxy, property);
    }

    ///////////////////////////////////////////////////////////////////////////////
    void EditorEvents::emitActorPropertyAboutToChange(osg::ref_ptr<dtDAL::ActorProxy> proxy,
        osg::ref_ptr<dtDAL::ActorProperty> property, std::string oldValue, std::string newValue)
    {
        // no logging here, just like with property changed.
        // note, if this is not sent out before a property changed event (see above)
        // then the undo manager will not treat it as a valid change event and will ignore it
        emit actorPropertyAboutToChange(proxy, property, oldValue, newValue);
    }

    ///////////////////////////////////////////////////////////////////////////////
    void EditorEvents::emitBeginChangeTransaction()
    {
        LOG_INFO("Beginning Change Transaction");
        emit beginChangeTransaction();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void EditorEvents::emitEndChangeTransaction()
    {
        LOG_INFO("Ending Change Transaction");
        emit endChangeTransaction();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void EditorEvents::emitMapLibraryImported()
    {
        LOG_INFO("User imported library");
        emit mapLibraryImported();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void EditorEvents::emitMapLibraryRemoved()
    {
        LOG_INFO("User removed library");
        emit mapLibraryRemoved();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void EditorEvents::emitLibraryAboutToBeRemoved()
    {
        LOG_INFO("User is about to remove a library");
        emit mapLibraryAboutToBeRemoved();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void EditorEvents::emitEditorPreferencesChanged()
    {
        LOG_INFO("User preferences have changed.");
        emit editorPreferencesChanged();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void EditorEvents::emitEditorCloseEvent()
    {
        LOG_INFO("Editor is closing.");
        emit editorCloseEvent();
    }

    ///////////////////////////////////////////////////////////////////////////////
    EditorEvents &EditorEvents::getInstance()
    {
        if (EditorEvents::instance.get() == NULL)
            EditorEvents::instance = new EditorEvents();
        return *(EditorEvents::instance.get());
    }

}
