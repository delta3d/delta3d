
////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include "NewCharacterDialog.h"
#include <QtGui/QPushButton>



////////////////////////////////////////////////////////////////////////////////
// CONSTANTS
////////////////////////////////////////////////////////////////////////////////
const std::string NewCharacterDialog::DEFAULT_CHARACTER_NAME("New Character");



////////////////////////////////////////////////////////////////////////////////
// CLASS CODE
////////////////////////////////////////////////////////////////////////////////
NewCharacterDialog::NewCharacterDialog(QWidget* parent)
   : BaseClass(parent)
{
   mUI.setupUi(this);
   QString qname(NewCharacterDialog::DEFAULT_CHARACTER_NAME.c_str());
   mUI.mName->setText(qname);

   connect(mUI.mName, SIGNAL(textChanged(const QString&)),
      this, SLOT(OnNameChanged(const QString&)));
}

NewCharacterDialog::~NewCharacterDialog()
{}

void NewCharacterDialog::OnNameChanged(const QString& text)
{
   QPushButton* buttonOk = mUI.mButtonBox->button(QDialogButtonBox::Ok);
   buttonOk->setEnabled( ! text.isEmpty());
}
