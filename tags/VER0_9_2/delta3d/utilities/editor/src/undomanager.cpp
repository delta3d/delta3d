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

#include <QAction>
#include "dtEditQt/editordata.h"
#include "dtEditQt/editoractions.h"
#include "dtEditQt/editorevents.h"
#include "dtEditQt/undomanager.h"
#include "dtEditQt/viewportmanager.h"
#include "dtCore/uniqueid.h"
#include "dtDAL/actorproxy.h"
#include "dtDAL/actortype.h"
#include "dtDAL/actorproperty.h"
#include "dtDAL/librarymanager.h"
#include "dtDAL/map.h"
#include "dtDAL/project.h"
#include "dtEditQt/mainwindow.h"
#include "dtEditQt/undomanager.h"


namespace dtEditQt
{

    ///////////////////////////////////////////////////////////////////////////////
    UndoManager::UndoManager()
        : recursePrevent(false)
    {
        LOG_INFO("Initializing the UndoManager.");

        // connect all my signals  that cause the undo list to be trashed.
        connect(&EditorEvents::getInstance(), SIGNAL(editorInitiationEvent()),
            this, SLOT(clearAllHistories()));
        connect(&EditorEvents::getInstance(), SIGNAL(editorCloseEvent()),
            this, SLOT(clearAllHistories()));
        connect(&EditorEvents::getInstance(), SIGNAL(projectChanged()),
            this, SLOT(clearAllHistories()));
        connect(&EditorEvents::getInstance(), SIGNAL(currentMapChanged()),
            this, SLOT(clearAllHistories()));
        connect(&EditorEvents::getInstance(), SIGNAL(mapLibraryImported()),
            this, SLOT(clearAllHistories()));
        connect(&EditorEvents::getInstance(), SIGNAL(mapLibraryRemoved()),
            this, SLOT(clearAllHistories()));

        // trap destry, create, change, and about to change
        connect(&EditorEvents::getInstance(), SIGNAL(actorProxyDestroyed(osg::ref_ptr<dtDAL::ActorProxy>)),
            this, SLOT(onActorProxyDestroyed(osg::ref_ptr<dtDAL::ActorProxy>)));
        connect(&EditorEvents::getInstance(), SIGNAL(actorProxyCreated(osg::ref_ptr<dtDAL::ActorProxy>, bool)),
            this, SLOT(onActorProxyCreated(osg::ref_ptr<dtDAL::ActorProxy>, bool)));
        connect(&EditorEvents::getInstance(),
            SIGNAL(actorPropertyChanged(osg::ref_ptr<dtDAL::ActorProxy>, osg::ref_ptr<dtDAL::ActorProperty>)),
            this, SLOT(onActorPropertyChanged(osg::ref_ptr<dtDAL::ActorProxy>, osg::ref_ptr<dtDAL::ActorProperty>)));
        connect(&EditorEvents::getInstance(),
            SIGNAL(actorPropertyAboutToChange(osg::ref_ptr<dtDAL::ActorProxy>, osg::ref_ptr<dtDAL::ActorProperty>,
            std::string, std::string)),
            this, SLOT(actorPropertyAboutToChange(osg::ref_ptr<dtDAL::ActorProxy>, osg::ref_ptr<dtDAL::ActorProperty>,
            std::string, std::string)));
        connect(&EditorEvents::getInstance(), SIGNAL(proxyNameChanged(osg::ref_ptr<dtDAL::ActorProxy>, std::string)),
            this, SLOT(onProxyNameChanged(osg::ref_ptr<dtDAL::ActorProxy>, std::string)));
    }


    ///////////////////////////////////////////////////////////////////////////////
    UndoManager::~UndoManager()
    {
    }

