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

#ifndef DIRECTORQT_GRAPH_TABS
#define DIRECTORQT_GRAPH_TABS

#include <dtDirectorQt/export.h>

#include <QtGui/QTabWidget>

class QPushButton;

namespace dtDirector
{
   class DirectorEditor;

   /**
   * @class GraphTabs
   * @brief This allows multiple tabbed editing of director graphs.
   */
   class GraphTabs : public QTabWidget
   {
      Q_OBJECT

   public:

      /**
      * Constructs a document tabs viewer.
      *
      * @param[in]  parent  The parent widget.
      */
      GraphTabs(QWidget* parent);

      /**
      * Sets the Graph Tabs DirectorEditor
      *
      * @param[in]  editor  The director editor that owns this widget.
      */
      void SetDirectorEditor(DirectorEditor* editor);

   public slots:

      void OnAddNewTabButton();

   protected:

      /**
      * Event handler when the mouse button is clicked.
      *
      * @param[in]  e  The mouse event.
      */
      void mousePressEvent(QMouseEvent* e);

      /**
      * Event handler when the mouse button is released.
      *
      * @param[in]  e  The mouse event.
      */
      void mouseReleaseEvent(QMouseEvent *e);

      /**
      * Event handler when the mouse button is double clicked.
      *
      * @param[in]  e  The mouse event.
      */
      void mouseDoubleClickEvent(QMouseEvent *e);


      DirectorEditor* mEditor;

      QPushButton* mAddButton;
   };
} // namespace dtDirector

#endif // DIRECTORQT_GRAPH_TABS
