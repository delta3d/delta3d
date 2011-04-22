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
#include <prefix/dtstageprefix-src.h>
#include <QtGui/QAction>
#include <dtEditQt/editordata.h>
#include <dtEditQt/editoractions.h>
#include <dtEditQt/editorevents.h>
#include <dtEditQt/undomanager.h>
#include <dtEditQt/viewportmanager.h>
#include <dtCore/uniqueid.h>
#include <dtDAL/actorproxy.h>
#include <dtDAL/actortype.h>
#include <dtDAL/actorproperty.h>
#include <dtDAL/librarymanager.h>
#include <dtDAL/map.h>
#include <dtDAL/project.h>
#include <dtEditQt/mainwindow.h>
#include <dtEditQt/undomanager.h>

namespace dtEditQt
{
    ///////////////////////////////////////////////////////////////////////////////
    UndoManager::UndoManager()
        : recursePrevent(false)
    {
        LOG_INFO("Initializing the UndoManager.");

        // connect all my signals  that cause the undo list to be trashed.
        connect(&EditorEvents::GetInstance(), SIGNAL(editorInitiationEvent()),
            this, SLOT(clearAllHistories()));
        connect(&EditorEvents::GetInstance(), SIGNAL(editorCloseEvent()),
            this, SLOT(clearAllHistories()));
        connect(&EditorEvents::GetInstance(), SIGNAL(projectChanged()),
            this, SLOT(clearAllHistories()));
        connect(&EditorEvents::GetInstance(), SIGNAL(currentMapChanged()),
            this, SLOT(clearAllHistories()));
        connect(&EditorEvents::GetInstance(), SIGNAL(mapLibraryImported()),
            this, SLOT(clearAllHistories()));
        connect(&EditorEvents::GetInstance(), SIGNAL(mapLibraryRemoved()),
            this, SLOT(clearAllHistories()));

        // trap destry, create, change, and about to change
        connect(&EditorEvents::GetInstance(), SIGNAL(actorProxyDestroyed(ActorProxyRefPtr)),
            this, SLOT(onActorProxyDestroyed(ActorProxyRefPtr)));
        connect(&EditorEvents::GetInstance(), SIGNAL(actorProxyCreated(ActorProxyRefPtr, bool)),
            this, SLOT(onActorProxyCreated(ActorProxyRefPtr, bool)));
        connect(&EditorEvents::GetInstance(),
            SIGNAL(actorPropertyChanged(ActorProxyRefPtr, ActorPropertyRefPtr)),
            this, SLOT(onActorPropertyChanged(ActorProxyRefPtr, ActorPropertyRefPtr)));
        connect(&EditorEvents::GetInstance(),
            SIGNAL(actorPropertyAboutToChange(ActorProxyRefPtr, ActorPropertyRefPtr,
            std::string, std::string)),
            this, SLOT(actorPropertyAboutToChange(ActorProxyRefPtr, ActorPropertyRefPtr,
            std::string, std::string)));
        connect(&EditorEvents::GetInstance(), SIGNAL(proxyNameChanged(ActorProxyRefPtr, std::string)),
            this, SLOT(onProxyNameChanged(ActorProxyRefPtr, std::string)));
    }


    ///////////////////////////////////////////////////////////////////////////////
    UndoManager::~UndoManager()
    {
    }

    //////////////////////////////////////////////////////////////////////////////
    void UndoManager::onActorPropertyChanged(dtCore::RefPtr<dtDAL::ActorProxy> proxy,
        dtCore::RefPtr<dtDAL::ActorProperty> property)
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
                dtCore::RefPtr<UndoPropertyData> propData = aboutToChangeEvent->undoPropData[0];
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
    void UndoManager::actorPropertyAboutToChange(dtCore::RefPtr<dtDAL::ActorProxy> proxy,
        dtCore::RefPtr<dtDAL::ActorProperty> property, std::string oldValue, std::string newValue)
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
    void UndoManager::onActorProxyCreated(dtCore::RefPtr<dtDAL::ActorProxy> proxy, bool forceNoAdjustments)
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
    void UndoManager::onProxyNameChanged(dtCore::RefPtr<dtDAL::ActorProxy> proxy, std::string oldName)
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
    void UndoManager::onActorProxyDestroyed(dtCore::RefPtr<dtDAL::ActorProxy> proxy)
    {
        if (!recursePrevent)
        {
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
            dtCore::RefPtr<ChangeEvent> redoEvent = redoStack.top();
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
            dtCore::RefPtr<ChangeEvent> undoEvent = undoStack.top();
            undoStack.pop();

            handleUndoRedoEvent(undoEvent.get(), true);
        }

        enableButtons();

    }

