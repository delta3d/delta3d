#include <dialogswitchline.h>

#include <QtGui/QComboBox>

#include <dtDirector/node.h>
#include <dtDirector/outputlink.h>


///////////////////////////////////////////////////////////////////////////////
DialogSwitchLineType::DialogSwitchLineType()
   : mSwitchType(DialogSwitchLineType::SWITCH_SEQUENTIAL)
{
}

///////////////////////////////////////////////////////////////////////////////
DialogSwitchLineType::~DialogSwitchLineType()
{
}

////////////////////////////////////////////////////////////////////////////////
QString DialogSwitchLineType::GetName() const
{
   return "Alternating";
}

////////////////////////////////////////////////////////////////////////////////
QColor DialogSwitchLineType::GetColor() const
{
   QColor color = Qt::yellow;
   color.setAlphaF(0.25f);
   return color;
}

////////////////////////////////////////////////////////////////////////////////
DialogLineType* DialogSwitchLineType::Create() const
{
   return new DialogSwitchLineType();
}

////////////////////////////////////////////////////////////////////////////////
void DialogSwitchLineType::GenerateNode(DialogLineItem* line, dtDirector::Node* prevNode, const std::string& output, DirectorDialogEditorPlugin* editor)
{
   int count = line->childCount();

   dtDirector::Node* switchNode = editor->CreateNode("Switch", "General", prevNode);
   switchNode->SetBoolean(true, "Looping");
   switchNode->SetInt(count, "OutputCount");
   editor->Connect(prevNode, switchNode, output, "In");

   GenerateNodeForReferences(line, switchNode, "In", editor);

   if (mSwitchType == SWITCH_RANDOM)
   {
      dtDirector::Node* randValue = editor->CreateNode("Random Int", "General");
      randValue->SetInt(1, "Min Value");
      randValue->SetInt(count, "Max Value");
      editor->Connect(switchNode, randValue, "CurrentIndex");
   }

   for (int index = 0; index < count; ++index)
   {
      DialogChoiceItem* choice = line->GetChildChoice(index);
      if (choice)
      {
         GenerateNodeForChild(choice->GetChildLine(), switchNode, switchNode->GetOutputLinks()[index].GetName(), editor);
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
bool DialogSwitchLineType::ShouldOperateOn(const dtDirector::Node* node) const
{
   if (node && node->GetType().GetFullName() == "General.Switch")
   {
      return true;
   }

   return false;
}

////////////////////////////////////////////////////////////////////////////////
void DialogSwitchLineType::OperateOn(DialogLineItem* line, dtDirector::Node* node, DirectorDialogEditorPlugin* editor)
{
   CheckForReferencing(line, node, "In", editor);

   dtDirector::ValueNode* randValue = node->GetValueNode("CurrentIndex");
   if (randValue)
   {
      mSwitchType = SWITCH_RANDOM;
   }
   else
   {
      mSwitchType = SWITCH_SEQUENTIAL;
   }

   int count = (int)node->GetOutputLinks().size();
   for (int index = 0; index < count; ++index)
   {
      QTreeWidgetItem* choice = CreateChoice(line, "", editor);
      if (choice)
      {
         OperateOnChild(choice, node, node->GetOutputLinks()[index].GetName(), editor);
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
QString DialogSwitchLineType::GetDisplayName(const DirectorDialogEditorPlugin* editor) const
{
   switch (mSwitchType)
   {
   case SWITCH_SEQUENTIAL:
      return "Sequentially";
      
   case SWITCH_RANDOM:
      return "Randomly";
   }

   return "";
}

////////////////////////////////////////////////////////////////////////////////
QWidget* DialogSwitchLineType::CreateInlineEditor(QWidget* parent, DirectorDialogEditorPlugin* editor) const
{
   QComboBox* control = new QComboBox(parent);
   control->addItem("Sequentially");
   control->addItem("Randomly");
   return control;
}

////////////////////////////////////////////////////////////////////////////////
void DialogSwitchLineType::RefreshInlineEditor(QWidget* editor) const
{
   QComboBox* control = dynamic_cast<QComboBox*>(editor);
   if (control)
   {
      control->setCurrentIndex(mSwitchType);
   }
}

////////////////////////////////////////////////////////////////////////////////
void DialogSwitchLineType::RefreshInlineData(QWidget* editor)
{
   QComboBox* control = dynamic_cast<QComboBox*>(editor);
   if (control)
   {
      mSwitchType = control->currentIndex();
   }
}

///////////////////////////////////////////////////////////////////////////////
