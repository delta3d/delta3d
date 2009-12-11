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
      , mToolbar(NULL)
      , mFileMenu(NULL)
      , mEditMenu(NULL)
      , mViewMenu(NULL)
      , mSaveAction(NULL)
      , mLoadAction(NULL)
      , mNewAction(NULL)
      , mParentAction(NULL)
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
      mEditMenu->addAction(mUndoAction);
      mEditMenu->addAction(mRedoAction);

      // View Menu.
      mViewMenu = mMenuBar->addMenu("&View");
      mViewMenu->addAction(mViewPropertiesAction);

      // Toolbar.
      mToolbar = new QToolBar(this);
      addToolBar(mToolbar);
      mToolbar->setObjectName("Toolbar");
      mToolbar->setWindowTitle(tr("Toolbar"));

      mToolbar->addAction(mSaveAction);
      mToolbar->addAction(mLoadAction);
      mToolbar->addAction(mNewAction);
      mToolbar->addSeparator();
      mToolbar->addAction(mParentAction);
      mToolbar->addSeparator();
      mToolbar->addAction(mUndoAction);
      mToolbar->addAction(mRedoAction);

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
         OpenGraph(mDirector->GetGraphData(), true);
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
         EditorScene* scene = new EditorScene(mDirector, mPropertyEditor, mGraphTabs);
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
   void DirectorEditor::RefreshButtonStates()
   {
      bool bHasParent = false;

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
         }
      }

            // Parent button.
      mParentAction->setEnabled(bHasParent);

      // Undo button.
      mUndoAction->setEnabled(mUndoManager->CanUndo());

      // Redo button.
      mRedoAction->setEnabled(mUndoManager->CanRedo());
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

         mDirector->LoadScript(fileName.toStdString());

         // Create a single tab with the default graph.
         OpenGraph(mDirector->GetGraphData());
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::OnNewButton()
   {
      // TODO: Check if the undo manager has some un-committed changes first.

      // Clear the script.
      mGraphTabs->clear();
      mDirector->Clear();

      // Create a single tab with the default graph.
      OpenGraph(mDirector->GetGraphData());
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
