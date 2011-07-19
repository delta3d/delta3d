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
#include <prefix/dtdirectorqtprefix.h>
#include <dtDirectorQt/directoreditor.h>
#include <dtDirectorQt/clipboard.h>
#include <dtDirectorQt/customeditortool.h>
#include <dtDirectorQt/editorview.h>
#include <dtDirectorQt/editorscene.h>
#include <dtDirectorQt/graphbrowser.h>
#include <dtDirectorQt/graphtabs.h>
#include <dtDirectorQt/libraryeditor.h>
#include <dtDirectorQt/nodetabs.h>
#include <dtDirectorQt/plugindialog.h>
#include <dtDirectorQt/pluginmanager.h>
#include <dtDirectorQt/undomanager.h>
#include <dtDirectorQt/undodeleteevent.h>
#include <dtDirectorQt/undocreateevent.h>
#include <dtDirectorQt/undopropertyevent.h>
#include <dtDirectorQt/undolinkvisibilityevent.h>
#include <dtDirectorQt/editornotifier.h>

#include <dtDirectorQt/actionitem.h>
#include <dtDirectorQt/valueitem.h>
#include <dtDirectorQt/macroitem.h>

#include <dtDirector/directortypefactory.h>

#include <dtQt/docbrowser.h>

#include <dtUtil/mathdefines.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/datapathutils.h>

#include <QtCore/QSettings>

#include <QtGui/QMenuBar>
#include <QtGui/QToolBar>
#include <QtGui/QAction>
#include <QtGui/QKeyEvent>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>

