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
#include <dtDirectorQt/graphtabs.h>
#include <dtDirectorQt/editorview.h>
#include <dtDirectorQt/editorscene.h>
#include <dtDirectorQt/undomanager.h>
#include <dtDirectorQt/undodeleteevent.h>

#include <dtDirectorQt/actionitem.h>
#include <dtDirectorQt/valueitem.h>
#include <dtDirectorQt/macroitem.h>

#include <dtUtil/mathdefines.h>

#include <QtGui/QMenuBar>
#include <QtGui/QToolBar>
#include <QtGui/QAction>
#include <QtGui/QKeyEvent>
#include <QtGui/QFileDialog>

#include <dtDAL/project.h>

namespace dtDirector
{
   //////////////////////////////////////////////////////////////////////////////
   DirectorEditor::DirectorEditor(QWidget* parent)
      : QMainWindow(parent, Qt::Window)
      , mDirector(NULL)
      , mGraphTabs(NULL)
      , mPropertyEditor(NULL)
      , mUndoManager(NULL)
      , mMenuBar(NULL)
      , mFileToolbar(NULL)
      , mEditToolbar(NULL)
      , mFileMenu(NULL)
      , mEditMenu(NULL)
      , mViewMenu(NULL)
      , mSaveAction(NULL)
      , mLoadAction(NULL)
      , mNewAction(NULL)
      , mParentAction(NULL)
      , mUndoAction(NULL)
      , mRedoAction(NULL)
      , mDeleteAction(NULL)
      , mViewPropertiesAction(NULL)
   {
      // Set the default size of the window.
      resize(900, 600);

      setWindowTitle("No Director Graph Loaded");

      // Undo Manager.
      mUndoManager = new UndoManager(this);

      // Property editor.
      mPropertyEditor = new PropertyEditor(this);
      addDockWidget(Qt::BottomDockWidgetArea, mPropertyEditor);

      // Save Action.
      mSaveAction = new QAction(QIcon(":/icons/save.png"), tr("&Save"), this);
      mSaveAction->setShortcut(tr("Ctrl+S"));
      mSaveAction->setToolTip(tr("Saves the current Director script (Ctrl+S)."));

      // Load Action.
      mLoadAction = new QAction(QIcon(":/icons/open.png"), tr("&Load"), this);
      mLoadAction->setShortcut(tr("Ctrl+L"));
      mLoadAction->setToolTip(tr("Loads a Director script from a file (Ctrl+L)."));

      // New Action.
      mNewAction = new QAction(QIcon(":/icons/new.png"), tr("&New"), this);
      mNewAction->setShortcut(tr("Ctrl+N"));
      mNewAction->setToolTip(tr("Begins a new Director script (Ctrl+N)."));

      // Parent Action.
      mParentAction = new QAction(QIcon(":/icons/parent.png"), tr("Goto Parent Graph"), this);
      mParentAction->setShortcut(tr("Ctrl+U"));
      mParentAction->setToolTip(tr("Returns to the parent graph (Ctrl+U)."));

      // Undo Action.
      mUndoAction = new QAction(QIcon(":/icons/undo.png"), tr("Undo"), this);
      mUndoAction->setShortcut(tr("Ctrl+Z"));
      mUndoAction->setToolTip(tr("Reverts to your last action (Ctrl+Z)."));

      // Redo Action.
      mRedoAction = new QAction(QIcon(":/icons/redo.png"), tr("Redo"), this);
      mRedoAction->setShortcut(tr("Ctrl+Y"));
      mRedoAction->setToolTip(tr("Reverts your last undo action (Ctrl+Y)."));

      // Delete Action.
      mDeleteAction = new QAction(QIcon(":/icons/delete.png"), tr("Delete"), this);
      mDeleteAction->setShortcut(tr("Delete"));
      mDeleteAction->setToolTip(tr("Reverts your last undo action (Delete)."));

      // Show Properties Action.
      mViewPropertiesAction = new QAction(tr("Property Editor"), this);
      mViewPropertiesAction->setShortcut(tr("Ctrl+P"));
      mViewPropertiesAction->setToolTip(tr("Shows the Property Editor(Ctrl+P)."));
      mViewPropertiesAction->setCheckable(true);
      mViewPropertiesAction->setChecked(true);

      // Graph tabs.
      mGraphTabs = new GraphTabs(this, this);
      mGraphTabs->setTabsClosable(true);
      mGraphTabs->setMovable(true);
      mGraphTabs->setTabShape(QTabWidget::Rounded);
      mPropertyEditor->SetGraphTabs(mGraphTabs);

      // Menu Bar.
      mMenuBar = new QMenuBar(this);
      setMenuBar(mMenuBar);
      mMenuBar->setObjectName("Menu Bar");
      mMenuBar->setWindowTitle("Menu Bar");

      // File Menu.
      mFileMenu = mMenuBar->addMenu("&File");
      mFileMenu->addAction(mSaveAction);
      mFileMenu->addAction(mLoadAction);
      mFileMenu->addAction(mNewAction);
      mFileMenu->addSeparator();

      // Edit Menu.
      mEditMenu = mMenuBar->addMenu("&Edit");
      mEditMenu->addAction(mParentAction);
      mEditMenu->addSeparator();
      mEditMenu->addAction(mUndoAction);
      mEditMenu->addAction(mRedoAction);
      mEditMenu->addSeparator();
      mEditMenu->addAction(mDeleteAction);

      // View Menu.
      mViewMenu = mMenuBar->addMenu("&View");
      mViewMenu->addAction(mViewPropertiesAction);

      // File Toolbar.
      mFileToolbar = new QToolBar(this);
      addToolBar(mFileToolbar);
      mFileToolbar->setObjectName("File Toolbar");
      mFileToolbar->setWindowTitle(tr("File Toolbar"));

      mFileToolbar->addAction(mSaveAction);
      mFileToolbar->addAction(mLoadAction);
      mFileToolbar->addAction(mNewAction);

      // Edit Toolbar.
      mEditToolbar = new QToolBar(this);
      addToolBar(mEditToolbar);
      mEditToolbar->setObjectName("Edit Toolbar");
      mEditToolbar->setWindowTitle(tr("Edit Toolbar"));

      mEditToolbar->addAction(mParentAction);
      mEditToolbar->addSeparator();
      mEditToolbar->addAction(mUndoAction);
      mEditToolbar->addAction(mRedoAction);
      mEditToolbar->addSeparator();
      mEditToolbar->addAction(mDeleteAction);

      // Main layout.
      setCentralWidget(mGraphTabs);

      // Connect slots.
      connect(mGraphTabs, SIGNAL(currentChanged(int)),
         this, SLOT(OnGraphTabChanged(int)));
      connect(mGraphTabs, SIGNAL(tabCloseRequested(int)),
         this, SLOT(OnGraphTabClosed(int)));

      connect(mPropertyEditor, SIGNAL(visibilityChanged(bool)),
         this, SLOT(OnPropertyEditorVisibilityChange(bool)));

      connect(mSaveAction, SIGNAL(triggered()),
         this, SLOT(OnSaveButton()));
      connect(mLoadAction, SIGNAL(triggered()),
         this, SLOT(OnLoadButton()));
      connect(mNewAction, SIGNAL(triggered()),
         this, SLOT(OnNewButton()));
      connect(mParentAction, SIGNAL(triggered()),
         this, SLOT(OnParentButton()));
      connect(mUndoAction, SIGNAL(triggered()),
         this, SLOT(OnUndo()));
      connect(mRedoAction, SIGNAL(triggered()),
         this, SLOT(OnRedo()));
      connect(mDeleteAction, SIGNAL(triggered()),
         this, SLOT(OnDelete()));
      connect(mViewPropertiesAction, SIGNAL(triggered()),
         this, SLOT(OnShowPropertyEditor()));
   }

