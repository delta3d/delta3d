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
#include <QtGui/QListWidget>
#include <QtGui/QGroupBox>
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

   mUI.mDialogTree->SetEditor(this);
   connect(mUI.mDialogTree, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
      this, SLOT(OnCurrentTreeItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));

   mUI.mSpeakerListWidget->SetTree(GetTree());

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
   mRoot = new DialogRootItem("Root", DialogLineRegistry::GetInstance().GetStartedLineType(), true, this);
   mEndDialog = new DialogRootItem("End Dialog", DialogLineRegistry::GetInstance().GetEndedLineType(), false, this);

   mUI.mDialogTree->addTopLevelItem(mRoot);
   mUI.mDialogTree->addTopLevelItem(mEndDialog);

   QSettings settings("MOVES", "Director Dialog Editor");

   settings.beginGroup("MainWindow");
   resize(settings.value("Size", QSize(800, 600)).toSize());
   move(settings.value("Pos", QPoint(100, 100)).toPoint());
   if (settings.contains("State"))
   {
      QByteArray state = settings.value("State").toByteArray();
      restoreState(state);
   }
   if (settings.contains("Geom"))
   {
      QByteArray state = settings.value("Geom").toByteArray();
      restoreGeometry(state);
   }
   settings.endGroup();

   settings.beginGroup("Splitter");
   if (settings.contains("State"))
   {
      QByteArray state = settings.value("State").toByteArray();
      mUI.mMainSplitter->restoreState(state);
   }
   if (settings.contains("Geom"))
   {
      QByteArray state = settings.value("Geom").toByteArray();
      mUI.mMainSplitter->restoreGeometry(state);
   }
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

   settings.beginGroup("Splitter");
   settings.setValue("State", mUI.mMainSplitter->saveState());
   settings.setValue("Geom", mUI.mMainSplitter->saveGeometry());
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
      mUI.mLinePropertyLayout->removeWidget(mEditWidget);
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
QStringList DirectorDialogEditorPlugin::GetSpeakerList() const
{
   return mUI.mSpeakerListWidget->GetSpeakerList();
}

////////////////////////////////////////////////////////////////////////////////
void DirectorDialogEditorPlugin::ConnectSpeaker(const std::string& speaker, dtDirector::Node* node, const std::string& linkName)
{
   dtDirector::Node* speakerNode = CreateNode("Reference", "Core");
   if (!speakerNode)
   {
      return;
   }

   speakerNode->SetString(std::string("Speaker ") + speaker, "Reference");

   Connect(node, speakerNode, linkName);
}

