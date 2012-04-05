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
#include <dtDirectorQt/undoremoveimportevent.h>
#include <dtDirectorQt/directoreditor.h>
#include <dtDirectorQt/graphtabs.h>
#include <dtDirectorQt/editorview.h>
#include <dtDirectorQt/editorscene.h>

#include <dtDirector/director.h>

#include <QtGui/QGraphicsRectItem>


namespace dtDirector
{
   //////////////////////////////////////////////////////////////////////////
   UndoRemoveImportEvent::UndoRemoveImportEvent(DirectorEditor* editor, const std::string& script)
      : UndoEvent(editor)
      , mScript(script)
   {
   }

   //////////////////////////////////////////////////////////////////////////
   UndoRemoveImportEvent::~UndoRemoveImportEvent()
   {
   }

   //////////////////////////////////////////////////////////////////////////
   void UndoRemoveImportEvent::Undo()
   {
      mEditor->GetDirector()->ImportScript(mScript);
      mEditor->RefreshGraphs();
   }

   //////////////////////////////////////////////////////////////////////////
   void UndoRemoveImportEvent::Redo()
   {
      mEditor->GetDirector()->RemoveImportedScript(mScript);
      mEditor->RefreshGraphs();
   }
}

//////////////////////////////////////////////////////////////////////////