    //////////////////////////////////////////////////////////////////////////////
    void UndoManager::onActorPropertyChanged(osg::ref_ptr<dtDAL::ActorProxy> proxy,
        osg::ref_ptr<dtDAL::ActorProperty> property)
    {
        if (!recursePrevent)
        {
            // clear the redo list anytime we add a new item to the undo list.
            clearRedoList();

            // verify that the about to change event is the same as this event
            if (aboutToChangeEvent != NULL && aboutToChangeEvent->objectId ==
                proxy->GetId().ToString() && aboutToChangeEvent->type ==
                ChangeEvent::PROPERTY_CHANGED && aboutToChangeEvent->undoPropData.size() > 0)
            {
                // double check the actual property.
                osg::ref_ptr<UndoPropertyData> propData = aboutToChangeEvent->undoPropData[0];
                if (propData->propertyName == property->GetName())
                {
                    // FINALLY, we get to add it to our undo list.
                    undoStack.push(aboutToChangeEvent);
                }
            }

            aboutToChangeEvent = NULL;

            enableButtons();
        }
    }

    //////////////////////////////////////////////////////////////////////////////
    void UndoManager::actorPropertyAboutToChange(osg::ref_ptr<dtDAL::ActorProxy> proxy,
        osg::ref_ptr<dtDAL::ActorProperty> property, std::string oldValue, std::string newValue)
    {
        if (!recursePrevent)
        {
            // clear the redo list anytime we add a new item to the undo list.
            clearRedoList();

            ChangeEvent *undoEvent = new ChangeEvent();
            undoEvent->type = ChangeEvent::PROPERTY_CHANGED;
            undoEvent->objectId = proxy->GetId().ToString();
            undoEvent->actorTypeName = proxy->GetActorType().GetName();
            undoEvent->actorTypeCategory = proxy->GetActorType().GetCategory();

            UndoPropertyData *propData = new UndoPropertyData();
            propData->propertyName = property->GetName();
            propData->oldValue = oldValue;
            propData->newValue = newValue;
            undoEvent->undoPropData.push_back(propData);

            // mark this event as the current event without adding it to our undo stack
            aboutToChangeEvent = undoEvent;

            enableButtons();
        }
    }

    //////////////////////////////////////////////////////////////////////////////
    void UndoManager::onActorProxyCreated(osg::ref_ptr<dtDAL::ActorProxy> proxy, bool forceNoAdjustments)
    {
        if (!recursePrevent)
        {
            // clear the redo list anytime we add a new item to the undo list.
            clearRedoList();
            // clear any incomplete property change events
            aboutToChangeEvent = NULL;

            ChangeEvent *undoEvent = createFullUndoEvent(proxy.get());
            undoEvent->type = ChangeEvent::PROXY_CREATED;

            // add it to our main undo stack
            undoStack.push(undoEvent);

            enableButtons();
        }
    }

    //////////////////////////////////////////////////////////////////////////////
    void UndoManager::onProxyNameChanged(osg::ref_ptr<dtDAL::ActorProxy> proxy, std::string oldName)
    {
        if (!recursePrevent)
        {
            // clear the redo list anytime we add a new item to the undo list.
            clearRedoList();
            // clear any incomplete property change events
            aboutToChangeEvent = NULL;

            ChangeEvent *undoEvent = new ChangeEvent();
            undoEvent->type = ChangeEvent::PROXY_NAME_CHANGED;
            undoEvent->objectId = proxy->GetId().ToString();
            undoEvent->actorTypeName = proxy->GetActorType().GetName();
            undoEvent->actorTypeCategory = proxy->GetActorType().GetCategory();
            undoEvent->oldName = oldName;

            // add it to our main undo stack
            undoStack.push(undoEvent);

            enableButtons();
        }
    }