#include <dtDAL/project.h>
#include <dtDAL/datatype.h>
#include <dtDAL/actorproperty.h>
#include <dtDAL/resourceactorproperty.h>

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
      , mPluginManager(NULL)
      , mUndoManager(NULL)
      , mDirector(NULL)
      , mSavedTabIndex(-1)
      , mClickSound(NULL)
   {
      mEditorsOpen.push_back(this);

      mUI.setupUi(this);

      // Plugin Manager.
      mPluginManager = new PluginManager(this);

      // Undo Manager.
      mUndoManager = new UndoManager(this);

      // Setup dock widgets
      mUI.graphTab->SetDirectorEditor(this);
      mUI.propertyEditor->SetDirectorEditor(this);
      mUI.graphBrowser->SetDirectorEditor(this);
      mUI.searchBrowser->SetDirectorEditor(this);
      mUI.searchBrowser->hide();
      mUI.threadBrowser->SetDirectorEditor(this);
      mUI.threadBrowser->hide();

      // Graph tabs.
      mUI.propertyEditor->SetGraphTabs(mUI.graphTab);

      CreateNodeScene(mUI.eventNodeTabWidget);
      CreateNodeScene(mUI.actionNodeTabWidget);
      CreateNodeScene(mUI.mutatorNodeTabWidget);
      CreateNodeScene(mUI.variableNodeTabWidget);
      CreateNodeScene(mUI.macroNodeTabWidget);
      CreateNodeScene(mUI.linkNodeTabWidget);
      CreateNodeScene(mUI.miscNodeTabWidget);
      CreateNodeScene(mUI.searchNodeTabWidget);
      CreateNodeScene(mUI.referenceNodeTabWidget);

      SetupPlugins();

      mClickSound = Phonon::createPlayer(Phonon::MusicCategory,
                                         Phonon::MediaSource(":/sounds/click.wav"));
      connect(mClickSound, SIGNAL(stateChanged(Phonon::State, Phonon::State)), this, SLOT(OnStateChanged(Phonon::State, Phonon::State)));

      connect(mUI.menuRecent_Files, SIGNAL(triggered(QAction*)), this, SLOT(OnRecentFile(QAction*)));

      RefreshRecentFiles();
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
   void DirectorEditor::SetupPlugins()
   {
      std::string pluginPath;
      if (dtUtil::IsEnvironment("DIRECTOR_PLUGIN_PATH"))
      {
         pluginPath = dtUtil::GetEnvironment("DIRECTOR_PLUGIN_PATH");;
      }

      if (pluginPath.empty())
      {
         pluginPath = QCoreApplication::applicationDirPath().toStdString() + "/directorplugins";
      }

#ifdef DELTA_WIN32
#ifdef _DEBUG
      pluginPath += "\\Debug";
#endif
#endif

      if (!dtUtil::FileUtils::GetInstance().DirExists(pluginPath))
      {
         //no plugin path found...lets not try to load any plugins
         LOG_INFO("No plugin path was found. No plugins will be loaded.");
         return;
      }

      LOG_INFO("Trying to load plugins from directory " + pluginPath);

      // instantiate all plugin factories and immediately start system plugins
      mPluginManager->LoadPluginsInDir(pluginPath);

      // start plugins that were set in config file
      mPluginManager->StartPluginsInConfigFile();
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::SetDirector(Director* director)
   {
      mDirector = director;

      // Setup node scenes
      mUI.eventNodeTabWidget->SetEditor(this);
      mUI.actionNodeTabWidget->SetEditor(this);
      mUI.mutatorNodeTabWidget->SetEditor(this);
      mUI.variableNodeTabWidget->SetEditor(this);
      mUI.macroNodeTabWidget->SetEditor(this);
      mUI.linkNodeTabWidget->SetEditor(this);
      mUI.miscNodeTabWidget->SetEditor(this);
      mUI.referenceNodeTabWidget->SetEditor(this);
      mUI.searchNodeTabWidget->SetEditor(this);
      RefreshNodeScenes();

      mUI.graphTab->clear();

      if (mDirector.valid())
      {
         std::string fileName = mDirector->GetScriptName();
         std::string contextDir = osgDB::convertFileNameToNativeStyle(dtDAL::Project::GetInstance().GetContext()+"/directors/");
         contextDir = osgDB::getRealPath(contextDir);
         if (!fileName.empty())
         {
            fileName = osgDB::getRealPath(fileName);
         }
         mFileName = dtUtil::FileUtils::GetInstance().RelativePath(contextDir, fileName);

         mUI.graphBrowser->BuildGraphList(mDirector->GetGraphRoot());

         // If we have a valid notifier, make sure all value nodes
         // expose their initial value property.
         EditorNotifier* notifier = GetNotifier();
         if (notifier)
         {
            std::vector<Node*> nodes;
            mDirector->GetAllNodes(nodes);
            int count = (int)nodes.size();
            for (int index = 0; index < count; ++index)
            {
               ValueNode* valueNode = nodes[index]->AsValueNode();
               if (valueNode)
               {
                  valueNode->ExposeInitialValue();
               }
            }
         }
      }
      else
      {
         setWindowTitle("No Director Script Loaded");
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool DirectorEditor::IsDebugging() const
   {
      if (GetNotifier())
      {
         return true;
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   EditorNotifier* DirectorEditor::GetNotifier() const
   {
      if (GetDirector())
      {
         return dynamic_cast<EditorNotifier*>(GetDirector()->GetNotifier());
      }

      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool DirectorEditor::LoadScript(const std::string& fileName)
   {
      if (!fileName.empty())
      {
         // Clear the script.
         mDirector->Clear();
         mUI.graphTab->clear();
         GetUndoManager()->Clear();

         mFileName.clear();

         // Remove this entry from the recent file listing.
         QSettings settings("MOVES", "Director Editor");
         QStringList files = settings.value("recentFileList").toStringList();
         files.removeAll(fileName.c_str());
         settings.setValue("recentFileList", files);

         try
         {
            std::string contextDir = osgDB::convertFileNameToNativeStyle(dtDAL::Project::GetInstance().GetContext()+"/directors/");
            contextDir = osgDB::getRealPath(contextDir);
            mFileName = dtUtil::FileUtils::GetInstance().RelativePath(contextDir, fileName);

            DirectorTypeFactory* factory = DirectorTypeFactory::GetInstance();
            if (factory)
            {
               // Determine if we need to change our Director object.
               if (mDirector->GetResource() == dtDAL::ResourceDescriptor::NULL_RESOURCE)
               {
                  mDirector = factory->LoadScript(mFileName, mDirector->GetGameManager(), mDirector->GetMap());
               }
               else
               {
                  factory->LoadScript(mDirector, mFileName);
               }
            }

            // Display a warning message if there were libraries that could not be loaded.
            const std::vector<std::string>& missingLibraries = mDirector->GetMissingLibraries();
            if (!missingLibraries.empty())
            {
               QString warning = "The following Node Libraries could not be included:\n";

               int count = (int)missingLibraries.size();
               for (int index = 0; index < count; ++index)
               {
                  QString libraryName = missingLibraries[index].c_str();
                  warning += "\t" + libraryName + "\n";
               }

               warning += "\nThis could happen either because the libraries were not found ";
               warning += "in any of this applications valid search paths or the binary files ";
               warning += "for these libraries were out of date and could not be linked ";
               warning += "properly with this application.\n\n";
               warning += "Saving this script will cause these libraries to be removed.";

               QMessageBox messageBox("Libraries were not loaded!",
                  warning, QMessageBox::Warning,
                  QMessageBox::Ok,
                  QMessageBox::NoButton,
                  QMessageBox::NoButton,
                  this);

               messageBox.exec();
            }

            // Display a warning message if there were nodes that could not be loaded.
            const std::set<std::string>& missingNodes = mDirector->GetMissingNodeTypes();
            if (!missingNodes.empty())
            {
               QString warning = "The following node types could not be created:\n";

               std::set<std::string>::const_iterator iter;
               for (iter = missingNodes.begin(); iter != missingNodes.end(); ++iter)
               {
                  QString nodeName = (*iter).c_str();
                  warning += "\t" + nodeName + "\n";
               }

               warning += "\nThis could happen either because the nodes have been removed ";
               warning += "from their libraries or the libraries that contained them could ";
               warning += "not be loaded themselves.\n\n";
               warning += "Saving this script will cause these nodes to be removed.";

               QMessageBox messageBox("Nodes were not loaded!",
                  warning, QMessageBox::Warning,
                  QMessageBox::Ok,
                  QMessageBox::NoButton,
                  QMessageBox::NoButton,
                  this);

               messageBox.exec();
            }

            // Update the recent file listing to have the
            // currently loaded item inserted to the front.
            files.prepend(mFileName.c_str());
            while (files.size() > 5)
            {
               files.removeLast();
            }

            settings.setValue("recentFileList", files);
            RefreshRecentFiles();

            // Create a single tab with the default graph.
            OpenGraph(mDirector->GetGraphRoot());
            mUI.graphBrowser->BuildGraphList(mDirector->GetGraphRoot());

            RefreshNodeScenes();
            return true;
         }
         catch (const dtUtil::Exception& e)
         {
            RefreshRecentFiles();

            QString error = QString("Unable to parse ") + fileName.c_str() + " with error " + e.What().c_str();

            QMessageBox messageBox("Load Failed!",
               error, QMessageBox::Critical,
               QMessageBox::Ok,
               QMessageBox::NoButton,
               QMessageBox::NoButton,
               this);

            messageBox.exec();
         }
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DirectorEditor::SaveTabStates()
   {
      mTabStates.clear();
      int count = mUI.graphTab->count();
      for (int index = 0; index < count; ++index)
      {
         EditorView* view = dynamic_cast<EditorView*>(mUI.graphTab->widget(index));
         if (view && view->GetScene() && view->GetScene()->GetGraph())
         {
            TabStateData data;
            data.id = view->GetScene()->GetGraph()->GetID();
            data.pos = view->GetScene()->GetTranslationItem()->pos();
            mTabStates.push_back(data);
         }
      }

      mSavedTabIndex = mUI.graphTab->currentIndex();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DirectorEditor::RestoreTabStates()
   {
      mUI.graphTab->clear();

      int count = (int)mTabStates.size();
      for (int index = 0; index < count; ++index)
      {
         TabStateData& data = mTabStates[index];

         // Search for the proper graph.
         dtDirector::DirectorGraph* graph = mDirector->GetGraph(data.id);
         if (graph)
         {
            OpenGraph(graph, true);
            EditorView* view = dynamic_cast<EditorView*>(mUI.graphTab->currentWidget());
            if (view && view->GetScene())
            {
               view->GetScene()->GetTranslationItem()->setPos(data.pos);
            }
         }
      }

      if (mSavedTabIndex > -1 && mSavedTabIndex < count)
      {
         mUI.graphTab->setCurrentIndex(mSavedTabIndex);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DirectorEditor::RefreshNewMenu()
   {
      // First determine if we have more than one script type to choose
      // between.
      std::vector<std::string> scriptTypes;
      DirectorTypeFactory* factory = DirectorTypeFactory::GetInstance();
      if (factory)
      {
         factory->GetScriptTypes(scriptTypes);
      }

      mUI.action_New->setMenu(NULL);

      // If we have more than one script type to choose from, then
      // prompt the user for the proper type to use.
      if (scriptTypes.size() > 1)
      {
         QMenu* menu = new QMenu();
         connect(menu, SIGNAL(triggered(QAction*)), this, SLOT(OnNewScriptTypeTriggered(QAction*)));
         
         int count = (int)scriptTypes.size();
         for (int index = 0; index < count; ++index)
         {
            menu->addAction(scriptTypes[index].c_str());
         }

         mUI.action_New->setMenu(menu);
      }
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

   ////////////////////////////////////////////////////////////////////////////////
   void DirectorEditor::RefreshRecentFiles()
   {
      QSettings settings("MOVES", "Director Editor");
      QStringList files = settings.value("recentFileList").toStringList();

      mUI.menuRecent_Files->clear();

      int count = files.count();
      for (int index = 0; index < count; ++index)
      {
         QString fileName = files[index];
         mUI.menuRecent_Files->addAction(fileName);
      }
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
            else
            {
               mUI.graphTab->removeTab(index);
               index--;
               count--;
            }
         }
      }

      if (!mUI.graphTab->count() && mDirector.valid())
      {
         OpenGraph(mDirector->GetGraphRoot(), true);
      }

      // Refresh the Scene.
      mUI.propertyEditor->GetScene()->Refresh();

      // Refresh the Properties.
      mUI.propertyEditor->GetScene()->RefreshProperties();

      // Refresh the graph list.
      mUI.graphBrowser->SelectGraph(mUI.propertyEditor->GetScene()->GetGraph());
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
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::RefreshNode(Node* node)
   {
      EditorView* view = dynamic_cast<EditorView*>(mUI.graphTab->currentWidget());
      if (view && view->GetScene())
      {
         NodeItem* item = view->GetScene()->GetNodeItem(node, true);
         if (item)
         {
            item->Draw();
            item->ConnectLinks(true);
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::RefreshGlow(Node* node)
   {
      EditorView* view = dynamic_cast<EditorView*>(mUI.graphTab->currentWidget());
      if (view && view->GetScene())
      {
         NodeItem* item = view->GetScene()->GetNodeItem(node);
         if (item)
         {
            item->DrawGlow();
         }
      }
   }
   
   ////////////////////////////////////////////////////////////////////////////////
   void DirectorEditor::RefreshGraphBrowser()
   {
      EditorView* view = dynamic_cast<EditorView*>(mUI.graphTab->currentWidget());
      if (view && view->GetScene())
      {
         mUI.graphBrowser->BuildGraphList(view->GetScene()->GetGraph());
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::RefreshButtonStates()
   {
      // Refresh the name of the window.
      QString title;
      if (mFileName.empty())
      {
         title = "Untitled";
      }
      else
      {
         title = mFileName.c_str();
      }

      if (GetUndoManager()->IsModified()) title += "*";

      if (GetDirector())
      {
         title += " (";
         title += GetDirector()->GetScriptType().c_str();
         title += ")";
      }

      setWindowTitle(title);

      bool bHasParent = false;
      bool bCanDelete = false;
      bool bCanCopy = false;

      bool bCanShowLinks = false;
      bool bCanHideLinks = false;

      bool bCanToggleBreakPoint = false;
      bool bBreakPointChecked = true;

      EditorNotifier* notifier = GetNotifier();

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
                  if (notifier && node->GetNode())
                  {
                     bCanToggleBreakPoint = true;

                     EditorNotifier::GlowData* glowData = notifier->GetGlowData(node->GetNode());

                     if (!glowData || !glowData->hasBreakPoint)
                     {
                        bBreakPointChecked = false;
                     }
                  }

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

      // Save button.
      mUI.action_Save->setEnabled(GetUndoManager()->IsModified());
      mUI.action_Save_as->setEnabled(true);

      // Parent button.
      mUI.action_Step_Out_Of_Graph->setEnabled(bHasParent);

      // Undo button.
      mUI.action_Undo->setEnabled(GetUndoManager()->CanUndo());
      std::string undoDescription = GetUndoManager()->GetUndoDescription();
      if (!undoDescription.empty())
      {
         mUI.action_Undo->setToolTip(QString("Reverts to your last action (Ctrl+Z).\n") + undoDescription.c_str());
      }
      else
      {
         mUI.action_Undo->setToolTip("Reverts to your last action (Ctrl+Z).");
      }

      // Redo button.
      mUI.action_Redo->setEnabled(GetUndoManager()->CanRedo());
      std::string redoDescription = GetUndoManager()->GetRedoDescription();
      if (!redoDescription.empty())
      {
         mUI.action_Redo->setToolTip(QString("Reverts your last undo action (Ctrl+Y).\n") + redoDescription.c_str());
      }
      else
      {
         mUI.action_Redo->setToolTip("Reverts your last undo action (Ctrl+Y).");
      }

      // Copy and Cut buttons.
      mUI.action_Cut->setEnabled(bCanCopy);
      mUI.action_Copy->setEnabled(bCanCopy);

      // Paste button.
      mUI.action_Paste->setEnabled(Clipboard::GetInstance().CanPaste());
      mUI.actionPaste_with_Links->setEnabled(Clipboard::GetInstance().CanPaste());

      // Delete button.
      mUI.action_Delete->setEnabled(bCanDelete);

      // Show Links
      mUI.action_Show_Links->setEnabled(bCanShowLinks);
      mUI.action_Hide_Links->setEnabled(bCanHideLinks);

      // Debugging
      if (notifier)
      {
         mUI.action_New->setEnabled(false);
         mUI.action_Load->setEnabled(false);
         mUI.menuRecent_Files->setEnabled(false);

         // If we have just entered debug mode, make sure
         // we show the thread browser.
         if (mDirector->IsDebugging() &&
            mUI.actionPause->isEnabled())
         {
            mUI.threadBrowser->show();
         }

         mUI.menuDebug->setEnabled(true);
         mUI.actionPause->setEnabled(!mDirector->IsDebugging());
         mUI.actionContinue->setEnabled(mDirector->IsDebugging());
         mUI.actionStep_Next->setEnabled(mDirector->IsDebugging());
         mUI.actionToggle_Break_Point->setEnabled(bCanToggleBreakPoint);
         mUI.actionToggle_Break_Point->setChecked(bBreakPointChecked);
      }
      else
      {
         if (mDirector->GetResource() != dtDAL::ResourceDescriptor::NULL_RESOURCE)
         {
            mUI.action_New->setEnabled(false);
            mUI.action_Load->setEnabled(false);
            mUI.menuRecent_Files->setEnabled(false);
         }

         mUI.menuDebug->setVisible(false);
         mUI.actionPause->setVisible(false);
         mUI.actionContinue->setVisible(false);
         mUI.actionStep_Next->setVisible(false);
         mUI.actionToggle_Break_Point->setVisible(false);
      }

      RefreshNewMenu();
      RefreshReferenceScene();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DirectorEditor::OnNodeCreated(Node* node)
   {
      if (node)
      {
         dtCore::RefPtr<UndoCreateEvent> event = new UndoCreateEvent(this, node->GetID(), node->GetGraph()->GetID());
         event->SetDescription("Creation of Node \'" + node->GetTypeName() + "\'");
         GetUndoManager()->AddEvent(event);

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
         event->SetDescription("Deletion of Node \'" + node->GetTypeName() + "\'");
         GetUndoManager()->AddEvent(event);

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

      // First find out if the node exists in the current script.
      Node* testNode = mDirector->GetNode(node->GetID());
      if (testNode == node)
      {
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
   void DirectorEditor::OnBeginDebug()
   {
      mUI.action_Thread_Browser->setEnabled(true);

      mUI.threadBrowser->show();
      mUI.threadBrowser->BuildThreadList();
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::OnEndDebug()
   {
      mUI.action_Thread_Browser->setEnabled(false);

      mUI.threadBrowser->hide();
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::OnStepDebug()
   {
      mUI.threadBrowser->BuildThreadList();
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
   void DirectorEditor::on_threadBrowser_visibilityChanged(bool visible)
   {
      mUI.action_Thread_Browser->setChecked(visible);
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::on_nodePalette_visibilityChanged(bool visible)
   {
      mUI.action_Node_Palette->setChecked(visible);
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
      if (GetUndoManager()->IsModified())
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

      LoadScript();
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::on_action_New_triggered()
   {
      OnNewScriptTypeTriggered(NULL);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DirectorEditor::OnNewScriptTypeTriggered(QAction* action)
   {
      std::string scriptType = "";

      if (action)
      {
         scriptType = action->text().toStdString();
      }

      // Check if the undo manager has some un-committed changes first.
      if (GetUndoManager()->IsModified())
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

      ClearScript(scriptType);
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
                  if (macro && macro->GetMacro() == graph)
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
      GetUndoManager()->Undo();
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::on_action_Redo_triggered()
   {
      GetUndoManager()->Redo();
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::on_action_Cut_triggered()
   {
      // Get the current selection.
      EditorScene* scene = NULL;
      EditorView* view = dynamic_cast<EditorView*>(mUI.graphTab->currentWidget());
      if (view)
      {
         scene = view->GetScene();
      }
      if (!scene) return;

      QList<QGraphicsItem*> selection = scene->selectedItems();
      int count = (int)selection.size();

      std::string undoDescription = "Cut operation of Node.";
      if (count == 1)
      {
         NodeItem* item = dynamic_cast<NodeItem*>(selection[0]);
         if (item && item->GetNode())
         {
            undoDescription = "Cut operation of Node \'" +
               item->GetNode()->GetTypeName() + "\'.";
         }
         else if (item && item->GetMacro())
         {
            if (item->GetMacro()->GetEditor().empty())
            {
               undoDescription = "Cut operation of Macro Node \'" +
                  item->GetMacro()->GetName() + "\'.";
            }
            else
            {
               undoDescription = "Cut operation of \'" +
                  item->GetMacro()->GetEditor() + "\' Macro Node \'" +
                  item->GetMacro()->GetName() + "\'.";
            }
         }
      }
      else
      {
         undoDescription = "Cut operation of multiple Nodes.";
      }
      GetUndoManager()->BeginMultipleEvents(undoDescription);

      // First copy the contents.
      on_action_Copy_triggered();
      on_action_Delete_triggered();

      GetUndoManager()->EndMultipleEvents();
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

   ////////////////////////////////////////////////////////////////////////////////
   void DirectorEditor::on_actionPaste_with_Links_triggered()
   {
      PasteNodes(false, true);
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::on_action_Delete_triggered()
   {
      bool graphsDeleted = false;

      // Get the current selection.
      EditorScene* scene = NULL;
      EditorView* view = dynamic_cast<EditorView*>(mUI.graphTab->currentWidget());
      if (view)
      {
         scene = view->GetScene();
      }
      if (!scene) return;

      QList<QGraphicsItem*> selection = scene->selectedItems();
      int count = (int)selection.size();

      std::string undoDescription = "Deletion of Node.";
      if (count == 1)
      {
         NodeItem* item = dynamic_cast<NodeItem*>(selection[0]);
         if (item && item->GetNode())
         {
            undoDescription = "Deletion of Node \'" +
               item->GetNode()->GetTypeName() + "\'.";
         }
         else if (item && item->GetMacro())
         {
            if (item->GetMacro()->GetEditor().empty())
            {
               undoDescription = "Deletion of Macro Node \'" +
                  item->GetMacro()->GetName() + "\'.";
            }
            else
            {
               undoDescription = "Deletion of \'" +
                  item->GetMacro()->GetEditor() + "\' Macro Node \'" +
                  item->GetMacro()->GetName() + "\'.";
            }
         }
      }
      else
      {
         undoDescription = "Deletion of multiple Nodes.";
      }
      GetUndoManager()->BeginMultipleEvents(undoDescription);

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
               if (item && item->GetMacro())
               {
                  id = item->GetMacro()->GetID();
                  parentID = item->GetMacro()->GetParent()->GetID();
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
            GetUndoManager()->AddEvent(event);

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

      GetUndoManager()->EndMultipleEvents();

      // Refresh the current view.
      Refresh();

      // If we deleted any graphs, then we must re-build the graph browser.
      mUI.graphBrowser->BuildGraphList(scene->GetGraph());
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::on_action_Manage_Plugins_triggered()
   {
      PluginDialog dlg(this);
      dlg.OnOpenDialog();
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::on_action_Manage_Libraries_triggered()
   {
      // We need a director to manage libraries.
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
   void DirectorEditor::on_action_Node_Palette_triggered()
   {
      if (mUI.action_Node_Palette->isChecked())
      {
         mUI.nodePalette->show();
      }
      else
      {
         mUI.nodePalette->hide();
      }
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

   ////////////////////////////////////////////////////////////////////////////////
   void DirectorEditor::on_action_Thread_Browser_triggered()
   {
      if (mUI.action_Thread_Browser->isChecked())
      {
         mUI.threadBrowser->show();
      }
      else
      {
         mUI.threadBrowser->hide();
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

      std::string undoDescription = "Show all links for ";
      if (count == 1)
      {
         NodeItem* item = dynamic_cast<NodeItem*>(selection[0]);
         if (item && item->GetNode())
         {
            undoDescription += "Node \'" + item->GetNode()->GetTypeName() + "\'.";
         }
         else if (item && item->GetMacro())
         {
            undoDescription += "Macro Node \'" + item->GetMacro()->GetName() + "\'.";
         }
      }
      else
      {
         undoDescription += "multiple Nodes.";
      }

      GetUndoManager()->BeginMultipleEvents(undoDescription);

      for (int index = 0; index < count; index++)
      {
         NodeItem* node = dynamic_cast<NodeItem*>(selection[index]);
         if (node)
         {
            int linkCount = node->GetInputs().size();
            for (int linkIndex = 0; linkIndex < linkCount; linkIndex++)
            {
               InputLink* link = node->GetInputs()[linkIndex].link;
               if (link && !link->GetVisible())
               {
                  link->SetVisible(true);

                  dtCore::RefPtr<UndoLinkVisibilityEvent> event = new UndoLinkVisibilityEvent(this, node->GetID(), 0, link->GetName(), true);
                  GetUndoManager()->AddEvent(event);
               }
            }

            linkCount = node->GetOutputs().size();
            for (int linkIndex = 0; linkIndex < linkCount; linkIndex++)
            {
               OutputLink* link = node->GetOutputs()[linkIndex].link;
               if (link && !link->GetVisible())
               {
                  link->SetVisible(true);

                  dtCore::RefPtr<UndoLinkVisibilityEvent> event = new UndoLinkVisibilityEvent(this, node->GetID(), 1, link->GetName(), true);
                  GetUndoManager()->AddEvent(event);
               }
            }

            linkCount = node->GetValues().size();
            for (int linkIndex = 0; linkIndex < linkCount; linkIndex++)
            {
               ValueLink* link = node->GetValues()[linkIndex].link;
               if (link && !link->GetVisible())
               {
                  link->SetVisible(true);

                  dtCore::RefPtr<UndoLinkVisibilityEvent> event = new UndoLinkVisibilityEvent(this, node->GetID(), 2, link->GetName(), true);
                  GetUndoManager()->AddEvent(event);
               }
            }
         }
      }

      GetUndoManager()->EndMultipleEvents();

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

      std::string undoDescription = "Hide all links for ";
      if (count == 1)
      {
         NodeItem* item = dynamic_cast<NodeItem*>(selection[0]);
         if (item && item->GetNode())
         {
            undoDescription += "Node \'" + item->GetNode()->GetTypeName() + "\'.";
         }
         else if (item && item->GetMacro())
         {
            undoDescription += "Macro Node \'" + item->GetMacro()->GetName() + "\'.";
         }
      }
      else
      {
         undoDescription += "multiple Nodes.";
      }

      GetUndoManager()->BeginMultipleEvents(undoDescription);

      for (int index = 0; index < count; index++)
      {
         NodeItem* node = dynamic_cast<NodeItem*>(selection[index]);
         if (node)
         {
            int linkCount = node->GetInputs().size();
            for (int linkIndex = 0; linkIndex < linkCount; linkIndex++)
            {
               InputLink* link = node->GetInputs()[linkIndex].link;
               if (link && link->GetVisible() && link->GetLinks().empty())
               {
                  link->SetVisible(false);

                  dtCore::RefPtr<UndoLinkVisibilityEvent> event = new UndoLinkVisibilityEvent(this, node->GetID(), 0, link->GetName(), false);
                  GetUndoManager()->AddEvent(event);
               }
            }

            linkCount = node->GetOutputs().size();
            for (int linkIndex = 0; linkIndex < linkCount; linkIndex++)
            {
               OutputLink* link = node->GetOutputs()[linkIndex].link;
               if (link && link->GetVisible() && link->GetLinks().empty())
               {
                  link->SetVisible(false);

                  dtCore::RefPtr<UndoLinkVisibilityEvent> event = new UndoLinkVisibilityEvent(this, node->GetID(), 1, link->GetName(), false);
                  GetUndoManager()->AddEvent(event);
               }
            }

            linkCount = node->GetValues().size();
            for (int linkIndex = 0; linkIndex < linkCount; linkIndex++)
            {
               ValueLink* link = node->GetValues()[linkIndex].link;
               if (link && link->GetVisible() && link->GetLinks().empty())
               {
                  link->SetVisible(false);

                  dtCore::RefPtr<UndoLinkVisibilityEvent> event = new UndoLinkVisibilityEvent(this, node->GetID(), 2, link->GetName(), false);
                  GetUndoManager()->AddEvent(event);
               }
            }
         }
      }

      GetUndoManager()->EndMultipleEvents();

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

   ////////////////////////////////////////////////////////////////////////////////
   void DirectorEditor::on_actionPause_triggered()
   {
      mDirector->ToggleDebugEnabled(true);
      mDirector->StepDebugger();
      RefreshButtonStates();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DirectorEditor::on_actionContinue_triggered()
   {
      mDirector->ToggleDebugEnabled(false);
      RefreshButtonStates();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DirectorEditor::on_actionStep_Next_triggered()
   {
      mDirector->StepDebugger();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DirectorEditor::on_actionToggle_Break_Point_triggered()
   {
      EditorNotifier* notifier = GetNotifier();

      if (!notifier)
      {
         return;
      }

      bool toggle = mUI.actionToggle_Break_Point->isChecked();

      int tabIndex = mUI.graphTab->currentIndex();
      if (tabIndex >= 0 && tabIndex < mUI.graphTab->count())
      {
         EditorView* view = dynamic_cast<EditorView*>(mUI.graphTab->widget(tabIndex));
         if (view && view->GetScene() && view->GetScene()->GetGraph())
         {
            EditorScene* scene = view->GetScene();

            QList<QGraphicsItem*> selection = scene->selectedItems();
            int count = (int)selection.size();
            for (int index = 0; index < count; index++)
            {
               NodeItem* node = dynamic_cast<NodeItem*>(selection[index]);
               if (node && node->GetNode())
               {
                  EditorNotifier::GlowData* glowData =
                     notifier->GetGlowData(node->GetNode());

                  if ((toggle && (!glowData || !glowData->hasBreakPoint)) ||
                     (!toggle && glowData && glowData->hasBreakPoint))
                  {
                     node->OnToggleBreakPoint();
                  }
               }
            }
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DirectorEditor::on_nodeSearchEdit_editingFinished()
   {
      QString text = mUI.nodeSearchEdit->text();

      mUI.nodeTabs->setCurrentWidget(mUI.searchNodeTab);

      EditorView* view = dynamic_cast<EditorView*>(mUI.graphTab->currentWidget());
      DirectorGraph* graph = NULL;
      if (view && view->GetScene())
      {
         graph = view->GetScene()->GetGraph();
      }
      mUI.searchNodeTabWidget->SearchNodes(text, graph);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DirectorEditor::OnRecentFile(QAction* action)
   {
      if (action)
      {
         // Check if the undo manager has some un-committed changes first.
         if (GetUndoManager()->IsModified())
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

         QString fileName = action->text();
         LoadScript(fileName.toStdString());
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DirectorEditor::OnCreateNodeEvent(const QString& name, const QString& category, const QString& refName)
   {
      EditorView* view = dynamic_cast<EditorView*>(mUI.graphTab->currentWidget());
      if (!view) return;

      EditorScene* scene = view->GetScene();
      if (!scene) return;

      QPointF pos = view->mapToScene(view->width()/2, view->height()/2);
      pos -= scene->GetTranslationItem()->scenePos();

      GetUndoManager()->BeginMultipleEvents("Creation of Node \'" + name.toStdString() + "\'.");
      Node* item = scene->CreateNodeItem(name.toStdString(), category.toStdString(), pos.x(), pos.y());
      if (!refName.isEmpty() && item)
      {
         item->SetString(refName.toStdString(), "Reference");
         dtCore::RefPtr<UndoPropertyEvent> event =
            new UndoPropertyEvent(this, scene->GetGraph()->GetID(), "Reference", "", refName.toStdString());
         GetUndoManager()->AddEvent(event.get());
         RefreshNode(item);
      }
      GetUndoManager()->EndMultipleEvents();

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

      // Retrieve the last loaded script.
      QSettings settings("MOVES", "Director Editor");

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

      settings.beginGroup("PaletteWindow");
      mUI.nodePalette->resize(settings.value("Size", QSize(219, 371)).toSize());
      mUI.nodePalette->move(settings.value("Pos", QPoint(0, 55)).toPoint());
      settings.endGroup();

      settings.beginGroup("PropertyWindow");
      mUI.propertyEditor->resize(settings.value("Size", QSize(400, 150)).toSize());
      mUI.propertyEditor->move(settings.value("Pos", QPoint(0, 430)).toPoint());
      settings.endGroup();

      settings.beginGroup("GraphWindow");
      mUI.graphBrowser->resize(settings.value("Size", QSize(396, 150)).toSize());
      mUI.graphBrowser->move(settings.value("Pos", QPoint(404, 430)).toPoint());
      settings.endGroup();

      settings.beginGroup("SearchWindow");
      mUI.searchBrowser->resize(settings.value("Size", QSize(195, 121)).toSize());
      mUI.searchBrowser->move(settings.value("Pos", QPoint(605, 180)).toPoint());
      settings.endGroup();

      settings.beginGroup("ThreadWindow");
      mUI.threadBrowser->resize(settings.value("Size", QSize(195, 121)).toSize());
      mUI.threadBrowser->move(settings.value("Pos", QPoint(605, 305)).toPoint());
      settings.endGroup();

      mUI.threadBrowser->hide();

      if (mDirector.valid() && mUI.graphTab->count() == 0)
      {
         OpenGraph(mDirector->GetGraphRoot());
      }

      RefreshNewMenu();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DirectorEditor::hideEvent(QHideEvent* event)
   {
      QMainWindow::hideEvent(event);
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::closeEvent(QCloseEvent* event)
   {
      // Save window settings.
      QSettings settings("MOVES", "Director Editor");
      settings.beginGroup("MainWindow");
      settings.setValue("Pos", pos());
      settings.setValue("Size", size());
      settings.setValue("State", saveState());
      settings.setValue("Geom", saveGeometry());
      settings.endGroup();

      settings.beginGroup("PaletteWindow");
      settings.setValue("Pos", mUI.nodePalette->pos());
      settings.setValue("Size", mUI.nodePalette->size());
      settings.endGroup();

      settings.beginGroup("PropertyWindow");
      settings.setValue("Pos", mUI.propertyEditor->pos());
      settings.setValue("Size", mUI.propertyEditor->size());
      settings.endGroup();

      settings.beginGroup("GraphWindow");
      settings.setValue("Pos", mUI.graphBrowser->pos());
      settings.setValue("Size", mUI.graphBrowser->size());
      settings.endGroup();

      settings.beginGroup("SearchWindow");
      settings.setValue("Pos", mUI.searchBrowser->pos());
      settings.setValue("Size", mUI.searchBrowser->size());
      settings.endGroup();

      settings.beginGroup("ThreadWindow");
      settings.setValue("Pos", mUI.threadBrowser->pos());
      settings.setValue("Size", mUI.threadBrowser->size());
      settings.endGroup();

      //Save the Plugin state
      mPluginManager->StoreActivePluginsToConfigFile();

      // Check if the undo manager has some un-committed changes first.
      if (GetUndoManager()->IsModified())
      {
         QMessageBox confirmationBox("Save Changes?",
            "Would you like to save your current Director Script first?",
            QMessageBox::Question,
            QMessageBox::Yes,
            QMessageBox::No,
            QMessageBox::Cancel, this);

         switch (confirmationBox.exec())
         {
         case QMessageBox::Cancel:
            event->ignore();
            return;
         case QMessageBox::Yes:
            if (SaveScript())
            {
               event->accept();
            }
         }
      }

      // If we get down to here, it means we are closing the editor.
      if (mDirector)
      {
         EditorNotifier* notifier = GetNotifier();
         if (notifier)
         {
            // If we have some unsaved changes left, make sure we undo
            // everything first.
            GetUndoManager()->Revert();

            notifier->RemoveEditor(this);

            // If no editors are currently viewing this script, we can
            // remove the notifier.
            if (notifier->GetEditors().empty())
            {
               mDirector->SetNotifier(NULL);
            }
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::ClearScript(const std::string& scriptType)
   {
      // Clear the script.
      mUI.graphTab->clear();
      mDirector->Clear();
      GetUndoManager()->Clear();
      mFileName.clear();

      if (!scriptType.empty() && scriptType != mDirector->GetScriptType())
      {
         dtGame::GameManager* gm = mDirector->GetGameManager();
         dtDAL::Map* map = mDirector->GetMap();

         DirectorTypeFactory* factory = DirectorTypeFactory::GetInstance();
         if (factory)
         {
            SetDirector(factory->CreateDirector(scriptType));
         }

         if (!mDirector.valid())
         {
            mDirector = new Director();
         }

         mDirector->Init(gm, map);
         RefreshNodeScenes();
      }

      // Create a single tab with the default graph.
      OpenGraph(mDirector->GetGraphRoot());
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

      std::string contextDir = osgDB::convertFileNameToNativeStyle(dtDAL::Project::GetInstance().GetContext()+"/directors/");
      contextDir = osgDB::getRealPath(contextDir);

      std::string fileName = mFileName;

      if (showFiles)
      {
         QString filter = tr(".dtdir");

         QFileDialog dialog;
         QFileInfo filePath = dialog.getSaveFileName(
            this, tr("Save a Director Script File"), tr(contextDir.c_str()), tr("XML Director Scripts (*.dtdir);;Binary Director Scripts (*.dtdirb)"), &filter);

         if (filePath.fileName().isEmpty())
         {
            return false;
         }

         fileName = osgDB::convertFileNameToNativeStyle(
            filePath.absolutePath().toStdString() + "/" + filePath.fileName().toStdString());
      }

      fileName = dtUtil::FileUtils::GetInstance().RelativePath(contextDir, fileName);

      if (!fileName.empty())
      {
         // Remove this file from the recent file listing.
         QSettings settings("MOVES", "Director Editor");
         QStringList files = settings.value("recentFileList").toStringList();
         files.removeAll(fileName.c_str());

         mDirector->SaveScript(fileName);

         // We only set the current edited file and save the undo manager
         // if we are not debugging.  If we are debugging, we should remain
         // referenced on the original script.
         if (!IsDebugging() || mFileName == fileName)
         {
            mFileName = fileName;

            GetUndoManager()->OnSaved();
         }

         RefreshButtonStates();

         // Input the new file to the recent file list.
         files.prepend(fileName.c_str());
         while (files.size() > 5)
         {
            files.removeLast();
         }

         settings.setValue("recentFileList", files);
         RefreshRecentFiles();

         dtDAL::ResourceDescriptor resource = mDirector->GetResource();

         // If we are saving this script as a new file while debugging
         // in game, then find out the resource type of the script
         // that was saved and only refresh scripts using that resource.
         if (mFileName != fileName)
         {
            std::string id = resource.GetResourceIdentifier();

            size_t pos = 0;
            while ((pos = fileName.find("\\", pos)) != std::string::npos)
            {
               fileName.replace(pos, 1, ":");
               pos++;
            }

            resource = dtDAL::ResourceDescriptor(std::string("Directors:") + fileName);
         }

         // If we are dealing with a resourced script, make sure we go through
         // every other script that is referencing this resource and reload
         // them.
         if (mDirector->GetResource() != dtDAL::ResourceDescriptor::NULL_RESOURCE)
         {
            // First find all editors that are editing this particular
            // script resource and clear their tabs.
            int editorCount = (int)mEditorsOpen.size();
            for (int editorIndex = 0; editorIndex < editorCount; ++editorIndex)
            {
               DirectorEditor* editor = mEditorsOpen[editorIndex];
               if (editor && editor->GetDirector())
               {
                  if (editor->GetDirector()->GetResource() == resource)
                  {
                     editor->SaveTabStates();
                  }
               }
            }

            std::map<Director*, Director::StateData> stateMap;

            DirectorTypeFactory* factory = DirectorTypeFactory::GetInstance();
            if (factory)
            {
               const std::vector<Director*>& scriptList = factory->GetScriptInstances();
               for (int index = 0; index < (int)scriptList.size(); ++index)
               {
                  Director* director = scriptList[index];
                  if (director)
                  {
                     std::vector<Node*> nodes;
                     director->GetAllNodes(nodes);

                     int nodeCount = (int)nodes.size();
                     for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
                     {
                        Node* node = nodes[nodeIndex];
                        if (!node)
                        {
                           continue;
                        }

                        std::vector<dtDAL::ActorProperty*> propList;
                        node->GetPropertyList(propList);
                        int propCount = (int)propList.size();
                        for (int propIndex = 0; propIndex < propCount; ++propIndex)
                        {
                           dtDAL::ActorProperty* prop = propList[propIndex];
                           if (!prop)
                           {
                              continue;
                           }

                           if (prop->GetDataType() == dtDAL::DataType::DIRECTOR)
                           {
                              dtDAL::ResourceActorProperty* resourceProp =
                                 dynamic_cast<dtDAL::ResourceActorProperty*>(prop);
                              if (!resourceProp)
                              {
                                 continue;
                              }

                              if (resourceProp->GetValue() == resource)
                              {
                                 // Make sure we save the state of the top most script.
                                 Director* parent = director;
                                 while (parent->GetParent())
                                 {
                                    parent = parent->GetParent();
                                 }

                                 if (stateMap.find(parent) == stateMap.end())
                                 {
                                    stateMap[parent] = parent->GetState();
                                 }

                                 resourceProp->SetValue(resource);
                              }
                           }
                        }
                     }
                  }
               }
            }

            // Restore all our states.
            std::map<Director*, Director::StateData>::iterator iter;
            for (iter = stateMap.begin(); iter != stateMap.end(); ++iter)
            {
               if (iter->first)
               {
                  iter->first->RestoreState(iter->second);
               }
            }

            editorCount = (int)mEditorsOpen.size();
            for (int editorIndex = 0; editorIndex < editorCount; ++editorIndex)
            {
               DirectorEditor* editor = mEditorsOpen[editorIndex];
               if (editor && editor->GetDirector())
               {
                  if (editor->GetDirector()->GetResource() == resource)
                  {
                     editor->RestoreTabStates();
                  }
               }
            }
         }

         return true;
      }

      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   bool DirectorEditor::LoadScript()
   {
      std::string contextDir = osgDB::convertFileNameToNativeStyle(dtDAL::Project::GetInstance().GetContext()+"/directors/");
      contextDir = osgDB::getRealPath(contextDir);

      QFileDialog dialog;
      QFileInfo filePath = dialog.getOpenFileName(
         this, tr("Load a Director Script File"), tr(contextDir.c_str()), tr("Director Scripts (*.dtdir *.dtdirb)"));

      if(!filePath.isFile())
      {
         return false;
      }

      std::string fileName  = osgDB::convertFileNameToNativeStyle(
         filePath.absolutePath().toStdString() + "/" + filePath.fileName().toStdString());
      fileName = dtUtil::FileUtils::GetInstance().RelativePath(contextDir, fileName);

      return LoadScript(fileName);
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorEditor::PasteNodes(bool createLinks, bool externalLinks)
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
         newSelection = clipboard.PasteObjects(scene->GetGraph(), GetUndoManager(), osg::Vec2(pos.x(), pos.y()), createLinks, externalLinks);

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

      connect(nodeTabs, SIGNAL(CreateNode(const QString&, const QString&, const QString&)),
         this, SLOT(OnCreateNodeEvent(const QString&, const QString&, const QString&)));
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

      EditorView* view = dynamic_cast<EditorView*>(mUI.graphTab->currentWidget());
      DirectorGraph* graph = NULL;
      if (view && view->GetScene())
      {
         graph = view->GetScene()->GetGraph();
      }
      mUI.searchNodeTabWidget->SearchNodes(mUI.nodeSearchEdit->text(), graph);

      RefreshReferenceScene();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DirectorEditor::RefreshNodeScene(NodeTabs* nodeTabs, NodeType::NodeTypeEnum nodeType)
   {
      nodeTabs->RefreshNodes(nodeType);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DirectorEditor::RefreshReferenceScene()
   {
      EditorView* view = dynamic_cast<EditorView*>(mUI.graphTab->currentWidget());
      if (view && view->GetScene())
      {
         mUI.referenceNodeTabWidget->SearchReferenceNodes(view->GetScene()->GetGraph());
      }
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
