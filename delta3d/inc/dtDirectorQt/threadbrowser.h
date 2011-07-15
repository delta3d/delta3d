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

#ifndef DIRECTORQT_THREAD_BROWSER
#define DIRECTORQT_THREAD_BROWSER

#include <dtDirectorQt/export.h>

#include <dtDirector/director.h>
#include <dtDirector/directorgraph.h>

#include <QtGui/QDockWidget>
#include <QtGui/QListWidgetItem>

#include <stack>

class QGroupBox;
class QListWidget;

namespace dtDirector
{
   class DirectorEditor;

   class DT_DIRECTOR_QT_EXPORT ThreadItem : public QListWidgetItem
   {
   public:

      /**
       * Constructor.
       *
       * @param[in]  editor  The editor.
       * @param[in]  node    The node.
       * @param[in]  parent  The parent.
       */
      ThreadItem(Node* node, QListWidget* parent);

      /**
       * Retrieves the active node.
       */
      Node* GetNode() {return mNode.get();}

   private:

      dtCore::ObserverPtr<Node>   mNode;
   };

   /**
    * @class ThreadBrowser
    *
    * @brief Browser tree for viewing currently active threads
    *        while debugging.
    */
   class DT_DIRECTOR_QT_EXPORT ThreadBrowser : public QDockWidget
   {
      Q_OBJECT

   public:

      /**
       * Constructor.
       *
       * @param[in]  parent    This editor parent.
       */
      ThreadBrowser(QMainWindow* parent);

      /**
      * Sets the Thread Browser's DirectorEditor
      *
      * @param[in]  editor  The director editor that owns this widget.
      */
      void SetDirectorEditor(DirectorEditor* editor);

      /**
       * Builds the Graph list.
       */
      void BuildThreadList();

   public slots:
      
      /**
       * Event handler when the current item has changed.
       *
       * @param[in]  current   The new current item.
       * @param[in]  previous  The previous item.
       */
      void OnItemChanged(QListWidgetItem* current, QListWidgetItem* previous);

      /**
       * Event handler when an item is double clicked.
       *
       * @param[in]  item  The item.
       */
      void OnItemDoubleClicked(QListWidgetItem* item);

   private:

      DirectorEditor* mEditor;

      QGroupBox*      mGroupBox;
      QListWidget*    mThreadList;
   };
}

#endif // DIRECTORQT_THREAD_BROWSER
