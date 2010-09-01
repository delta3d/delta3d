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

#ifndef DIRECTORQT_REPLAY_BROWSER
#define DIRECTORQT_REPLAY_BROWSER

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

   class DT_DIRECTOR_QT_EXPORT ReplayThreadItem : public QListWidgetItem
   {
   public:

      /**
       * Constructor.
       *
       * @param[in]  editor  The editor.
       * @param[in]  thread  The thread.
       * @param[in]  node    The node.
       * @param[in]  output  The output link that triggered this thread node.
       * @param[in]  time    The current thread time.
       * @param[in]  parent  The parent.
       */
      ReplayThreadItem(DirectorEditor* editor, Director::RecordThreadData* thread, Director::RecordNodeData* node, OutputLink* output, float time, QListWidget* parent);

      /**
       * Retrieves the node data.
       */
      Director::RecordNodeData* GetNode() {return mNode;}

      /**
       * Retrieves the output link that triggered the node.
       */
      OutputLink* GetOutput() {return mOutput;}

      /**
       * Retrieves the thread for this node.
       */
      Director::RecordThreadData* GetThread() {return mThread;}

      /**
       * Retrieves whether this has a valid node.
       */
      bool IsValid() {return mValid;}

      /**
       * Retrieves whether this item is the back option.
       */
      bool IsBackOption() {return !mEditor;}


   private:

      DirectorEditor*             mEditor;
      Director::RecordNodeData*   mNode;
      OutputLink*                 mOutput;
      Director::RecordThreadData* mThread;
      bool                        mValid;
   };

   /**
    * @class ReplayBrowser
    *
    * @brief Browser tree for viewing Director Graphs.
    */
   class DT_DIRECTOR_QT_EXPORT ReplayBrowser : public QDockWidget
   {
      Q_OBJECT

   public:

      /**
       * Constructor.
       *
       * @param[in]  parent    This editor parent.
       */
      ReplayBrowser(DirectorEditor* parent);

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

      /**
       * Tests a thread for a specific starting graph.
       *
       * @param[in]   graph   The graph to test for.
       * @param[in]   thread  The thread.
       *
       * @return     Returns true if the graph matches the thread.
       */
      bool TestThreadGraph(DirectorGraph* graph, Director::RecordThreadData* thread);

      /**
       * Tests a thread for a specific node.
       *
       * @param[in]  nodeID             The ID of the node to search for.
       * @param[in]  thread             The thread to search.
       * @param[in]  testCurrentThread  True to test the current thread.
       */
      bool TestThreadNode(const dtCore::UniqueId& nodeID, Director::RecordThreadData* thread, bool testCurrentThread = true);

      struct ThreadPathData
      {
         Director::RecordNodeData* node;
         OutputLink*               output;
      };

      DirectorEditor* mEditor;
      ThreadPathData             mCurrentNode;
      std::stack<ThreadPathData> mNodePath;

      QGroupBox*     mGroupBox;
      QListWidget*   mThreadList;
   };
}

#endif // DIRECTORQT_REPLAY_BROWSER
