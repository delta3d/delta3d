#include <dtEditQt/externaltoolargeditor.h>
#include <dtEditQt/externaltoolargparser.h>

//////////////////////////////////////////////////////////////////////////
dtEditQt::ExternalToolArgEditor::ExternalToolArgEditor(const QList<const ExternalToolArgParser*> &parsers,
                                                       const QString& args,
                                                       QWidget* parent)
: QDialog(parent)
{
   ui.setupUi(this);
   ui.argEdit->setText(args);

   for (int parserIdx=0; parserIdx<parsers.size(); ++parserIdx)
   {     
      QListWidgetItem* item = new QListWidgetItem(ui.parserList);
      const ExternalToolArgParser* p = parsers.at(parserIdx);
      item->setText(p->GetVariableText());//set the text of the item to be the variable
      item->setData(Qt::UserRole, p->GetDescription()); //store the description for later
      item->setToolTip(p->ExpandArguments(p->GetVariableText())); //tooltip the expanded variable
   }
 
   connect(ui.parserList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), 
           this, SLOT(OnInsertArgument(QListWidgetItem*)));

   connect(ui.parserList, SIGNAL(itemEntered(QListWidgetItem*)), 
           this, SLOT(OnDisplayParserDescription(QListWidgetItem*)));
}

//////////////////////////////////////////////////////////////////////////
dtEditQt::ExternalToolArgEditor::~ExternalToolArgEditor()
{
}

//////////////////////////////////////////////////////////////////////////
QString dtEditQt::ExternalToolArgEditor::GetArgs() const
{
   return ui.argEdit->text();
}

//////////////////////////////////////////////////////////////////////////
void dtEditQt::ExternalToolArgEditor::OnInsertArgument(QListWidgetItem* item)
{
   //insert the variable text into the arg list where the cursor currently is 

   const QString itemText = item->text(); //the variable to use
   QString currText = ui.argEdit->text(); //the current arg list

   ui.argEdit->setText(currText.insert(ui.argEdit->cursorPosition(), itemText));
}

//////////////////////////////////////////////////////////////////////////
void dtEditQt::ExternalToolArgEditor::OnDisplayParserDescription(QListWidgetItem* item)
{
   ui.variableDescription->setText(item->data(Qt::UserRole).toString());
}
