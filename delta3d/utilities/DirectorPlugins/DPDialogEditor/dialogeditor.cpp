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

#include <dialogeditor.h>

#include <dialogtreeitems.h>
#include <dialoglinetype.h>
#include <dialogrefline.h>
#include <dialoglineregistry.h>

#include <QtGui/QWidget>
#include <QtGui/QLayout>
#include <QtGui/QMessageBox>

#include <QtCore/QSettings>


const std::string DirectorDialogEditorPlugin::PLUGIN_NAME = "Dialog Editor";

////////////////////////////////////////////////////////////////////////////////
DirectorDialogEditorPlugin::DirectorDialogEditorPlugin()
   : dtDirector::CustomEditorTool("Dialog")
   , mRoot(NULL)
   , mEndDialog(NULL)
   , mEditWidget(NULL)
{
   mUI.setupUi(this);
   
   setWindowTitle("Dialog Editor");

   connect(mUI.mDialogTree, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
      this, SLOT(OnCurrentTreeItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));

   // Save and close buttons
   connect(mUI.mSaveButton, SIGNAL(clicked()), this, SLOT(OnSave()));
   connect(mUI.mSaveAndCloseButton, SIGNAL(clicked()), this, SLOT(OnSaveAndClose()));
   connect(mUI.mCancelButton, SIGNAL(clicked()), this, SLOT(OnCancel()));
}

////////////////////////////////////////////////////////////////////////////////
DirectorDialogEditorPlugin::~DirectorDialogEditorPlugin()
{
}

////////////////////////////////////////////////////////////////////////////////
void DirectorDialogEditorPlugin::Initialize()
{
   // Create our root node.
   mRoot = new DialogRootItem("Root", DialogLineRegistry::GetInstance().GetStartedLineType(), true);
   mEndDialog = new DialogRootItem("End Dialog", DialogLineRegistry::GetInstance().GetEndedLineType(), false);

   mUI.mDialogTree->addTopLevelItem(mRoot);
   mUI.mDialogTree->addTopLevelItem(mEndDialog);

   QSettings settings("MOVES", "Director Dialog Editor");

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

   settings.beginGroup("Dialog Tree");
   mUI.mDialogTree->resize(settings.value("Size", QSize(195, 121)).toSize());
   mUI.mDialogTree->move(settings.value("Pos", QPoint(605, 180)).toPoint());
   settings.endGroup();

   settings.beginGroup("Property Editor");
   mUI.mPropertyScrollArea->resize(settings.value("Size", QSize(195, 121)).toSize());
   mUI.mPropertyScrollArea->move(settings.value("Pos", QPoint(605, 180)).toPoint());
   settings.endGroup();
}

////////////////////////////////////////////////////////////////////////////////
void DirectorDialogEditorPlugin::Open(dtDirector::DirectorEditor* editor, dtDirector::DirectorGraph* graph)
{
   CustomEditorTool::Open(editor, graph);

   Initialize();

   OnLoad();
   show();
}

////////////////////////////////////////////////////////////////////////////////
void DirectorDialogEditorPlugin::Close()
{
   CustomEditorTool::Close();

   QSettings settings("MOVES", "Director Dialog Editor");
   settings.beginGroup("MainWindow");
   settings.setValue("Pos", pos());
   settings.setValue("Size", size());
   settings.setValue("State", saveState());
   settings.setValue("Geom", saveGeometry());
   settings.endGroup();

   settings.beginGroup("Dialog Tree");
   settings.setValue("Pos", mUI.mDialogTree->pos());
   settings.setValue("Size", mUI.mDialogTree->size());
   settings.endGroup();

   settings.beginGroup("Property Editor");
   settings.setValue("Pos", mUI.mPropertyScrollArea->pos());
   settings.setValue("Size", mUI.mPropertyScrollArea->size());
   settings.endGroup();

   if (mRoot)
   {
      delete mRoot;
      mRoot = NULL;
   }

   if (mEndDialog)
   {
      delete mEndDialog;
      mEndDialog = NULL;
   }

   if (mEditWidget)
   {
      delete mEditWidget;
      mEditWidget = NULL;
   }

   mUI.mDialogTree->Reset();

   hide();
}

