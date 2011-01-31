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

#include <dtDirectorQt/graphtabs.h>
#include <dtDirectorQt/directoreditor.h>
#include <dtDirectorQt/undomanager.h>

#include <dtDirector/director.h>

#include <QtGui/QMouseEvent>

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   GraphTabs::GraphTabs(QWidget* parent)
      : QTabWidget(parent)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GraphTabs::SetDirectorEditor(DirectorEditor* editor)
   {
      mEditor = editor;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GraphTabs::mousePressEvent(QMouseEvent *e)
   {
      QTabWidget::mousePressEvent(e);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GraphTabs::mouseReleaseEvent(QMouseEvent *e)
   {
      QTabWidget::mouseReleaseEvent(e);

      if (e->button() == Qt::MidButton)
      {
         // TODO: Find the tab that you are mousing over.
         //mEditor->OnGraphTabClosed(currentIndex());
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GraphTabs::mouseDoubleClickEvent(QMouseEvent *e)
   {
      QTabWidget::mouseDoubleClickEvent(e);

      // Create a new tab and set it to the home graph.
      mEditor->OpenGraph(mEditor->GetDirector()->GetGraphRoot(), true);
   }
} // namespace dtDirector

//////////////////////////////////////////////////////////////////////////
