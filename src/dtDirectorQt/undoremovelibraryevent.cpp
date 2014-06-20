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
#include <prefix/dtdirectorqtprefix.h>
#include <dtDirectorQt/undoremovelibraryevent.h>
#include <dtDirectorQt/directoreditor.h>
#include <dtDirectorQt/graphtabs.h>
#include <dtDirectorQt/editorview.h>
#include <dtDirectorQt/editorscene.h>

#include <dtDirector/director.h>
#include <dtDirector/directorgraph.h>
#include <dtDirector/node.h>
#include <dtDirector/nodemanager.h>

#include <QtGui/QGraphicsRectItem>


namespace dtDirector
{
   //////////////////////////////////////////////////////////////////////////
   UndoRemoveLibraryEvent::UndoRemoveLibraryEvent(DirectorEditor* editor, const std::string& library, int index)
      : UndoEvent(editor)
      , mLibrary(library)
      , mIndex(index)
   {
   }

   //////////////////////////////////////////////////////////////////////////
   UndoRemoveLibraryEvent::~UndoRemoveLibraryEvent()
   {
   }

   //////////////////////////////////////////////////////////////////////////
   void UndoRemoveLibraryEvent::Undo()
   {
      mEditor->GetDirector()->InsertLibrary(mIndex, mLibrary, "");
      mEditor->RefreshNodeScenes();
   }

   //////////////////////////////////////////////////////////////////////////
   void UndoRemoveLibraryEvent::Redo()
   {
      mEditor->GetDirector()->RemoveLibrary(mLibrary);
      mEditor->RefreshNodeScenes();
   }
}

//////////////////////////////////////////////////////////////////////////