   ////////////////////////////////////////////////////////////////////////////////
   DirectorEditor::~DirectorEditor()
   {
      delete mPropertyEditor;
      delete mUndoManager;
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::SetDirector(Director* director)
   {
      mDirector = director;

      mGraphTabs->clear();
      if (mDirector)
      {
         setWindowTitle(mDirector->GetName().c_str());

         // Open the home graph.
         OpenGraph(mDirector->GetGraphRoot(), true);
      }
      else
      {
         setWindowTitle("No Director Graph Loaded");
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::OpenGraph(dtDirector::DirectorGraph* graph, bool newTab)
   {
      // Create a new page if we are forcing a new page or
      // if we don't have any pages yet.
      if (mGraphTabs->count() < 1 || newTab)
      {
         EditorScene* scene = new EditorScene(mPropertyEditor, mGraphTabs);
         EditorView* view = new EditorView(scene, this);
         scene->SetEditor(this);
         scene->SetView(view);

         int index = mGraphTabs->addTab(view, "");
         mGraphTabs->setCurrentIndex(index);
      }

      EditorView* view = dynamic_cast<EditorView*>(mGraphTabs->currentWidget());
      if (view && graph)
      {
         view->GetScene()->SetGraph(graph);
      }

      RefreshButtonStates();
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::Refresh()
   {
      // Refresh the button states.
      RefreshButtonStates();

      // Refresh the graph tabs with their graph names.
      int count = mGraphTabs->count();
      for (int index = 0; index < count; index++)
      {
         EditorView* view = dynamic_cast<EditorView*>(mGraphTabs->widget(index));
         if (view && view->GetScene())
         {
            DirectorGraph* graph = view->GetScene()->GetGraph();
            if (graph)
            {
               mGraphTabs->setTabText(index, graph->mName.c_str());
            }
         }
      }

      // Refresh the Scene.
      mPropertyEditor->GetScene()->Refresh();
      
      // Refresh the Properties.
      mPropertyEditor->GetScene()->RefreshProperties();

      // Refresh the tab names.
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::RefreshButtonStates()
   {
      // Refresh the name of the window.
      QString title = mDirector->GetName().c_str();
      if (mUndoManager->IsModified()) title += "*";
      setWindowTitle(title);

      bool bHasParent = false;
      bool bCanDelete = false;

      int tabIndex = mGraphTabs->currentIndex();
      if (tabIndex >= 0 && tabIndex < mGraphTabs->count())
      {
         EditorView* view = dynamic_cast<EditorView*>(mGraphTabs->widget(tabIndex));
         if (view && view->GetScene() && view->GetScene()->GetGraph())
         {
            if (view->GetScene()->GetGraph()->mParent)
            {
               bHasParent = true;
            }

            if (view->GetScene()->HasSelection())
            {
               bCanDelete = true;
            }
         }
      }

      // Parent button.
      mParentAction->setEnabled(bHasParent);

      // Undo button.
      mUndoAction->setEnabled(mUndoManager->CanUndo());

      // Redo button.
      mRedoAction->setEnabled(mUndoManager->CanRedo());

      // Delete button.
      mDeleteAction->setEnabled(bCanDelete);
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::OnPropertyEditorVisibilityChange(bool visible)
   {
      mViewPropertiesAction->setChecked(visible);
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
            RefreshButtonStates();
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
            RefreshButtonStates();
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::OnSaveButton()
   {
      QString filter = tr(".dtDir");
      std::string context = dtDAL::Project::GetInstance().GetContext();

      QFileDialog dialog;
      QFileInfo filePath = dialog.getSaveFileName(this, tr("Director File Name"),
         tr((context + "\\scripts\\").c_str()),
         tr("Director Scripts (*.dtDir)"), &filter);

      QString fileName = filePath.baseName();
      if (!fileName.isEmpty())
      {
         mDirector->SaveScript(fileName.toStdString());
         mUndoManager->OnSaved();

         RefreshButtonStates();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::OnLoadButton()
   {
      // TODO: Check if the undo manager has some un-committed changes first.


      QString filter = tr(".dtDir");
      std::string context = dtDAL::Project::GetInstance().GetContext();

      QFileDialog dialog;
      QFileInfo filePath = dialog.getOpenFileName(this, tr("Director File Name"),
         tr((context + "\\scripts\\").c_str()),
         tr("Director Scripts (*.dtDir)"), &filter);

      QString fileName = filePath.baseName();
      if (!fileName.isEmpty())
      {
         // Clear the script.
         mDirector->Clear();
         mGraphTabs->clear();
         mUndoManager->Clear();

         mDirector->LoadScript(fileName.toStdString());

         // Create a single tab with the default graph.
         OpenGraph(mDirector->GetGraphRoot());
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::OnNewButton()
   {
      // TODO: Check if the undo manager has some un-committed changes first.

      // Clear the script.
      mGraphTabs->clear();
      mDirector->Clear();
      mUndoManager->Clear();

      // Create a single tab with the default graph.
      OpenGraph(mDirector->GetGraphRoot());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DirectorEditor::OnParentButton()
   {
      int index = mGraphTabs->currentIndex();
      if (index >= 0 && index < mGraphTabs->count())
      {
         EditorView* view = dynamic_cast<EditorView*>(mGraphTabs->widget(index));
         if (view)
         {
            dtDirector::DirectorGraph* graph = view->GetScene()->GetGraph();
            if (graph && graph->mParent)
            {
               view->GetScene()->SetGraph(graph->mParent);
               RefreshButtonStates();
            }
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::OnUndo()
   {
      mUndoManager->Undo();
      RefreshButtonStates();
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::OnRedo()
   {
      mUndoManager->Redo();
      RefreshButtonStates();
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::OnDelete()
   {
      // Get the current selection.
      EditorScene* scene = mPropertyEditor->GetScene();
      if (!scene) return;

      mUndoManager->BeginMultipleEvents();

      QList<QGraphicsItem*> selection = scene->selectedItems();
      int count = (int)selection.size();
      for (int index = 0; index < count; index++)
      {
         NodeItem* item = dynamic_cast<NodeItem*>(selection[index]);
         if (item)
         {
            dtCore::UniqueId id;
            dtCore::UniqueId parentID;

            // Find the node IDs.
            if (item->GetNode())
            {
               id = item->GetNode()->GetID();
               parentID = item->GetNode()->GetGraph()->GetID();
            }
            else
            {
               // Check if the item is a graph.
               MacroItem* macro = dynamic_cast<MacroItem*>(item);
               if (macro && macro->GetGraph())
               {
                  id = macro->GetGraph()->GetID();
                  parentID = macro->GetGraph()->mParent->GetID();
               }
               else
               {
                  // Should never go here, it means the item
                  // is not a node or a graph.
                  continue;
               }
            }

            // Create an undo event.
            dtCore::RefPtr<UndoDeleteEvent> event = new UndoDeleteEvent(this, id, parentID);
            mUndoManager->AddEvent(event);

            // Delete the node or graph.
            mDirector->DeleteNode(id);
            mDirector->DeleteGraph(id);

            // Remove the node from all UI's
            int graphCount = mGraphTabs->count();
            for (int graphIndex = 0; graphIndex < graphCount; graphIndex++)
            {
               EditorView* view = dynamic_cast<EditorView*>(mGraphTabs->widget(graphIndex));
               if (view && view->GetScene())
               {
                  // If the scene is displaying the contents of a graph that is deleted,
                  // remove the tab.
                  if (view->GetScene()->GetGraph()->GetID() == id)
                  {
                     mGraphTabs->removeTab(graphIndex);
                     graphIndex--;
                     continue;
                  }
                  else
                  {
                     // We need to find the node item that belongs to the scene.
                     NodeItem* nodeItem = view->GetScene()->GetNodeItem(id, true);
                     if (nodeItem) view->GetScene()->DeleteNode(nodeItem);
                  }
               }
            }
         }
      }

      mUndoManager->EndMultipleEvents();

      // Refresh the current view.
      Refresh();
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::OnShowPropertyEditor()
   {
      if (mViewPropertiesAction->isChecked())
      {
         mPropertyEditor->show();
      }
      else
      {
         mPropertyEditor->hide();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DirectorEditor::keyPressEvent(QKeyEvent* e)
   {
      bool holdingControl = false;
      if (e->modifiers() & Qt::ControlModifier)
      {
         holdingControl = true;
      }

      bool holdingShift = false;
      if (e->modifiers() & Qt::ShiftModifier)
      {
         holdingShift = true;
      }

      // Redo event.
      if (e->key() == Qt::Key_Z && holdingControl && holdingShift)
      {
         OnUndo();
      }
   }
} // namespace dtDirector

//////////////////////////////////////////////////////////////////////////
