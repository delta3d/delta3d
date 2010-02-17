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
#include <dtDirectorQt/graphbrowser.h>
#include <dtDirectorQt/replaybrowser.h>
#include <dtDirectorQt/editorview.h>
#include <dtDirectorQt/editorscene.h>
#include <dtDirectorQt/undomanager.h>
#include <dtDirectorQt/undodeleteevent.h>
#include <dtDirectorQt/clipboard.h>
#include <dtDirectorQt/libraryeditor.h>

#include <dtDirectorQt/actionitem.h>
#include <dtDirectorQt/valueitem.h>
#include <dtDirectorQt/macroitem.h>

#include <dtUtil/mathdefines.h>

#include <QtGui/QMenuBar>
#include <QtGui/QToolBar>
#include <QtGui/QAction>
#include <QtGui/QKeyEvent>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>

#include <dtDAL/project.h>

namespace dtDirector
{
   //////////////////////////////////////////////////////////////////////////////
   DirectorEditor::DirectorEditor(QWidget* parent)
      : QMainWindow(parent, Qt::Window)
      , mGraphTabs(NULL)
      , mPropertyEditor(NULL)
      , mGraphBrowser(NULL)
      , mReplayBrowser(NULL)
      , mUndoManager(NULL)
      , mDirector(NULL)
      , mReplayMode(false)
      , mReplayInput(NULL)
      , mReplayOutput(NULL)
      , mMenuBar(NULL)
      , mFileToolbar(NULL)
      , mEditToolbar(NULL)
      , mFileMenu(NULL)
      , mEditMenu(NULL)
      , mViewMenu(NULL)
      , mNewAction(NULL)
      , mLoadAction(NULL)
      , mSaveAction(NULL)
      , mSaveAsAction(NULL)
      , mLoadRecordingAction(NULL)
      , mParentAction(NULL)
      , mSnapGridAction(NULL)
      , mUndoAction(NULL)
      , mRedoAction(NULL)
      , mDeleteAction(NULL)
      , mCutAction(NULL)
      , mCopyAction(NULL)
      , mPasteAction(NULL)
      , mLibrariesAction(NULL)
      , mViewPropertiesAction(NULL)
      , mViewGraphBrowserAction(NULL)
      , mViewReplayBrowserAction(NULL)
      , mShowLinksAction(NULL)
      , mHideLinksAction(NULL)
      , mRefreshAction(NULL)
   {
      // Set the default size of the window.
      resize(900, 600);

      setWindowTitle("No Director Graph Loaded");

      // Undo Manager.
      mUndoManager = new UndoManager(this);

      // Property editor.
      mPropertyEditor = new PropertyEditor(this);
      addDockWidget(Qt::BottomDockWidgetArea, mPropertyEditor);

      // Graph browser.
      mGraphBrowser = new GraphBrowser(this);
      addDockWidget(Qt::BottomDockWidgetArea, mGraphBrowser);

      // Replay browser.
      mReplayBrowser = new ReplayBrowser(this);
      addDockWidget(Qt::RightDockWidgetArea, mReplayBrowser);
      mReplayBrowser->hide();

      // New Action.
      mNewAction = new QAction(QIcon(":/icons/new.png"), tr("&New"), this);
      mNewAction->setShortcut(tr("Ctrl+N"));
      mNewAction->setToolTip(tr("Begins a new Director script (Ctrl+N)."));

      // Load Action.
      mLoadAction = new QAction(QIcon(":/icons/open.png"), tr("&Load"), this);
      mLoadAction->setShortcut(tr("Ctrl+L"));
      mLoadAction->setToolTip(tr("Loads a Director script from a file (Ctrl+L)."));

      // Save Action.
      mSaveAction = new QAction(QIcon(":/icons/save.png"), tr("&Save"), this);
      mSaveAction->setShortcut(tr("Ctrl+S"));
      mSaveAction->setToolTip(tr("Saves the current Director script (Ctrl+S)."));

      // Save Action.
      mSaveAsAction = new QAction(QIcon(":/icons/save.png"), tr("&Save as..."), this);
      mSaveAsAction->setToolTip(tr("Saves the current Director script."));

      // Load Recording Action.
      mLoadRecordingAction = new QAction(QIcon(":/icons/open.png"), tr("&Load Recording"), this);
      mLoadRecordingAction->setToolTip(tr("Loads recorded script data."));

      // Parent Action.
      mParentAction = new QAction(QIcon(":/icons/parent.png"), tr("Step out of Graph"), this);
      mParentAction->setShortcut(tr("Ctrl+U"));
      mParentAction->setToolTip(tr("Returns to the parent graph (Ctrl+U)."));

      // Show Links Action.
      mSnapGridAction = new QAction(QIcon(":/icons/snapgrid.png"), tr("Smart Grid Snap"), this);
      mSnapGridAction->setShortcut(tr("Ctrl+G"));
      mSnapGridAction->setToolTip(tr("Snaps nodes to a smart grid determined by the placement of other nodes (Ctrl+G)."));
      mSnapGridAction->setCheckable(true);
      mSnapGridAction->setChecked(true);

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

      // Cut Action.
      mCutAction = new QAction(QIcon(":/icons/cut.png"), tr("Cut"), this);
      mCutAction->setShortcut(tr("Ctrl+X"));
      mCutAction->setToolTip(tr("Cuts the currently selected nodes to the clipboard (Ctrl+X)."));

      // Copy Action.
      mCopyAction = new QAction(QIcon(":/icons/duplicate.png"), tr("Copy"), this);
      mCopyAction->setShortcut(tr("Ctrl+C"));
      mCopyAction->setToolTip(tr("Copies the currently selected nodes to the clipboard (Ctrl+C)."));

      // Cut Action.
      mPasteAction = new QAction(QIcon(":/icons/paste.png"), tr("Paste"), this);
      mPasteAction->setShortcut(tr("Ctrl+V"));
      mPasteAction->setToolTip(tr("Pastes the nodes saved in the clipboard to the current graph (Ctrl+V)."));

      // Libraries Action.
      mLibrariesAction = new QAction("Manage Libraries...", this);
      mLibrariesAction->setToolTip(tr("Manages the Node Libraries for the script."));

      // Show Properties Action.
      mViewPropertiesAction = new QAction(tr("Property Editor"), this);
      mViewPropertiesAction->setShortcut(tr("Ctrl+P"));
      mViewPropertiesAction->setToolTip(tr("Shows the Property Editor (Ctrl+P)."));
      mViewPropertiesAction->setCheckable(true);
      mViewPropertiesAction->setChecked(true);

      // Show Graph browser Action.
      mViewGraphBrowserAction = new QAction(tr("Graph Browser"), this);
      mViewGraphBrowserAction->setShortcut(tr("Ctrl+B"));
      mViewGraphBrowserAction->setToolTip(tr("Shows the Graph Browser (Ctrl+B)."));
      mViewGraphBrowserAction->setCheckable(true);
      mViewGraphBrowserAction->setChecked(true);

      // Show Replay browser Action.
      mViewReplayBrowserAction = new QAction(tr("Replay Browser"), this);
      mViewReplayBrowserAction->setShortcut(tr("Ctrl+R"));
      mViewReplayBrowserAction->setToolTip(tr("Shows the Replay Browser (Ctrl+R)."));
      mViewReplayBrowserAction->setCheckable(true);
      mViewReplayBrowserAction->setChecked(false);

      // Show Links Action.
      mShowLinksAction = new QAction(QIcon(":/icons/showlinks.png"), tr("Show Links"), this);
      mShowLinksAction->setShortcut(tr("Ctrl+U"));
      mShowLinksAction->setToolTip(tr("Shows all hidden links on selected nodes (Ctrl+U)."));

      // Hide Links Action.
      mHideLinksAction = new QAction(QIcon(":/icons/hidelinks.png"), tr("Hide Links"), this);
      mHideLinksAction->setShortcut(tr("Ctrl+H"));
      mHideLinksAction->setToolTip(tr("Hides all unused links on selected nodes (Ctrl+H)."));

      // Show refresh Action.
      mRefreshAction = new QAction(QIcon(":/icons/refresh.png"), tr("Refresh"), this);
      mRefreshAction->setShortcut(tr("F5"));
      mRefreshAction->setToolTip(tr("Refresh the current view (F5)."));

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
      mFileMenu->addAction(mNewAction);
      mFileMenu->addAction(mLoadAction);
      mFileMenu->addAction(mSaveAction);
      mFileMenu->addAction(mSaveAsAction);
      mFileMenu->addSeparator();
      mFileMenu->addAction(mLoadRecordingAction);

      // Edit Menu.
      mEditMenu = mMenuBar->addMenu("&Edit");
      mEditMenu->addAction(mParentAction);
      mEditMenu->addSeparator();
      mEditMenu->addAction(mSnapGridAction);
      mEditMenu->addSeparator();
      mEditMenu->addAction(mUndoAction);
      mEditMenu->addAction(mRedoAction);
      mEditMenu->addSeparator();
      mEditMenu->addAction(mCutAction);
      mEditMenu->addAction(mCopyAction);
      mEditMenu->addAction(mPasteAction);
      mEditMenu->addSeparator();
      mEditMenu->addAction(mDeleteAction);
      mEditMenu->addSeparator();
      mEditMenu->addAction(mLibrariesAction);

      // View Menu.
      mViewMenu = mMenuBar->addMenu("&View");
      mViewMenu->addAction(mViewPropertiesAction);
      mViewMenu->addAction(mViewGraphBrowserAction);
      mViewMenu->addAction(mViewReplayBrowserAction);
      mViewMenu->addSeparator();
      mViewMenu->addAction(mShowLinksAction);
      mViewMenu->addAction(mHideLinksAction);
      mViewMenu->addSeparator();
      mViewMenu->addAction(mRefreshAction);

      // File Toolbar.
      mFileToolbar = new QToolBar(this);
      addToolBar(mFileToolbar);
      mFileToolbar->setObjectName("File Toolbar");
      mFileToolbar->setWindowTitle(tr("File Toolbar"));

      mFileToolbar->addAction(mNewAction);
      mFileToolbar->addAction(mLoadAction);
      mFileToolbar->addAction(mSaveAction);

      // Edit Toolbar.
      mEditToolbar = new QToolBar(this);
      addToolBar(mEditToolbar);
      mEditToolbar->setObjectName("Edit Toolbar");
      mEditToolbar->setWindowTitle(tr("Edit Toolbar"));

      mEditToolbar->addAction(mParentAction);
      mEditToolbar->addSeparator();
      mEditToolbar->addAction(mSnapGridAction);
      mEditToolbar->addSeparator();
      mEditToolbar->addAction(mUndoAction);
      mEditToolbar->addAction(mRedoAction);
      mEditToolbar->addSeparator();
      mEditToolbar->addAction(mCutAction);
      mEditToolbar->addAction(mCopyAction);
      mEditToolbar->addAction(mPasteAction);
      mEditToolbar->addSeparator();
      mEditToolbar->addAction(mDeleteAction);

      mEditToolbar->addSeparator();
      mEditToolbar->addAction(mShowLinksAction);
      mEditToolbar->addAction(mHideLinksAction);
      mEditToolbar->addSeparator();
      mEditToolbar->addAction(mRefreshAction);

      // Main layout.
      setCentralWidget(mGraphTabs);

      // Connect slots.
      connect(mGraphTabs, SIGNAL(currentChanged(int)),
         this, SLOT(OnGraphTabChanged(int)));
      connect(mGraphTabs, SIGNAL(tabCloseRequested(int)),
         this, SLOT(OnGraphTabClosed(int)));

      connect(mPropertyEditor, SIGNAL(visibilityChanged(bool)),
         this, SLOT(OnPropertyEditorVisibilityChange(bool)));
      connect(mGraphBrowser, SIGNAL(visibilityChanged(bool)),
         this, SLOT(OnGraphBrowserVisibilityChange(bool)));
      connect(mReplayBrowser, SIGNAL(visibilityChanged(bool)),
         this, SLOT(OnReplayBrowserVisibilityChange(bool)));

      connect(mSaveAction, SIGNAL(triggered()),
         this, SLOT(OnSaveButton()));
      connect(mSaveAsAction, SIGNAL(triggered()),
         this, SLOT(OnSaveAsButton()));
      connect(mLoadAction, SIGNAL(triggered()),
         this, SLOT(OnLoadButton()));
      connect(mNewAction, SIGNAL(triggered()),
         this, SLOT(OnNewButton()));
      connect(mLoadRecordingAction, SIGNAL(triggered()),
         this, SLOT(OnLoadRecordingButton()));

      connect(mParentAction, SIGNAL(triggered()),
         this, SLOT(OnParentButton()));
      connect(mUndoAction, SIGNAL(triggered()),
         this, SLOT(OnUndo()));
      connect(mRedoAction, SIGNAL(triggered()),
         this, SLOT(OnRedo()));
      connect(mCutAction, SIGNAL(triggered()),
         this, SLOT(OnCut()));
      connect(mCopyAction, SIGNAL(triggered()),
         this, SLOT(OnCopy()));
      connect(mPasteAction, SIGNAL(triggered()),
         this, SLOT(OnPaste()));
      connect(mDeleteAction, SIGNAL(triggered()),
         this, SLOT(OnDelete()));
      connect(mLibrariesAction, SIGNAL(triggered()),
         this, SLOT(OnManageLibraries()));

      connect(mViewPropertiesAction, SIGNAL(triggered()),
         this, SLOT(OnShowPropertyEditor()));
      connect(mViewGraphBrowserAction, SIGNAL(triggered()),
         this, SLOT(OnShowGraphBrowser()));
      connect(mViewReplayBrowserAction, SIGNAL(triggered()),
         this, SLOT(OnShowReplayBrowser()));
      connect(mShowLinksAction, SIGNAL(triggered()),
         this, SLOT(OnShowLinks()));
      connect(mHideLinksAction, SIGNAL(triggered()),
         this, SLOT(OnHideLinks()));
      connect(mRefreshAction, SIGNAL(triggered()),
         this, SLOT(OnRefresh()));
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
         mGraphBrowser->BuildGraphList(mDirector->GetGraphRoot());
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
   void DirectorEditor::SetReplayNode(Director::RecordNodeData* replayNode, InputLink* input, OutputLink* output)
   {
      if (!replayNode)
      {
         mReplayNode.input = "";
         mReplayNode.nodeID = "";
         mReplayNode.outputs.clear();
         mReplayNode.subThreads.clear();
      }
      else
      {
         mReplayNode = *replayNode;
      }

      mReplayInput = input;
      mReplayOutput = output;
   }

   //////////////////////////////////////////////////////////////////////////
   osg::Vec2 DirectorEditor::GetSnapPosition(osg::Vec2 position)
   {
      if (mSnapGridAction->isChecked())
      {
         position.x() = float(int(position.x() / 10) * 10);
         position.y() = float(int(position.y() / 10) * 10);
      }

      return position;
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

      // Refresh the graph list.
      mGraphBrowser->SelectGraph(mPropertyEditor->GetScene()->GetGraph());

      mReplayBrowser->BuildThreadList();
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::RefreshGraph(DirectorGraph* graph)
   {
      // Now refresh the all editors that view the same graph.
      int count = mGraphTabs->count();
      for (int index = 0; index < count; index++)
      {
         EditorView* view = dynamic_cast<EditorView*>(mGraphTabs->widget(index));
         if (view && view->GetScene())
         {
            if (view->GetScene()->GetGraph() == graph)
            {
               // First remember the position of the translation node.
               QPointF trans = view->GetScene()->GetTranslationItem()->pos();
               view->GetScene()->SetGraph(graph);
               view->GetScene()->GetTranslationItem()->setPos(trans);
            }
         }
      }

      // Now make sure we re-build our graph list.
      mGraphBrowser->BuildGraphList(graph);
      mReplayBrowser->BuildThreadList();
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
      bool bCanCopy = false;

      bool bCanShowLinks = false;
      bool bCanHideLinks = false;

      int tabIndex = mGraphTabs->currentIndex();
      if (tabIndex >= 0 && tabIndex < mGraphTabs->count())
      {
         EditorView* view = dynamic_cast<EditorView*>(mGraphTabs->widget(tabIndex));
         if (view && view->GetScene() && view->GetScene()->GetGraph())
         {
            bool bCanCreateSubMacro = false;

            EditorScene* scene = view->GetScene();

            if (scene->GetGraph()->mParent)
            {
               bHasParent = true;
            }

            if (scene->HasSelection())
            {
               bCanDelete = true;
               bCanCopy = true;
               bCanCreateSubMacro = true;
            }

            scene->GetMacroSelectionAction()->setEnabled(bCanCreateSubMacro);

            QList<QGraphicsItem*> selection = scene->selectedItems();
            int count = (int)selection.size();
            for (int index = 0; index < count; index++)
            {
               NodeItem* node = dynamic_cast<NodeItem*>(selection[index]);
               if (node)
               {
                  bool inputsExposed = true;
                  bool outputsExposed = true;
                  bool valuesExposed = true;
                  if (node->GetNode())
                  {
                     inputsExposed = node->GetNode()->InputsExposed();
                     outputsExposed = node->GetNode()->OutputsExposed();
                     valuesExposed = node->GetNode()->ValuesExposed();
                  }

                  if (inputsExposed)
                  {
                     int linkCount = node->GetInputs().size();
                     for (int linkIndex = 0; linkIndex < linkCount; linkIndex++)
                     {
                        InputLink* link = node->GetInputs()[linkIndex].link;
                        if (link)
                        {
                           if (link->GetVisible())
                           {
                              if (link->GetLinks().empty()) bCanHideLinks = true;
                           }
                           else
                           {
                              bCanShowLinks = true;
                           }
                        }
                     }
                  }

                  if (outputsExposed)
                  {
                     int linkCount = node->GetOutputs().size();
                     for (int linkIndex = 0; linkIndex < linkCount; linkIndex++)
                     {
                        OutputLink* link = node->GetOutputs()[linkIndex].link;
                        if (link)
                        {
                           if (link->GetVisible())
                           {
                              if (link->GetLinks().empty()) bCanHideLinks = true;
                           }
                           else
                           {
                              bCanShowLinks = true;
                           }
                        }
                     }
                  }

                  if (valuesExposed)
                  {
                     int linkCount = node->GetValues().size();
                     for (int linkIndex = 0; linkIndex < linkCount; linkIndex++)
                     {
                        ValueLink* link = node->GetValues()[linkIndex].link;
                        if (link)
                        {
                           if (link->GetVisible())
                           {
                              if (link->GetLinks().empty()) bCanHideLinks = true;
                           }
                           else
                           {
                              bCanShowLinks = true;
                           }
                        }
                     }
                  }
               }
            }
         }
      }

      // Parent button.
      mParentAction->setEnabled(bHasParent);

      // Undo button.
      mUndoAction->setEnabled(mUndoManager->CanUndo());

      // Redo button.
      mRedoAction->setEnabled(mUndoManager->CanRedo());

      // Copy and Cut buttons.
      mCutAction->setEnabled(bCanCopy);
      mCopyAction->setEnabled(bCanCopy);

      // Paste button.
      mPasteAction->setEnabled(Clipboard::GetInstance().CanPaste());

      // Delete button.
      mDeleteAction->setEnabled(bCanDelete);

      // Show Links
      mShowLinksAction->setEnabled(bCanShowLinks);
      mHideLinksAction->setEnabled(bCanHideLinks);
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::OnPropertyEditorVisibilityChange(bool visible)
   {
      mViewPropertiesAction->setChecked(visible);
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::OnGraphBrowserVisibilityChange(bool visible)
   {
      mViewGraphBrowserAction->setChecked(visible);
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::OnReplayBrowserVisibilityChange(bool visible)
   {
      mViewReplayBrowserAction->setChecked(visible);
      mReplayMode = visible;

      // Refresh the replay browser.
      mReplayBrowser->BuildThreadList();

      // Always refresh all the scenes.
      Refresh();
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
            mReplayBrowser->BuildThreadList();
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
      SaveScript();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DirectorEditor::OnSaveAsButton()
   {
      SaveScript(true);
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::OnLoadButton()
   {
      // Check if the undo manager has some un-committed changes first.
      if (mUndoManager->IsModified())
      {
         QMessageBox confirmationBox("Save Changes?",
            "Would you like to save your current Director Graph first?",
            QMessageBox::Question,
            QMessageBox::Yes,
            QMessageBox::No,
            QMessageBox::Cancel, this);

         switch (confirmationBox.exec())
         {
         case QMessageBox::Yes:
            if (!SaveScript()) return;
            break;
         case QMessageBox::Cancel:
            return;
         }
      }

      LoadScript();
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::OnNewButton()
   {
      // Check if the undo manager has some un-committed changes first.
      if (mUndoManager->IsModified())
      {
         QMessageBox confirmationBox("Save Changes?",
            "Would you like to save your current Director Graph first?",
            QMessageBox::Question,
            QMessageBox::Yes,
            QMessageBox::No,
            QMessageBox::Cancel, this);

         switch (confirmationBox.exec())
         {
         case QMessageBox::Yes:
            if (!SaveScript()) return;
            break;
         case QMessageBox::Cancel:
            return;
         }
      }

      ClearScript();
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::OnLoadRecordingButton()
   {
      QString filter = tr(".dtDirReplay");
      std::string context = dtDAL::Project::GetInstance().GetContext();

      QFileDialog dialog;
      QFileInfo filePath = dialog.getOpenFileName(this, tr("Load a Director Graph Replay File"),
         tr((context + "\\directors\\").c_str()),
         tr("Director Graph Replay (*.dtDirReplay)"), &filter);

      QString fileName = filePath.baseName();
      if (!fileName.isEmpty())
      {
         if (!mDirector->LoadRecording(fileName.toStdString()))
         {
            QMessageBox okBox("Failed",
               "The Replay file failed to load.  This could be because you are "
               "loading a replay for a script that is not loaded, or has been modified.",
               QMessageBox::Critical,
               QMessageBox::Ok,
               QMessageBox::NoButton,
               QMessageBox::NoButton, this);

            okBox.exec();
         }
         else
         {
            mReplayBrowser->show();
            
            // Make sure we refresh all the views for replay mode.
            Refresh();
         }
      }
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
               
               // Find the sub graph that we just zoomed out of, and center on it.
               QList<QGraphicsItem*> nodes = view->GetScene()->items();
               int count = nodes.count();
               for (int index = 0; index < count; index++)
               {
                  MacroItem* macro = dynamic_cast<MacroItem*>(nodes[index]);
                  if (macro && macro->GetGraph() == graph)
                  {
                     macro->setSelected(true);
                     view->GetScene()->CenterSelection();
                     break;
                  }
               }
            }
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::OnUndo()
   {
      mUndoManager->Undo();
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::OnRedo()
   {
      mUndoManager->Redo();
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::OnCut()
   {
      // First copy the contents.
      OnCopy();
      OnDelete();
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::OnCopy()
   {
      Clipboard& clipboard = Clipboard::GetInstance();
      clipboard.Clear();

      int index = mGraphTabs->currentIndex();
      if (index >= 0 && index < mGraphTabs->count())
      {
         EditorView* view = dynamic_cast<EditorView*>(mGraphTabs->widget(index));
         if (!view) return;

         EditorScene* scene = view->GetScene();
         if (!scene) return;

         std::vector<dtCore::RefPtr<dtDAL::PropertyContainer> >& selection = scene->GetSelection();
         int count = (int)selection.size();
         for (int index = 0; index < count; index++)
         {
            clipboard.AddObject(selection[index].get());
         }
      }

      RefreshButtonStates();
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::OnPaste()
   {
      PasteNodes();
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::OnDelete()
   {
      // Get the current selection.
      EditorScene* scene = mPropertyEditor->GetScene();
      if (!scene) return;

      bool graphsDeleted = false;

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
                  graphsDeleted = true;
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
                  // If the current graph or any of its parents are being deleted,
                  // change the current graph to the parent of that parent.
                  DirectorGraph* graph = view->GetScene()->GetGraph();
                  while (graph)
                  {
                     if (graph->GetID() == id)
                     {
                        view->GetScene()->SetGraph(graph->mParent);
                        break;
                     }

                     graph = graph->mParent;
                  }

                  // We need to find the node item that belongs to the scene.
                  NodeItem* nodeItem = view->GetScene()->GetNodeItem(id, true);
                  if (nodeItem) view->GetScene()->DeleteNode(nodeItem);
               }
            }
         }
      }

      mUndoManager->EndMultipleEvents();

      // Refresh the current view.
      Refresh();

      // If we deleted any graphs, then we must re-build the graph browser.
      mGraphBrowser->BuildGraphList(scene->GetGraph());

      mReplayBrowser->BuildThreadList();
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::OnManageLibraries()
   {
      // We need a director to manager libraries.
      if (!GetDirector())
      {
         QMessageBox::critical(NULL, tr("Failure"),
            tr("A Director must be open in order to manage libraries"),
            tr("OK"));
         return;
      }

      LibraryEditor libEdit(this);
      libEdit.exec();
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

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::OnShowGraphBrowser()
   {
      if (mViewGraphBrowserAction->isChecked())
      {
         mGraphBrowser->show();
      }
      else
      {
         mGraphBrowser->hide();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::OnShowReplayBrowser()
   {
      if (mViewReplayBrowserAction->isChecked())
      {
         mReplayBrowser->show();
      }
      else
      {
         mReplayBrowser->hide();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::OnShowLinks()
   {
      // Get the current selection.
      EditorScene* scene = mPropertyEditor->GetScene();
      if (!scene) return;

      QList<QGraphicsItem*> selection = scene->selectedItems();
      int count = (int)selection.size();
      for (int index = 0; index < count; index++)
      {
         NodeItem* node = dynamic_cast<NodeItem*>(selection[index]);
         if (node)
         {
            int linkCount = node->GetInputs().size();
            for (int linkIndex = 0; linkIndex < linkCount; linkIndex++)
            {
               InputLink* link = node->GetInputs()[linkIndex].link;
               if (link) link->SetVisible(true);
            }

            linkCount = node->GetOutputs().size();
            for (int linkIndex = 0; linkIndex < linkCount; linkIndex++)
            {
               OutputLink* link = node->GetOutputs()[linkIndex].link;
               if (link) link->SetVisible(true);
            }

            linkCount = node->GetValues().size();
            for (int linkIndex = 0; linkIndex < linkCount; linkIndex++)
            {
               ValueLink* link = node->GetValues()[linkIndex].link;
               if (link) link->SetVisible(true);
            }
         }
      }

      Refresh();
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::OnHideLinks()
   {
      // Get the current selection.
      EditorScene* scene = mPropertyEditor->GetScene();
      if (!scene) return;

      QList<QGraphicsItem*> selection = scene->selectedItems();
      int count = (int)selection.size();
      for (int index = 0; index < count; index++)
      {
         NodeItem* node = dynamic_cast<NodeItem*>(selection[index]);
         if (node)
         {
            int linkCount = node->GetInputs().size();
            for (int linkIndex = 0; linkIndex < linkCount; linkIndex++)
            {
               InputLink* link = node->GetInputs()[linkIndex].link;
               if (link && link->GetLinks().empty())
               {
                  link->SetVisible(false);
               }
            }

            linkCount = node->GetOutputs().size();
            for (int linkIndex = 0; linkIndex < linkCount; linkIndex++)
            {
               OutputLink* link = node->GetOutputs()[linkIndex].link;
               if (link && link->GetLinks().empty())
               {
                  link->SetVisible(false);
               }
            }

            linkCount = node->GetValues().size();
            for (int linkIndex = 0; linkIndex < linkCount; linkIndex++)
            {
               ValueLink* link = node->GetValues()[linkIndex].link;
               if (link && link->GetLinks().empty())
               {
                  link->SetVisible(false);
               }
            }
         }
      }

      Refresh();
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::OnRefresh()
   {
      Refresh();
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

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::showEvent(QShowEvent* event)
   {
      QMainWindow::showEvent(event);

      if (mDirector.valid() && mGraphTabs->count() == 0)
      {
         OpenGraph(mDirector->GetGraphRoot());
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::ClearScript()
   {
      // Clear the script.
      mGraphTabs->clear();
      mDirector->Clear();
      mUndoManager->Clear();

      // Create a single tab with the default graph.
      OpenGraph(mDirector->GetGraphRoot());
   }

   //////////////////////////////////////////////////////////////////////////
   bool DirectorEditor::SaveScript(bool saveAs)
   {
      bool showFiles = saveAs;

      QString fileName = mFileName.c_str();

      // We must show the file dialog if there is no Director loaded.
      if (!showFiles && fileName.isEmpty())
      {
         showFiles = true;
      }

      if (showFiles)
      {
         QString filter = tr(".dtDir");
         std::string context = dtDAL::Project::GetInstance().GetContext();

         QFileDialog dialog;
         QFileInfo filePath = dialog.getSaveFileName(this, tr("Save a Director Graph File"),
            tr((context + "\\directors\\").c_str()),
            tr("Director Scripts (*.dtDir)"), &filter);

         fileName = filePath.baseName();
      }

      if (!fileName.isEmpty())
      {
         mDirector->SaveScript(fileName.toStdString());
         mFileName = fileName.toStdString();

         mUndoManager->OnSaved();

         RefreshButtonStates();
         return true;
      }

      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   bool DirectorEditor::LoadScript()
   {
      QString filter = tr(".dtDir");
      std::string context = dtDAL::Project::GetInstance().GetContext();

      QFileDialog dialog;
      QFileInfo filePath = dialog.getOpenFileName(this, tr("Load a Director Graph File"),
         tr((context + "\\directors\\").c_str()),
         tr("Director Scripts (*.dtDir)"), &filter);

      QString fileName = filePath.baseName();
      if (!fileName.isEmpty())
      {
         // Clear the script.
         mDirector->Clear();
         mGraphTabs->clear();
         mUndoManager->Clear();

         mDirector->LoadScript(fileName.toStdString());
         mFileName = fileName.toStdString();

         // Create a single tab with the default graph.
         OpenGraph(mDirector->GetGraphRoot());
         mReplayBrowser->BuildThreadList();
         mGraphBrowser->BuildGraphList(mDirector->GetGraphRoot());
         return true;
      }

      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::PasteNodes(bool createLinks)
   {
      Clipboard& clipboard = Clipboard::GetInstance();

      int index = mGraphTabs->currentIndex();
      if (index >= 0 && index < mGraphTabs->count())
      {
         EditorView* view = dynamic_cast<EditorView*>(mGraphTabs->widget(index));
         if (!view) return;

         EditorScene* scene = view->GetScene();
         if (!scene) return;

         QPointF pos = view->mapToScene(view->width()/2, view->height()/2);
         pos -= scene->GetTranslationItem()->scenePos();

         std::vector<dtDAL::PropertyContainer*> newSelection;
         newSelection = clipboard.PasteObjects(scene->GetGraph(), mUndoManager, osg::Vec2(pos.x(), pos.y()), createLinks);

         scene->clearSelection();

         // Refresh the graph to create all the newly created node items.
         RefreshGraph(scene->GetGraph());

         // Now auto-select the newly created nodes.
         int count = (int)newSelection.size();
         for (index = 0; index < count; index++)
         {
            Node* node = dynamic_cast<Node*>(newSelection[index]);
            if (node)
            {
               NodeItem* item = scene->GetNodeItem(node->GetID(), true);
               if (item) item->setSelected(true);
            }
            else
            {
               DirectorGraph* graph = dynamic_cast<DirectorGraph*>(newSelection[index]);
               if (graph)
               {
                  MacroItem* item = scene->GetGraphItem(graph->GetID());
                  if (item) item->setSelected(true);
               }
            }
         }
      }
   }
} // namespace dtDirector

//////////////////////////////////////////////////////////////////////////