    //////////////////////////////////////////////////////////////////////////////
    void UndoManager::onActorProxyDestroyed(osg::ref_ptr<dtDAL::ActorProxy> proxy)
    {
        if (!recursePrevent)
        {
            dtDAL::ActorProperty *curProp;
            std::vector<dtDAL::ActorProperty *> propList;
            std::vector<dtDAL::ActorProperty *>::const_iterator propIter;

            // clear the redo list anytime we add a new item to the undo list.
            clearRedoList();
            // clear any incomplete property change events
            aboutToChangeEvent = NULL;

            ChangeEvent *undoEvent = createFullUndoEvent(proxy.get());
            undoEvent->type = ChangeEvent::PROXY_DELETED;

            // add it to our main undo stack
            undoStack.push(undoEvent);

            enableButtons();
        }

    }

    //////////////////////////////////////////////////////////////////////////////
    void UndoManager::doRedo()
    {
        // clear any incomplete property change events
        aboutToChangeEvent = NULL;

        if (!redoStack.empty())
        {
            osg::ref_ptr<ChangeEvent> redoEvent = redoStack.top();
            redoStack.pop();

            handleUndoRedoEvent(redoEvent.get(), false);
        }

        enableButtons();
    }

    //////////////////////////////////////////////////////////////////////////////
    void UndoManager::doUndo()
    {
        // clear any incomplete property change events
        aboutToChangeEvent = NULL;

        if (!undoStack.empty())
        {
            osg::ref_ptr<ChangeEvent> undoEvent = undoStack.top();
            undoStack.pop();

            handleUndoRedoEvent(undoEvent.get(), true);
        }

        enableButtons();

    }