////////////////////////////////////////////////////////////////////////////////
void DirectorDialogEditorPlugin::Destroy()
{
   Close();
}

////////////////////////////////////////////////////////////////////////////////
void DirectorDialogEditorPlugin::closeEvent(QCloseEvent* event)
{
   Close();
}

////////////////////////////////////////////////////////////////////////////////
DialogTreeWidget* DirectorDialogEditorPlugin::GetTree() const
{
   return mUI.mDialogTree;
}

////////////////////////////////////////////////////////////////////////////////
void DirectorDialogEditorPlugin::MapReference(const QString& refName, const dtCore::UniqueId& id)
{
   std::map<QString, dtCore::UniqueId>::iterator iter = mRefMap.find(refName);

   if (iter == mRefMap.end())
   {
      mRefMap[refName] = id;
   }
}

////////////////////////////////////////////////////////////////////////////////
void DirectorDialogEditorPlugin::RegisterReference(DialogRefLineType* refLine, const QString& refName)
{
   std::map<QString, std::vector<DialogRefLineType*> >::iterator iter = mRefRegister.find(refName);
   
   if (iter == mRefRegister.end())
   {
      std::vector<DialogRefLineType*> refLines;
      refLines.push_back(refLine);
      mRefRegister[refName] = refLines;
   }
   else
   {
      iter->second.push_back(refLine);
   }
}

