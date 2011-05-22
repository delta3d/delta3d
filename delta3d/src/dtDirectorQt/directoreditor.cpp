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
#include <dtDirectorQt/clipboard.h>
#include <dtDirectorQt/customeditortool.h>
#include <dtDirectorQt/editorview.h>
#include <dtDirectorQt/editorscene.h>
#include <dtDirectorQt/graphbrowser.h>
#include <dtDirectorQt/graphtabs.h>
#include <dtDirectorQt/libraryeditor.h>
#include <dtDirectorQt/nodetabs.h>
#include <dtDirectorQt/replaybrowser.h>
#include <dtDirectorQt/undomanager.h>
#include <dtDirectorQt/undodeleteevent.h>
#include <dtDirectorQt/undocreateevent.h>

#include <dtDirectorQt/actionitem.h>
#include <dtDirectorQt/valueitem.h>
#include <dtDirectorQt/macroitem.h>

#include <dtQt/docbrowser.h>

#include <dtUtil/mathdefines.h>

#include <QtCore/QSettings>

#include <QtGui/QMenuBar>
#include <QtGui/QToolBar>
#include <QtGui/QAction>
#include <QtGui/QKeyEvent>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>

#include <dtDAL/project.h>
#include <osgDB/FileNameUtils>
#include <phonon/MediaObject> //for sounds

namespace dtDirector
{
   std::map<std::string, CustomEditorTool*> DirectorEditor::mCustomTools;
   std::vector<DirectorEditor*> DirectorEditor::mEditorsOpen;

   //////////////////////////////////////////////////////////////////////////////
   DirectorEditor::DirectorEditor(QWidget* parent)
      : QMainWindow(parent, Qt::Window)
      , mDocBrowser(NULL)
      , mUndoManager(NULL)
      , mDirector(NULL)
      , mReplayMode(false)
      , mReplayInput(NULL)
      , mReplayOutput(NULL)
      , mClickSound(NULL)
   {
      mEditorsOpen.push_back(this);

      mUI.setupUi(this);

      // Undo Manager.
      mUndoManager = new UndoManager(this);

      // Setup dock widgets
      mUI.graphTab->SetDirectorEditor(this);
      mUI.propertyEditor->SetDirectorEditor(this);
      mUI.graphBrowser->SetDirectorEditor(this);
      mUI.searchBrowser->SetDirectorEditor(this);
      mUI.searchBrowser->hide();
      mUI.replayBrowser->SetDirectorEditor(this);
      mUI.replayBrowser->hide();

      // Graph tabs.
      mUI.propertyEditor->SetGraphTabs(mUI.graphTab);

      mClickSound = Phonon::createPlayer(Phonon::MusicCategory,
                                         Phonon::MediaSource(":/sounds/click.wav"));
      connect(mClickSound, SIGNAL(stateChanged(Phonon::State, Phonon::State)), this, SLOT(OnStateChanged(Phonon::State, Phonon::State)));
   }

