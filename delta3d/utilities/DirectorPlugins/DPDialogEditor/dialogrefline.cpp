#include <dialogrefline.h>

#include <dialogtreeitems.h>
#include <dialoglineregistry.h>

#include <QtGui/QComboBox>
#include <QtGui/QTreeWidgetItem>

#include <dtDirector/node.h>

///////////////////////////////////////////////////////////////////////////////
DialogRefLineType::DialogRefLineType()
   : mReference("")
{
}

///////////////////////////////////////////////////////////////////////////////
DialogRefLineType::~DialogRefLineType()
{
}

////////////////////////////////////////////////////////////////////////////////
DialogLineType* DialogRefLineType::Create() const
{
   return new DialogRefLineType();
}

////////////////////////////////////////////////////////////////////////////////
void DialogRefLineType::GenerateNode(DialogLineItem* line, dtDirector::Node* prevNode, const std::string& output, DirectorDialogEditorPlugin* editor)
{
   DialogLineItem* refLine = NULL;
   QString result;
   int count = editor->GetTree()->topLevelItemCount();
   for (int index = 0; index < count; ++index)
   {
      QTreeWidgetItem* item = editor->GetTree()->topLevelItem(index);
      refLine = recurseFindRef(item, result);
      if (refLine)
      {
         break;
      }
   }

   if (!refLine)
   {
      return;
   }

   dtDirector::Node* callNode = editor->CreateNode("Call Remote Event", "Core", prevNode, 80);
   callNode->SetString(std::string("Line ") + QString::number(refLine->GetIndex()).toStdString(), "EventName");
   editor->Connect(prevNode, callNode, output, "Call Event");
}

////////////////////////////////////////////////////////////////////////////////
bool DialogRefLineType::ShouldOperateOn(const dtDirector::Node* node) const
{
   if (node && node->GetType().GetFullName() == "Core.Call Remote Event")
   {
      return true;
   }

   return false;
}

////////////////////////////////////////////////////////////////////////////////
void DialogRefLineType::OperateOn(DialogLineItem* line, dtDirector::Node* node, DirectorDialogEditorPlugin* editor)
{
   std::string refName = node->GetString("EventName");
   editor->RegisterReference(this, refName.c_str());
}

////////////////////////////////////////////////////////////////////////////////
bool DialogRefLineType::CanHaveSubLine() const
{
   return false;
}

////////////////////////////////////////////////////////////////////////////////
QString DialogRefLineType::GetDisplayName(const DialogTreeWidget* tree) const
{
   QString result = "<Unknown>";

   if (mReference.ToString().empty())
   {
      result = "<None>";
   }
   else
   {
      int count = tree->topLevelItemCount();
      for (int index = 0; index < count; ++index)
      {
         QTreeWidgetItem* item = tree->topLevelItem(index);
         if (recurseFindRef(item, result))
         {
            break;
         }
      }
   }

   return result;
}

////////////////////////////////////////////////////////////////////////////////
DialogLineItem* DialogRefLineType::recurseFindRef(QTreeWidgetItem* item, QString& outValue) const
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
      if (line && line->GetID() == mReference)
      {
         outValue = QString(" (") + line->text(0) + ")";
         return line;
      }

      line = recurseFindRef(child, outValue);
      if (line)
      {
         return line;
      }
   }

   return NULL;
}

////////////////////////////////////////////////////////////////////////////////
QWidget* DialogRefLineType::CreateInlineEditor(QWidget* parent, DialogTreeWidget* tree) const
{
   QComboBox* control = new QComboBox(parent);
   int count = tree->topLevelItemCount();
   control->addItem("<None>");
   for (int index = 0; index < count; ++index)
   {
      QTreeWidgetItem* item = tree->topLevelItem(index);
      recurseItemTree(item, control);
   }
   return control;
}

////////////////////////////////////////////////////////////////////////////////
void DialogRefLineType::recurseItemTree(QTreeWidgetItem* item, QComboBox* control) const
{
   if (!item)
   {
      return;
   }

   int count = item->childCount();
   for (int index = 0; index < count; ++index)
   {
      QTreeWidgetItem* child = item->child(index);
      DialogLineItem* line = dynamic_cast<DialogLineItem*>(child);
      if (line && line->GetTypeName() != "Reference")
      {
         control->addItem(line->text(0), QVariant(line->GetID().ToString().c_str()));
      }

      recurseItemTree(child, control);
   }
}

////////////////////////////////////////////////////////////////////////////////
void DialogRefLineType::RefreshInlineEditor(QWidget* editor) const
{
   QComboBox* control = dynamic_cast<QComboBox*>(editor);
   if (control)
   {
      int index = control->findData(QVariant(mReference.ToString().c_str()));

      if (index > -1 && index < control->count())
      {
         control->setCurrentIndex(index);
      }
      else
      {
         control->setCurrentIndex(0);
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void DialogRefLineType::RefreshInlineData(QWidget* editor)
{
   QComboBox* control = dynamic_cast<QComboBox*>(editor);
   if (control)
   {
      mReference = control->itemData(control->currentIndex()).toString().toStdString();
   }
}

///////////////////////////////////////////////////////////////////////////////
