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
#include <prefix/stageprefix.h>

#include <dtEditQt/editorevents.h>
#include <dtUtil/log.h>

namespace dtEditQt
{

   // Singleton instance of the event manager.
   dtCore::RefPtr<EditorEvents> EditorEvents::sInstance(NULL);

   ///////////////////////////////////////////////////////////////////////////////
   EditorEvents::EditorEvents()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   EditorEvents::~EditorEvents()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void EditorEvents::emitActorsSelected(std::vector< dtCore::ActorPtr >& actors)
   {
      LOG_INFO("Emitting UI event - [actorsSelected]");
      emit selectedActors(actors);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void EditorEvents::emitGotoActor(dtCore::ActorPtr actor)
   {
      LOG_INFO("Emitting UI event - [gotoActor]");
      emit gotoActor(actor);
   }

   //////////////////////////////////////////////////////////////////////////
   void EditorEvents::emitGotoPosition(double x, double y, double z)
   {
      emit EditorEvents::gotoPosition(x, y, z);
   }

   //////////////////////////////////////////////////////////////////////////
   void EditorEvents::emitCreateActor()
   {
      emit createActor();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void EditorEvents::emitActorProxyCreated(dtCore::ActorPtr proxy, bool forceNoAdjustments)
   {
      LOG_INFO("Emitting UI event - [actorProxyCreated]");
      emit actorProxyCreated(proxy, forceNoAdjustments);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void EditorEvents::emitActorProxyAboutToBeDestroyed(dtCore::ActorPtr proxy)
   {
      LOG_INFO("Emitting UI event - [actorProxyAboutToBeDestroyed]");
      emit actorProxyAboutToBeDestroyed(proxy);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void EditorEvents::emitActorProxyDestroyed(dtCore::ActorPtr proxy)
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
      // emit
   }

   ///////////////////////////////////////////////////////////////////////////////
   void EditorEvents::emitCurrentMapChanged()
   {
      LOG_INFO("Emitting UI event - [currentMapChanged]");
      emit currentMapChanged();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void EditorEvents::emitCurrentMapSaved()
   {
      LOG_INFO("Emitting UI event - [emitCurrentMapSaved]");
      emit currentMapSaved();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void EditorEvents::emitResetWindows()
   {
      LOG_INFO("Emitting UI event - [emitResetWindows]");
      emit resetWindows();
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
   void EditorEvents::emitActorPropertyChanged(dtCore::ActorPtr proxy,
      dtCore::RefPtr<dtCore::ActorProperty> property)
   {
      emit actorPropertyChanged(proxy, property);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void EditorEvents::emitActorPropertyAboutToChange(dtCore::ActorPtr proxy,
      dtCore::RefPtr<dtCore::ActorProperty> property, std::string oldValue, std::string newValue)
   {
      emit actorPropertyAboutToChange(proxy, property, oldValue, newValue);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void EditorEvents::emitActorHierarchyChanged(dtCore::ActorPtr actor, dtCore::ActorPtr oldParent)
   {
      emit actorHierarchyUpdated(actor, oldParent);
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
   void EditorEvents::emitGameEventAdded()
   {
      LOG_INFO("User added a game event");
      emit mapGameEventAdded();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void EditorEvents::emitGameEventEdited()
   {
      LOG_INFO("User edited a game event");
      emit mapGameEventEdited();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void EditorEvents::emitGameEventAboutToBeRemoved()
   {
      LOG_INFO("User is about to remove an event");
      emit mapGameEventAboutToBeRemoved();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void EditorEvents::emitGameEventRemoved()
   {
      LOG_INFO("User removed an event");
      emit mapGameEventRemoved();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void EditorEvents::emitEditorPreferencesChanged()
   {
      LOG_INFO("User preferences have changed.");
      emit editorPreferencesChanged();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void EditorEvents::emitShowStatusBarMessage(const QString message, int timeout)
   {
      emit showStatusBarMessage(message, timeout);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void EditorEvents::emitEditorCloseEvent()
   {
      LOG_INFO("Editor is closing.");
      emit editorCloseEvent();
   }

   ///////////////////////////////////////////////////////////////////////////////
   EditorEvents& EditorEvents::GetInstance()
   {
      if (EditorEvents::sInstance.get() == NULL)
      {
         EditorEvents::sInstance = new EditorEvents();
      }
      return *(EditorEvents::sInstance.get());
   }

} // namespace dtEditQt