////////////////////////////////////////////////////////////////////////////////
void DirectorDialogEditorPlugin::OnCurrentTreeItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
   QLayout* layout = mUI.mLinePropertyWidget->layout();
   if (layout)
   {
      // Remove any current widgets provided by the previous item.
      if (mEditWidget)
      {
         layout->removeWidget(mEditWidget);
         delete mEditWidget;
         mEditWidget = NULL;
      }

      DialogLineItem* line = dynamic_cast<DialogLineItem*>(current);
      if (line)
      {
         DialogLineType* type = line->GetType();
         if (type)
         {
            mEditWidget = type->CreatePropertyEditor(mUI.mDialogTree);
            if (mEditWidget)
            {
               layout->addWidget(mEditWidget);
            }
         }
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void DirectorDialogEditorPlugin::OnLoad()
{
   if (!GetGraph()) return;

   std::vector<dtDirector::Node*> nodes;
   GetGraph()->GetNodes("Remote Event", "Core", "EventName", "Started", nodes);

   // Find the Started remote event, if it exists.
   dtDirector::Node* startEventNode = NULL;
   if (nodes.size())
   {
      startEventNode = nodes[0];
   }

   if (startEventNode)
   {
      dtDirector::Node* lineNode = NULL;
      std::vector<dtDirector::InputLink*> links;
      if (GetNext(startEventNode, "Out", links))
      {
         lineNode = links[0]->GetOwner();
      }

      if (!lineNode)
      {
         return;
      }

      mRefMap.clear();
      mRefRegister.clear();

      if (mRoot->GetType())
      {
         if (mRoot->GetType()->ShouldOperateOn(lineNode))
         {
            mRoot->GetType()->OperateOn(mRoot, lineNode, this);
         }
      }
      else
      {
         const DialogLineType* type = DialogLineRegistry::GetInstance().GetLineTypeForNode(lineNode);
         if (type)
         {
            DialogLineItem* newLine = new DialogLineItem(type->GetName(), type, GetTree()->CreateIndex(), GetTree());
            mRoot->addChild(newLine);
            mRoot->setExpanded(true);

            newLine->GetType()->OperateOn(newLine, lineNode, this);
         }
         else
         {
            QString error = QString("Failed to find line type for node \'") + lineNode->GetTypeName().c_str() + "\'.";

            QMessageBox messageBox("Load Failed!",
               error, QMessageBox::Critical,
               QMessageBox::Ok,
               QMessageBox::NoButton,
               QMessageBox::NoButton,
               this);

            messageBox.exec();
         }
      }
   }

   // Now go through all our registered reference lines and connect them to
   // their property references.
   std::map<QString, std::vector<DialogRefLineType*> >::iterator iter;
   for (iter = mRefRegister.begin(); iter != mRefRegister.end(); ++iter)
   {
      QString refName = iter->first;
      std::map<QString, dtCore::UniqueId>::iterator mapIter = mRefMap.find(refName);
      if (mapIter != mRefMap.end())
      {
         dtCore::UniqueId& id = mapIter->second;

         std::vector<DialogRefLineType*>& refLines = iter->second;

         int count = (int)refLines.size();
         for (int index = 0; index < count; ++index)
         {
            DialogRefLineType* refLine = refLines[index];
            if (refLine)
            {
               refLine->SetReference(id);
            }
         }
      }
   }

   GetTree()->UpdateLabels();
}

////////////////////////////////////////////////////////////////////////////////
void DirectorDialogEditorPlugin::OnSave()
{
   BeginSave();

   dtDirector::Node* newInputNode = CreateNode("Input Link", "Core", NULL, 80);

   dtDirector::Node* newCallStartEventNode = CreateNode("Call Remote Event", "Core", newInputNode);
   newCallStartEventNode->SetString("Started", "EventName");
   Connect(newInputNode, newCallStartEventNode, "Out", "Call Event");

   dtDirector::Node* newCallEndEventNode = CreateNode("Call Remote Event", "Core", newCallStartEventNode);
   newCallEndEventNode->SetString("Ended", "EventName");
   Connect(newCallStartEventNode, newCallEndEventNode, "Event Finished", "Call Event");

   dtDirector::Node* newOutputNode = CreateNode("Output Link", "Core", newCallEndEventNode, 80);
   newOutputNode->SetString("Finished", "Name");
   Connect(newCallEndEventNode, newOutputNode, "Event Finished", "In");

   dtDirector::Node* newStartedEventNode = CreateNode("Remote Event", "Core", NULL, 80);
   newStartedEventNode->SetString("Started", "EventName");

   if (mRoot->GetType())
   {
      mRoot->GetType()->GenerateNode(mRoot, newStartedEventNode, "Out", this);
   }
   else if (mRoot->childCount() > 0)
   {
      DialogLineItem* line = dynamic_cast<DialogLineItem*>(mRoot->child(0));
      if (line)
      {
         line->GetType()->GenerateNode(line, newStartedEventNode, "Out", this);
      }
   }

   dtDirector::Node* newEndedEventNode = CreateNode("Remote Event", "Core", NULL, 80);
   newEndedEventNode->SetString("Ended", "EventName");

   if (mEndDialog->GetType())
   {
      mEndDialog->GetType()->GenerateNode(mEndDialog, newEndedEventNode, "Out", this);
   }

   EndSave();
}

////////////////////////////////////////////////////////////////////////////////
void DirectorDialogEditorPlugin::OnSaveAndClose()
{
   OnSave();
   Close();
}

////////////////////////////////////////////////////////////////////////////////
void DirectorDialogEditorPlugin::OnCancel()
{
   Close();
}

//////////////////////////////////////////////////////////////////////////
namespace DirectorDialogEditor
{
class DT_DIRECTOR_DIALOG_EDITOR_EXPORT PluginFactory : public dtDirector::PluginFactory
{
public:

   PluginFactory() {}
   ~PluginFactory() {}

   /** get the name of the plugin */
   virtual std::string GetName() { return DirectorDialogEditorPlugin::PLUGIN_NAME; }

   /** get a description of the plugin */
   virtual std::string GetDescription() { return "Dialog Editor Plugin Tool."; }

   virtual void GetDependencies(std::list<std::string>& deps)
   {
   }

   /** construct the plugin and return a pointer to it */
   virtual dtDirector::Plugin* Create()
   {
      mPlugin = new DirectorDialogEditorPlugin();
      return mPlugin;
   }

   virtual void Destroy()
   {
      delete mPlugin;
   }

private:

   dtDirector::Plugin* mPlugin;
};
} //namespace DirectorDialogEditorPlugin

extern "C" DT_DIRECTOR_DIALOG_EDITOR_EXPORT dtDirector::PluginFactory* CreatePluginFactory()
{
   return new DirectorDialogEditor::PluginFactory;
}

////////////////////////////////////////////////////////////////////////////////
