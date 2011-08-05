#include <dialogconditionline.h>

#include <QtGui/QLineEdit>

#include <dtDirector/node.h>
#include <dtDirector/outputlink.h>


///////////////////////////////////////////////////////////////////////////////
DialogConditionLineType::DialogConditionLineType()
   : mValueName("Boolean Name")
{
}

///////////////////////////////////////////////////////////////////////////////
DialogConditionLineType::~DialogConditionLineType()
{
}

////////////////////////////////////////////////////////////////////////////////
QString DialogConditionLineType::GetName() const
{
   return "Condition";
}

////////////////////////////////////////////////////////////////////////////////
QColor DialogConditionLineType::GetColor() const
{
   QColor color = Qt::white;
   color.setAlphaF(0.25f);
   return color;
}

////////////////////////////////////////////////////////////////////////////////
DialogLineType* DialogConditionLineType::Create() const
{
   return new DialogConditionLineType();
}

////////////////////////////////////////////////////////////////////////////////
void DialogConditionLineType::Init(DialogLineItem* line, DirectorDialogEditorPlugin* editor)
{
   CreateChoice(line, "True", editor, false, false);
   CreateChoice(line, "False", editor, false, false);
}

////////////////////////////////////////////////////////////////////////////////
void DialogConditionLineType::GenerateNode(DialogLineItem* line, dtDirector::Node* prevNode, const std::string& output, DirectorDialogEditorPlugin* editor)
{
   dtDirector::Node* compareNode = editor->CreateNode("Compare Bool", "General", prevNode);
   compareNode->SetBoolean(true, "B");
   editor->Connect(prevNode, compareNode, output, "In");

   GenerateNodeForReferences(line, compareNode, "In", editor);

   dtDirector::Node* valNode = editor->CreateNode("Reference", "Core");
   valNode->SetString(mValueName.toStdString(), "Reference");
   editor->Connect(compareNode, valNode, "A");

   GenerateNodeForChild(line->GetChildChoice(0)->GetChildLine(), compareNode, "A == B", editor);
   GenerateNodeForChild(line->GetChildChoice(1)->GetChildLine(), compareNode, "A != B", editor);
}

////////////////////////////////////////////////////////////////////////////////
bool DialogConditionLineType::ShouldOperateOn(const dtDirector::Node* node) const
{
   if (node && node->GetType().GetFullName() == "General.Compare Bool")
   {
      return true;
   }

   return false;
}

////////////////////////////////////////////////////////////////////////////////
void DialogConditionLineType::OperateOn(DialogLineItem* line, dtDirector::Node* node, DirectorDialogEditorPlugin* editor)
{
   CheckForReferencing(line, node, "In", editor);

   dtDirector::ValueNode* valNode = node->GetValueNode("A");
   if (valNode)
   {
      mValueName = valNode->GetString("Reference").c_str();
   }

   if (line->childCount() >= 2)
   {
      OperateOnChild(line->GetChildChoice(0), node, "A == B", editor);
      OperateOnChild(line->GetChildChoice(1), node, "A != B", editor);
   }
}

////////////////////////////////////////////////////////////////////////////////
QString DialogConditionLineType::GetDisplayName(const DirectorDialogEditorPlugin* editor) const
{
   return QString("Boolean Name: \'") + mValueName + "\'";
}

////////////////////////////////////////////////////////////////////////////////
QWidget* DialogConditionLineType::CreateInlineEditor(QWidget* parent, DirectorDialogEditorPlugin* editor) const
{
   return new QLineEdit(parent);
}

////////////////////////////////////////////////////////////////////////////////
void DialogConditionLineType::RefreshInlineEditor(QWidget* editor) const
{
   QLineEdit* control = dynamic_cast<QLineEdit*>(editor);
   if (control)
   {
      control->setText(mValueName);
   }
}

////////////////////////////////////////////////////////////////////////////////
void DialogConditionLineType::RefreshInlineData(QWidget* editor)
{
   QLineEdit* control = dynamic_cast<QLineEdit*>(editor);
   if (control)
   {
      mValueName = control->text();
   }
}

///////////////////////////////////////////////////////////////////////////////