    //////////////////////////////////////////////////////////////////////////////
    void UndoManager::handleUndoRedoEvent(ChangeEvent *event, bool isUndo)
    {
        dtCore::RefPtr<dtDAL::Map> currMap = EditorData::GetInstance().getCurrentMap();

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
        dtCore::RefPtr<dtDAL::ActorProxy> ActorProxyRefPtr = proxy;
        recursePrevent = true;
        EditorEvents::GetInstance().emitProxyNameChanged(ActorProxyRefPtr, currentName);
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
        EditorData::GetInstance().getMainWindow()->startWaitCursor();
        dtCore::RefPtr<dtDAL::Map> currMap = EditorData::GetInstance().getCurrentMap();
        EditorActions::GetInstance().deleteProxy(proxy, currMap);

        //We are deleting an object, so clear the current selection for safety.
        std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > emptySelection;
        EditorEvents::GetInstance().emitActorsSelected(emptySelection);

        EditorData::GetInstance().getMainWindow()->endWaitCursor();
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

        std::vector<dtCore::RefPtr < UndoPropertyData > >::const_iterator undoPropIter;
        dtCore::RefPtr<dtDAL::Map> currMap = EditorData::GetInstance().getCurrentMap();

        // figure out the actor type
        dtCore::RefPtr<const dtDAL::ActorType> actorType = dtDAL::LibraryManager::GetInstance().
            FindActorType(event->actorTypeCategory, event->actorTypeName);

        if (currMap.valid() && actorType.valid())
        {
            EditorData::GetInstance().getMainWindow()->startWaitCursor();

            // recreate the actor!
            dtCore::RefPtr<dtDAL::ActorProxy> proxy =
                    dtDAL::LibraryManager::GetInstance().CreateActorProxy(*actorType.get()).get();
            if (proxy.valid())
            {
                // set all of our old properties before telling anyone else about it
                for (undoPropIter = event->undoPropData.begin(); undoPropIter != event->undoPropData.end(); ++undoPropIter)
                {
                    dtCore::RefPtr < UndoPropertyData> undoProp = (*undoPropIter);
                    // find the prop on the real actor
                    dtDAL::ActorProperty *actorProp = proxy->GetProperty(undoProp->propertyName);

                    // put our value back
                    if (actorProp != NULL)
                    {
                        actorProp->FromString(undoProp->oldValue);
                    }
                }

                proxy->SetId(dtCore::UniqueId(event->objectId));
                proxy->SetName(event->oldName);
                currMap->AddProxy(*(proxy.get()));
                recursePrevent = true;
                EditorEvents::GetInstance().emitBeginChangeTransaction();
                EditorEvents::GetInstance().emitActorProxyCreated(proxy, true);
                ViewportManager::GetInstance().placeProxyInFrontOfCamera(proxy.get());
                EditorEvents::GetInstance().emitEndChangeTransaction();
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
        EditorData::GetInstance().getMainWindow()->endWaitCursor();

    }

    //////////////////////////////////////////////////////////////////////////////
    void UndoManager::handleUndoRedoPropertyValue(ChangeEvent *event, dtDAL::ActorProxy *proxy, bool isUndo)
    {
        // NOTE - The undo/redo methods do both the undo and the redo.  If you are modifying
        // these methods, be VERY careful

        if (event->undoPropData.size() == 0)
            return;

        // for property changed, simply find the old property and reset the value
        dtCore::RefPtr<UndoPropertyData> propData = event->undoPropData[0];
        if (propData.valid())
        {
            dtDAL::ActorProperty *property = proxy->GetProperty(propData->propertyName);
            if (property != NULL)
            {
                std::string currentValue = property->ToString();
                property->FromString(propData->oldValue);

                // notify the world of our change to the data.
                dtCore::RefPtr<dtDAL::ActorProxy> ActorProxyRefPtr = proxy;
                dtCore::RefPtr<dtDAL::ActorProperty> ActorPropertyRefPtr = property;
                recursePrevent = true;
                EditorData::GetInstance().getMainWindow()->startWaitCursor();
                EditorEvents::GetInstance().emitActorPropertyChanged(ActorProxyRefPtr, ActorPropertyRefPtr);
                EditorData::GetInstance().getMainWindow()->endWaitCursor();
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
        EditorActions::GetInstance().actionEditUndo->setEnabled(!undoStack.empty());
        EditorActions::GetInstance().actionEditRedo->setEnabled(!redoStack.empty());
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
            dtCore::RefPtr<ChangeEvent> undoEvent = undoStack.top();
            undoStack.pop();
        }

    }

    //////////////////////////////////////////////////////////////////////////////
    void UndoManager::clearRedoList()
    {
        // clear redo
        while (!redoStack.empty())
        {
            dtCore::RefPtr<ChangeEvent> redoEvent = redoStack.top();
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