    //////////////////////////////////////////////////////////////////////////////
    void UndoManager::handleUndoRedoEvent(ChangeEvent *event, bool isUndo)
    {
        osg::ref_ptr<dtDAL::Map> currMap = EditorData::getInstance().getCurrentMap();

        if (currMap.valid())
        {
            dtDAL::ActorProxy *proxy = currMap->GetProxyById(event->objectId);

            // delete is special since the proxy is always NULL :)
            if (event->type == ChangeEvent::PROXY_DELETED)
            {
                handleUndoRedoDeleteObject(event, isUndo);
                return; // best to return because event can be modified as a side effect
            }

            if (proxy != NULL)
            {
                if (event->type == ChangeEvent::PROXY_NAME_CHANGED)
                {
                    handleUndoRedoNameChange(event, proxy, isUndo);
                    return; // best to return because event can be modified as a side effect
                }
                else if (event->type == ChangeEvent::PROPERTY_CHANGED)
                {
                    handleUndoRedoPropertyValue(event, proxy, isUndo);
                    return; // best to return because event can be modified as a side effect
                }
                else if (event->type == ChangeEvent::PROXY_CREATED)
                {
                    handleUndoRedoCreateObject(event, proxy, isUndo);
                    return; // best to return because event can be modified as a side effect
                }
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////////
    void UndoManager::handleUndoRedoNameChange(ChangeEvent *event, dtDAL::ActorProxy *proxy, bool isUndo)
    {
        // NOTE - The undo/redo methods do both the undo and the redo.  If you are modifying
        // these methods, be VERY careful

        std::string currentName = proxy->GetName();

        // do the undo
        proxy->SetName(event->oldName);
        // notify the world of our change to the data.
        osg::ref_ptr<dtDAL::ActorProxy> proxyRefPtr = proxy;
        recursePrevent = true;
        EditorEvents::getInstance().emitProxyNameChanged(proxyRefPtr, currentName);
        recursePrevent = false;

        // now turn the undo into a redo event
        event->oldName = currentName;

        if (isUndo)
        {
            // add it REDO stack
            redoStack.push(event);
        }
        else
        {
            // add it UNDO stack
            undoStack.push(event);
        }

        enableButtons();
    }

    //////////////////////////////////////////////////////////////////////////////
    void UndoManager::handleUndoRedoCreateObject(ChangeEvent *event, dtDAL::ActorProxy *proxy, bool isUndo)
    {
        // NOTE - The undo/redo methods do both the undo and the redo.  If you are modifying
        // these methods, be VERY careful

        // We are UNDO'ing a create, so we delete it. That means that we add a
        // Delete change event to the Undo or redo list.

        event->type = ChangeEvent::PROXY_DELETED;

        if (isUndo)
        {
            // add it REDO stack
            redoStack.push(event);
        }
        else
        {
            // add it UNDO stack
            undoStack.push(event);
        }

        // Delete the sucker
        recursePrevent = true;
        EditorData::getInstance().getMainWindow()->startWaitCursor();
        osg::ref_ptr<dtDAL::Map> currMap = EditorData::getInstance().getCurrentMap();
        EditorActions::getInstance().deleteProxy(proxy, currMap);

        //We are deleting an object, so clear the current selection for safety.
        std::vector<osg::ref_ptr<dtDAL::ActorProxy> > emptySelection;
        EditorEvents::getInstance().emitActorsSelected(emptySelection);

        EditorData::getInstance().getMainWindow()->endWaitCursor();
        recursePrevent = false;
    }

    //////////////////////////////////////////////////////////////////////////////
    void UndoManager::handleUndoRedoDeleteObject(ChangeEvent *event, bool isUndo)
    {
        // NOTE - The undo/redo methods do both the undo and the redo.  If you are modifying
        // these methods, be VERY careful

        // Note, it seems a bit backwards, just like the create.  We are UNDO'ing a
        // delete event.  Which means, we need to recreate the object and set all its
        // properties. Then, we need to add a CREATE change event to the REDO list.

        std::vector<osg::ref_ptr < UndoPropertyData > >::const_iterator undoPropIter;
        osg::ref_ptr<dtDAL::Map> currMap = EditorData::getInstance().getCurrentMap();

        // figure out the actor type
        osg::ref_ptr<dtDAL::ActorType> actorType = dtDAL::LibraryManager::GetInstance().
            FindActorType(event->actorTypeCategory, event->actorTypeName);

        if (currMap.valid() && actorType.valid())
        {
            EditorData::getInstance().getMainWindow()->startWaitCursor();

            // recreate the actor!
            osg::ref_ptr<dtDAL::ActorProxy> proxy =
                    dtDAL::LibraryManager::GetInstance().CreateActorProxy(*actorType.get());
            if (proxy.valid())
            {
                // set all of our old properties before telling anyone else about it
                for (undoPropIter = event->undoPropData.begin(); undoPropIter != event->undoPropData.end(); ++undoPropIter)
                {
                    osg::ref_ptr < UndoPropertyData> undoProp = (*undoPropIter);
                    // find the prop on the real actor
                    dtDAL::ActorProperty *actorProp = proxy->GetProperty(undoProp->propertyName);

                    // put our value back
                    if (actorProp != NULL)
                    {
                        actorProp->SetStringValue(undoProp->oldValue);
                    }
                }

                proxy->SetId(dtCore::UniqueId(event->objectId));
                proxy->SetName(event->oldName);
                currMap->AddProxy(*(proxy.get()));
                recursePrevent = true;
                EditorEvents::getInstance().emitBeginChangeTransaction();
                EditorEvents::getInstance().emitActorProxyCreated(proxy, true);
                ViewportManager::getInstance().placeProxyInFrontOfCamera(proxy.get());
                EditorEvents::getInstance().emitEndChangeTransaction();
                recursePrevent = false;

                // create our redo event
                event->type = ChangeEvent::PROXY_CREATED;
                if (isUndo)
                {
                    redoStack.push(event);
                }
                else
                {
                    undoStack.push(event);
                }
            }
        }
        EditorData::getInstance().getMainWindow()->endWaitCursor();

    }

    //////////////////////////////////////////////////////////////////////////////
    void UndoManager::handleUndoRedoPropertyValue(ChangeEvent *event, dtDAL::ActorProxy *proxy, bool isUndo)
    {
        // NOTE - The undo/redo methods do both the undo and the redo.  If you are modifying
        // these methods, be VERY careful

        if (event->undoPropData.size() == 0)
            return;

        // for property changed, simply find the old property and reset the value
        osg::ref_ptr<UndoPropertyData> propData = event->undoPropData[0];
        if (propData.valid())
        {
            dtDAL::ActorProperty *property = proxy->GetProperty(propData->propertyName);
            if (property != NULL)
            {
                std::string currentValue = property->GetStringValue();
                property->SetStringValue(propData->oldValue);

                // notify the world of our change to the data.
                osg::ref_ptr<dtDAL::ActorProxy> proxyRefPtr = proxy;
                osg::ref_ptr<dtDAL::ActorProperty> propertyRefPtr = property;
                recursePrevent = true;
                EditorData::getInstance().getMainWindow()->startWaitCursor();
                EditorEvents::getInstance().emitActorPropertyChanged(proxyRefPtr, propertyRefPtr);
                EditorData::getInstance().getMainWindow()->endWaitCursor();
                recursePrevent = false;

                // Create the Redo event - reverse old and new.
                event->type = UndoManager::ChangeEvent::PROPERTY_CHANGED;
                propData->newValue = propData->oldValue;
                propData->oldValue = currentValue;
                if (isUndo)
                {
                    // add it REDO stack
                    redoStack.push(event);
                }
                else
                {
                    // add it UNDO stack
                    undoStack.push(event);
                }
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////////
    UndoManager::ChangeEvent *UndoManager::createFullUndoEvent(dtDAL::ActorProxy *proxy)
    {
        dtDAL::ActorProperty *curProp;
        std::vector<dtDAL::ActorProperty *> propList;
        std::vector<dtDAL::ActorProperty *>::const_iterator propIter;

        ChangeEvent *undoEvent = new ChangeEvent();
        undoEvent->objectId = proxy->GetId().ToString();
        undoEvent->actorTypeName = proxy->GetActorType().GetName();
        undoEvent->actorTypeCategory = proxy->GetActorType().GetCategory();
        undoEvent->oldName = proxy->GetName();

        // for each property, create a property data object and add it to our event's list.
        proxy->GetPropertyList(propList);
        for (propIter = propList.begin(); propIter != propList.end(); ++propIter)
        {
            curProp = (*propIter);
            UndoPropertyData *undoData = new UndoPropertyData();
            undoData->propertyName = curProp->GetName();
            undoData->oldValue = curProp->ToString();
            undoData->newValue = ""; // ain't a new value - put here for completeness and readability

            undoEvent->undoPropData.push_back(undoData);
        }

        return undoEvent;
    }

    //////////////////////////////////////////////////////////////////////////////
    void UndoManager::enableButtons()
    {
        EditorActions::getInstance().actionEditUndo->setEnabled(!undoStack.empty());
        EditorActions::getInstance().actionEditRedo->setEnabled(!redoStack.empty());
    }

    //////////////////////////////////////////////////////////////////////////////
    void UndoManager::clearAllHistories()
    {
        clearUndoList();
        clearRedoList();

        enableButtons();
    }

    //////////////////////////////////////////////////////////////////////////////
    void UndoManager::clearUndoList()
    {
        // clear undo
        while (!undoStack.empty())
        {
            osg::ref_ptr<ChangeEvent> undoEvent = undoStack.top();
            undoStack.pop();
        }

    }

    //////////////////////////////////////////////////////////////////////////////
    void UndoManager::clearRedoList()
    {
        // clear redo
        while (!redoStack.empty())
        {
            osg::ref_ptr<ChangeEvent> redoEvent = redoStack.top();
            redoStack.pop();
        }

    }

    //////////////////////////////////////////////////////////////////////////////
    bool UndoManager::hasUndoItems()
    {
        return !undoStack.empty();
    }

    //////////////////////////////////////////////////////////////////////////////
    bool UndoManager::hasRedoItems()
    {
        return !undoStack.empty();
    }
}

