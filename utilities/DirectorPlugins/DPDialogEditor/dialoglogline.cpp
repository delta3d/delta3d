#include <dialoglogline.h>

#include <QtGui/QLineEdit>
#include <QtGui/QGridLayout>
#include <QtGui/QSpacerItem>
#include <QtGui/QLabel>

#include <dtDirector/node.h>

///////////////////////////////////////////////////////////////////////////////
DialogLogLineType::DialogLogLineType()
   : mLog("")
{
}

///////////////////////////////////////////////////////////////////////////////
DialogLogLineType::~DialogLogLineType()
{
}

////////////////////////////////////////////////////////////////////////////////
QString DialogLogLineType::GetName() const
{
   return "Debug Message";
}

////////////////////////////////////////////////////////////////////////////////
QColor DialogLogLineType::GetColor() const
{
   QColor color = Qt::green;
   color.setAlphaF(0.25f);
   return color;
}

////////////////////////////////////////////////////////////////////////////////
DialogLineType* DialogLogLineType::Create() const
{
   return new DialogLogLineType();
}

////////////////////////////////////////////////////////////////////////////////
void DialogLogLineType::GenerateNode(DialogLineItem* line, dtDirector::Node* prevNode, const std::string& output, DirectorDialogEditorPlugin* editor)
{
   dtDirector::Node* logNode = editor->CreateNode("Log Message", "General", prevNode, 150);
   logNode->SetString(mLog.toStdString(), "Message");
   editor->Connect(prevNode, logNode, output, "In");

   GenerateNodeForReferences(line, logNode, "In", editor);

   GenerateNodeForChild(line->GetChildLine(), logNode, "Out", editor);
}

////////////////////////////////////////////////////////////////////////////////
bool DialogLogLineType::ShouldOperateOn(const dtDirector::Node* node) const
{
   if (node && node->GetType().GetFullName() == "General.Log Message")
   {
      return true;
   }

   return false;
}

////////////////////////////////////////////////////////////////////////////////
void DialogLogLineType::OperateOn(DialogLineItem* line, dtDirector::Node* node, DirectorDialogEditorPlugin* editor)
{
   CheckForReferencing(line, node, "In", editor);

   mLog = node->GetString("Message").c_str();

   OperateOnChild(line, node, "Out", editor);
}

////////////////////////////////////////////////////////////////////////////////
QString DialogLogLineType::GetDisplayName(const DirectorDialogEditorPlugin* editor) const
{
   if (mLog.isEmpty())
   {
      return "<None>";
   }

   return mLog;
}

////////////////////////////////////////////////////////////////////////////////
QWidget* DialogLogLineType::CreateInlineEditor(QWidget* parent, DirectorDialogEditorPlugin* editor) const
{
   return new QLineEdit(parent);
}

////////////////////////////////////////////////////////////////////////////////
void DialogLogLineType::RefreshInlineEditor(QWidget* editor) const
{
   QLineEdit* lineEdit = dynamic_cast<QLineEdit*>(editor);
   if (lineEdit)
   {
      lineEdit->setText(mLog);
   }
}

////////////////////////////////////////////////////////////////////////////////
void DialogLogLineType::RefreshInlineData(QWidget* editor)
{
   QLineEdit* lineEdit = dynamic_cast<QLineEdit*>(editor);
   if (lineEdit)
   {
      mLog = lineEdit->text();
   }
}

///////////////////////////////////////////////////////////////////////////////
