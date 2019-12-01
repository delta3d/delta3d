#include <dialoglinetype.h>
#include <dialogrefline.h>
#include <dialoglineregistry.h>

#include <dtDirector/node.h>

#include <QtGui/QGroupBox>
#include <QtGui/QGridLayout>
#include <QtGui/QSpacerItem>
#include <QtGui/QCheckBox>
#include <QtGui/QLineEdit>
#include <QtGui/QMessageBox>


///////////////////////////////////////////////////////////////////////////////
DialogLineType::DialogLineType()
   : mWrapper(NULL)
   , mPreEventEdit(NULL)
   , mDuringEventEdit(NULL)
   , mPostEventEdit(NULL)
   , mHasPreEvent(false)
   , mHasDuringEvent(false)
   , mHasPostEvent(false)
{
}

///////////////////////////////////////////////////////////////////////////////
DialogLineType::~DialogLineType()
{
}

////////////////////////////////////////////////////////////////////////////////
QColor DialogLineType::GetColor() const
{
   QColor color = Qt::cyan;
   color.setAlphaF(0.15f);
   return color;
}

////////////////////////////////////////////////////////////////////////////////
void DialogLineType::Init(DialogLineItem* line, DirectorDialogEditorPlugin* editor)
{
}

////////////////////////////////////////////////////////////////////////////////
bool DialogLineType::CanHaveSubLine() const
{
   return true;
}

////////////////////////////////////////////////////////////////////////////////
bool DialogLineType::IsChoice() const
{
   return false;
}

////////////////////////////////////////////////////////////////////////////////
int DialogLineType::GetChoiceLimit() const
{
   return -1;
}

////////////////////////////////////////////////////////////////////////////////
QWidget* DialogLineType::CreateInlineEditor(QWidget* parent, DirectorDialogEditorPlugin* editor) const
{
   return NULL;
}

////////////////////////////////////////////////////////////////////////////////
void DialogLineType::RefreshInlineEditor(QWidget* editor) const
{
}

////////////////////////////////////////////////////////////////////////////////
void DialogLineType::RefreshInlineData(QWidget* editor)
{
}

////////////////////////////////////////////////////////////////////////////////
QLayout* DialogLineType::CreatePropertyEditor(DialogTreeWidget* tree)
{
   mWrapper = new QGridLayout();

   // Pre event.
   QGroupBox* preEventBox = new QGroupBox();
   preEventBox->setTitle("Before Line Event:");
   QGridLayout* preGrid = new QGridLayout(preEventBox);
   QCheckBox* preCheckBox = new QCheckBox();
   mPreEventEdit = new QLineEdit();
   mPreEventEdit->setDisabled(true);
   preGrid->addWidget(preCheckBox, 0, 0);
   preGrid->addWidget(mPreEventEdit, 0, 1);
   preGrid->setColumnStretch(0, 0);
   preGrid->setColumnStretch(1, 1);
   connect(preCheckBox, SIGNAL(stateChanged(int)), this, SLOT(OnPreEventCheckBoxChanged(int)));
   connect(mPreEventEdit, SIGNAL(textEdited(const QString&)), this, SLOT(OnPreEventTextEdited(const QString&)));

   // During event.
   QGroupBox* durEventBox = new QGroupBox();
   durEventBox->setTitle("During Line Event:");
   QGridLayout* durGrid = new QGridLayout(durEventBox);
   QCheckBox* durCheckBox = new QCheckBox();
   mDuringEventEdit = new QLineEdit();
   mDuringEventEdit->setDisabled(true);
   durGrid->addWidget(durCheckBox, 0, 0);
   durGrid->addWidget(mDuringEventEdit, 0, 1);
   durGrid->setColumnStretch(0, 0);
   durGrid->setColumnStretch(1, 1);
   connect(durCheckBox, SIGNAL(stateChanged(int)), this, SLOT(OnDuringEventCheckBoxChanged(int)));
   connect(mDuringEventEdit, SIGNAL(textEdited(const QString&)), this, SLOT(OnDuringEventTextEdited(const QString&)));

   // Post event.
   QGroupBox* postEventBox = new QGroupBox();
   postEventBox->setTitle("After Line Event:");
   QGridLayout* postGrid = new QGridLayout(postEventBox);
   QCheckBox* postCheckBox = new QCheckBox();
   mPostEventEdit = new QLineEdit();
   mPostEventEdit->setDisabled(true);
   postGrid->addWidget(postCheckBox, 0, 0);
   postGrid->addWidget(mPostEventEdit, 0, 1);
   postGrid->setColumnStretch(0, 0);
   postGrid->setColumnStretch(1, 1);
   connect(postCheckBox, SIGNAL(stateChanged(int)), this, SLOT(OnPostEventCheckBoxChanged(int)));
   connect(mPostEventEdit, SIGNAL(textEdited(const QString&)), this, SLOT(OnPostEventTextEdited(const QString&)));

   mWrapper->layout()->addWidget(preEventBox);
   mWrapper->layout()->addWidget(durEventBox);
   mWrapper->layout()->addWidget(postEventBox);

   // Setup initial values.
   preCheckBox->setChecked(mHasPreEvent);
   durCheckBox->setChecked(mHasDuringEvent);
   postCheckBox->setChecked(mHasPostEvent);

   mPreEventEdit->setText(mPreEventName);
   mDuringEventEdit->setText(mDuringEventName);
   mPostEventEdit->setText(mPostEventName);

   return mWrapper;
}

