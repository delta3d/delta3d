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

#include <dtDirectorQt/directoreditor.h>
#include <dtDirectorQt/actionitem.h>
#include <dtDirectorQt/valueitem.h>

#include <QtGui/QToolBar>
#include <QtGui/QAction>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QSplitter>
#include <QtGui/QPushButton>
#include <QtGui/QScrollBar>
#include <QtGui/QLabel>
#include <QtGui/QPixmap>
#include <QtGui/QTextEdit>
#include <QtGui/QTabWidget>
#include <QtCore/QTextStream>
#include <QtCore/QFile>
#include <QtGui/QIcon>
#include <QtGui/QMouseEvent>
#include <QtGui/QApplication>

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   GraphTabs::GraphTabs(QWidget* parent, DirectorEditor* editor)
      : QTabWidget(parent)
      , mEditor(editor)
   {
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
      mEditor->OpenGraph(&mEditor->GetDirector()->GetGraphData(), true);
   }

   ////////////////////////////////////////////////////////////////////////////////
   EditorScene::EditorScene(QWidget* parent)
      : QGraphicsScene(parent)
      , mGraph(NULL)
   {
      setBackgroundBrush(Qt::lightGray);
   }

   //////////////////////////////////////////////////////////////////////////
   void EditorScene::SetGraph(dtDirector::DirectorGraphData* graph)
   {
      // First clear the current items.
      clear();

      mGraph = graph;

      if (!mGraph) return;

      // HACK: For now, we're displaying the sub graph.
      if (graph->GetSubGraphs().size())
      {
         mGraph = &graph->GetSubGraphs()[0];
      }

      // Create all nodes in the graph.
      for (int index = 0; index < (int)mGraph->mEventNodes.size(); index++)
      {
         Node* node = mGraph->mEventNodes[index].get();
         ActionItem* item = new ActionItem(node, NULL, this);
         if (item)
         {
            item->setPos(node->GetPosition().x(), node->GetPosition().y());
            addItem(item);
         }
      }

      for (int index = 0; index < (int)mGraph->mActionNodes.size(); index++)
      {
         Node* node = mGraph->mActionNodes[index].get();
         ActionItem* item = new ActionItem(node, NULL, this);
         if (item)
         {
            item->setPos(node->GetPosition().x(), node->GetPosition().y());
            addItem(item);
         }
      }

      for (int index = 0; index < (int)mGraph->mValueNodes.size(); index++)
      {
         Node* node = mGraph->mValueNodes[index].get();
         ValueItem* item = new ValueItem(node, NULL, this);
         if (item)
         {
            item->setPos(node->GetPosition().x(), node->GetPosition().y());
            addItem(item);
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void EditorScene::Refresh()
   {
      SetGraph(mGraph);
   }


   //////////////////////////////////////////////////////////////////////////////
   DirectorEditor::DirectorEditor(Director* director, QWidget* parent)
      : QMainWindow(parent, Qt::Window)
      , mDirector(director)
      , mGraphTabs(NULL)
   {
      // Set the default size of the window.
      resize(900, 600);

      setWindowTitle(mDirector->GetName().c_str());

      // Setup the UI
      QToolBar*    toolbar       = new QToolBar(this);

      mParentAction              = new QAction(tr("Parent"), this);

      mGraphTabs                 = new GraphTabs(this, this);

      // Toolbar
      addToolBar(toolbar);
      toolbar->setObjectName("Toolbar");
      toolbar->setWindowTitle(tr("Toolbar"));

      mParentAction->setStatusTip(tr("Returns to the parent graph"));
      toolbar->addAction(mParentAction);

      // Graph tabs.
      mGraphTabs->setTabsClosable(true);
      mGraphTabs->setMovable(true);
      mGraphTabs->setTabShape(QTabWidget::Rounded);

      // Main layout.
      setCentralWidget(mGraphTabs);

      // Connect slots.
      connect(mGraphTabs, SIGNAL(currentChanged(int)),
         this, SLOT(OnGraphTabChanged(int)));
      connect(mGraphTabs, SIGNAL(tabCloseRequested(int)),
         this, SLOT(OnGraphTabClosed(int)));

      connect(mParentAction, SIGNAL(triggered()),
         this, SLOT(OnParentButton()));

      // Open the home graph.
      OpenGraph(&mDirector->GetGraphData());
   }

   ////////////////////////////////////////////////////////////////////////////////
   DirectorEditor::~DirectorEditor()
   {
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::OpenGraph(dtDirector::DirectorGraphData* graph, bool newTab)
   {
      // Create a new page if we are forcing a new page or
      // if we don't have any pages yet.
      if (mGraphTabs->count() < 1 || newTab)
      {
         EditorScene* scene = new EditorScene();
         scene->setSceneRect(0, 0, 800, 600);
         EditorView* view = new EditorView(scene, this);

         int index = mGraphTabs->addTab(view, "");
         mGraphTabs->setCurrentIndex(index);
      }

      EditorView* view = dynamic_cast<EditorView*>(mGraphTabs->currentWidget());
      if (view && graph)
      {
         mGraphTabs->setTabText(mGraphTabs->currentIndex(), graph->mName.c_str());

         view->GetScene()->SetGraph(graph);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DirectorEditor::OnParentButton()
   {
      // TODO: Find and open the parent graph,
      // then center the view on that child graph.
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DirectorEditor::OnGraphTabChanged(int index)
   {
      // Refresh the graph.
      if (index < mGraphTabs->count())
      {
         EditorView* view = dynamic_cast<EditorView*>(mGraphTabs->widget(index));
         if (view)
         {
            view->GetScene()->Refresh();
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DirectorEditor::OnGraphTabClosed(int index)
   {
      // Remove the tab.
      if (index < mGraphTabs->count())
      {
         EditorView* view = dynamic_cast<EditorView*>(mGraphTabs->widget(index));
         if (view)
         {
            mGraphTabs->removeTab(index);
         }
      }
   }

} // namespace dtDirector
