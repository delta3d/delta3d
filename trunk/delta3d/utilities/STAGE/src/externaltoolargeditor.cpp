#include <dtEditQt/externaltoolargeditor.h>
#include <dtEditQt/externaltoolargparser.h>

//////////////////////////////////////////////////////////////////////////
dtEditQt::ExternalToolArgEditor::ExternalToolArgEditor(const QList<const ExternalToolArgParser*>& parsers,
                                                       const QString& args,
                                                       QWidget* parent)
   : QDialog(parent)
{
   mUI.setupUi(this);
   mUI.argEdit->setText(args);

   for (int parserIdx = 0; parserIdx < parsers.size(); ++parserIdx)
   {
      QListWidgetItem* item = new QListWidgetItem(mUI.parserList);
      const ExternalToolArgParser* p = parsers.at(parserIdx);
      item->setText(p->GetVariableText());//set the text of the item to be the variable
      item->setData(Qt::UserRole, p->GetDescription()); //store the description for later
      item->setToolTip(p->ExpandArguments(p->GetVariableText())); //tooltip the expanded variable
   }

   connect(mUI.parserList, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
           this, SLOT(OnInsertArgument(QListWidgetItem*)));

   connect(mUI.parserList, SIGNAL(itemEntered(QListWidgetItem*)),
           this, SLOT(OnDisplayParserDescription(QListWidgetItem*)));
}

//////////////////////////////////////////////////////////////////////////
dtEditQt::ExternalToolArgEditor::~ExternalToolArgEditor()
{
}

//////////////////////////////////////////////////////////////////////////
QString dtEditQt::ExternalToolArgEditor::GetArgs() const
{
   return mUI.argEdit->text();
}

//////////////////////////////////////////////////////////////////////////
void dtEditQt::ExternalToolArgEditor::OnInsertArgument(QListWidgetItem* item)
{
   //insert the variable text into the arg list where the cursor currently is

   const QString itemText = item->text(); //the variable to use
   QString currText = mUI.argEdit->text(); //the current arg list

   mUI.argEdit->setText(currText.insert(mUI.argEdit->cursorPosition(), itemText));
}

//////////////////////////////////////////////////////////////////////////
void dtEditQt::ExternalToolArgEditor::OnDisplayParserDescription(QListWidgetItem* item)
{
   mUI.variableDescription->setText(item->data(Qt::UserRole).toString());
}