////////////////////////////////////////////////////////////////////////////////
void DialogLineType::ClosePropertyEditor(DialogTreeWidget* tree)
{
   mWrapper = NULL;
   mPreEventEdit = NULL;
   mDuringEventEdit = NULL;
   mPostEventEdit = NULL;
}

////////////////////////////////////////////////////////////////////////////////
QLayout* DialogLineType::CreatePropertyEditorForChild(DialogTreeWidget* tree, DialogChoiceItem* choice, int index)
{
   return NULL;
}

////////////////////////////////////////////////////////////////////////////////
void DialogLineType::ClosePropertyEditorForChild(DialogTreeWidget* tree, DialogChoiceItem* choice, int index)
{
}

////////////////////////////////////////////////////////////////////////////////
void DialogLineType::OnChildChoiceRemoved(DialogChoiceItem* choice, int index)
{
}

////////////////////////////////////////////////////////////////////////////////
void DialogLineType::OnChildChoiceAdded(DialogChoiceItem* choice, int index)
{
}

////////////////////////////////////////////////////////////////////////////////
void DialogLineType::OnChildChoiceMoved(DialogChoiceItem* choice, int oldIndex, int newIndex)
{
}

////////////////////////////////////////////////////////////////////////////////
void DialogLineType::OnSpeakerRemoved(const QString& speaker)
{
}

////////////////////////////////////////////////////////////////////////////////
void DialogLineType::OnSpeakerRenamed(const QString& oldName, const QString& newName)
{
}

////////////////////////////////////////////////////////////////////////////////
void DialogLineType::GeneratePreEventNode(dtDirector::Node*& prevNode, std::string& output, DirectorDialogEditorPlugin* editor)
{
   if (mHasPreEvent && !mPreEventName.isEmpty())
   {
      dtDirector::Node* preEventNode = editor->CreateNode("Call Remote Event", "Core", prevNode, 80);
      preEventNode->SetString(std::string("Pre Event ") + mPreEventName.toStdString(), "EventName");
      preEventNode->SetString("Local Scope", "Event Scope");
      editor->Connect(prevNode, preEventNode, output, "Call Event");

      editor->RegisterEvent(mPreEventName, DirectorDialogEditorPlugin::PRE_EVENT);
      prevNode = preEventNode;
      output = "Event Finished";
   }

   if (mHasDuringEvent && !mDuringEventName.isEmpty())
   {
      dtDirector::Node* durEventNode = editor->CreateNode("Call Remote Event", "Core", prevNode, 80);
      durEventNode->SetString("Local Scope", "Event Scope");
      durEventNode->SetString(std::string("During Event ") + mDuringEventName.toStdString(), "EventName");
      editor->Connect(prevNode, durEventNode, output, "Call Event");

      editor->RegisterEvent(mDuringEventName, DirectorDialogEditorPlugin::DURING_EVENT);
   }
}

////////////////////////////////////////////////////////////////////////////////
void DialogLineType::GeneratePostEventNode(dtDirector::Node*& prevNode, std::string& output, DirectorDialogEditorPlugin* editor)
{
   if (mHasPostEvent && !mPostEventName.isEmpty())
   {
      dtDirector::Node* postEventNode = editor->CreateNode("Call Remote Event", "Core", prevNode, 80);
      postEventNode->SetString("Local Scope", "Event Scope");
      postEventNode->SetString(std::string("Post Event ") + mPostEventName.toStdString(), "EventName");
      editor->Connect(prevNode, postEventNode, output, "Call Event");

      editor->RegisterEvent(mPostEventName, DirectorDialogEditorPlugin::POST_EVENT);
      prevNode = postEventNode;
      output = "Event Finished";
   }
}