   ////////////////////////////////////////////////////////////////////////////////
   DirectorEditor::~DirectorEditor()
   {
      int editorCount = (int)mEditorsOpen.size();
      for (int editorIndex = 0; editorIndex < editorCount; ++editorIndex)
      {
         if (mEditorsOpen[editorIndex] == this)
         {
            mEditorsOpen.erase(mEditorsOpen.begin() + editorIndex);
            break;
         }
      }

      delete mUndoManager;
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::SetDirector(Director* director)
   {
      mDirector = director;

      // Setup node scenes
      CreateNodeScene(mUI.eventNodeTabWidget);
      CreateNodeScene(mUI.actionNodeTabWidget);
      CreateNodeScene(mUI.mutatorNodeTabWidget);
      CreateNodeScene(mUI.variableNodeTabWidget);
      CreateNodeScene(mUI.macroNodeTabWidget);
      CreateNodeScene(mUI.linkNodeTabWidget);
      CreateNodeScene(mUI.miscNodeTabWidget);
      RefreshNodeScenes();

      mUI.graphTab->clear();

      if (mDirector)
      {
         setWindowTitle(mDirector->GetName().c_str());
         mUI.graphBrowser->BuildGraphList(mDirector->GetGraphRoot());
      }
      else
      {
         setWindowTitle("No Director Script Loaded");
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool DirectorEditor::LoadScript(const std::string& fileName)
   {
      if (!fileName.empty())
      {
         // Clear the script.
         mDirector->Clear();
         mUI.graphTab->clear();
         mUndoManager->Clear();

         try
         {
            mDirector->LoadScript(fileName);

            std::string contextDir = osgDB::convertFileNameToNativeStyle(dtDAL::Project::GetInstance().GetContext()+"/");
            mFileName = dtUtil::FileUtils::GetInstance().RelativePath(contextDir, fileName);
         }
         catch (const dtUtil::Exception& e)
         {
            QString error = QString("Unable to parse ") + fileName.c_str() + " with error " + e.What().c_str();

            QMessageBox messageBox("Load Failed!",
               error, QMessageBox::Critical,
               QMessageBox::Ok,
               QMessageBox::NoButton, 
               QMessageBox::NoButton,
               this);

            messageBox.exec();
         }

         // Create a single tab with the default graph.
         OpenGraph(mDirector->GetGraphRoot());
         mUI.replayBrowser->BuildThreadList();
         mUI.graphBrowser->BuildGraphList(mDirector->GetGraphRoot());

         RefreshNodeScenes();
         return true;
      }

      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::OpenGraph(dtDirector::DirectorGraph* graph, bool newTab)
   {
      // Create a new page if we are forcing a new page or
      // if we don't have any pages yet.
      if (mUI.graphTab->count() < 1 || newTab)
      {
         EditorScene* scene = new EditorScene(mUI.propertyEditor, mUI.graphTab);
         EditorView* view = new EditorView(scene, this);
         scene->SetEditor(this);
         scene->SetView(view);

         int index = mUI.graphTab->addTab(view, "");
         mUI.graphTab->setCurrentIndex(index);
      }

      EditorView* view = dynamic_cast<EditorView*>(mUI.graphTab->currentWidget());
      if (view && graph)
      {
         if (view->GetScene()->GetGraph() != graph)
         {
            view->GetScene()->SetGraph(graph);
         }
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
   void DirectorEditor::Refresh()
   {
      if (!mDirector.valid()) {return;}

      // Refresh the button states.
      RefreshButtonStates();

      // Refresh the graph tabs with their graph names.
      int count = mUI.graphTab->count();
      for (int index = 0; index < count; index++)
      {
         EditorView* view = dynamic_cast<EditorView*>(mUI.graphTab->widget(index));
         if (view && view->GetScene())
         {
            DirectorGraph* graph = view->GetScene()->GetGraph();
            if (graph)
            {
               mUI.graphTab->setTabText(index, graph->GetName().c_str());
            }
         }
      }

      // Refresh the Scene.
      mUI.propertyEditor->GetScene()->Refresh();

      // Refresh the Properties.
      mUI.propertyEditor->GetScene()->RefreshProperties();

      // Refresh the graph list.
      mUI.graphBrowser->SelectGraph(mUI.propertyEditor->GetScene()->GetGraph());

      mUI.replayBrowser->BuildThreadList();
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::RefreshGraph(DirectorGraph* graph)
   {
      // Now refresh the all editors that view the same graph.
      int count = mUI.graphTab->count();
      for (int index = 0; index < count; index++)
      {
         EditorView* view = dynamic_cast<EditorView*>(mUI.graphTab->widget(index));
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
      mUI.graphBrowser->BuildGraphList(graph);
      mUI.replayBrowser->BuildThreadList();
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::RefreshNode(Node* node)
   {
      EditorView* view = dynamic_cast<EditorView*>(mUI.graphTab->currentWidget());
      if (view && view->GetScene())
      {
         NodeItem* item = view->GetScene()->GetNodeItem(node);
         if (item)
         {
            item->Draw();
            item->ConnectLinks(true);
         }
      }
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

      int tabIndex = mUI.graphTab->currentIndex();
      if (tabIndex >= 0 && tabIndex < mUI.graphTab->count())
      {
         EditorView* view = dynamic_cast<EditorView*>(mUI.graphTab->widget(tabIndex));
         if (view && view->GetScene() && view->GetScene()->GetGraph())
         {
            bool bCanCreateSubMacro = false;

            EditorScene* scene = view->GetScene();

            if (scene->GetGraph()->GetParent())
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
      mUI.action_Step_Out_Of_Graph->setEnabled(bHasParent);

      // Undo button.
      mUI.action_Undo->setEnabled(mUndoManager->CanUndo());

      // Redo button.
      mUI.action_Redo->setEnabled(mUndoManager->CanRedo());

      // Copy and Cut buttons.
      mUI.action_Cut->setEnabled(bCanCopy);
      mUI.action_Copy->setEnabled(bCanCopy);

      // Paste button.
      mUI.action_Paste->setEnabled(Clipboard::GetInstance().CanPaste());

      // Delete button.
      mUI.action_Delete->setEnabled(bCanDelete);

      // Show Links
      mUI.action_Show_Links->setEnabled(bCanShowLinks);
      mUI.action_Hide_Links->setEnabled(bCanHideLinks);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DirectorEditor::OnNodeCreated(Node* node)
   {
      if (node)
      {
         dtCore::RefPtr<UndoCreateEvent> event = new UndoCreateEvent(this, node->GetID(), node->GetGraph()->GetID());
         mUndoManager->AddEvent(event);

         // Now refresh the all editors that view the same graph.
         int count = mUI.graphTab->count();
         for (int index = 0; index < count; index++)
         {
            EditorView* view = dynamic_cast<EditorView*>(mUI.graphTab->widget(index));
            if (view && view->GetScene())
            {
               if (view->GetScene()->GetGraph() == node->GetGraph())
               {
                  // First remember the position of the translation node.
                  QPointF trans = view->GetScene()->GetTranslationItem()->pos();
                  view->GetScene()->SetGraph(node->GetGraph());
                  view->GetScene()->GetTranslationItem()->setPos(trans);
               }
            }
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DirectorEditor::DeleteNode(dtCore::UniqueId id)
   {
      Node* node = mDirector->GetNode(id);
      if (node)
      {
         // Create an undo event.
         dtCore::RefPtr<UndoDeleteEvent> event = new UndoDeleteEvent(this, id, node->GetGraph()->GetID());
         mUndoManager->AddEvent(event);

         // Delete the node.
         mDirector->DeleteNode(id);

         // Remove the node from all UI's
         int graphCount = mUI.graphTab->count();
         for (int graphIndex = 0; graphIndex < graphCount; graphIndex++)
         {
            EditorView* view = dynamic_cast<EditorView*>(mUI.graphTab->widget(graphIndex));
            if (view && view->GetScene())
            {
               // We need to find the node item that belongs to the scene.
               NodeItem* nodeItem = view->GetScene()->GetNodeItem(node);
               if (nodeItem) view->GetScene()->DeleteNode(nodeItem);
            }
         }
      }

      mUI.replayBrowser->BuildThreadList();
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool DirectorEditor::RegisterCustomEditorTool(CustomEditorTool* tool)
   {
      if (!tool) return false;

      // First make sure this tool is not already registered.
      std::map<std::string, CustomEditorTool*>::iterator i = mCustomTools.find(tool->GetName());
      if (i != mCustomTools.end())
      {
         return false;
      }

      mCustomTools.insert(std::make_pair<std::string, CustomEditorTool*>(tool->GetName(), tool));

      // Notify all open editors of this new tool.
      int editorCount = (int)mEditorsOpen.size();
      for (int editorIndex = 0; editorIndex < editorCount; ++editorIndex)
      {
         DirectorEditor* editor = mEditorsOpen[editorIndex];
         if (editor)
         {
            editor->AddCustomEditor(tool->GetName());
         }
      }

      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool DirectorEditor::UnRegisterCustomEditorTool(CustomEditorTool* tool)
   {
      if (!tool) return false;

      return UnRegisterCustomEditorTool(tool->GetName());
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool DirectorEditor::UnRegisterCustomEditorTool(const std::string& toolName)
   {
      // First make sure this tool is not already registered.
      std::map<std::string, CustomEditorTool*>::iterator i = mCustomTools.find(toolName);
      if (i == mCustomTools.end())
      {
         return false;
      }

      // Make sure we close this editor, if it is opened.
      CustomEditorTool* tool = i->second;
      if (tool && tool->IsOpen())
      {
         tool->Close();
      }

      mCustomTools.erase(i);

      // Notify all open editors of this removed tool.
      int editorCount = (int)mEditorsOpen.size();
      for (int editorIndex = 0; editorIndex < editorCount; ++editorIndex)
      {
         DirectorEditor* editor = mEditorsOpen[editorIndex];
         if (editor)
         {
            editor->RefreshNodeScenes();
         }
      }

      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   CustomEditorTool* DirectorEditor::GetRegisteredTool(const std::string& name)
   {
      std::map<std::string, CustomEditorTool*>::iterator i = mCustomTools.find(name);
      if (i != mCustomTools.end())
      {
         return i->second;
      }

      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::vector<std::string> DirectorEditor::GetRegisteredToolList(Director* director)
   {
      std::vector<std::string> toolList;

      std::map<std::string, CustomEditorTool*>::iterator i = mCustomTools.begin();
      while (i != mCustomTools.end())
      {
         if (!director || i->second->IsDirectorSupported(director))
         {
            toolList.push_back(i->first);
         }

         i++;
      }

      return toolList;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DirectorEditor::AddCustomEditor(const std::string& name)
   {
      mUI.macroNodeTabWidget->AddCustomEditor(name);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DirectorEditor::FocusNode(Node* node)
   {
      if (!node)
      {
         return;
      }

      OpenGraph(node->GetGraph());
      EditorScene* scene = GetPropertyEditor()->GetScene();
      if (scene)
      {
         scene->clearSelection();
         NodeItem* item = scene->GetNodeItem(node);
         if (item)
         {
            item->setSelected(true);
         }
         scene->CenterSelection();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DirectorEditor::FocusGraph(DirectorGraph* graph)
   {
      if (!graph)
      {
         return;
      }

      DirectorGraph* parent = graph->GetParent();

      // Can't focus on the root graph.
      if (!parent)
      {
         return;
      }

      OpenGraph(parent);
      EditorScene* scene = GetPropertyEditor()->GetScene();
      if (scene)
      {
         scene->clearSelection();
         MacroItem* item = scene->GetGraphItem(graph->GetID());
         if (item)
         {
            item->setSelected(true);
         }
         scene->CenterSelection();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::on_propertyEditor_visibilityChanged(bool visible)
   {
      mUI.action_Property_Editor->setChecked(visible);
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::on_graphBrowser_visibilityChanged(bool visible)
   {
      mUI.action_Graph_Browser->setChecked(visible);
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::on_searchBrowser_visibilityChanged(bool visible)
   {
      mUI.action_Search_Browser->setChecked(visible);
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::on_replayBrowser_visibilityChanged(bool visible)
   {
      mUI.action_Replay_Browser->setChecked(visible);
      mReplayMode = visible;

      // Refresh the replay browser.
      mUI.replayBrowser->BuildThreadList();

      // Always refresh all the scenes.
      Refresh();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DirectorEditor::on_graphTab_currentChanged(int index)
   {
      // Refresh the graph.
      if (index < mUI.graphTab->count())
      {
         EditorView* view = dynamic_cast<EditorView*>(mUI.graphTab->widget(index));
         if (view)
         {
            mUI.propertyEditor->SetScene(view->GetScene());
            view->GetScene()->Refresh();
            view->GetScene()->RefreshProperties();
            mUI.replayBrowser->BuildThreadList();
            RefreshButtonStates();
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DirectorEditor::on_graphTab_tabCloseRequested(int index)
   {
      // Remove the tab.
      if (index < mUI.graphTab->count())
      {
         EditorView* view = dynamic_cast<EditorView*>(mUI.graphTab->widget(index));
         if (view)
         {
            mUI.graphTab->removeTab(index);
            RefreshButtonStates();
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::on_action_Save_triggered()
   {
      SaveScript();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DirectorEditor::on_action_Save_as_triggered()
   {
      SaveScript(true);
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::on_action_Load_triggered()
   {
      // Check if the undo manager has some un-committed changes first.
      if (mUndoManager->IsModified())
      {
         QMessageBox confirmationBox("Save Changes?",
            "Would you like to save your current Director Script first?",
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

      if (LoadScript())
      {
         // Retrieve the last loaded script.
         QSettings settings("MOVES", "Director Editor");
         QStringList files = settings.value("recentFileList").toStringList();
         files.removeAll(mFileName.c_str());
         files.prepend(mFileName.c_str());

         while (files.size() > 5)
         {
            files.removeLast();
         }

         settings.setValue("recentFileList", files);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::on_action_New_triggered()
   {
      // Check if the undo manager has some un-committed changes first.
      if (mUndoManager->IsModified())
      {
         QMessageBox confirmationBox("Save Changes?",
            "Would you like to save your current Director Script first?",
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
   void DirectorEditor::on_action_Load_Recording_triggered()
   {
      QString filter = tr(".dtdirreplay");
      std::string contextDir = osgDB::convertFileNameToNativeStyle(dtDAL::Project::GetInstance().GetContext()+"/");
      std::string directorsDir = contextDir + osgDB::convertFileNameToNativeStyle("directors/");

      QFileDialog dialog;
      QFileInfo filePath = dialog.getOpenFileName(
         this, tr("Load a Director Script Replay File"), tr(directorsDir.c_str()), tr("Director Script Replays (*.dtdirreplay)"), &filter);

      if( !filePath.isFile() )
         return;

      std::string fileName  = osgDB::convertFileNameToNativeStyle(
         filePath.absolutePath().toStdString() + "/" + filePath.baseName().toStdString());
      //mFileName = dtUtil::FileUtils::GetInstance().RelativePath( contextDir, absFileName );





      //QString filter = tr(".dtdirreplay");
      //std::string context = dtDAL::Project::GetInstance().GetContext();

      //QFileDialog dialog;
      //QFileInfo filePath = dialog.getOpenFileName(this, tr("Load a Director Script Replay File"),
      //   tr((context + "\\directors\\").c_str()),
      //   tr("Director Script Replay (*.dtdirreplay)"), &filter);

      //QString fileName = filePath.baseName();




      if (!fileName.empty())
      {
         if (!mDirector->LoadRecording(fileName))
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
            mUI.replayBrowser->show();

            // Make sure we refresh all the views for replay mode.
            Refresh();
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DirectorEditor::on_action_Exit_triggered()
   {
      close();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DirectorEditor::on_action_Step_Out_Of_Graph_triggered()
   {
      int index = mUI.graphTab->currentIndex();
      if (index >= 0 && index < mUI.graphTab->count())
      {
         EditorView* view = dynamic_cast<EditorView*>(mUI.graphTab->widget(index));
         if (view)
         {
            dtDirector::DirectorGraph* graph = view->GetScene()->GetGraph();
            if (graph && graph->GetParent())
            {
               view->GetScene()->SetGraph(graph->GetParent());
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
   void DirectorEditor::on_action_Undo_triggered()
   {
      mUndoManager->Undo();
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::on_action_Redo_triggered()
   {
      mUndoManager->Redo();
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::on_action_Cut_triggered()
   {
      // First copy the contents.
      on_action_Copy_triggered();
      on_action_Delete_triggered();
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::on_action_Copy_triggered()
   {
      Clipboard& clipboard = Clipboard::GetInstance();
      clipboard.Clear();

      int index = mUI.graphTab->currentIndex();
      if (index >= 0 && index < mUI.graphTab->count())
      {
         EditorView* view = dynamic_cast<EditorView*>(mUI.graphTab->widget(index));
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

      // Make sure all open editors know there is something in the clipboard.
      int editorCount = (int)mEditorsOpen.size();
      for (int editorIndex = 0; editorIndex < editorCount; ++editorIndex)
      {
         DirectorEditor* editor = mEditorsOpen[editorIndex];
         if (editor)
         {
            editor->RefreshButtonStates();
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::on_action_Paste_triggered()
   {
      PasteNodes();
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::on_action_Delete_triggered()
   {
      // Get the current selection.
      EditorScene* scene = mUI.propertyEditor->GetScene();
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
                  parentID = macro->GetGraph()->GetParent()->GetID();
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
            int graphCount = mUI.graphTab->count();
            for (int graphIndex = 0; graphIndex < graphCount; graphIndex++)
            {
               EditorView* view = dynamic_cast<EditorView*>(mUI.graphTab->widget(graphIndex));
               if (view && view->GetScene())
               {
                  // If the current graph or any of its parents are being deleted,
                  // change the current graph to the parent of that parent.
                  DirectorGraph* graph = view->GetScene()->GetGraph();
                  while (graph)
                  {
                     if (graph->GetID() == id)
                     {
                        view->GetScene()->SetGraph(graph->GetParent());
                        break;
                     }

                     graph = graph->GetParent();
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
      mUI.graphBrowser->BuildGraphList(scene->GetGraph());

      mUI.replayBrowser->BuildThreadList();
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::on_action_Manage_Libraries_triggered()
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

      RefreshNodeScenes();
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::on_action_Property_Editor_triggered()
   {
      if (mUI.action_Property_Editor->isChecked())
      {
         mUI.propertyEditor->show();
      }
      else
      {
         mUI.propertyEditor->hide();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::on_action_Graph_Browser_triggered()
   {
      if (mUI.action_Graph_Browser->isChecked())
      {
         mUI.graphBrowser->show();
      }
      else
      {
         mUI.graphBrowser->hide();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::on_action_Search_Browser_triggered()
   {
      if (mUI.action_Search_Browser->isChecked())
      {
         mUI.searchBrowser->show();
         mUI.searchBrowser->FocusSearch();
      }
      else
      {
         mUI.searchBrowser->hide();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::on_action_Replay_Browser_triggered()
   {
      if (mUI.action_Replay_Browser->isChecked())
      {
         mUI.replayBrowser->show();
      }
      else
      {
         mUI.replayBrowser->hide();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::on_action_Show_Links_triggered()
   {
      // Get the current selection.
      EditorScene* scene = mUI.propertyEditor->GetScene();
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
   void DirectorEditor::on_action_Hide_Links_triggered()
   {
      // Get the current selection.
      EditorScene* scene = mUI.propertyEditor->GetScene();
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
   void DirectorEditor::on_action_Refresh_triggered()
   {
      Refresh();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DirectorEditor::on_actionDirector_Help_triggered()
   {
      if (!mDocBrowser)
      {
         mDocBrowser = new dtQt::DocBrowser(":director/contents.xml", this);
      }

      mDocBrowser->show();
      mDocBrowser->raise();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DirectorEditor::OnCreateNodeEvent(const QString& name, const QString& category)
   {
      EditorView* view = dynamic_cast<EditorView*>(mUI.graphTab->currentWidget());
      if (!view) return;

      EditorScene* scene = view->GetScene();
      if (!scene) return;

      QPointF pos = view->mapToScene(view->width()/2, view->height()/2);
      pos -= scene->GetTranslationItem()->scenePos();

      scene->CreateNodeItem(name.toStdString(), category.toStdString(), pos.x(), pos.y());

      // Refresh the graph to create all the newly created node items.
      RefreshGraph(scene->GetGraph());
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
         on_action_Undo_triggered();
      }

      // Show Search Browser.
      if (e->key() == Qt::Key_F && holdingControl)
      {
         if (mUI.searchBrowser->HasSearchFocus())
         {
            mUI.action_Search_Browser->toggle();
            on_action_Search_Browser_triggered();
         }
         else
         {
            mUI.action_Search_Browser->setChecked(true);
            mUI.searchBrowser->show();
            mUI.searchBrowser->FocusSearch();
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::showEvent(QShowEvent* event)
   {
      QMainWindow::showEvent(event);

      if (mDirector.valid() && mUI.graphTab->count() == 0)
      {
         // Retrieve the last loaded script.
         QSettings settings("MOVES", "Director Editor");
         //QStringList files = settings.value("recentFileList").toStringList();

         //if (!files.empty())
         //{
         //   QString lastScript = files.first();

         //   LoadScript(lastScript.toStdString());
         //}

         settings.beginGroup("MainWindow");
         resize(settings.value("Size", QSize(800, 600)).toSize());
         move(settings.value("Pos", QPoint(100, 100)).toPoint());

         // When restoring the window state, first see if the key exists.
         if (settings.contains("State"))
         {
            QByteArray state = settings.value("State").toByteArray();
            restoreState(state);
         }

         // When restoring the window state, first see if the key exists.
         if (settings.contains("Geom"))
         {
            QByteArray state = settings.value("Geom").toByteArray();
            restoreGeometry(state);
         }
         settings.endGroup();

         // TODO: Restore property and graph tree windows.

         OpenGraph(mDirector->GetGraphRoot());
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DirectorEditor::hideEvent(QHideEvent* event)
   {
      QMainWindow::hideEvent(event);

      QSettings settings("MOVES", "Director Editor");
      settings.beginGroup("MainWindow");
      settings.setValue("Pos", pos());
      settings.setValue("Size", size());
      settings.setValue("State", saveState());
      settings.setValue("Geom", saveGeometry());
      settings.endGroup();

      // TODO: Also save and close the property/graph tree windows.
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::closeEvent(QCloseEvent* event)
   {
      // Check if the undo manager has some un-committed changes first.
      if (mUndoManager->IsModified())
      {
         QMessageBox confirmationBox("Save Changes?",
            "Would you like to save your current Director Script first?",
            QMessageBox::Question,
            QMessageBox::Yes,
            QMessageBox::No,
            QMessageBox::Cancel, this);

         switch (confirmationBox.exec())
         {
         case QMessageBox::Yes:
            if (SaveScript())
            {
               event->accept();
               return;
            }
         case QMessageBox::Cancel:
            event->ignore();
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::ClearScript()
   {
      // Clear the script.
      mUI.graphTab->clear();
      mDirector->Clear();
      mUndoManager->Clear();
      mFileName.clear();

      // Create a single tab with the default graph.
      OpenGraph(mDirector->GetGraphRoot());
      mUI.replayBrowser->BuildThreadList();
      mUI.graphBrowser->BuildGraphList(mDirector->GetGraphRoot());
   }

   //////////////////////////////////////////////////////////////////////////
   bool DirectorEditor::SaveScript(bool saveAs)
   {
      bool showFiles = saveAs;

      // We must show the file dialog if there is no Director loaded.
      if (!showFiles && mFileName.empty())
      {
         showFiles = true;
      }

      if (showFiles)
      {
         QString filter = tr(".dtdir");
         std::string contextDir = osgDB::convertFileNameToNativeStyle(dtDAL::Project::GetInstance().GetContext()+"/");
         std::string directorsDir = contextDir + osgDB::convertFileNameToNativeStyle("directors/");

         QFileDialog dialog;
         QFileInfo filePath = dialog.getSaveFileName(
            this, tr("Save a Director Script File"), tr(directorsDir.c_str()), tr("Director Scripts (*.dtdir)"), &filter);

         if( filePath.fileName().isEmpty() )
            return false;

         std::string absFileName = osgDB::convertFileNameToNativeStyle(
            filePath.absolutePath().toStdString() + "/" + filePath.baseName().toStdString());
         mFileName = dtUtil::FileUtils::GetInstance().RelativePath(contextDir, absFileName);
      }

      if (!mFileName.empty())
      {
         mDirector->SaveScript(mFileName);

         mUndoManager->OnSaved();

         RefreshButtonStates();

         // Input the new file to the recent file list.
         QSettings settings("MOVES", "Director Editor");
         QStringList files = settings.value("recentFileList").toStringList();
         files.removeAll(mFileName.c_str());
         files.prepend(mFileName.c_str());

         while (files.size() > 5)
         {
            files.removeLast();
         }

         settings.setValue("recentFileList", files);

         return true;
      }

      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   bool DirectorEditor::LoadScript()
   {
      QString filter = tr(".dtdir");
      std::string contextDir = osgDB::convertFileNameToNativeStyle(dtDAL::Project::GetInstance().GetContext()+"/");
      std::string directorsDir = contextDir + osgDB::convertFileNameToNativeStyle("directors/");

      QFileDialog dialog;
      QFileInfo filePath = dialog.getOpenFileName(
         this, tr("Load a Director Script File"), tr(directorsDir.c_str()), tr("Director Scripts (*.dtdir)"), &filter);

      if(!filePath.isFile())
      {
         return false;
      }

      std::string absFileName  = osgDB::convertFileNameToNativeStyle(
         filePath.absolutePath().toStdString() + "/" + filePath.baseName().toStdString());

      return LoadScript(absFileName);
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::PasteNodes(bool createLinks)
   {
      Clipboard& clipboard = Clipboard::GetInstance();

      int index = mUI.graphTab->currentIndex();
      if (index >= 0 && index < mUI.graphTab->count())
      {
         EditorView* view = dynamic_cast<EditorView*>(mUI.graphTab->widget(index));
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

   ///////////////////////////////////////////////////////////////////////////////
   void DirectorEditor::CreateNodeScene(NodeTabs* nodeTabs)
   {
      nodeTabs->SetEditor(this);

      connect(nodeTabs, SIGNAL(CreateNode(const QString&, const QString&)),
         this, SLOT(OnCreateNodeEvent(const QString&, const QString&)));
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DirectorEditor::RefreshNodeScenes()
   {
      RefreshNodeScene(mUI.eventNodeTabWidget, NodeType::EVENT_NODE);
      RefreshNodeScene(mUI.actionNodeTabWidget, NodeType::ACTION_NODE);
      RefreshNodeScene(mUI.mutatorNodeTabWidget, NodeType::MUTATOR_NODE);
      RefreshNodeScene(mUI.variableNodeTabWidget, NodeType::VALUE_NODE);
      RefreshNodeScene(mUI.macroNodeTabWidget, NodeType::MACRO_NODE);
      RefreshNodeScene(mUI.linkNodeTabWidget, NodeType::LINK_NODE);
      RefreshNodeScene(mUI.miscNodeTabWidget, NodeType::MISC_NODE);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DirectorEditor::RefreshNodeScene(NodeTabs* nodeTabs, NodeType::NodeTypeEnum nodeType)
   {
      nodeTabs->RefreshNodes(nodeType);
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::OnPlayClickSound()
   {
      mClickSound->play();
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::OnStateChanged(Phonon::State newState, Phonon::State oldState)
   {
      //when the play is finished, the sound goes to paused.  Set it to "stop" to
      //get it ready for next play.
      if (mClickSound->state() == Phonon::PausedState)
      {
         mClickSound->stop();
      }
   }

} // namespace dtDirector

//////////////////////////////////////////////////////////////////////////
