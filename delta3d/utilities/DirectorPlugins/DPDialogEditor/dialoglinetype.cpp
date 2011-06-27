#include <dialoglinetype.h>
#include <dialogrefline.h>
#include <dialoglineregistry.h>

#include <dtDirector/node.h>

#include <QtGui/QWidget>
#include <QtGui/QLineEdit>
#include <QtGui/QMessageBox>


///////////////////////////////////////////////////////////////////////////////
DialogLineType::DialogLineType()
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
void DialogLineType::GenerateNodeForChild(DialogLineItem* childLine, dtDirector::Node* prevNode, const std::string& output, DirectorDialogEditorPlugin* editor)
{
   if (!childLine || !childLine->GetType())
   {
      return;
   }

   childLine->GetType()->GenerateNode(childLine, prevNode, output, editor);
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

   if (links.size())
   {
      // There should only be one node connected.
      dtDirector::Node* childNode = links[0]->GetOwner();
      if (childNode)
      {
         const DialogLineType* type = DialogLineRegistry::GetInstance().GetLineTypeForNode(childNode);
         if (type)
         {
            DialogLineItem* newLine = new DialogLineItem(type->GetName(), type, editor->GetTree()->CreateIndex(), editor);
            item->addChild(newLine);
            newLine->GetType()->Init(newLine, editor);
            item->setExpanded(true);

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
QTreeWidgetItem* DialogLineType::CreateChoice(DialogLineItem* line, const std::string& choiceName, DirectorDialogEditorPlugin* editor, bool moveable, bool nameable)
{
   DialogChoiceItem* newChoice = new DialogChoiceItem(editor->GetTree()->CreateIndex(), choiceName.c_str(), moveable, nameable);
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
         if (eventName.contains("Ref"))
         {
            editor->MapReference(eventName, line->GetID());
            return;
         }
      }
   }
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
QWidget* DialogLineType::CreatePropertyEditor(DialogTreeWidget* tree) const
{
   return NULL;
}

///////////////////////////////////////////////////////////////////////////////
