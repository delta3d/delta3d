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
#include <dtDirectorQt/macroitem.h>

#include <dtUtil/mathdefines.h>

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
      mEditor->OpenGraph(mEditor->GetDirector()->GetGraphData(), true);
   }

   ////////////////////////////////////////////////////////////////////////////////
   EditorScene::EditorScene(Director* director, PropertyEditor* propEditor, QWidget* parent)
      : QGraphicsScene(parent)
      , mDirector(director)
      , mPropertyEditor(propEditor)
      , mView(NULL)
      , mGraph(NULL)
      , mAllowDrag(false)
      , mDragging(false)
      , mTranslationItem(NULL)
   {
      mSelected.push_back(mDirector.get());
      mPropertyEditor->SetScene(this);

      setBackgroundBrush(Qt::lightGray);
   }

   //////////////////////////////////////////////////////////////////////////
   void EditorScene::SetGraph(dtDirector::DirectorGraphData* graph)
   {
      // First clear the current items.
      clear();
      mNodes.clear();

      // The translation item is the parent class for all other items.
      // This simulates the translation of the view by moving all children
      // nodes with it.  When the user translates the view, this item is
      // actually being translated instead.
      mTranslationItem = new QGraphicsRectItem(NULL, this);

      mGraph = graph;

      if (!mGraph) return;

      // Create all nodes in the graph.
      int count = (int)mGraph->mEventNodes.size();
      for (int index = 0; index < count; index++)
      {
         Node* node = mGraph->mEventNodes[index].get();
         ActionItem* item = new ActionItem(node, mTranslationItem, this);
         if (item)
         {
            item->setZValue(0.0f);
            mNodes.push_back(item);
         }
      }

      count = (int)mGraph->mActionNodes.size();
      for (int index = 0; index < count; index++)
      {
         Node* node = mGraph->mActionNodes[index].get();
         ActionItem* item = new ActionItem(node, mTranslationItem, this);
         if (item)
         {
            item->setZValue(0.0f);
            mNodes.push_back(item);
         }
      }

      count = (int)mGraph->mValueNodes.size();
      for (int index = 0; index < count; index++)
      {
         Node* node = mGraph->mValueNodes[index].get();
         ValueItem* item = new ValueItem(node, mTranslationItem, this);
         if (item)
         {
            item->setZValue(10.0f);
            mNodes.push_back(item);
         }
      }

      count = (int)mGraph->mSubGraphs.size();
      for (int index = 0; index < count; index++)
      {
         DirectorGraphData* graph = mGraph->mSubGraphs[index].get();
         MacroItem* item = new MacroItem(graph, mTranslationItem, this);
         if (item)
         {
            item->setZValue(0.0f);
            mNodes.push_back(item);
         }
      }

      Refresh();
   }

   //////////////////////////////////////////////////////////////////////////
   void EditorScene::Refresh()
   {
      // Re-draw all nodes.
      int count = (int)mNodes.size();
      for (int index = 0; index < count; index++)
      {
         NodeItem* item = mNodes[index];
         if (item)
         {
            item->Draw();
         }
      }

      // Re-connect all nodes.
      for (int index = 0; index < count; index++)
      {
         NodeItem* item = mNodes[index];
         if (item)
         {
            item->ConnectLinks();
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   NodeItem* EditorScene::GetNodeItem(const dtCore::UniqueId& id)
   {
      int count = (int)mNodes.size();
      for (int index = 0; index < count; index++)
      {
         NodeItem* item = mNodes[index];
         if (item && item->HasID(id))
         {
            return item;
         }
      }

      return NULL;
   }

   //////////////////////////////////////////////////////////////////////////
   MacroItem* EditorScene::GetGraphItem(DirectorGraphData* graph)
   {
      int count = (int)mNodes.size();
      for (int index = 0; index < count; index++)
      {
         NodeItem* item = mNodes[index];

         MacroItem* macro = dynamic_cast<MacroItem*>(item);
         if (macro && macro->GetGraph() == graph)
         {
            return macro;
         }
      }

      return NULL;
   }

   //////////////////////////////////////////////////////////////////////////
   void EditorScene::AddSelected(dtDAL::PropertyContainer* container)
   {
      // If we have the director container in the list, remove it.
      if (mSelected.size() == 1 && mSelected[0].get() == mDirector.get())
      {
         mSelected.clear();
      }

      mSelected.push_back(container);

      // Update the property editor.
      mPropertyEditor->HandlePropertyContainersSelected(mSelected);
   }

   //////////////////////////////////////////////////////////////////////////
   void EditorScene::RemoveSelected(dtDAL::PropertyContainer* container)
   {
      int count = (int)mSelected.size();
      for (int index = 0; index < count; index++)
      {
         if (mSelected[index].get() == container)
         {
            mSelected.erase(mSelected.begin() + index);
            break;
         }
      }

      // If we have removed our last selected item, add the director.
      if (mSelected.empty())
      {
         mSelected.push_back(mDirector.get());
      }

      // Update the property editor.
      mPropertyEditor->HandlePropertyContainersSelected(mSelected);
   }

   //////////////////////////////////////////////////////////////////////////
   void EditorScene::RefreshProperties()
   {
      mPropertyEditor->HandlePropertyContainersSelected(mSelected);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
   {
      // If we are not mouse clicking on a node...
      QGraphicsScene::mousePressEvent(event);

      if (!itemAt(event->scenePos()))
      {
         mAllowDrag = true;
         mDragOrigin = event->scenePos();
      }
      else
      {
         mAllowDrag = false;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
   {
      QGraphicsScene::mouseReleaseEvent(event);

      mAllowDrag = false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
   {
      QGraphicsScene::mouseMoveEvent(event);

      if (mDragging) return;

      // If we are dragging the view.
      if (mAllowDrag && mView)
      {
         mDragging = true;
         QPointF mousePos = event->scenePos();
         QPointF inc = mDragOrigin - mousePos;
         mDragOrigin = mousePos;

         //inc = mView->matrix().map(inc);

         // Translate the background item to simulate the entire view moving.
         if (mTranslationItem)
         {
            mTranslationItem->setPos(mTranslationItem->pos() - inc);
         }

         //mView->horizontalScrollBar()->setValue(mView->horizontalScrollBar()->value() + inc.x() / 5.0f);
         //mView->verticalScrollBar()->setValue(mView->verticalScrollBar()->value() + inc.y() / 5.0f);

         mDragging = false;
      }
   }


   //////////////////////////////////////////////////////////////////////////
   EditorView::EditorView(EditorScene* scene, QWidget* parent)
      : QGraphicsView(scene, parent)
      , mScene(scene)
      , mMinScale(0.1f)
      , mMaxScale(1.0f)
      , mCurrentScale(1.0f)
   {
      setObjectName("Graph Tab");
      setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

      // Always hide the scroll bars.
      setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
      setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorView::wheelEvent(QWheelEvent* event)
   {
      // Delta is in eighths of degrees
      int numberOfDegrees = event->delta() / 8;

      // Typical mice move in 15 degree steps
      float numberOfSteps = (float)numberOfDegrees / 15.0f;

      float inc = (numberOfSteps * 0.1f) + 1.0f;

      // Clamp the scale to the min and max range.
      if (mCurrentScale * inc > mMaxScale)
      {
         inc = mMaxScale / mCurrentScale;
      }
      else if (mCurrentScale * inc < mMinScale)
      {
         inc = mMinScale / mCurrentScale;
      }

      mCurrentScale *= inc;
      scale(inc, inc);

      // Translate the view towards the mouse cursor.
      if (numberOfSteps > 0.0f)
      {
         QPointF centerPos(width()/2, height()/2);
         QPointF mousePos = event->pos();

         QPointF translation = mousePos - centerPos;
         translation = mapToScene(translation.x(), translation.y());
         translation *= 0.2f;
         mScene->GetTranslationItem()->setPos(
            mScene->GetTranslationItem()->pos() - translation);
      }
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

      mPropertyEditor            = new PropertyEditor(this);
      addDockWidget(Qt::BottomDockWidgetArea, mPropertyEditor);

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
      OpenGraph(mDirector->GetGraphData(), true);
      OpenGraph(mDirector->GetGraphData()->GetSubGraphs()[0], true);
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
         EditorScene* scene = new EditorScene(mDirector, mPropertyEditor);
         EditorView* view = new EditorView(scene, this);
         scene->SetView(view);

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
            mPropertyEditor->SetScene(view->GetScene());
            view->GetScene()->Refresh();
            view->GetScene()->RefreshProperties();
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