///////////////////////////////////////////////////////////////////////////////
QStringList DirectorDialogEditorPlugin::GetConnectedSpeakers(dtDirector::Node* node, const std::string& linkName)
{
   QStringList speakers;
   int count = node->GetValueNodeCount(linkName);
   for (int refNodeIndex = 0; refNodeIndex < count; ++refNodeIndex)
   {
      dtDirector::ValueNode* referenceNode = node->GetValueNode(linkName, refNodeIndex);
      if (referenceNode != NULL && referenceNode->GetType().GetFullName() == "Core.Reference")
      {
         QString name = QString::fromStdString(referenceNode->GetString("Reference"));
         if (name.startsWith("Speaker "))
         {
            // Cut off "Speaker " from name
            name = name.mid(8);
            speakers.push_back(name);
         }
      }
   }
   return speakers;
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
void DirectorDialogEditorPlugin::RegisterReference(DialogLineItem* refLine, const QString& refName)
{
   std::map<QString, std::vector<DialogLineItem*> >::iterator iter = mRefRegister.find(refName);

   if (iter == mRefRegister.end())
   {
      std::vector<DialogLineItem*> refLines;
      refLines.push_back(refLine);
      mRefRegister[refName] = refLines;
   }
   else
   {
      iter->second.push_back(refLine);
   }
}

////////////////////////////////////////////////////////////////////////////////
void DirectorDialogEditorPlugin::RegisterEvent(const QString& name, int eventType)
{
   mEventRegister[name] |= eventType;
}

////////////////////////////////////////////////////////////////////////////////
void DirectorDialogEditorPlugin::OnCurrentTreeItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
   if (mUI.mLinePropertyLayout)
   {
      if (mEditWidget)
      {
         mUI.mLinePropertyLayout->removeWidget(mEditWidget);

         // Remove any current widgets provided by the previous item.
         DialogLineItem* prevLine = dynamic_cast<DialogLineItem*>(previous);
         if (prevLine)
         {
            if (prevLine->GetType())
            {
               prevLine->GetType()->ClosePropertyEditor(GetTree());
            }
         }
         else
         {
            DialogChoiceItem* prevChoice = dynamic_cast<DialogChoiceItem*>(previous);
            if (prevChoice)
            {
               DialogLineItem* parentLine = dynamic_cast<DialogLineItem*>(prevChoice->parent());
               if (parentLine && parentLine->GetType())
               {
                  parentLine->GetType()->ClosePropertyEditorForChild(GetTree(),
                     prevChoice, parentLine->indexOfChild(prevChoice));
               }
            }
         }

         delete mEditWidget;
         mEditWidget = NULL;
      }

      QLayout* layout = NULL;
      DialogLineItem* line = dynamic_cast<DialogLineItem*>(current);
      if (line)
      {
         if (line->GetType())
         {
            layout = line->GetType()->CreatePropertyEditor(GetTree());
         }
      }
      else
      {
         DialogChoiceItem* choice = dynamic_cast<DialogChoiceItem*>(current);
         if (choice)
         {
            DialogLineItem* parentLine = dynamic_cast<DialogLineItem*>(choice->parent());
            if (parentLine && parentLine->GetType())
            {
               layout = parentLine->GetType()->CreatePropertyEditorForChild(GetTree(),
                  choice, parentLine->indexOfChild(choice));
            }
         }
      }

      if (!current)
      {
         mUI.mSpeakerGroupBox->show();
      }
      else
      {
         mUI.mSpeakerGroupBox->hide();
      }

      if (layout)
      {
         mEditWidget = new QWidget();
         QGridLayout* grid = new QGridLayout(mEditWidget);
         grid->setMargin(0);
         grid->addLayout(layout, 0, 0);

         mUI.mLinePropertyLayout->insertWidget(0, mEditWidget);
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

      // First attempt to load any pre or during event data.
      QString preEventName;
      if (DialogLineType::OperateOnPreEvent(lineNode, preEventName))
      {
         links.clear();
         if (GetNext(lineNode, "Event Finished", links))
         {
            lineNode = links[0]->GetOwner();
         }
         else
         {
            QString error = QString("Attempted to load a Pre-Event when there is no line present!");

            QMessageBox messageBox("Load Failed!",
               error, QMessageBox::Critical,
               QMessageBox::Ok,
               QMessageBox::NoButton,
               QMessageBox::NoButton,
               this);

            messageBox.exec();
            return;
         }
      }

      QString durEventName;
      if (DialogLineType::OperateOnDuringEvent(lineNode, durEventName))
      {
         lineNode = NULL;
         if (links.size() > 1)
         {
            lineNode = links[1]->GetOwner();
         }

         if (!lineNode)
         {
            QString error = QString("Attempted to load a During-Event when there is no line present!");

            QMessageBox messageBox("Load Failed!",
               error, QMessageBox::Critical,
               QMessageBox::Ok,
               QMessageBox::NoButton,
               QMessageBox::NoButton,
               this);

            messageBox.exec();
            return;
         }
      }

      if (mRoot->GetType())
      {
         if (mRoot->GetType()->ShouldOperateOn(lineNode))
         {
            mRoot->GetType()->OperateOn(mRoot, lineNode, this);

            if (!preEventName.isEmpty())
            {
               mRoot->GetType()->mHasPreEvent = true;
               mRoot->GetType()->mPreEventName = preEventName;
            }

            if (!durEventName.isEmpty())
            {
               mRoot->GetType()->mHasDuringEvent = true;
               mRoot->GetType()->mDuringEventName = durEventName;
            }
         }
      }
      else
      {
         const DialogLineType* type = DialogLineRegistry::GetInstance().GetLineTypeForNode(lineNode);
         if (type)
         {
            DialogLineItem* newLine = new DialogLineItem(type->GetName(), type, GetTree()->CreateIndex(), this);
            mRoot->addChild(newLine);
            mRoot->setExpanded(true);

            newLine->GetType()->Init(newLine, this);
            if (!preEventName.isEmpty())
            {
               newLine->GetType()->mHasPreEvent = true;
               newLine->GetType()->mPreEventName = preEventName;
            }

            if (!durEventName.isEmpty())
            {
               newLine->GetType()->mHasDuringEvent = true;
               newLine->GetType()->mDuringEventName = durEventName;
            }

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

   GetTree()->UpdateLabels();

   // Now go through all our registered reference lines and connect them to
   // their property references.
   std::map<QString, std::vector<DialogLineItem*> >::iterator iter;
   for (iter = mRefRegister.begin(); iter != mRefRegister.end(); ++iter)
   {
      QString refName = iter->first;
      std::map<QString, dtCore::UniqueId>::iterator mapIter = mRefMap.find(refName);
      if (mapIter != mRefMap.end())
      {
         dtCore::UniqueId& id = mapIter->second;

         std::vector<DialogLineItem*>& refLines = iter->second;

         int count = (int)refLines.size();
         for (int index = 0; index < count; ++index)
         {
            DialogLineItem* refLine = refLines[index];
            DialogRefLineType* refType = dynamic_cast<DialogRefLineType*>(refLine->GetType());
            if (refType)
            {
               refType->SetReference(id);
               refLine->UpdateLabel();
            }
         }
      }
   }

   // Find all speakers.
   mUI.mSpeakerGroupBox->show();
   mUI.mSpeakerListWidget->Reset();

   GetGraph()->GetNodes("Value Link", "Core", nodes);
   int count = (int)nodes.size();
   for (int index = 0; index < count; ++index)
   {
      dtDirector::Node* node = nodes[index];
      QString valName = node->GetString("Name").c_str();
      if (valName.startsWith("Speaker "))
      {
         valName = valName.mid(8);
         mUI.mSpeakerListWidget->AddSpeaker(valName);
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void DirectorDialogEditorPlugin::OnSave()
{
   BeginSave();

   mEventRegister.clear();

   dtDirector::Node* newInputNode = CreateNode("Input Link", "Core", NULL, 100);
   newInputNode->SetString("Play", "Name");

   dtDirector::Node* newCallStartEventNode = CreateNode("Call Remote Event", "Core", newInputNode, 100);
   newCallStartEventNode->SetString("Started", "EventName");
   newCallStartEventNode->SetString("Local Scope", "Event Scope");
   Connect(newInputNode, newCallStartEventNode, "Out", "Call Event");

   dtDirector::Node* newCallEndEventNode = CreateNode("Call Remote Event", "Core", newCallStartEventNode, 100);
   newCallEndEventNode->SetString("Ended", "EventName");
   newCallEndEventNode->SetString("Local Scope", "Event Scope");
   Connect(newCallStartEventNode, newCallEndEventNode, "Event Finished", "Call Event");

   dtDirector::Node* newOutputNode = CreateNode("Output Link", "Core", newCallEndEventNode, 100);
   newOutputNode->SetString("Finished", "Name");
   Connect(newCallEndEventNode, newOutputNode, "Event Finished", "In");

   // Generate our speaker value links.
   dtDirector::Node* prevNode = NULL;
   QStringList speakerList = GetSpeakerList();
   int count = speakerList.count();
   for (int index = 0; index < count; ++index)
   {
      QString& speaker = speakerList[index];
      if (!speaker.isEmpty())
      {
         dtDirector::Node* linkNode = CreateNode("Value Link", "Core");

         if (linkNode)
         {
            linkNode->SetName(std::string("Speaker ") + speaker.toStdString());
            AutoPositionNode(linkNode, prevNode, 150, 90);
            prevNode = linkNode;
         }
      }
   }

   dtDirector::Node* newStartedEventNode = CreateNode("Remote Event", "Core", NULL, 100);
   newStartedEventNode->SetString("Started", "EventName");

   if (mRoot->GetType())
   {
      dtDirector::Node* prevNode = newStartedEventNode;
      std::string outputName = "Out";

      mRoot->GetType()->GeneratePreEventNode(prevNode, outputName, this);
      mRoot->GetType()->GenerateNode(mRoot, prevNode, outputName, this);
   }
   else if (mRoot->childCount() > 0)
   {
      DialogLineItem* line = dynamic_cast<DialogLineItem*>(mRoot->child(0));
      if (line)
      {
         dtDirector::Node* prevNode = newStartedEventNode;
         std::string outputName = "Out";

         line->GetType()->GeneratePreEventNode(prevNode, outputName, this);
         line->GetType()->GenerateNode(line, prevNode, outputName, this);
      }
   }

   dtDirector::Node* newEndedEventNode = CreateNode("Remote Event", "Core", NULL, 100);
   newEndedEventNode->SetString("Ended", "EventName");

   if (mEndDialog->GetType())
   {
      dtDirector::Node* prevNode = newEndedEventNode;
      std::string outputName = "Out";

      mEndDialog->GetType()->GeneratePreEventNode(prevNode, outputName, this);
      mEndDialog->GetType()->GenerateNode(mEndDialog, prevNode, outputName, this);
   }

   // Generate our events.
   std::map<QString, int>::iterator iter;
   for (iter = mEventRegister.begin(); iter != mEventRegister.end(); ++iter)
   {
      QString eventName  = iter->first;
      int     eventTypes = iter->second;

      dtDirector::Node* topNode = NULL;
      dtDirector::Node* preNode = NULL;
      dtDirector::Node* durNode = NULL;
      dtDirector::Node* postNode = NULL;

      if (eventTypes & PRE_EVENT)
      {
         preNode = CreateNode("Remote Event", "Core", NULL, 100);
         preNode->SetString(std::string("Pre Event ") + eventName.toStdString(), "EventName");
         if (!topNode) topNode = preNode;
      }

      if (eventTypes & DURING_EVENT)
      {
         durNode = CreateNode("Remote Event", "Core", NULL, 100);
         durNode->SetString(std::string("During Event ") + eventName.toStdString(), "EventName");
         if (!topNode) topNode = durNode;
      }

      if (eventTypes & POST_EVENT)
      {
         postNode = CreateNode("Remote Event", "Core", NULL, 100);
         postNode->SetString(std::string("Post Event ") + eventName.toStdString(), "EventName");
         if (!topNode) topNode = postNode;
      }

      dtDirector::Node* outputNode = CreateNode("Output Link", "Core", topNode, 100);
      outputNode->SetString(eventName.toStdString(), "Name");
      Connect(preNode, outputNode, "Out", "In");
      Connect(durNode, outputNode, "Out", "In");
      Connect(postNode, outputNode, "Out", "In");
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