////////////////////////////////////////////////////////////////////////////////
void DialogLineType::GenerateNodeForChild(DialogLineItem* childLine, dtDirector::Node* prevNode, const std::string& output, DirectorDialogEditorPlugin* editor)
{
   dtDirector::Node* newPrevNode = prevNode;
   std::string newOutput = output;
   GeneratePostEventNode(newPrevNode, newOutput, editor);

   if (!childLine || !childLine->GetType())
   {
      return;
   }

   childLine->GetType()->GeneratePreEventNode(newPrevNode, newOutput, editor);
   childLine->GetType()->GenerateNode(childLine, newPrevNode, newOutput, editor);
}

////////////////////////////////////////////////////////////////////////////////
void DialogLineType::GenerateNodeForReferences(DialogLineItem* line, dtDirector::Node* node, const std::string& input, DirectorDialogEditorPlugin* editor)
{
   if (!node)
   {
      return;
   }

   DialogTreeWidget* tree = editor->GetTree();
   if (tree)
   {
      int count = tree->topLevelItemCount();
      for (int index = 0; index < count; ++index)
      {
         QTreeWidgetItem* item = tree->topLevelItem(index);
         if (recurseFindRef(line->GetID(), item))
         {
            dtDirector::Node* refEvent = editor->CreateNode("Remote Event", "Core", node, 0);
            refEvent->SetString(std::string("Ref ") + QString::number(line->GetIndex()).toStdString(), "EventName");
            refEvent->SetPosition(refEvent->GetPosition() - osg::Vec2(600, 0));
            editor->Connect(refEvent, node, "Out", input);
            break;
         }
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
bool DialogLineType::recurseFindRef(const dtCore::UniqueId& id, QTreeWidgetItem* item) const
{
   if (!item)
   {
      return false;
   }

   int count = item->childCount();
   for (int index = 0; index < count; ++index)
   {
      QTreeWidgetItem* child = item->child(index);
      DialogLineItem* line = dynamic_cast<DialogLineItem*>(child);
      if (line)
      {
         DialogRefLineType* refLine = dynamic_cast<DialogRefLineType*>(line->GetType());
         if (refLine && refLine->GetReference() == id)
         {
            return true;
         }
      }

      if (recurseFindRef(id, child))
      {
         return true;
      }
   }

   return false;
}

////////////////////////////////////////////////////////////////////////////////
void DialogLineType::OperateOnChild(QTreeWidgetItem* item, dtDirector::Node* node, const std::string& output, DirectorDialogEditorPlugin* editor)
{
   std::vector<dtDirector::InputLink*> links;
   editor->GetNext(node, output, links);

   QString preEventName;
   QString durEventName;

   if (links.size())
   {
      // The first link is either the next line, a pre-event for the next
      // line, or a post-event for the current line.
      dtDirector::Node* childNode = links[0]->GetOwner();
      if (childNode)
      {
         // Handle post-event nodes.
         if (OperateOnPostEvent(childNode, mPostEventName))
         {
            mHasPostEvent = true;

            links.clear();
            if (!editor->GetNext(childNode, "Event Finished", links))
            {
               return;
            }

            childNode = links[0]->GetOwner();
         }

         // Handle pre-event nodes.
         if (OperateOnPreEvent(childNode, preEventName))
         {
            links.clear();
            if (editor->GetNext(childNode, "Event Finished", links))
            {
               childNode = links[0]->GetOwner();
            }
            else
            {
               QString error = QString("Attempted to load a Pre-Event when there is no line present!");

               QMessageBox messageBox("Load Failed!",
                  error, QMessageBox::Critical,
                  QMessageBox::Ok,
                  QMessageBox::NoButton,
                  QMessageBox::NoButton,
                  editor);

               messageBox.exec();
               return;
            }
         }

         if (OperateOnDuringEvent(childNode, durEventName))
         {
            childNode = NULL;

            // If we found a during event for the current line, then
            // we should still have another link that will be the line.
            if (links.size() > 1)
            {
               childNode = links[1]->GetOwner();
            }

            if (!childNode)
            {
               QString error = QString("Attempted to load a During-Event when there is no line present!");

               QMessageBox messageBox("Load Failed!",
                  error, QMessageBox::Critical,
                  QMessageBox::Ok,
                  QMessageBox::NoButton,
                  QMessageBox::NoButton,
                  editor);

               messageBox.exec();
               return;
            }
         }

         const DialogLineType* type = DialogLineRegistry::GetInstance().GetLineTypeForNode(childNode);
         if (type)
         {
            DialogLineItem* newLine = new DialogLineItem(type->GetName(), type, editor->GetTree()->CreateIndex(), editor);
            item->addChild(newLine);
            item->setExpanded(true);

            newLine->GetType()->Init(newLine, editor);
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

            newLine->GetType()->OperateOn(newLine, childNode, editor);
         }
         else
         {
            QString error = QString("Failed to find line type for node \'") + childNode->GetTypeName().c_str() + "\'.";

            QMessageBox messageBox("Load Failed!",
               error, QMessageBox::Critical,
               QMessageBox::Ok,
               QMessageBox::NoButton,
               QMessageBox::NoButton,
               editor);

            messageBox.exec();
         }
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
bool DialogLineType::OperateOnPreEvent(dtDirector::Node* node, QString& eventName)
{
   if (node->GetType().GetFullName() == "Core.Call Remote Event")
   {
      QString name = node->GetString("EventName").c_str();
      if (name.startsWith("Pre Event "))
      {
         eventName = name.mid(10);
         return true;
      }
   }

   return false;
}

////////////////////////////////////////////////////////////////////////////////
bool DialogLineType::OperateOnDuringEvent(dtDirector::Node* node, QString& eventName)
{
   if (node->GetType().GetFullName() == "Core.Call Remote Event")
   {
      QString name = node->GetString("EventName").c_str();
      if (name.startsWith("During Event "))
      {
         eventName = name.mid(13);
         return true;
      }
   }

   return false;
}

////////////////////////////////////////////////////////////////////////////////
bool DialogLineType::OperateOnPostEvent(dtDirector::Node* node, QString& eventName)
{
   if (node->GetType().GetFullName() == "Core.Call Remote Event")
   {
      QString name = node->GetString("EventName").c_str();
      if (name.startsWith("Post Event "))
      {
         eventName = name.mid(11);
         return true;
      }
   }

   return false;
}

////////////////////////////////////////////////////////////////////////////////
QTreeWidgetItem* DialogLineType::CreateChoice(DialogLineItem* line, const QString& choiceName, DirectorDialogEditorPlugin* editor, bool moveable, bool nameable)
{
   DialogChoiceItem* newChoice = new DialogChoiceItem(editor->GetTree()->CreateIndex(), choiceName, moveable, nameable);
   if (newChoice)
   {
      line->addChild(newChoice);
      line->setExpanded(true);
   }

   return newChoice;
}

////////////////////////////////////////////////////////////////////////////////
void DialogLineType::CheckForReferencing(DialogLineItem* line, dtDirector::Node* node, const std::string& input, DirectorDialogEditorPlugin* editor)
{
   std::vector<dtDirector::OutputLink*> links;
   editor->GetPrev(node, input, links);

   int count = (int)links.size();
   for (int index = 0; index < count; ++index)
   {
      dtDirector::Node* prevNode = links[index]->GetOwner();
      if (prevNode && prevNode->GetTypeName() == "Remote Event")
      {
         QString eventName = prevNode->GetString("EventName").c_str();
         if (eventName.startsWith("Ref"))
         {
            editor->MapReference(eventName, line->GetID());
            return;
         }
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void DialogLineType::OnPreEventCheckBoxChanged(int state)
{
   if (state == Qt::Checked)
   {
      mHasPreEvent = true;
   }
   else
   {
      mHasPreEvent = false;
   }

   mPreEventEdit->setEnabled(mHasPreEvent);
}

////////////////////////////////////////////////////////////////////////////////
void DialogLineType::OnDuringEventCheckBoxChanged(int state)
{
   if (state == Qt::Checked)
   {
      mHasDuringEvent = true;
   }
   else
   {
      mHasDuringEvent = false;
   }

   mDuringEventEdit->setEnabled(mHasDuringEvent);
}

////////////////////////////////////////////////////////////////////////////////
void DialogLineType::OnPostEventCheckBoxChanged(int state)
{
   if (state == Qt::Checked)
   {
      mHasPostEvent = true;
   }
   else
   {
      mHasPostEvent = false;
   }

   mPostEventEdit->setEnabled(mHasPostEvent);
}

////////////////////////////////////////////////////////////////////////////////
void DialogLineType::OnPreEventTextEdited(const QString& text)
{
   mPreEventName = text;
}

////////////////////////////////////////////////////////////////////////////////
void DialogLineType::OnDuringEventTextEdited(const QString& text)
{
   mDuringEventName = text;
}

////////////////////////////////////////////////////////////////////////////////
void DialogLineType::OnPostEventTextEdited(const QString& text)
{
   mPostEventName = text;
}

///////////////////////////////////////////////////////////////////////////////
